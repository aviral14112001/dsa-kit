// 684. Redundant Connection: https://leetcode.com/problems/redundant-connection/
// Pattern: DSU cycle detection, the first edge whose endpoints are already connected. Module 14 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
    vector<int> parent, sz;
    int find(int x) { return parent[x] == x ? x : parent[x] = find(parent[x]); }
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) swap(a, b);
        parent[b] = a;
        sz[a] += sz[b];
        return true;
    }
public:
    vector<int> findRedundantConnection(vector<vector<int>>& edges) {
        int n = edges.size();                    // a tree on n nodes has n-1 edges; we got one extra
        parent.resize(n + 1);                    // nodes are 1..n
        iota(parent.begin(), parent.end(), 0);
        sz.assign(n + 1, 1);
        for (auto& e : edges)
            if (!unite(e[0], e[1])) return e;    // already connected: this edge closes the cycle
        return {};                               // unreachable for valid input
    }
};
// [/snippet]

// Brute force: try removing each edge from the last one backwards; the first removal that leaves
// a tree (n-1 edges that connect all n nodes) is the answer.
vector<int> brute(const vector<vector<int>>& edges) {
    int n = edges.size();
    for (int skip = n - 1; skip >= 0; skip--) {
        vector<vector<int>> adj(n + 1);
        for (int i = 0; i < n; i++)
            if (i != skip) {
                adj[edges[i][0]].push_back(edges[i][1]);
                adj[edges[i][1]].push_back(edges[i][0]);
            }
        vector<bool> seen(n + 1, false);
        vector<int> stk{1};
        seen[1] = true;
        int reached = 1;
        while (!stk.empty()) {
            int u = stk.back();
            stk.pop_back();
            for (int v : adj[u])
                if (!seen[v]) seen[v] = true, reached++, stk.push_back(v);
        }
        if (reached == n) return edges[skip];    // n-1 edges reaching all n nodes: a tree
    }
    return {};
}

// A random tree on 1..n, plus one extra edge between two nodes that aren't already adjacent,
// in random order with random endpoint order: exactly the problem's input shape.
vector<vector<int>> random_input(int n) {
    vector<int> label(n);
    iota(label.begin(), label.end(), 1);
    shuffle(label.begin(), label.end(), t::rng());
    set<pair<int, int>> present;
    vector<vector<int>> edges;
    auto add = [&](int a, int b) {
        present.insert({min(a, b), max(a, b)});
        if (t::rand_int(0, 1)) swap(a, b);
        edges.push_back({a, b});
    };
    for (int i = 1; i < n; i++) add(label[i], label[(int)t::rand_int(0, i - 1)]);
    while (true) {
        int a = (int)t::rand_int(1, n), b = (int)t::rand_int(1, n);
        if (a != b && !present.count({min(a, b), max(a, b)})) {
            add(a, b);
            break;
        }
    }
    shuffle(edges.begin(), edges.end(), t::rng());
    return edges;
}

int main() {
    Solution sol;
    vector<vector<int>> ex1{{1, 2}, {1, 3}, {2, 3}};
    CHECK_EQ(sol.findRedundantConnection(ex1), vector<int>{2, 3});
    vector<vector<int>> ex2{{1, 2}, {2, 3}, {3, 4}, {1, 4}, {1, 5}};
    CHECK_EQ(sol.findRedundantConnection(ex2), vector<int>{1, 4});
    vector<vector<int>> late{{1, 2}, {2, 3}, {3, 1}, {3, 4}};   // the cycle closes at edge 3, not the last edge
    CHECK_EQ(sol.findRedundantConnection(late), vector<int>{3, 1});

    for (int iter = 0; iter < 300; iter++) {
        auto edges = random_input((int)t::rand_int(3, 12));
        CHECK_EQ(Solution().findRedundantConnection(edges), brute(edges));
    }
    return t::summary("0684-redundant-connection");
}
