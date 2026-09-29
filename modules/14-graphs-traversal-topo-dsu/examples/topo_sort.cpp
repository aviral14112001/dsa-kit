// Topological sort variants: DFS post-order reversal, and the lexicographically smallest order (Kahn's
// algorithm with a min-heap). Module 14 section 3. Plain Kahn's algorithm is topo_sort() in graph.hpp.
#include <bits/stdc++.h>
#include "test.hpp"
#include "graph.hpp"   // topo_sort (Kahn), for cross-checks
using namespace std;

// [snippet:topo_dfs]
// A node finishes only after everything it points to has finished, so the reversed finish order is a
// topological order. Meeting a GREY node (still on the DFS path) means a cycle: return {}.
vector<int> topo_sort_dfs(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<int> colour(n, 0), order;              // 0 = white, 1 = grey (on the path), 2 = black (done)
    bool cycle = false;
    function<void(int)> dfs = [&](int u) {
        colour[u] = 1;
        for (int v : adj[u]) {
            if (colour[v] == 1) cycle = true;     // back edge
            else if (colour[v] == 0) dfs(v);
        }
        colour[u] = 2;
        order.push_back(u);                       // post-order: after all of u's descendants
    };
    for (int u = 0; u < n; u++)
        if (colour[u] == 0) dfs(u);
    if (cycle) return {};
    reverse(order.begin(), order.end());
    return order;
}
// [/snippet]

// [snippet:topo_lex]
// The lexicographically smallest topological order: Kahn's algorithm with a min-heap instead of a
// queue, so the smallest available node always goes next. O((V + E) log V). {} on a cycle.
vector<int> topo_sort_smallest(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<int> indegree(n, 0);
    for (int u = 0; u < n; u++)
        for (int v : adj[u]) indegree[v]++;
    priority_queue<int, vector<int>, greater<int>> ready;   // min-heap: priority_queue is a max-heap by default
    for (int u = 0; u < n; u++)
        if (indegree[u] == 0) ready.push(u);
    vector<int> order;
    while (!ready.empty()) {
        int u = ready.top();
        ready.pop();
        order.push_back(u);
        for (int v : adj[u])
            if (--indegree[v] == 0) ready.push(v);
    }
    if ((int)order.size() < n) return {};
    return order;
}
// [/snippet]

bool is_topo_order(const vector<vector<int>>& adj, const vector<int>& order) {
    int n = adj.size();
    if ((int)order.size() != n) return false;
    vector<int> pos(n, -1);
    for (int i = 0; i < n; i++) {
        if (pos[order[i]] != -1) return false;
        pos[order[i]] = i;
    }
    for (int u = 0; u < n; u++)
        for (int v : adj[u])
            if (pos[u] >= pos[v]) return false;
    return true;
}

// Brute force: permutations come out of next_permutation in lexicographic order, so the first valid
// one is the lexicographically smallest topological order. {} if none is valid (a cycle).
vector<int> smallest_order_brute(const vector<vector<int>>& adj) {
    vector<int> perm(adj.size());
    iota(perm.begin(), perm.end(), 0);
    do {
        if (is_topo_order(adj, perm)) return perm;
    } while (next_permutation(perm.begin(), perm.end()));
    return {};
}

int main() {
    //  5 -> 2 -> 3 -> 1,  5 -> 0 <- 4 -> 1   (a classic textbook DAG)
    vector<vector<int>> dag{{}, {}, {3}, {1}, {0, 1}, {2, 0}};
    auto by_dfs = topo_sort_dfs(dag);
    CHECK(is_topo_order(dag, by_dfs));
    CHECK_EQ(by_dfs, vector<int>{5, 4, 2, 3, 1, 0});
    CHECK_EQ(topo_sort_smallest(dag), vector<int>{4, 5, 0, 2, 3, 1});
    CHECK_EQ(topo_sort(dag), vector<int>{4, 5, 2, 0, 3, 1});       // FIFO Kahn: another valid order
    CHECK_EQ(topo_sort_dfs({{1}, {2}, {0}}), vector<int>{});       // cycle
    CHECK_EQ(topo_sort_smallest({{1}, {2}, {0}}), vector<int>{});
    CHECK_EQ(topo_sort_dfs({{0}}), vector<int>{});                 // self-loop
    // FIFO Kahn is not always the smallest: 2 is ready first, but 0 becomes ready later and is smaller
    vector<vector<int>> tricky{{}, {0}, {}};   // 1 -> 0, and 2 alone
    CHECK_EQ(topo_sort(tricky), vector<int>{1, 2, 0});
    CHECK_EQ(topo_sort_smallest(tricky), vector<int>{1, 0, 2});

    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 7);
        vector<vector<int>> adj(n);
        int m = (int)t::rand_int(0, 12);
        bool make_dag = t::rand_int(0, 2) != 0;
        vector<int> rank(n);
        iota(rank.begin(), rank.end(), 0);
        shuffle(rank.begin(), rank.end(), t::rng());
        for (int i = 0; i < m; i++) {
            int u = (int)t::rand_int(0, n - 1), v = (int)t::rand_int(0, n - 1);
            if (make_dag && rank[u] >= rank[v]) continue;
            adj[u].push_back(v);
        }
        auto kahn = topo_sort(adj), dfs_order = topo_sort_dfs(adj), smallest = topo_sort_smallest(adj);
        CHECK_EQ(dfs_order.empty(), kahn.empty());                  // same cycle verdict
        if (!kahn.empty()) CHECK(is_topo_order(adj, dfs_order));
        CHECK_EQ(smallest, smallest_order_brute(adj));
    }
    return t::summary("14 topo_sort");
}
