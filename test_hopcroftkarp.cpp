#include <bits/stdc++.h>
#include "hopcroftKarp.h"
#include "CycleTimer.h"
using namespace std;


int main(int argc, char *argv[]) {
	ifstream in;
	if (argc == 2) {
		in.open(string(argv[1]));
	} else {
		in.open("graph_100000_100000_10.txt");
	}
	
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
	// int now = Parallel_hopcroftKarp_new(v, btoa, m);
    out << now << '\n';
	cout << "took : " <<  CycleTimer::currentSeconds() - start << '\n';

	vector<int> btoa2(m, -1);
	start = CycleTimer::currentSeconds();
	now = Parallel_hopcroftKarp_new(v, btoa2, m);
	// now = hopcroftKarp(v, btoa2);
    out << now << '\n';
	cout << "took : " <<  CycleTimer::currentSeconds() - start << '\n';
	return 0;
}
