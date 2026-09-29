// Module 09 section 3: the queue as a BFS frontier. Module 14 covers BFS in depth; this file shows why a
// FIFO queue yields shortest paths in an unweighted graph.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:bfs]
// Fewest edges from src to every node (-1 = unreachable). adj[u] lists u's neighbours.
// Invariant: the queue holds nodes at distance d, then nodes at distance d + 1, never anything
// else, so nodes leave in order of distance and the first time you reach a node is the shortest.
vector<int> bfs(const vector<vector<int>>& adj, int src) {
    vector<int> dist(adj.size(), -1);
    queue<int> frontier;
    dist[src] = 0;                  // mark a node when you PUSH it, so it's queued only once
    frontier.push(src);
    while (!frontier.empty()) {
        int u = frontier.front();
        frontier.pop();
        for (int v : adj[u]) {
            if (dist[v] != -1) continue;
            dist[v] = dist[u] + 1;
            frontier.push(v);
        }
    }
    return dist;
}
// [/snippet]

// Brute force: Bellman-Ford with unit weights (relax every edge n times). O(n * edges).
vector<int> brute(const vector<vector<int>>& adj, int src) {
    int n = (int)adj.size();
    vector<int> dist(n, INT_MAX);
    dist[src] = 0;
    for (int round = 0; round < n; round++)
        for (int u = 0; u < n; u++)
            if (dist[u] != INT_MAX)
                for (int v : adj[u]) dist[v] = min(dist[v], dist[u] + 1);
    for (int& d : dist)
        if (d == INT_MAX) d = -1;
    return dist;
}

int main() {
    //  0 - 1 - 2      edges are undirected; 5 is isolated
    //  |       |
    //  3 ----- 4      5
    vector<vector<int>> adj{{1, 3}, {0, 2}, {1, 4}, {0, 4}, {2, 3}, {}};
    CHECK_EQ(bfs(adj, 0), vector<int>{0, 1, 2, 1, 2, -1});
    CHECK_EQ(bfs(adj, 5), vector<int>{-1, -1, -1, -1, -1, 0});

    // stress: random directed graphs vs Bellman-Ford
    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(1, 12);
        int edges = (int)t::rand_int(0, 3 * n);
        vector<vector<int>> g(n);
        for (int e = 0; e < edges; e++) g[t::rand_int(0, n - 1)].push_back((int)t::rand_int(0, n - 1));
        int src = (int)t::rand_int(0, n - 1);
        CHECK_EQ(bfs(g, src), brute(g, src));
    }
    return t::summary("bfs_frontier");
}
