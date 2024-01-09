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

	set<pair<int, int> > S;
	
	while(S.size() != m) {	
		int x = dist(rng);
		int y = dist(rng);
		if(S.count({x, y})) continue;
		S.insert({x, y});
		out << x << ' ' << y << '\n';
	}	
	// below for generating queries
	out << q << '\n';

	uniform_int_distribution<> zero_one(0, 1);
	uniform_int_distribution<int64_t> del(0, n * n);

	for(int i = 0; i < q; i ++) {
		int d = zero_one(rng);
		if(S.size() > 0 && d == 0) {
			int td = del(rng) % S.size();
			auto it = S.begin();
			advance(it, td);
			S.erase(it);
			pair<int, int> t = *it;
			out << d << ' ' << t.first << ' ' << t.second << '\n';
		}
		else {
			int x = dist(rng);
			int y = dist(rng);
			while(S.count({x, y})) {
				x = dist(rng);
				y = dist(rng);
			}
			S.insert({x, y});
			out << d << ' ' << x << ' ' << y << '\n';
		}
	}
}
