// Module 09 section 4: a preview of 0-1 BFS, a deque instead of a priority queue when every edge weighs
// 0 or 1. Module 15 teaches it properly.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:zero_one_bfs]
// Shortest paths when every edge weighs 0 or 1. adj[u] holds (v, w) pairs with w in {0, 1}.
// A 0-edge keeps the distance, so v goes to the FRONT; a 1-edge adds one, so v goes to the BACK.
// The deque then holds distances d..d, d+1..d+1, like the BFS frontier, and pops in order.
vector<int> zero_one_bfs(const vector<vector<pair<int, int>>>& adj, int src) {
    vector<int> dist(adj.size(), INT_MAX);
    deque<int> dq;
    dist[src] = 0;
    dq.push_back(src);
    while (!dq.empty()) {
        int u = dq.front();
        dq.pop_front();
        for (auto [v, w] : adj[u]) {
            if (dist[u] + w >= dist[v]) continue;  // no improvement
            dist[v] = dist[u] + w;                 // a node may be queued twice; the stale copy
            if (w == 0) dq.push_front(v);          // relaxes nothing when it's popped later
            else dq.push_back(v);
        }
    }
    return dist;  // INT_MAX = unreachable
}
// [/snippet]

// Brute force for 0-1 BFS: Bellman-Ford (relax every edge n times).
vector<int> brute(const vector<vector<pair<int, int>>>& adj, int src) {
    int n = (int)adj.size();
    vector<int> dist(n, INT_MAX);
    dist[src] = 0;
    for (int round = 0; round < n; round++)
        for (int u = 0; u < n; u++)
            if (dist[u] != INT_MAX)
                for (auto [v, w] : adj[u]) dist[v] = min(dist[v], dist[u] + w);
    return dist;
}

int main() {
    //   0 --1--> 1 --0--> 2
    //   |                 ^      the direct 0 -> 2 edge costs 1; 0 -> 3 -> 2 costs 0 + 0
    //   +--0--> 3 --0-----+
    vector<vector<pair<int, int>>> g{{{1, 1}, {3, 0}, {2, 1}}, {{2, 0}}, {}, {{2, 0}}, {}};
    CHECK_EQ(zero_one_bfs(g, 0), vector<int>{0, 1, 0, 0, INT_MAX});

    // stress: random directed 0/1 graphs vs Bellman-Ford
    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(1, 10);
        int edges = (int)t::rand_int(0, 3 * n);
        vector<vector<pair<int, int>>> adj(n);
        for (int e = 0; e < edges; e++)
            adj[t::rand_int(0, n - 1)].push_back({(int)t::rand_int(0, n - 1), (int)t::rand_int(0, 1)});
        int src = (int)t::rand_int(0, n - 1);
        CHECK_EQ(zero_one_bfs(adj, src), brute(adj, src));
    }
    return t::summary("zero_one_bfs");
}
