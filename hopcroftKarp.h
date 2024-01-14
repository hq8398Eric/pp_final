/**
 * Author: Chen Xing
 * Date: 2009-10-13
 * License: CC0
 * Source: N/A
 * Description: Fast bipartite matching algorithm. Graph $g$ should be a list
 * of neighbors of the left partition, and $btoa$ should be a vector full of
 * -1's of the same size as the right partition. Returns the size of
 * the matching. $btoa[i]$ will be the match for vertex $i$ on the right side,
 * or $-1$ if it's not matched.
 * Usage: vi btoa(m, -1); hopcroftKarp(g, btoa);
 * Time: O(\sqrt{V}E)
 * Status: stress-tested by MinimumVertexCover, and tested on oldkattis.adkbipmatch and SPOJ:MATCHING
 */
#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

#define rep(i, a, b) for(int i = a; i < (b); ++i)
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
typedef long long ll;
typedef pair<int, int> pii;
typedef vector<int> vi;

bool dfs(int a, int L, vector<vi>& g, vi& btoa, vi& A, vi& B) {
	if (A[a] != L) return 0;
	A[a] = -1;
	for (int b : g[a]) if (B[b] == L + 1) {
		B[b] = 0;
		if (btoa[b] == -1 || dfs(btoa[b], L + 1, g, btoa, A, B))
			return btoa[b] = a, 1;
	}
	return 0;
}

int hopcroftKarp(vector<vi>& g, vi& btoa) {
	int res = 0;
	vi A(g.size()), B(btoa.size()), cur, next;
	for (;;) {
		fill(all(A), 0);
		fill(all(B), 0);
		/// Find the starting nodes for BFS (i.e. layer 0).
		cur.clear();
		for (int a : btoa) if(a != -1) A[a] = -1;
		rep(a ,0 ,sz(g)) if(A[a] == 0) cur.emplace_back(a);
		/// Find all layers using bfs.
		for (int lay = 1;; lay++) {
			bool islast = 0;
			next.clear();
			// printf("In serial cur.size() = %d\n", cur.size());
			for (int a : cur) for (int b : g[a]) {
				if (btoa[b] == -1) {
					B[b] = lay;
					islast = 1;
					// printf("Fucked at %d, %d\n", a, b);
				}
				else if (btoa[b] != a && !B[b]) {
					B[b] = lay;
					next.emplace_back(btoa[b]);
				}
			}
			if (islast) break;
			if (next.empty()) return res;
			for (int a : next) A[a] = lay;
			cur.swap(next);
		}
		/// Use DFS to scan for augmenting paths.
		rep(a,0,sz(g))
			res += dfs(a, 0, g, btoa, A, B);
	}
	return res;
}


int Parallel_hopcroftKarp(vector<vi>&g, vi& btoa, int num_edges) {
	int res = 0;
	vi A(g.size()), B(btoa.size()), cur(num_edges), next(num_edges);

	int next_size = 0, cur_size;
	for (;;) {
		fill(all(A), 0);
		fill(all(B), 0);
		/// Find the starting nodes for BFS (i.e. layer 0).
		// cur.clear();
		next_size = 0;
		cur_size = 0;
		for (int a : btoa) if(a != -1) A[a] = -1;
		rep(a ,0 ,sz(g)) if(A[a] == 0) cur[cur_size++] = a;
		/// Find all layers using bfs.
		for (int lay = 1;; lay++) {
			bool islast = 0;
			next_size = 0;
			printf("cur_size = %d\n", cur_size);
			#pragma omp parallel for schedule(dynamic, 1024)
			for (int i = 0; i < cur_size; i ++) {
				vector<int> vi;
				int a = cur[i];
				for (int b : g[a]) {
					if (btoa[b] == -1) {
						// printf("fucked at %d, %d\n", a, b);
						B[b] = lay;
						islast = 1;
					}
					else if (btoa[b] != a && !B[b]) {
						B[b] = lay;
						// int idx = __sync_fetch_and_add(&next_size, 1);
						// printf("next_size = %d\n", next_size);
						// next[idx] = btoa[b];
						vi.push_back(btoa[b]);
					}
				}
				int cur = __sync_fetch_and_add(&next_size, vi.size());
				for (int i = 0; i < vi.size(); i ++) {
					next[cur + i] = vi[i];
				}
			}
			// printf("next_size = %d\n", next_size);
			cur_size = next_size;
			if (islast) break;
			if (next_size == 0) return res;
			// if (next.empty()) return res;
			// for (int a : next) A[a] = lay;
			for (int i = 0; i < next_size; i ++) {
				A[next[i]] = lay;
			}
			cur.swap(next);
		}
		/// Use DFS to scan for augmenting paths.
		// #pragma omp parallel for
		rep(a,0,sz(g))
			res += dfs(a, 0, g, btoa, A, B);
	}
	return res;
}
int Parallel_hopcroftKarp_new(vector<vi>&g, vi& btoa, int num_edges) {
	int res = 0;
	vi A(g.size()), B(btoa.size());
	
	int max_thread_num = omp_get_max_threads();
	long long x = btoa.size();
	long long y = A.size();

	vector<vi> cur(max_thread_num), next_node(max_thread_num);
	for (;;) {
		fill(all(A), 0);
		fill(all(B), 0);
		/// Find the starting nodes for BFS (i.e. layer 0).
		for (int i = 0; i < max_thread_num; i ++) {
			cur[i].clear();
		}
		int rot = 0;
		for (int a : btoa) if(a != -1) A[a] = -1;
		rep(a ,0 ,sz(g)) if(A[a] == 0) cur[rot ++ % max_thread_num].emplace_back(a);
		/// Find all layers using bfs.
		for (int lay = 1;; lay++) {
			bool islast = 0;
			for (int i = 0; i < max_thread_num; i ++) {
				next_node[i].clear();
			}
			int tot = 0;
			#pragma omp parallel shared(cur, next_node, islast, tot, btoa)
			{
				int tid = omp_get_thread_num();
				// printf("In tid = %d, size = %d\n", tid, cur[tid].size());
				for (int a : cur[tid]) {
					for (int b : g[a]) {
						if (btoa[b] == -1) {
							B[b] = lay;
							islast = 1;
						}
						else if (btoa[b] != a && !B[b]) {
							B[b] = lay;
							next_node[tid].emplace_back(btoa[b]);
							tot = 1;
						}
					}
				}
			}
			
			if (islast) break;
			if (!tot) return res;
			// #pragma omp parallel for shared(A, next_node)
			for (int i = 0; i < max_thread_num; i ++) {
				for (int a : next_node[i]) A[a] = lay;
			}
			cur.swap(next_node);
		}
		// Use DFS to scan for augmenting paths.
		// #pragma omp parallel for reduction(+:res) schedule(dynamic, 1024)
		rep(a,0,sz(g))
			res += dfs(a, 0, g, btoa, A, B);
	}
	return res;
}
