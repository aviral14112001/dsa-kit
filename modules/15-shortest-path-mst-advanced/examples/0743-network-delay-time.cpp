// 743. Network Delay Time: https://leetcode.com/problems/network-delay-time/
// Pattern: single-source shortest paths with non-negative weights (Dijkstra, lazy deletion). Module 15 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        vector<vector<pair<int, int>>> adj(n + 1);          // nodes are 1..n
        for (auto& e : times) adj[e[0]].push_back({e[1], e[2]});
        vector<int> dist(n + 1, INT_MAX);
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;   // (time, node), min first
        dist[k] = 0;
        pq.push({0, k});
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d > dist[u]) continue;                      // stale: u was settled earlier, faster
            for (auto [v, w] : adj[u]) {
                if (d + w < dist[v]) {
                    dist[v] = d + w;
                    pq.push({dist[v], v});
                }
            }
        }
        int slowest = *max_element(dist.begin() + 1, dist.end());
        return slowest == INT_MAX ? -1 : slowest;           // some node never heard the signal
    }
};
// [/snippet]

// Brute force: Floyd-Warshall on the small random graphs, obviously correct and O(n^3).
int brute(const vector<vector<int>>& times, int n, int k) {
    const long long INF = LLONG_MAX / 4;
    vector<vector<long long>> d(n + 1, vector<long long>(n + 1, INF));
    for (int i = 1; i <= n; i++) d[i][i] = 0;
    for (auto& e : times) d[e[0]][e[1]] = min(d[e[0]][e[1]], (long long)e[2]);
    for (int m = 1; m <= n; m++)
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= n; j++) d[i][j] = min(d[i][j], d[i][m] + d[m][j]);
    long long slowest = 0;
    for (int v = 1; v <= n; v++) slowest = max(slowest, d[k][v]);
    return slowest >= INF ? -1 : (int)slowest;
}

int main() {
    Solution sol;
    vector<vector<int>> ex1{{2, 1, 1}, {2, 3, 1}, {3, 4, 1}};
    CHECK_EQ(sol.networkDelayTime(ex1, 4, 2), 2);
    vector<vector<int>> ex2{{1, 2, 1}};
    CHECK_EQ(sol.networkDelayTime(ex2, 2, 1), 1);
    CHECK_EQ(sol.networkDelayTime(ex2, 2, 2), -1);          // edges are one-way
    vector<vector<int>> none;
    CHECK_EQ(sol.networkDelayTime(none, 1, 1), 0);          // a single node hears itself at time 0
    vector<vector<int>> detour{{1, 2, 10}, {1, 3, 1}, {3, 2, 1}};
    CHECK_EQ(sol.networkDelayTime(detour, 3, 1), 2);        // two cheap hops beat one expensive edge

    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 8), k = (int)t::rand_int(1, n);
        vector<vector<int>> times;
        set<pair<int, int>> used;                            // the problem promises no repeated (u, v) pairs
        int m = (int)t::rand_int(0, 20);
        for (int i = 0; i < m; i++) {
            int u = (int)t::rand_int(1, n), v = (int)t::rand_int(1, n);
            if (u == v || used.count({u, v})) continue;
            used.insert({u, v});
            times.push_back({u, v, (int)t::rand_int(0, 100)});
        }
        CHECK_EQ(sol.networkDelayTime(times, n, k), brute(times, n, k));
    }
    return t::summary("0743-network-delay-time");
}
