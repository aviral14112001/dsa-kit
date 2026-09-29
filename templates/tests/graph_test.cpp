#include <bits/stdc++.h>
#include "test.hpp"
#include "graph.hpp"
using namespace std;

// ---------------------------------------------------------------- helpers

// Random directed edge list: parallel edges and self-loops allowed unless turned off.
vector<Edge> random_edges(int n, int m, ll lo, ll hi, bool self_loops = true) {
    vector<Edge> es;
    while ((int)es.size() < m) {
        int u = (int)t::rand_int(0, n - 1), v = (int)t::rand_int(0, n - 1);
        if (u == v && !self_loops) {
            if (n == 1) break;
            continue;
        }
        es.push_back({u, v, t::rand_int(lo, hi)});
    }
    return es;
}

WGraph to_wgraph(int n, const vector<Edge>& es, bool undirected = false) {
    WGraph adj(n);
    for (auto [u, v, w] : es) {
        adj[u].push_back({v, w});
        if (undirected) adj[v].push_back({u, w});
    }
    return adj;
}

vector<vector<int>> to_adj(int n, const vector<Edge>& es) {
    vector<vector<int>> adj(n);
    for (auto [u, v, w] : es) adj[u].push_back(v);
    return adj;
}

// Floyd input: min over parallel edges, 0 on the diagonal (a negative self-loop goes below 0).
vector<vector<ll>> to_matrix(int n, const vector<Edge>& es, bool undirected = false) {
    vector<vector<ll>> d(n, vector<ll>(n, INF));
    for (int i = 0; i < n; i++) d[i][i] = 0;
    for (auto [u, v, w] : es) {
        d[u][v] = min(d[u][v], w);
        if (undirected) d[v][u] = min(d[v][u], w);
    }
    return d;
}

// Brute-force reachability: a plain DFS from every node (independent of Floyd).
vector<vector<bool>> reach_brute(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<vector<bool>> reach(n, vector<bool>(n, false));
    for (int s = 0; s < n; s++) {
        vector<int> stk{s};
        reach[s][s] = true;
        while (!stk.empty()) {
            int u = stk.back();
            stk.pop_back();
            for (int v : adj[u])
                if (!reach[s][v]) {
                    reach[s][v] = true;
                    stk.push_back(v);
                }
        }
    }
    return reach;
}

// A valid topological order: a permutation of 0..n-1 with every edge pointing forward.
bool is_topo_order(const vector<vector<int>>& adj, const vector<int>& order) {
    int n = adj.size();
    if ((int)order.size() != n) return false;
    vector<int> pos(n, -1);
    for (int i = 0; i < n; i++) {
        if (order[i] < 0 || order[i] >= n || pos[order[i]] != -1) return false;
        pos[order[i]] = i;
    }
    for (int u = 0; u < n; u++)
        for (int v : adj[u])
            if (pos[u] >= pos[v]) return false;
    return true;
}

bool has_cycle_brute(const vector<vector<int>>& adj) {
    auto reach = reach_brute(adj);
    for (int u = 0; u < (int)adj.size(); u++)
        for (int v : adj[u])
            if (reach[v][u]) return true;  // edge u->v plus a path v ~> u
    return false;
}

// Components of an undirected graph, ignoring one edge id and/or one vertex (-1 = none).
int components_without(int n, const vector<pair<int, int>>& edges, int skip_edge, int skip_vertex) {
    DSU dsu(n);
    for (int id = 0; id < (int)edges.size(); id++) {
        auto [a, b] = edges[id];
        if (id == skip_edge || a == skip_vertex || b == skip_vertex) continue;
        dsu.unite(a, b);
    }
    return dsu.components - (skip_vertex != -1 ? 1 : 0);  // the removed vertex is not a component
}

// A negative cycle as returned by Bellman-Ford: distinct nodes, consecutive edges exist, total < 0.
bool valid_negative_cycle(const vector<Edge>& es, const vector<int>& cycle, int n) {
    if (cycle.empty()) return false;
    set<int> distinct(cycle.begin(), cycle.end());
    if (distinct.size() != cycle.size()) return false;
    vector<vector<ll>> best(n, vector<ll>(n, INF));
    for (auto [u, v, w] : es) best[u][v] = min(best[u][v], w);
    ll total = 0;
    for (size_t i = 0; i < cycle.size(); i++) {
        int a = cycle[i], b = cycle[(i + 1) % cycle.size()];
        if (best[a][b] == INF) return false;
        total += best[a][b];
    }
    return total < 0;
}

// Every consecutive pair on the path is an edge that is "tight": dist[a] + w == dist[b].
bool valid_shortest_path(const vector<Edge>& es, const ShortestPaths& sp, int src, int target) {
    vector<int> path = path_to(sp, target);
    if (sp.dist[target] == INF) return path.empty();
    if (path.empty() || path.front() != src || path.back() != target) return false;
    for (size_t i = 0; i + 1 < path.size(); i++) {
        int a = path[i], b = path[i + 1];
        bool tight = false;
        for (auto [u, v, w] : es)
            if (u == a && v == b && sp.dist[a] + w == sp.dist[b]) tight = true;
        if (!tight) return false;
    }
    return true;
}

// Brute-force MST: try every subset of n-1 edges; nullopt if no spanning tree exists.
optional<ll> mst_brute(int n, const vector<Edge>& es) {
    int m = es.size();
    optional<ll> best;
    if (n <= 1) return 0;
    for (int mask = 0; mask < (1 << m); mask++) {
        if (__builtin_popcount(mask) != n - 1) continue;
        DSU dsu(n);
        ll w = 0;
        bool acyclic = true;
        for (int i = 0; i < m; i++)
            if (mask >> i & 1) {
                acyclic &= dsu.unite(es[i].u, es[i].v);
                w += es[i].w;
            }
        if (acyclic && (!best || w < *best)) best = w;
    }
    return best;
}

// Brute-force min cut: the cheapest S with s in S, t not in S; cut = sum of capacities leaving S.
ll min_cut_brute(int n, const vector<Edge>& es, int s, int t) {
    ll best = INF;
    for (int mask = 0; mask < (1 << n); mask++) {
        if (!(mask >> s & 1) || (mask >> t & 1)) continue;
        ll cut = 0;
        for (auto [u, v, c] : es)
            if ((mask >> u & 1) && !(mask >> v & 1)) cut += c;
        best = min(best, cut);
    }
    return best;
}

int matching_brute(const vector<pair<int, int>>& pairs) {
    int m = pairs.size(), best = 0;
    for (int mask = 0; mask < (1 << m); mask++) {
        set<int> ls, rs;
        bool ok = true;
        for (int i = 0; i < m && ok; i++)
            if (mask >> i & 1) ok = ls.insert(pairs[i].first).second && rs.insert(pairs[i].second).second;
        if (ok) best = max(best, __builtin_popcount(mask));
    }
    return best;
}

// ---------------------------------------------------------------- tests

void test_bfs() {
    //  0 - 1 - 3 - 4     5 (isolated)
    //   \     /
    //    2 ---
    vector<vector<int>> adj{{1, 2}, {0, 3}, {0, 3}, {1, 2, 4}, {3}, {}};
    CHECK_EQ(bfs_dist(adj, {0}), vector<int>{0, 1, 1, 2, 3, -1});
    CHECK_EQ(bfs_dist(adj, {0, 4}), vector<int>{0, 1, 1, 1, 0, -1});  // multi-source
    CHECK_EQ(bfs_dist(adj, {0, 0}), vector<int>{0, 1, 1, 2, 3, -1});  // duplicate source is harmless
    CHECK_EQ(bfs_dist(adj, {}), vector<int>(6, -1));

    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(1, 10);
        auto es = random_edges(n, (int)t::rand_int(0, 25), 1, 1);
        auto adj2 = to_adj(n, es);
        auto wg = to_wgraph(n, es);
        int src = (int)t::rand_int(0, n - 1);
        auto bfs = bfs_dist(adj2, {src});
        auto dj = dijkstra(wg, src).dist;
        for (int v = 0; v < n; v++) CHECK_EQ(bfs[v], dj[v] == INF ? -1 : dj[v]);
        // multi-source = the minimum over single-source runs
        vector<int> sources{src, (int)t::rand_int(0, n - 1)};
        auto multi = bfs_dist(adj2, sources);
        auto other = bfs_dist(adj2, {sources[1]});
        for (int v = 0; v < n; v++) {
            int expect = bfs[v] == -1 ? other[v] : (other[v] == -1 ? bfs[v] : min(bfs[v], other[v]));
            CHECK_EQ(multi[v], expect);
        }
    }
}

void test_topo() {
    vector<vector<int>> dag{{1, 2}, {3}, {3}, {4}, {}, {}};  // 5 has no edges but must appear
    auto order = topo_sort(dag);
    CHECK(is_topo_order(dag, order));
    CHECK_EQ(order, vector<int>{0, 5, 1, 2, 3, 4});  // FIFO queue order
    CHECK_EQ(topo_sort({{1}, {2}, {0}, {}}), vector<int>{});  // 0 -> 1 -> 2 -> 0
    CHECK_EQ(topo_sort({{0}}), vector<int>{});                // a self-loop is a cycle
    CHECK_EQ(topo_sort({}), vector<int>{});                   // n = 0: empty order, size == n
    CHECK_EQ(topo_sort({{}, {}}), vector<int>{0, 1});

    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 9);
        vector<vector<int>> adj(n);
        int m = (int)t::rand_int(0, 16);
        bool make_dag = t::rand_int(0, 1);
        vector<int> rank(n);
        iota(rank.begin(), rank.end(), 0);
        shuffle(rank.begin(), rank.end(), t::rng());
        for (int i = 0; i < m; i++) {
            int u = (int)t::rand_int(0, n - 1), v = (int)t::rand_int(0, n - 1);
            if (make_dag && rank[u] >= rank[v]) continue;  // edges only go "up" the hidden order
            adj[u].push_back(v);
        }
        auto got = topo_sort(adj);
        bool cyclic = has_cycle_brute(adj);
        if (make_dag) CHECK(!cyclic);
        CHECK_EQ(got.empty(), cyclic);
        if (!cyclic) CHECK(is_topo_order(adj, got));
    }
}

void test_dijkstra() {
    vector<Edge> es{{0, 1, 4}, {0, 2, 1}, {2, 1, 2}, {1, 3, 1}, {2, 3, 5}, {3, 4, 3}};
    auto sp = dijkstra(to_wgraph(6, es), 0);
    CHECK_EQ(sp.dist, vector<ll>{0, 3, 1, 4, 7, INF});
    CHECK_EQ(path_to(sp, 4), vector<int>{0, 2, 1, 3, 4});
    CHECK_EQ(path_to(sp, 0), vector<int>{0});
    CHECK_EQ(path_to(sp, 5), vector<int>{});  // unreachable
    CHECK(sp.negative_cycle.empty());

    // distances beyond 32 bits
    vector<Edge> big{{0, 1, 1'000'000'000'000LL}, {1, 2, 1'000'000'000'000LL}, {2, 3, 1'000'000'000'000LL}};
    CHECK_EQ(dijkstra(to_wgraph(4, big), 0).dist[3], 3'000'000'000'000LL);

    // cross-check on random non-negative graphs: Dijkstra == Bellman-Ford == Floyd, and paths are tight
    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 9);
        auto rnd = random_edges(n, (int)t::rand_int(0, 25), 0, 20);
        int src = (int)t::rand_int(0, n - 1);
        auto dj = dijkstra(to_wgraph(n, rnd), src);
        auto bf = bellman_ford(n, rnd, src);
        auto fw = to_matrix(n, rnd);
        floyd_warshall(fw);
        CHECK(bf.negative_cycle.empty());
        CHECK_EQ(dj.dist, bf.dist);
        CHECK_EQ(dj.dist, fw[src]);
        for (int v = 0; v < n; v++) {
            CHECK(valid_shortest_path(rnd, dj, src, v));
            CHECK(valid_shortest_path(rnd, bf, src, v));
        }
    }
}

void test_zero_one_bfs() {
    vector<Edge> es{{0, 1, 1}, {0, 2, 0}, {2, 1, 0}, {1, 3, 1}};
    CHECK_EQ(zero_one_bfs(to_wgraph(5, es), 0), vector<ll>{0, 0, 0, 1, INF});

    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 12);
        auto rnd = random_edges(n, (int)t::rand_int(0, 40), 0, 1);
        bool undirected = t::rand_int(0, 1);
        auto wg = to_wgraph(n, rnd, undirected);
        int src = (int)t::rand_int(0, n - 1);
        CHECK_EQ(zero_one_bfs(wg, src), dijkstra(wg, src).dist);
    }
}

void test_bellman_ford() {
    // negative edge, no negative cycle
    vector<Edge> es{{0, 1, 4}, {0, 2, 5}, {2, 1, -3}, {1, 3, 2}};
    auto bf = bellman_ford(4, es, 0);
    CHECK_EQ(bf.dist, vector<ll>{0, 2, 5, 4});
    CHECK(bf.negative_cycle.empty());
    CHECK_EQ(path_to(bf, 3), vector<int>{0, 2, 1, 3});

    // 1 -> 2 -> 3 -> 1 has weight -1
    vector<Edge> cyc{{0, 1, 1}, {1, 2, -1}, {2, 3, -1}, {3, 1, 1}};
    auto bc = bellman_ford(4, cyc, 0);
    CHECK(valid_negative_cycle(cyc, bc.negative_cycle, 4));
    CHECK_EQ(set<int>(bc.negative_cycle.begin(), bc.negative_cycle.end()), set<int>{1, 2, 3});

    // a negative cycle that src cannot reach is invisible from src, but src = -1 finds it
    vector<Edge> far{{0, 1, 1}, {2, 3, -5}, {3, 2, 1}};
    CHECK(bellman_ford(4, far, 0).negative_cycle.empty());
    CHECK_EQ(bellman_ford(4, far, 0).dist, vector<ll>{0, 1, INF, INF});
    auto any = bellman_ford(4, far, -1);
    CHECK(valid_negative_cycle(far, any.negative_cycle, 4));

    // a negative self-loop is a cycle of length 1
    vector<Edge> loop{{0, 0, -1}};
    CHECK_EQ(bellman_ford(1, loop, 0).negative_cycle, vector<int>{0});

    // random graphs with negative weights vs Floyd
    int with_cycle = 0;
    for (int iter = 0; iter < 600; iter++) {
        int n = (int)t::rand_int(1, 7);
        auto rnd = random_edges(n, (int)t::rand_int(0, 14), -4, 10);
        auto fw = to_matrix(n, rnd);
        floyd_warshall(fw);
        int src = (int)t::rand_int(0, n - 1);
        bool neg_from_src = false, neg_any = false;
        for (int v = 0; v < n; v++) {
            if (fw[v][v] < 0) neg_any = true;
            if (fw[v][v] < 0 && fw[src][v] < INF) neg_from_src = true;
        }
        with_cycle += neg_any;
        auto r = bellman_ford(n, rnd, src);
        CHECK_EQ(!r.negative_cycle.empty(), neg_from_src);
        if (neg_from_src) {
            CHECK(valid_negative_cycle(rnd, r.negative_cycle, n));
        } else {
            CHECK_EQ(r.dist, fw[src]);
            for (int v = 0; v < n; v++) CHECK(valid_shortest_path(rnd, r, src, v));
        }
        auto all = bellman_ford(n, rnd, -1);
        CHECK_EQ(!all.negative_cycle.empty(), neg_any);
        if (neg_any) CHECK(valid_negative_cycle(rnd, all.negative_cycle, n));
    }
    CHECK(with_cycle > 50);  // the generator really exercises both branches
}

void test_floyd() {
    //  0 --3--> 1 --(-2)--> 2,  0 --4--> 2,  2 --1--> 3
    vector<vector<ll>> d{{0, 3, 4, INF}, {INF, 0, -2, INF}, {INF, INF, 0, 1}, {INF, INF, INF, 0}};
    floyd_warshall(d);
    CHECK_EQ(d[0], vector<ll>{0, 3, 1, 2});
    CHECK_EQ(d[1], vector<ll>{INF, 0, -2, -1});
    CHECK_EQ(d[3], vector<ll>{INF, INF, INF, 0});

    // negative cycle 0 -> 1 -> 0 of weight -1 shows up on the diagonal
    vector<vector<ll>> neg{{0, 1, INF}, {-2, 0, 5}, {INF, INF, 0}};
    floyd_warshall(neg);
    CHECK(neg[0][0] < 0 && neg[1][1] < 0);
    CHECK_EQ(neg[2][2], 0);

    // the clamp: a dense, very negative graph would overflow long long without it (UBSan checks this)
    int n = 70;
    vector<vector<ll>> dense(n, vector<ll>(n, -1'000'000'000'000LL));
    for (int i = 0; i < n; i++) dense[i][i] = 0;
    floyd_warshall(dense);
    for (int i = 0; i < n; i++) CHECK(dense[i][i] < 0 && dense[i][i] >= -INF);

    // transitive closure vs a DFS from every node
    for (int iter = 0; iter < 300; iter++) {
        int m = (int)t::rand_int(1, 10);
        auto adj = to_adj(m, random_edges(m, (int)t::rand_int(0, 20), 1, 1));
        CHECK_EQ(transitive_closure(adj), reach_brute(adj));
    }
}

void test_mst() {
    //  0 -1- 1 -2- 2 -4- 3,  plus 0-2 (3) and 1-3 (5)
    vector<Edge> es{{0, 1, 1}, {1, 2, 2}, {0, 2, 3}, {2, 3, 4}, {1, 3, 5}};
    auto mst = kruskal(4, es);
    CHECK_EQ(mst.weight, 7);
    CHECK_EQ(mst.edges.size(), 3);
    CHECK_EQ(prim_dense(to_matrix(4, es, true)), 7);
    CHECK_EQ(prim_heap(to_wgraph(4, es, true)), 7);

    vector<Edge> split{{0, 1, 1}, {2, 3, 1}};  // two components: a forest, no spanning tree
    CHECK_EQ(kruskal(4, split).edges.size(), 2);
    CHECK_EQ(kruskal(4, split).weight, 2);
    CHECK_EQ(prim_dense(to_matrix(4, split, true)), INF);
    CHECK_EQ(prim_heap(to_wgraph(4, split, true)), INF);
    CHECK_EQ(prim_dense(to_matrix(1, {}, true)), 0);
    CHECK_EQ(prim_heap(to_wgraph(1, {}, true)), 0);
    CHECK_EQ(prim_dense({}), 0);

    // Kruskal vs dense Prim vs heap Prim vs trying every (n-1)-subset of edges
    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 6);
        auto rnd = random_edges(n, (int)t::rand_int(0, 10), -5, 20);  // MSTs don't mind negative weights
        auto brute = mst_brute(n, rnd);
        auto k = kruskal(n, rnd);
        ll pd = prim_dense(to_matrix(n, rnd, true));
        ll ph = prim_heap(to_wgraph(n, rnd, true));
        CHECK_EQ((int)k.edges.size() == n - 1, brute.has_value());
        if (brute) {
            CHECK_EQ(k.weight, *brute);
            CHECK_EQ(pd, *brute);
            CHECK_EQ(ph, *brute);
        } else {
            CHECK_EQ(pd, INF);
            CHECK_EQ(ph, INF);
        }
    }
    // bigger random graphs: the three agree
    for (int iter = 0; iter < 100; iter++) {
        int n = (int)t::rand_int(1, 60);
        auto rnd = random_edges(n, (int)t::rand_int(0, 300), 1, 1000);
        auto k = kruskal(n, rnd);
        ll pd = prim_dense(to_matrix(n, rnd, true));
        ll ph = prim_heap(to_wgraph(n, rnd, true));
        CHECK_EQ(pd, ph);
        CHECK_EQ((int)k.edges.size() == n - 1 ? k.weight : INF, pd);
    }
}

void test_scc() {
    // {0,1,2} -> {3,4},  5 alone
    vector<vector<int>> adj{{1}, {2}, {0, 3}, {4}, {3}, {}};
    auto scc = scc_tarjan(adj);
    CHECK_EQ(scc.count, 3);
    CHECK(scc.comp[0] == scc.comp[1] && scc.comp[1] == scc.comp[2]);
    CHECK_EQ(scc.comp[3], scc.comp[4]);
    CHECK(scc.comp[2] > scc.comp[3]);  // reverse topological ids: the sink {3,4} finished first
    CHECK_EQ(scc_tarjan({}).count, 0);

    // vs brute force: same component iff mutually reachable
    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 9);
        auto g = to_adj(n, random_edges(n, (int)t::rand_int(0, 18), 1, 1));
        auto r = scc_tarjan(g);
        auto reach = reach_brute(g);
        set<int> ids(r.comp.begin(), r.comp.end());
        CHECK_EQ((int)ids.size(), r.count);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) CHECK_EQ(r.comp[i] == r.comp[j], reach[i][j] && reach[j][i]);
        for (int u = 0; u < n; u++)
            for (int v : g[u]) CHECK(r.comp[u] >= r.comp[v]);
    }
}

void test_bridges() {
    // triangle 0-1-2 with a tail 1-3: edge 3 is the only bridge, node 1 the only cut vertex
    auto a = bridges_and_articulation(4, {{0, 1}, {1, 2}, {2, 0}, {1, 3}});
    CHECK_EQ(a.bridges, vector<int>{3});
    CHECK_EQ(a.articulation, vector<int>{1});
    // two parallel edges: neither is a bridge
    auto b = bridges_and_articulation(2, {{0, 1}, {0, 1}});
    CHECK(b.bridges.empty() && b.articulation.empty());
    // a path: every edge is a bridge, the middle node is a cut vertex
    auto c = bridges_and_articulation(3, {{0, 1}, {1, 2}});
    CHECK_EQ(set<int>(c.bridges.begin(), c.bridges.end()), set<int>{0, 1});
    CHECK_EQ(c.articulation, vector<int>{1});
    // a self-loop changes nothing
    auto d = bridges_and_articulation(2, {{0, 0}, {0, 1}});
    CHECK_EQ(d.bridges, vector<int>{1});
    CHECK(d.articulation.empty());

    // vs "remove it and recount components", on multigraphs with self-loops
    for (int iter = 0; iter < 500; iter++) {
        int n = (int)t::rand_int(1, 8);
        vector<pair<int, int>> edges;
        int m = (int)t::rand_int(0, 12);
        for (int i = 0; i < m; i++) edges.push_back({(int)t::rand_int(0, n - 1), (int)t::rand_int(0, n - 1)});
        auto got = bridges_and_articulation(n, edges);
        int base = components_without(n, edges, -1, -1);
        vector<int> bridges, cuts;
        for (int id = 0; id < m; id++)
            if (components_without(n, edges, id, -1) > base) bridges.push_back(id);
        for (int v = 0; v < n; v++)
            if (components_without(n, edges, -1, v) > base) cuts.push_back(v);
        sort(got.bridges.begin(), got.bridges.end());
        CHECK_EQ(got.bridges, bridges);
        CHECK_EQ(got.articulation, cuts);
    }
}

void test_flow() {
    // the classic 6-node example: max flow 23
    vector<Edge> es{{0, 1, 16}, {0, 2, 13}, {1, 2, 10}, {2, 1, 4}, {1, 3, 12},
                    {3, 2, 9},  {2, 4, 14}, {4, 3, 7},  {3, 5, 20}, {4, 5, 4}};
    Dinic g(6);
    for (auto [u, v, c] : es) g.add_edge(u, v, c);
    CHECK_EQ(g.max_flow(0, 5), 23);
    auto side = g.min_cut_side(0);
    ll cut = 0;
    for (auto [u, v, c] : es)
        if (side[u] && !side[v]) cut += c;
    CHECK_EQ(cut, 23);

    Dinic big(3);  // capacities beyond 32 bits
    big.add_edge(0, 1, 2'000'000'000'000LL);
    big.add_edge(1, 2, 3'000'000'000'000LL);
    CHECK_EQ(big.max_flow(0, 2), 2'000'000'000'000LL);

    Dinic none(2);  // no path
    CHECK_EQ(none.max_flow(0, 1), 0);

    // max flow == brute-force min cut over all subsets, and min_cut_side is such a cut
    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(2, 8);
        auto rnd = random_edges(n, (int)t::rand_int(0, 16), 0, 10);
        int s = (int)t::rand_int(0, n - 1), tt = (int)t::rand_int(0, n - 2);
        if (tt >= s) tt++;
        Dinic f(n);
        for (auto [u, v, c] : rnd) f.add_edge(u, v, c);
        ll flow = f.max_flow(s, tt);
        CHECK_EQ(flow, min_cut_brute(n, rnd, s, tt));
        auto cut_side = f.min_cut_side(s);
        CHECK(cut_side[s] && !cut_side[tt]);
        ll cap = 0;
        for (auto [u, v, c] : rnd)
            if (cut_side[u] && !cut_side[v]) cap += c;
        CHECK_EQ(cap, flow);
    }

    // bipartite matching
    CHECK_EQ(max_bipartite_matching(3, 3, {{0, 0}, {0, 1}, {1, 0}, {2, 2}}), 3);
    CHECK_EQ(max_bipartite_matching(3, 1, {{0, 0}, {1, 0}, {2, 0}}), 1);
    CHECK_EQ(max_bipartite_matching(2, 2, {}), 0);
    for (int iter = 0; iter < 300; iter++) {
        int L = (int)t::rand_int(1, 5), R = (int)t::rand_int(1, 5);
        vector<pair<int, int>> pairs;
        int m = (int)t::rand_int(0, 10);
        for (int i = 0; i < m; i++) pairs.push_back({(int)t::rand_int(0, L - 1), (int)t::rand_int(0, R - 1)});
        CHECK_EQ(max_bipartite_matching(L, R, pairs), matching_brute(pairs));
    }
}

int main() {
    test_bfs();
    test_topo();
    test_dijkstra();
    test_zero_one_bfs();
    test_bellman_ford();
    test_floyd();
    test_mst();
    test_scc();
    test_bridges();
    test_flow();
    return t::summary("graph");
}
