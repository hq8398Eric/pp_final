#include "hopcroftKarp.h"
#include <vector>
#include <cstdio>
#include <mpi.h>
#include <pthread.h>

#define T 4
#define EPSILON 8

using namespace std;



int meta[2];

void construct_graph(vector<vector<int>> &G) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    vector<int> buf;
    vector<int> edge_count;
    
    MPI_Request request = MPI_REQUEST_NULL;
    
    if (rank == 0) {
        int n, m;
        cin >> n >> m;
        G.resize(n);
        for(int i = 0; i < m; i ++) {
            int x, y; cin >> x >> y;
            G[x].push_back(y);
        }
        
        int tot_edge_num = 0;
        for (int i = 0; i < n; i ++) {
            edge_count.push_back(G[i].size());
            tot_edge_num += G[i].size();
            for (auto &j : G[i]) {
                buf.push_back(j);
            }
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
        
    } else {
        MPI_Recv(&meta[0], 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int idx = __builtin_clz(rank | 1);
        int from = rank - (1U << 32 - (idx + 1));

        edge_count.resize(meta[0]);
        buf.resize(meta[1]);

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
    }
    // MPI_Wait(&request, MPI_STATUS_IGNORE);
}

void check_graph(vector<vector<int>> &G) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_rank(MPI_COMM_WORLD, &size);
    for (int i = 0; i < meta[0]; i ++) {
        printf("in rank = %d, G[%d].size() = %d, edge = ", rank, i, G[i].size());
        for (auto &j : G[i]) {
            printf("%d ", j);
        }
        printf("\n");
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
        MPI_Isend(&update[0], 3, MPI_INT, i, cur_time, MPI_COMM_WORLD, &request[i]);
    }
    MPI_Waitall(size - 1, &request[1], MPI_STATUS_IGNORE);
    // printf("Master fin\n");
    return;
}


struct pargs {
    int time;
    int *ans;
};

void *recv_ans(void *args) {
    struct pargs *parg = (struct pargs *) args;
    int cur_ans_time = parg->time;
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int buf[size][2];
    
    // MPI_Request recv_request[size];
    int ans = 0;
    
    for (int i = 1; i < size; i ++) {
        MPI_Recv(&buf[i][0], 2, MPI_INT, i, cur_ans_time, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (cur_ans_time - EPSILON < buf[i][0] && buf[i][0] != -1) {
            ans = buf[i][1];
        }
    }
    parg->ans[cur_ans_time] = ans;
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
    vector<pthread_t> threads;
    
    if (rank == 0) {
        int q;
        cin >> q;
        ans = new int[size];

        MPI_Request recv_request[size];
        MPI_Status status[size];
        for (int i = 0; i < q; i ++) {
            int d, x, y;
            cin >> d >> x >> y;
            MPI_Request request = MPI_REQUEST_NULL;
            
            update[0] = d;
            update[1] = x;
            update[2] = y;
            
            for (int i = 1; i < size; i ++) {
                MPI_Isend(&update[0], 3, MPI_INT, i, cur_time, MPI_COMM_WORLD, &request);
            }
            
            
            // int cur_ans = 0;
            // int best_ans_time = 0x3f3f3f3f;
            // for (int i = size - 1; i > 0; i --) {
            //     MPI_Irecv(&ans[i][0], 2, MPI_INT, i, 0, MPI_COMM_WORLD, &recv_request[i]);
            // }
            // int flag;
            // int outcomes = 0;
            // int idx[size];
            
            
            struct pargs args;
            pthread_t t;
            args.time = cur_ans_time;
            args.ans = ans;
            pthread_create(&t, NULL, recv_ans, (void *) &args);
            threads.push_back(t);
            // MPI_Testall(size - 1, &recv_request[1], &flag, &status[1]);
            // MPI_Testsome(size - 1, &recv_request[1], &idx[0], &outcomes, &status[1]);
            // if (outcomes == 0) {
            //     MPI_Waitsome(size - 1, &recv_request[1], &idx[0], &outcomes, &status[1]);
            // }
            // for (int i = 0; i < outcomes; i ++) {
            //     if (ans[idx[i]][0] != -1 && ans[idx[i]][0] - cur_ans_time < best_ans_time && ans[idx[i]][0] - EPSILON < cur_ans_time) {
            //         best_ans_time = ans[idx[i]][0] - cur_ans_time;
            //         cur_ans = ans[idx[i]][1];
            //     }
            // }
            // cout << cur_ans << endl;
            cur_time += 1;
        }
        for (int i = 0; i < q; i ++) {
            pthread_join(threads[i], NULL);
            cout << ans[i] << '\n';
        }
        send_stop(cur_time);
        
    } else {
        int next_time = rank - 1;
        MPI_Request request = MPI_REQUEST_NULL;
        int respond[2];
        respond[0] = -1;
        respond[1] = -1;

        while (true) {
            MPI_Status status;
            MPI_Recv(&update[0], 3, MPI_INT, 0, cur_time, MPI_COMM_WORLD, &status);
            
            if (update[0] == -1) {
                break;
            }
            
            if (update[0] == 0) {
                G[update[1]] = G[update[2]];
            } else {
				auto fi = find(G[update[1]].begin(), G[update[1]].end(), update[2]);
				if(fi == G[update[1]].end()) {
					cout << "invalid mod\n";
					exit(-1);
				}
				G[update[1]].erase(fi);
            }
            
            if (next_time == cur_time) {
		        vector<int> btoa(meta[0], -1);
                int res = hopcroftKarp(G, btoa);
                next_time += T;
                respond[0] = cur_time;
                respond[1] = res;
            } 
            MPI_Isend(&respond[0], 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
            cur_time += 1;
        }
    }
}

int main() {
    
    MPI_Init(NULL, NULL);
    vector<vector<int>> G;
    construct_graph(G);
    compute_matching(G);
    MPI_Finalize();
    


    return 0;
}