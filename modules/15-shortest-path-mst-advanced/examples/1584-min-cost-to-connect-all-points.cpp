// 1584. Min Cost to Connect All Points: https://leetcode.com/problems/min-cost-to-connect-all-points/
// Pattern: MST on a complete graph: O(n^2) Prim with no heap and no edge list. Module 15 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int minCostConnectPoints(vector<vector<int>>& points) {
        int n = points.size();
        vector<int> best(n, INT_MAX);        // best[v] = cheapest edge from the tree to point v so far
        vector<bool> in_tree(n, false);
        best[0] = 0;                         // start the tree at point 0
        int total = 0;
        for (int iter = 0; iter < n; iter++) {
            int u = -1;
            for (int v = 0; v < n; v++)      // the closest point not yet in the tree: O(n) scan, no heap
                if (!in_tree[v] && (u == -1 || best[v] < best[u])) u = v;
            in_tree[u] = true;
            total += best[u];
            for (int v = 0; v < n; v++) {    // edges from u are computed on the fly, never stored
                if (in_tree[v]) continue;
                int d = abs(points[u][0] - points[v][0]) + abs(points[u][1] - points[v][1]);
                best[v] = min(best[v], d);
            }
        }
        return total;
    }
};
// [/snippet]

// [snippet:kruskal]
// Kruskal on all n(n-1)/2 edges: correct too, but O(n^2 log n) time and O(n^2) memory for the edge list.
class SolutionKruskal {
    vector<int> parent;
    int find(int x) { return parent[x] == x ? x : parent[x] = find(parent[x]); }
public:
    int minCostConnectPoints(vector<vector<int>>& points) {
        int n = points.size();
        vector<array<int, 3>> edges;                   // (weight, i, j)
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++)
                edges.push_back({abs(points[i][0] - points[j][0]) + abs(points[i][1] - points[j][1]), i, j});
        sort(edges.begin(), edges.end());
        parent.resize(n);
        iota(parent.begin(), parent.end(), 0);
        int total = 0, used = 0;
        for (auto [w, i, j] : edges) {
            int a = find(i), b = find(j);
            if (a == b) continue;                      // would close a cycle
            parent[a] = b;                             // (no union by size: path compression alone is O(log n) amortized)
            total += w;
            if (++used == n - 1) break;                // a spanning tree has exactly n - 1 edges
        }
        return total;
    }
};
// [/snippet]

// Brute force: try every set of n-1 edges, keep the cheapest one that forms a spanning tree.
int brute(const vector<vector<int>>& pts) {
    int n = pts.size();
    if (n == 1) return 0;
    vector<array<int, 3>> edges;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            edges.push_back({abs(pts[i][0] - pts[j][0]) + abs(pts[i][1] - pts[j][1]), i, j});
    int m = edges.size(), best = INT_MAX;
    for (int mask = 0; mask < (1 << m); mask++) {
        if (__builtin_popcount(mask) != n - 1) continue;
        vector<int> comp(n);
        iota(comp.begin(), comp.end(), 0);
        int w = 0;
        bool tree = true;
        for (int e = 0; e < m && tree; e++) {
            if (!(mask >> e & 1)) continue;
            auto [cost, a, b] = edges[e];
            int ca = comp[a], cb = comp[b];
            if (ca == cb) tree = false;                // this subset has a cycle
            for (int& c : comp)
                if (c == cb) c = ca;
            w += cost;
        }
        if (tree) best = min(best, w);
    }
    return best;
}

int main() {
    Solution prim;
    SolutionKruskal kruskal;
    vector<vector<int>> ex1{{0, 0}, {2, 2}, {3, 10}, {5, 2}, {7, 0}};
    CHECK_EQ(prim.minCostConnectPoints(ex1), 20);
    CHECK_EQ(kruskal.minCostConnectPoints(ex1), 20);
    vector<vector<int>> ex2{{3, 12}, {-2, 5}, {-4, 1}};
    CHECK_EQ(prim.minCostConnectPoints(ex2), 18);
    vector<vector<int>> one{{0, 0}};
    CHECK_EQ(prim.minCostConnectPoints(one), 0);
    CHECK_EQ(kruskal.minCostConnectPoints(one), 0);
    vector<vector<int>> far{{-1000000, -1000000}, {1000000, 1000000}};
    CHECK_EQ(prim.minCostConnectPoints(far), 4000000);   // the largest single distance

    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(1, 6);
        vector<vector<int>> pts;
        for (int i = 0; i < n; i++) pts.push_back({(int)t::rand_int(-10, 10), (int)t::rand_int(-10, 10)});
        int expect = brute(pts);
        CHECK_EQ(prim.minCostConnectPoints(pts), expect);
        CHECK_EQ(kruskal.minCostConnectPoints(pts), expect);
    }
    // n = 1000 random points in the full coordinate range: the two methods agree
    vector<vector<int>> many;
    for (int i = 0; i < 1000; i++) many.push_back({(int)t::rand_int(-1000000, 1000000), (int)t::rand_int(-1000000, 1000000)});
    CHECK_EQ(prim.minCostConnectPoints(many), kruskal.minCostConnectPoints(many));
    return t::summary("1584-min-cost-to-connect-all-points");
}
