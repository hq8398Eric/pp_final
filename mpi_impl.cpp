#include "hopcroftKarp.h"
#include <vector>
#include <cstdio>
#include <mpi.h>
#include <pthread.h>
#include "CycleTimer.h"

#ifndef T
#define T 16
#endif

#ifndef EPSILON
#define EPSILON 16
#endif

using namespace std;



int meta[2];
int cur_finish_time = 0;

ifstream in;
ofstream out;

void construct_graph(vector<vector<int>> &G) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    MPI_Request request = MPI_REQUEST_NULL;
    
    if (rank == 0) {

        int n, m;
        in >> n >> m;
        int *edge_count = new int[n];
        int *buf = new int[m];
        G.resize(n);
        for(int i = 0; i < m; i ++) {
            int x, y; in >> x >> y;
            G[x].push_back(y);
        }
        
        int tot_edge_num = 0;
        for (int i = 0; i < n; i ++) {
            edge_count[i] = G[i].size();
            for (int j = 0; j < G[i].size(); j ++) {
                buf[tot_edge_num + j] = G[i][j];
            }
            tot_edge_num += G[i].size();
        }
        
        meta[0] = n;
        meta[1] = m;
        for (int i = 1; i < size; i ++) {
            MPI_Isend(&meta[0], 2, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
            int next_id = (1 << (i - 1));
            if (next_id < size) {
                MPI_Isend(&edge_count[0], n, MPI_INT, next_id, 1, MPI_COMM_WORLD, &request);
                MPI_Isend(&buf[0], tot_edge_num, MPI_INT, next_id, 2, MPI_COMM_WORLD, &request);
            } 
        }
        MPI_Wait(&request, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(&meta[0], 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int idx = __builtin_clz(rank | 1);
        int from = rank - (1U << 32 - (idx + 1));

        int *buf = new int[meta[1]];
        int *edge_count = new int[meta[0]];

        MPI_Recv(&edge_count[0], meta[0], MPI_INT, from, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&buf[0], meta[1], MPI_INT, from, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int next_b = (1U << (32 - idx));
        while (next_b + rank < size) {
            MPI_Isend(&edge_count[0], meta[0], MPI_INT, next_b + rank, 1, MPI_COMM_WORLD, &request);
            MPI_Isend(&buf[0], meta[1], MPI_INT, next_b + rank, 2, MPI_COMM_WORLD, &request);
            next_b <<= 1;
        }
        
        G.resize(meta[0]);
        int cur_edge_idx = 0;
        for (int i = 0; i < meta[0]; i ++) {
            G[i].resize(edge_count[i]);
            for (int j = 0; j < edge_count[i]; j ++) {
                G[i][j] = buf[cur_edge_idx + j];
            }
            cur_edge_idx += edge_count[i];
        }
        MPI_Wait(&request, MPI_STATUS_IGNORE);
    }

}

void check_graph(vector<vector<int>> &G) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_rank(MPI_COMM_WORLD, &size);
    if (rank == 1) {
        for (int i = 0; i < meta[0]; i ++) {
            printf("G[%d].size() = %d, edge = ", i, G[i].size());
            for (auto &j : G[i]) {
                printf("%d ", j);
            }
            printf("\n");
        }
        vector<int> btoa(meta[1], -1);
        hopcroftKarp(G, btoa);
        
    }
    MPI_Request request = MPI_REQUEST_NULL;
    MPI_Wait(&request, MPI_STATUS_IGNORE);
}

void send_stop(int cur_time) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int update[3] = {-1, -1, -1};
    MPI_Request request[size + 1];
    for (int i = 1; i < size; i ++) {
        MPI_Send(&update[0], 3, MPI_INT, i, cur_time + 3, MPI_COMM_WORLD);
    }
    return;
}


struct pargs {
    int tot_num;
    int *ans;
};

void *recv_ans(void *args) {
    struct pargs *parg = (struct pargs *) args;
    // int cur_ans_time = parg->time;
    int cur_time = 0;
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int buf[2];
    
    // int ans = 0;
    // int cur_best = 0x3f3f3f3f;
    // for (int i = 1; i < size; i ++) {
    //     MPI_Recv(buf[i], 2, MPI_INT, i, cur_ans_time, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     if (buf[i][0] != -1 && cur_ans_time - EPSILON < buf[i][0] && cur_best > buf[i][0] - cur_ans_time) {
    //         ans = buf[i][1];
    //     }
    // }
    int from = 0;
    int cur_ans = 0;
    for (int i = 0; i < parg->tot_num; i ++) {
        if (i % T == 0) {
            MPI_Recv(&buf[0], 2, MPI_INT, from + 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cur_ans = buf[1];
            from += 1;
            from %= (size - 1);
        }
        parg->ans[i] = cur_ans;
        // printf("After %d\n", i);
 //       __sync_fetch_and_add(&cur_finish_time, 1);
        // printf("cur_finish_time =  %d\n", cur_finish_time);
    }
    return NULL;
}

int *ans;

void compute_matching(vector<vector<int>> &G) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    
    MPI_Status status[size];
    
    int update[3];
    
    int cur_tag = 3;
    int cur_time = 0;
    int cur_ans_time = 0;
    pthread_t recv_thread;
    pargs args;
    if (rank == 0) {

        int q;
        in >> q;
        ans = new int[q + 10];

        args.ans = &ans[0];
        args.tot_num = q;
        pthread_create(&recv_thread, NULL, recv_ans, (void *) &args);
        for (int i = 0; i < q; i ++) {
            int d, x, y;
            in >> d >> x >> y;
            MPI_Request request = MPI_REQUEST_NULL;
            
//            while(*(&cur_finish_time) < i - T * (size - 1)) {
//		    __sync_synchronize();
//	    }
	    // printf("cur_finish_time = %d\n, cur_time = %d\n", cur_finish_time, i);
            
            update[0] = d;
            update[1] = x;
            update[2] = y;
            
            for (int i = 1; i < size; i ++) {
                MPI_Send(&update[0], 3, MPI_INT, i, cur_time + 3, MPI_COMM_WORLD);
            }
            
            
            args.ans = &ans[0];
            cur_time += 1;
        }
        pthread_join(recv_thread, NULL);
        send_stop(cur_time);
        
        for (int i = 0; i < q; i ++) {
            out << ans[i] << '\n';
        }
        
    } else {
        int next_time = (rank - 1) * T;
        MPI_Request request = MPI_REQUEST_NULL;
        int respond[2];
        respond[0] = -1;
        respond[1] = -1;
        
        while (true) {
            MPI_Status status;
            MPI_Recv(&update[0], 3, MPI_INT, 0, cur_time + 3, MPI_COMM_WORLD, &status);
	    // printf("After recv time %d info, update[0] = %d\n", cur_time, update[0]);
            
            if (update[0] == -1) {
                break;
            }
            
            if (update[0] == 1) {
                G[update[1]].push_back(update[2]);
            } else {
				auto fi = find(G[update[1]].begin(), G[update[1]].end(), update[2]);
				if(fi == G[update[1]].end()) {
					cerr << "invalid mod\n";
					exit(-1);
				}
				G[update[1]].erase(fi);
            }
            
            if (next_time == cur_time) {
		        vector<int> btoa(meta[1], -1);
                int res = hopcroftKarp(G, btoa);
                next_time += T * (size - 1);
                respond[0] = cur_time;
                respond[1] = res;
                // printf("rank = %d, cur_time = %d\n", rank, cur_time);
		        // fflush(stdout);
                MPI_Isend(&respond[0], 2, MPI_INT, 0, cur_time, MPI_COMM_WORLD, &request);
            	MPI_Wait(&request, MPI_STATUS_IGNORE);
            } 
            cur_time += 1;
        }
    }
}

int main(int argc, char *argv[]) {
    
    
    int provides;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provides);

    double start = CycleTimer::currentSeconds();
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) {
        if (argc == 2) {
            in.open(string(argv[1]));
        } else {
            in.open("graph.txt");
        }
        out.open("out_mpi.txt");
    }
    vector<vector<int>> G;
    construct_graph(G);
    compute_matching(G);
    double end = CycleTimer::currentSeconds();
    if (rank == 0)
        cout << "took : " << end - start << '\n';
    MPI_Finalize();
    


    return 0;
}


