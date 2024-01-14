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
	string filename = "graph_" + string(argv[1]) + "_" + string(argv[2]) + "_" + string(argv[3]) + ".txt";
	if(m > n * n) {
		cout << "the fuck\n";
		exit(-1);
	}
	cout << "generating bipartite graph of " << n << " nodes on each side and " << m << " edges\n";

	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<> dist(0, n - 1);

	ofstream out(filename);

	out << n << ' ' << m << '\n';

	// set<pair<int, int> > S;
	int tot_cnt = 0;
	map<pair<int, int>, int> S;
	map<int, pair<int, int>> mp;
	
	while(S.size() != m) {	
		int x = dist(rng);
		int y = dist(rng);
		if(S.count({x, y})) continue;
		S[{x, y}] = 1;
		mp[tot_cnt ++] = {x, y};
		out << x << ' ' << y << '\n';
	}	
	// below for generating queries
	out << q << '\n';

	uniform_int_distribution<> zero_one(0, 1);
	uniform_int_distribution<int64_t> del(0, 0x3f3f3f3f3f3f3f3f);
	int failed = 0;
	long long adv = 0;

	for(int i = 0; i < q; i ++) {
		int d = zero_one(rng);
		if(S.size() > 0 && d == 0) {
			int td = del(rng) % mp.size();
			pair<int, int> cur = mp[td];
			while (S.find(cur) == S.end()) {
				td = del(rng) % mp.size();
				cur = mp[td];
			}
			
			out << d << ' ' << cur.first << ' ' << cur.second << '\n';
			S.erase(cur);
			// adv += td;
			// auto it = S.begin();
			// advance(it, td);
			// S.erase(it);
			// pair<int, int> t = *it;
		}
		else {
			int x = dist(rng);
			int y = dist(rng);
			while(S.count({x, y})) {
				failed += 1;
				x = dist(rng);
				y = dist(rng);
			}
			S[{x, y}] = 1;
			mp[tot_cnt ++] = {x, y};
			out << d << ' ' << x << ' ' << y << '\n';
		}
	}
	cout << "failed = " << failed << "adv = " << adv << endl;
}
