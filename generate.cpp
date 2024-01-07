#include <bits/stdc++.h>
using namespace std;

#include <fstream>
int main(int argc, char* argv[]) {
	if(argc < 4) {
		cout << "Usage: ./generate <nodes> <edges> <queries>\n";
		exit(-1);
	}
	long long n = atoi(argv[1]);
	long long m = atoi(argv[2]);
	long long q = atoi(argv[3]);
	if(m > n * n) {
		cout << "the fuck\n";
		exit(-1);
	}
	cout << "generating bipartite graph of " << n << " nodes on each side and " << m << " edges\n";

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<> dist(0, n - 1);

	ofstream out("graph.txt");

	out << n << ' ' << m << '\n';

	set<pair<int, int> > in, nin;

	if(m > n * n / 2) {
		set<pair<int, int> > S;
		for(int i = 0; i < n; i ++) {
			for(int j = 0; j < n; j ++) {
				in.insert({i, j});
			}
		}
		while(in.size() != m) {
			int x = dist(rng);
			int y = dist(rng);
			if(in.count({x, y})) {
				nin.insert({x, y});
				in.erase({x, y});
			}
		}
		for(auto x : in) {
			out << x.first << ' ' << x.second << '\n';
		}
	}
	else {
		for(int i = 0; i < n; i ++) {
			for(int j = 0; j < n; j ++) {
				nin.insert({i, j});
			}
		}
		while(in.size() != m) {	
			int x = dist(rng);
			int y = dist(rng);
			if(in.count({x, y})) continue;
			in.insert({x, y});
			nin.erase({x, y});
			out << x << ' ' << y << '\n';
		}	
	}
	// below for generating queries
	out << q << '\n';

	uniform_int_distribution<> zero_one(0, 1);
	uniform_int_distribution<int64_t> del(0, m * m);

	for(int i = 0; i < q; i ++) {
		int d = zero_one(rng);
		if(in.size() > 0 && d == 0) {
			int td = del(rng) % in.size();
			auto it = in.begin();
			advance(it, td);
			pair<int, int> t = *it;
			in.erase(t);
			nin.insert(t);
			out << d << ' ' << t.first << ' ' << t.second << '\n';
		}
		else {
			int td = del(rng) % nin.size();
			auto it = nin.begin();
			advance(it, td);
			pair<int, int> t = *it;
			nin.erase(t);
			in.insert(t);
			out << d << ' ' << t.first << ' ' << t.second << '\n';
		}
	}
}
