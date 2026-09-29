// Euler trails and circuits (every edge exactly once) with Hierholzer's algorithm. Module 15 section 4.
// Undirected multigraph version: edges are tracked by id, so parallel edges and self-loops work.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:hierholzer]
// Exists iff every edge is in one connected piece and 0 or 2 nodes have odd degree (with 2, the trail
// must start at one odd node and end at the other; with 0 it is a circuit). Returns the trail as m + 1
// nodes, or {} if there is none (or no edges at all). O(V + E).
// Hierholzer: walk unused edges until stuck. A stuck node is final at that position of the trail, so
// emit it and back up; the emitted sequence, reversed, is the trail with every detour spliced in.
vector<int> euler_trail(int n, const vector<pair<int, int>>& edges) {
    int m = edges.size();
    vector<vector<pair<int, int>>> adj(n);         // (neighbour, edge id)
    vector<int> degree(n, 0);
    for (int id = 0; id < m; id++) {
        auto [a, b] = edges[id];
        adj[a].push_back({b, id});
        adj[b].push_back({a, id});
        degree[a]++, degree[b]++;
    }
    int start = -1, odd = 0;
    for (int v = 0; v < n; v++) {
        if (degree[v] % 2 == 1) odd++, start = v;             // an odd node, if any, must be an end
        else if (start == -1 && degree[v] > 0) start = v;
    }
    if (m == 0 || (odd != 0 && odd != 2)) return {};
    vector<bool> used(m, false);
    vector<size_t> next(n, 0);                     // adj[u][next[u]..] are the edges not tried yet
    vector<int> stk{start}, trail;
    while (!stk.empty()) {
        int u = stk.back();
        while (next[u] < adj[u].size() && used[adj[u][next[u]].second]) next[u]++;
        if (next[u] == adj[u].size()) {            // stuck: u is final here
            trail.push_back(u);
            stk.pop_back();
        } else {                                   // walk one more unused edge
            auto [v, id] = adj[u][next[u]];
            used[id] = true;
            stk.push_back(v);
        }
    }
    if ((int)trail.size() != m + 1) return {};     // some edges were in another component
    reverse(trail.begin(), trail.end());
    return trail;
}
// [/snippet]

// The trail uses every edge exactly once, as consecutive node pairs.
bool valid_trail(int n, const vector<pair<int, int>>& edges, const vector<int>& trail) {
    if (trail.size() != edges.size() + 1) return false;
    multiset<pair<int, int>> left;
    for (auto [a, b] : edges) left.insert({min(a, b), max(a, b)});
    for (size_t i = 0; i + 1 < trail.size(); i++) {
        int a = trail[i], b = trail[i + 1];
        if (a < 0 || a >= n || b < 0 || b >= n) return false;
        auto it = left.find({min(a, b), max(a, b)});
        if (it == left.end()) return false;
        left.erase(it);
    }
    return left.empty();
}

// Brute force: backtracking over unused edges from every possible start node.
bool extend(int u, int remaining, const vector<pair<int, int>>& edges, vector<bool>& used) {
    if (remaining == 0) return true;
    for (int id = 0; id < (int)edges.size(); id++) {
        if (used[id]) continue;
        auto [a, b] = edges[id];
        if (a != u && b != u) continue;
        used[id] = true;
        if (extend(a == u ? b : a, remaining - 1, edges, used)) return true;
        used[id] = false;
    }
    return false;
}

bool exists_brute(int n, const vector<pair<int, int>>& edges) {
    for (int s = 0; s < n; s++) {
        vector<bool> used(edges.size(), false);
        if (extend(s, edges.size(), edges, used)) return true;
    }
    return false;
}

int main() {
    // a triangle: an Euler circuit that ends where it starts
    vector<pair<int, int>> tri{{0, 1}, {1, 2}, {2, 0}};
    auto c = euler_trail(3, tri);
    CHECK(valid_trail(3, tri, c));
    CHECK_EQ(c.front(), c.back());
    // two odd nodes (0 and 3): the trail must run from one to the other
    vector<pair<int, int>> path_like{{0, 1}, {1, 2}, {2, 0}, {0, 3}};
    auto p = euler_trail(4, path_like);
    CHECK(valid_trail(4, path_like, p));
    CHECK((p.front() == 3 && p.back() == 0) || (p.front() == 0 && p.back() == 3));
    // a "bow tie": the walk from 0 gets stuck back at 0 before using the second triangle; the detour is spliced in
    vector<pair<int, int>> bow{{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 2}};
    CHECK(valid_trail(5, bow, euler_trail(5, bow)));
    // four odd nodes: impossible; two separate cycles: impossible
    CHECK(euler_trail(4, {{0, 1}, {0, 2}, {0, 3}}).empty());
    CHECK(euler_trail(6, {{0, 1}, {1, 2}, {2, 0}, {3, 4}, {4, 5}, {5, 3}}).empty());
    // parallel edges and a self-loop
    vector<pair<int, int>> multi{{0, 1}, {0, 1}, {1, 1}};
    CHECK(valid_trail(2, multi, euler_trail(2, multi)));

    int positive = 0;
    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 6);
        vector<pair<int, int>> edges;
        int m = (int)t::rand_int(1, 8);
        if (t::rand_int(0, 1)) {                   // a random walk: an Euler trail exists by construction
            int u = (int)t::rand_int(0, n - 1);
            for (int i = 0; i < m; i++) {
                int v = (int)t::rand_int(0, n - 1);
                edges.push_back({u, v});
                u = v;
            }
        } else {
            for (int i = 0; i < m; i++) edges.push_back({(int)t::rand_int(0, n - 1), (int)t::rand_int(0, n - 1)});
        }
        shuffle(edges.begin(), edges.end(), t::rng());
        auto trail = euler_trail(n, edges);
        bool exists = exists_brute(n, edges);
        positive += exists;
        CHECK_EQ(!trail.empty(), exists);
        if (!trail.empty()) CHECK(valid_trail(n, edges, trail));
    }
    CHECK(positive > 150);                         // both outcomes are well covered
    return t::summary("15 euler_trail");
}
