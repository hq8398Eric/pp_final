#include "hopcroftKarp.h"
#include "CycleTimer.h"
#include <pthread.h>
#include <semaphore.h>
#define MAX_G 1000000
#define pool_size 4 
#define T 16
#define epsilon 32 


int n, m, q;
vector<int> ans;
vector<vector<int> > V;
vector<pair<int, pair<int, int> > > mod;

pthread_t ids[pool_size];

int now;
bool finished = false;
// time stamp when told to calculate
// updated by workers
// needs the semaphore 

sem_t job_sig;
sem_t update_now;
sem_t update_R;
pthread_rwlock_t mod_lock;

void* workers(void* fuck) {
	vector<vector<int> > localV = V;
	int local_time = 0;
	// copy the graph here
	while(true) {
		sem_wait(&job_sig);
		// waits for the signal to work
	
		sem_wait(&update_now);
		int to_cal = now;
		now = now + T;
		sem_post(&update_now);
		// gets the place that needs to be worked on

		while(local_time < to_cal) {
			pthread_rwlock_rdlock(&mod_lock);
			auto t = mod[local_time];
			pthread_rwlock_unlock(&mod_lock);

			int x = t.second.first;
			int y = t.second.second;

			if(t.first) {
				localV[x].push_back(y);
			}
			else {
				auto fi = find(localV[x].begin(), localV[x].end(), y);
				if(fi == localV[x].end()) {
					cout << "invalid mod\n";
					exit(-1);
				}
				localV[x].erase(fi);
			}

			local_time ++;
		}
		// apply the changes till the place I want to calculate
		vector<int> btoa(n, -1);
		int res = hopcroftKarp(localV, btoa);

		
		if(to_cal == 0) {
			for(int i = 0; i < to_cal + epsilon; i ++) {
				ans[i] = res;
			}
		}
		else {
			for(int i = to_cal + epsilon - T; i < min(q, to_cal + epsilon); i ++) {
				ans[i] = res;
			}
		}
		
		if(to_cal == q / T * T) finished = true;
	}
	return NULL;
}

void init() {
	sem_init(&job_sig, 0, 0);
	sem_init(&update_now, 0, 1);
	sem_init(&update_R, 0, 1);

	pthread_rwlock_init(&mod_lock, NULL);

	for(int i = 0; i < pool_size; i ++) {
		pthread_create(&ids[i], NULL, workers, NULL);
	}

	now = 0;
}
void destruct() {
	sem_destroy(&job_sig);
	sem_destroy(&update_now);
	sem_destroy(&update_R);

	pthread_rwlock_destroy(&mod_lock);

	for(int i = 0; i < pool_size; i ++) {
		pthread_cancel(ids[i]);
	}
}

int main() {
	
	ifstream in("graph.txt");

	in >> n >> m;
	V.resize(n);
	for(int i = 0; i < m; i ++) {
		int x, y; in >> x >> y;
		V[x].push_back(y);
	}
	
	init(); // initialize semaphore and shits

	in >> q;
	ans.resize(q + 1);
	
	double start = CycleTimer::currentSeconds();
	sem_post(&job_sig);

	for(int i = 1; i <= q; i ++) {
		int d, x, y; in >> d >> x >> y;
		pthread_rwlock_wrlock(&mod_lock);
		mod.push_back({d, {x, y}});
		pthread_rwlock_unlock(&mod_lock);

		if(i % T == 0) {
			sem_post(&job_sig);
		}
	}
	
	while(!finished);

	double tim = CycleTimer::currentSeconds() - start;
	destruct(); // destruct semaphore and shits.
	
	/*	
	for(int i = 0; i < q; i ++) {
		cout << i << " : " << ans[i] << '\n';
	}
	*/

	cout << "took : " << tim << '\n';
		
}
