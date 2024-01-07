#include "hopcroftKarp.h"
#include <pthread.h>
#include <semaphore.h>
#define MAX_G 1000000
#define pool_size 4
#define T 4
#define epsilon 8


int n, m;
vector<vector<int> > V;
vector<pair<int, pair<int, int> > > mod;

pthread_t ids[pool_size];

int now;
// time stamp when told to calculate
// updated by workers
// needs the semaphore 

int curR, fromR;
// curR = the latest result
// fromR = the timestamp curR is calculated based on

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

		cout << "someone working on " << to_cal << '\n';

		while(local_time <= to_cal) {
			pthread_rwlock_rdlock(&mod_lock);
			auto t = mod[local_time];
			pthread_rwlock_unlock(&mod_lock);

			int x = t.second.first;
			int y = t.second.second;


			if(t.first == 0) {
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
	
		sem_wait(&update_R);
		if(to_cal > fromR) {
			fromR = to_cal;
			curR = res;
		}
		sem_post(&update_R);
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

	fromR = INT_MIN;
	now = -T;
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
	cin >> n >> m;
	V.resize(n);
	for(int i = 0; i < m; i ++) {
		int x, y; cin >> x >> y;
		V[x].push_back(y);
	}
	
	init(); // initialize semaphore and shits
	
	sem_post(&job_sig);
	while(fromR != -T);
	cout << curR << '\n';

	int q; cin >> q;
	for(int i = 0; i < q; i ++) {
		int d, x, y; cin >> d >> x >> y;
		pthread_rwlock_wrlock(&mod_lock);
		mod.push_back({d, {x, y}});
		pthread_rwlock_unlock(&mod_lock);

		if(i % T == 0) {
			sem_post(&job_sig);
		}
		// as the result by research
		while(fromR < i - epsilon);
		cout << curR << '\n';

	}
	
	destruct(); // destruct semaphore and shits.
}
