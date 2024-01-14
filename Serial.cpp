#include <bits/stdc++.h>
#include "hopcroftKarp.h"
#include "CycleTimer.h"
using namespace std;

#ifndef EPSILON
#define EPSILON 64
#endif

int main(int argc, char *argv[]) {
	ifstream in;
	if (argc == 2) {
		in.open(string(argv[1]));
		
	} else {
		in.open("graph_10000_100000_100000.txt");
	}
	ofstream out("output.txt");

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
				cout << "Error\n";
				exit(-1);
			}
			v[x].erase(it);
		}
		else {
			v[x].push_back(y);
		}
		if(i % EPSILON == 0) {
			vector<int> btoa(m, -1);
			now = Parallel_hopcroftKarp_new(v, btoa, m);
			from = i;
		}
		out << now << '\n';
		// cout << i - 1 << " : " << now << '\n'; 
		// cout << i << " : " << CycleTimer::currentSeconds() - start << '\n'; 
	}
	cout << "took : " <<  CycleTimer::currentSeconds() - start << '\n';
	return 0;
}
