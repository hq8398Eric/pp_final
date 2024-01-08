#include <bits/stdc++.h>
#include "hopcroftKarp.h"
#include "CycleTimer.h"
using namespace std;

#define epsilon 1024 

int main() {
	int n, m; cin >> n >> m;
	vector<vector<int> > v(n);
	for(int i = 0; i < m; i ++) {
		int x, y; cin >> x >> y;
		v[x].push_back(y);
	}
	double start = CycleTimer::currentSeconds();
	vector<int> btoa(m, -1);
	int now = hopcroftKarp(v, btoa);
	int from = 0; 
	cout << 0 << " : " << now << '\n';
	int q; cin >> q;
	for(int i = 1; i <= q; i ++) {
		int d, x, y; cin >> d >> x >> y;
		if(d == 0) {
			auto it = find(v[x].begin(), v[x].end(), y);
			if(it == v[x].end()) {
				cout << "the fuck\n";
				exit(-1);
			}
			v[x].erase(it);
		}
		else {
			v[x].push_back(y);
		}
		if(i % epsilon == 0) {
			vector<int> btoa(m, -1);
			now = hopcroftKarp(v, btoa);
			from = i;
		}
		// cout << i << " : " << now << ' ' << from << '\n';
		cout << i << " : " << CycleTimer::currentSeconds() - start << '\n'; 
	}
	return 0;
}
