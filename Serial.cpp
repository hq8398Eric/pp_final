#include <bits/stdc++.h>
#include "hopcroftKarp.h"
#include "CycleTimer.h"
using namespace std;

#define epsilon 32 

int main() {
	ifstream in("graph.txt");

	int n, m; in >> n >> m;
	vector<vector<int> > v(n);
	for(int i = 0; i < m; i ++) {
		int x, y; in >> x >> y;
		v[x].push_back(y);
	}
	double start = CycleTimer::currentSeconds();
	vector<int> btoa(m, -1);
	int now = hopcroftKarp(v, btoa);
	int from = 0; 
	int q; in >> q;
	for(int i = 1; i <= q; i ++) {
		int d, x, y; in >> d >> x >> y;
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
		// cout << i - 1 << " : " << now << '\n'; 
		// cout << i << " : " << CycleTimer::currentSeconds() - start << '\n'; 
	}
	cout << "took : " <<  CycleTimer::currentSeconds() - start << '\n';
	return 0;
}
