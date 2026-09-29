// Strongly connected components with Kosaraju's two passes, and the condensation DAG. Module 15 section 4.
// Tarjan's one-pass version is scc_tarjan() in templates/graph.hpp; the tests check they agree.
#include <bits/stdc++.h>
#include "test.hpp"
#include "graph.hpp"   // SCC, scc_tarjan, topo_sort, transitive_closure
using namespace std;

// [snippet:kosaraju]
// Pass 1: DFS on G and list nodes by finish time. Pass 2: DFS on the reversed graph, starting from
// the latest-finishing unassigned node; everything it reaches is one SCC. O(V + E).
// Ids come out in topological order of the condensation: every edge u->v has comp[u] <= comp[v].
SCC scc_kosaraju(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<vector<int>> radj(n);                    // G with every edge reversed
    for (int u = 0; u < n; u++)
        for (int v : adj[u]) radj[v].push_back(u);

    vector<bool> seen(n, false);
    vector<int> finish_order;
    function<void(int)> dfs1 = [&](int u) {
        seen[u] = true;
        for (int v : adj[u])
            if (!seen[v]) dfs1(v);
        finish_order.push_back(u);                  // u finishes after everything it reaches
    };
    for (int u = 0; u < n; u++)
        if (!seen[u]) dfs1(u);

    SCC res;
    res.comp.assign(n, -1);
    function<void(int)> dfs2 = [&](int u) {
        res.comp[u] = res.count;
        for (int v : radj[u])
            if (res.comp[v] == -1) dfs2(v);         // in reversed G, only u's own SCC is still unassigned
    };
    for (int i = n - 1; i >= 0; i--) {              // latest finish first: a source SCC of G
        int u = finish_order[i];
        if (res.comp[u] == -1) {
            dfs2(u);
            res.count++;
        }
    }
    return res;
}
// [/snippet]

// [snippet:condensation]
// Condensation: one node per SCC and an edge wherever an original edge crosses two SCCs.
// It is always a DAG (a cycle of SCCs would be one bigger SCC), so topo sort and DAG DP apply.
vector<vector<int>> condense(const vector<vector<int>>& adj, const SCC& scc) {
    vector<vector<int>> dag(scc.count);
    for (int u = 0; u < (int)adj.size(); u++)
        for (int v : adj[u])
            if (scc.comp[u] != scc.comp[v]) dag[scc.comp[u]].push_back(scc.comp[v]);
    for (auto& out : dag) {                         // drop repeated edges between the same two SCCs
        sort(out.begin(), out.end());
        out.erase(unique(out.begin(), out.end()), out.end());
    }
    return dag;
}
// [/snippet]

bool same_partition(const vector<int>& a, const vector<int>& b) {
    for (size_t i = 0; i < a.size(); i++)
        for (size_t j = 0; j < a.size(); j++)
            if ((a[i] == a[j]) != (b[i] == b[j])) return false;
    return true;
}

int main() {
    // {0,1,2} -> {3,4} -> {5},  6 alone
    vector<vector<int>> adj{{1}, {2}, {0, 3}, {4}, {3, 5}, {}, {}};
    auto k = scc_kosaraju(adj);
    CHECK_EQ(k.count, 4);
    CHECK(k.comp[0] == k.comp[1] && k.comp[1] == k.comp[2]);
    CHECK_EQ(k.comp[3], k.comp[4]);
    CHECK(k.comp[2] < k.comp[3] && k.comp[4] < k.comp[5]);   // topological ids
    auto dag = condense(adj, k);
    CHECK_EQ((int)topo_sort(dag).size(), k.count);           // the condensation is acyclic
    int edges = 0;
    for (auto& out : dag) edges += out.size();
    CHECK_EQ(edges, 2);                                       // {0,1,2}->{3,4} and {3,4}->{5}

    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 9);
        vector<vector<int>> g(n);
        int m = (int)t::rand_int(0, 18);
        for (int i = 0; i < m; i++) g[t::rand_int(0, n - 1)].push_back((int)t::rand_int(0, n - 1));
        auto kos = scc_kosaraju(g);
        auto tar = scc_tarjan(g);
        CHECK_EQ(kos.count, tar.count);
        CHECK(same_partition(kos.comp, tar.comp));
        auto reach = transitive_closure(g);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) CHECK_EQ(kos.comp[i] == kos.comp[j], reach[i][j] && reach[j][i]);
        for (int u = 0; u < n; u++)
            for (int v : g[u]) CHECK(kos.comp[u] <= kos.comp[v]);
        auto cd = condense(g, kos);
        CHECK_EQ((int)topo_sort(cd).size(), kos.count);
    }
    return t::summary("15 scc_kosaraju");
}
