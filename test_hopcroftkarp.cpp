#include <bits/stdc++.h>
#include "hopcroftKarp.h"
#include "CycleTimer.h"
using namespace std;


int main() {
	ifstream in("graph_400000_20000000_1.txt");
	ofstream out("output.txt");

	int n, m; in >> n >> m;
	vector<vector<int> > v(n);
	for(int i = 0; i < m; i ++) {
		int x, y; in >> x >> y;
		v[x].push_back(y);
	}
	vector<int> btoa(m, -1);
	double start = CycleTimer::currentSeconds();
	int now = hopcroftKarp(v, btoa);
    out << now << '\n';
	cout << "took : " <<  CycleTimer::currentSeconds() - start << '\n';

	vector<int> btoa2(m, -1);
	start = CycleTimer::currentSeconds();
	now = Parallel_hopcroftKarp_new(v, btoa2, m);
    out << now << '\n';
	cout << "took : " <<  CycleTimer::currentSeconds() - start << '\n';
	return 0;
}
