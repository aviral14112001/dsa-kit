// 1192. Critical Connections in a Network: https://leetcode.com/problems/critical-connections-in-a-network/
// Pattern: bridges via Tarjan's low-link values. Module 15 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
    vector<vector<int>> adj;
    vector<int> tin, low;                     // entry time; lowest entry time reachable from the subtree
    vector<vector<int>> bridges;
    int timer = 0;

    void dfs(int u, int parent) {
        tin[u] = low[u] = timer++;
        for (int v : adj[u]) {
            if (v == parent) continue;        // safe here: the problem has no repeated connections
            if (tin[v] != -1) {               // visited: a back edge (harmless if v is below u)
                low[u] = min(low[u], tin[v]);
            } else {                          // tree edge: explore, then pull up the child's low
                dfs(v, u);
                low[u] = min(low[u], low[v]);
                if (low[v] > tin[u]) bridges.push_back({u, v});   // v's subtree can't get around (u, v)
            }
        }
    }
public:
    vector<vector<int>> criticalConnections(int n, vector<vector<int>>& connections) {
        adj.assign(n, {});
        for (auto& c : connections) {
            adj[c[0]].push_back(c[1]);
            adj[c[1]].push_back(c[0]);
        }
        tin.assign(n, -1);
        low.assign(n, 0);
        bridges.clear();
        timer = 0;
        dfs(0, -1);                           // the network is connected, so one DFS reaches everyone
        return bridges;
    }
};
// [/snippet]

// Brute force: an edge is critical iff removing it disconnects the (connected) network.
vector<vector<int>> brute(int n, const vector<vector<int>>& connections) {
    vector<vector<int>> out;
    for (int skip = 0; skip < (int)connections.size(); skip++) {
        vector<int> comp(n);
        iota(comp.begin(), comp.end(), 0);
        for (bool changed = true; changed;) {           // label propagation, no DSU/DFS needed
            changed = false;
            for (int i = 0; i < (int)connections.size(); i++) {
                if (i == skip) continue;
                int a = connections[i][0], b = connections[i][1];
                int m = min(comp[a], comp[b]);
                if (comp[a] != m || comp[b] != m) comp[a] = comp[b] = m, changed = true;
            }
        }
        if (set<int>(comp.begin(), comp.end()).size() > 1) out.push_back(connections[skip]);
    }
    return out;
}

// Bridges come back in any order and orientation: normalize before comparing.
vector<vector<int>> normalized(vector<vector<int>> edges) {
    for (auto& e : edges)
        if (e[0] > e[1]) swap(e[0], e[1]);
    sort(edges.begin(), edges.end());
    return edges;
}

int main() {
    Solution sol;
    vector<vector<int>> ex1{{0, 1}, {1, 2}, {2, 0}, {1, 3}};
    CHECK_EQ(normalized(sol.criticalConnections(4, ex1)), vector<vector<int>>{{1, 3}});
    vector<vector<int>> ex2{{0, 1}};
    CHECK_EQ(normalized(sol.criticalConnections(2, ex2)), vector<vector<int>>{{0, 1}});
    vector<vector<int>> cycle{{0, 1}, {1, 2}, {2, 3}, {3, 0}};
    CHECK_EQ(sol.criticalConnections(4, cycle), vector<vector<int>>{});   // a cycle has no bridges
    vector<vector<int>> two_triangles{{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 5}, {5, 3}};
    CHECK_EQ(normalized(sol.criticalConnections(6, two_triangles)), vector<vector<int>>{{2, 3}});

    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(2, 9);
        // a random spanning tree keeps the network connected; extra edges create cycles
        vector<int> label(n);
        iota(label.begin(), label.end(), 0);
        shuffle(label.begin(), label.end(), t::rng());
        set<pair<int, int>> used;
        vector<vector<int>> conn;
        auto add = [&](int a, int b) {
            if (a == b || used.count({min(a, b), max(a, b)})) return;
            used.insert({min(a, b), max(a, b)});
            conn.push_back({a, b});
        };
        for (int i = 1; i < n; i++) add(label[i], label[(int)t::rand_int(0, i - 1)]);
        int extra = (int)t::rand_int(0, n);
        for (int i = 0; i < extra; i++) add((int)t::rand_int(0, n - 1), (int)t::rand_int(0, n - 1));
        shuffle(conn.begin(), conn.end(), t::rng());
        CHECK_EQ(normalized(sol.criticalConnections(n, conn)), normalized(brute(n, conn)));
    }
    // a path of 10^4 nodes: every edge is a bridge, and the recursion goes 10^4 deep. (A 10^5 path, the
    // problem's limit, overflows this Mac's default 8 MB stack even at -O2; see the notes on recursion depth.)
    int n = 10000;
    vector<vector<int>> path;
    for (int i = 0; i + 1 < n; i++) path.push_back({i, i + 1});
    CHECK_EQ((int)sol.criticalConnections(n, path).size(), n - 1);
    return t::summary("1192-critical-connections-in-a-network");
}
