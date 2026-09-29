// Graph algorithms for modules 14 and 15: one section per family, one snippet region per algorithm.
//
// Conventions, so the pieces fit together:
//   - Nodes are 0..n-1. Convert 1-indexed input as you read it.
//   - Unweighted graph: vector<vector<int>> adj, adj[u] = the nodes u has an edge to.
//   - Weighted graph:   WGraph adj, adj[u] = {(v, w), ...}. Undirected = add both directions.
//   - Edge list:        vector<Edge>, Edge{u, v, w}. Bellman-Ford and Kruskal take this form.
//   - Distances are long long. INF means "unreachable"; it is LLONG_MAX / 4 so INF + INF can't overflow.
//
// Interview-style header (see binary_search.hpp): unqualified std names, snippet-ready.
#pragma once
#include <bits/stdc++.h>
#include "dsu.hpp"
using namespace std;

using ll = long long;
using WGraph = vector<vector<pair<int, ll>>>;
struct Edge {
    int u, v;
    ll w;
};
constexpr ll INF = LLONG_MAX / 4;

// =============================== 1. Unweighted graphs: BFS, topological sort ===============================

// [snippet:bfs]
// Fewest edges from the nearest source to every node; -1 = unreachable. O(V + E).
// One source: bfs_dist(adj, {s}). Several sources = multi-source BFS: they all start at distance 0,
// exactly as if a virtual super-source had an edge to each of them.
vector<int> bfs_dist(const vector<vector<int>>& adj, const vector<int>& sources) {
    vector<int> dist(adj.size(), -1);
    queue<int> q;
    for (int s : sources) {
        if (dist[s] == -1) {
            dist[s] = 0;
            q.push(s);
        }
    }
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : adj[u]) {
            if (dist[v] == -1) {          // mark when pushed, so nobody is queued twice
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }
    return dist;
}
// [/snippet]

// [snippet:topo_kahn]
// Kahn's algorithm. Returns a topological order of a directed graph (every edge u->v has u before v),
// or {} if there is a cycle: nodes on or behind a cycle never reach in-degree 0. O(V + E).
// Test success with order.size() == n (also right for n = 0).
vector<int> topo_sort(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<int> indegree(n, 0);
    for (int u = 0; u < n; u++)
        for (int v : adj[u]) indegree[v]++;
    queue<int> ready;                              // nodes whose prerequisites are all output
    for (int u = 0; u < n; u++)
        if (indegree[u] == 0) ready.push(u);
    vector<int> order;
    while (!ready.empty()) {
        int u = ready.front();
        ready.pop();
        order.push_back(u);
        for (int v : adj[u])
            if (--indegree[v] == 0) ready.push(v);   // u was v's last unfinished prerequisite
    }
    if ((int)order.size() < n) return {};        // a cycle blocked the rest
    return order;
}
// [/snippet]

// =============================== 2. Single-source shortest paths ===============================

// [snippet:dijkstra]
// Result of a single-source search. dist[v] = INF if unreachable; parent[v] = previous node on a
// shortest path (-1 for the source and unreachable nodes). negative_cycle: only Bellman-Ford fills it.
struct ShortestPaths {
    vector<ll> dist;
    vector<int> parent;
    vector<int> negative_cycle;
};

// Dijkstra with lazy deletion. All weights must be >= 0. O((V + E) log V).
ShortestPaths dijkstra(const WGraph& adj, int src) {
    int n = adj.size();
    vector<ll> dist(n, INF);
    vector<int> parent(n, -1);
    // min-heap of (distance, node): greater<> turns priority_queue's default max-heap around
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>> pq;
    dist[src] = 0;
    pq.push({0, src});
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;               // stale entry: u was already settled with a smaller d
        for (auto [v, w] : adj[u]) {             // d == dist[u] is final here: relax u's edges once
            if (d + w < dist[v]) {
                dist[v] = d + w;
                parent[v] = u;
                pq.push({dist[v], v});           // no decrease-key: push a new entry, skip the old one later
            }
        }
    }
    return {dist, parent, {}};
}
// [/snippet]

// [snippet:path_to]
// Nodes of a shortest path src -> target, by walking parent pointers back and reversing.
// {} if target is unreachable. Only meaningful when there is no negative cycle.
vector<int> path_to(const ShortestPaths& sp, int target) {
    if (sp.dist[target] == INF) return {};
    vector<int> path;
    for (int v = target; v != -1; v = sp.parent[v]) path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}
// [/snippet]

// [snippet:zero_one_bfs]
// Shortest paths when every weight is 0 or 1: a deque instead of a heap. O(V + E).
// Invariant: the deque holds distances D...D, D+1...D+1 in order, so its front is always a minimum.
// A 0-edge keeps the distance (push front), a 1-edge adds one (push back).
vector<ll> zero_one_bfs(const WGraph& adj, int src) {
    int n = adj.size();
    vector<ll> dist(n, INF);
    deque<int> dq;
    dist[src] = 0;
    dq.push_back(src);
    while (!dq.empty()) {
        int u = dq.front();
        dq.pop_front();
        for (auto [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                if (w == 0) dq.push_front(v);
                else dq.push_back(v);
            }
        }
    }
    return dist;
}
// [/snippet]

// [snippet:bellman_ford]
// Bellman-Ford: negative weights allowed. O(V * E).
// After round i, dist[v] <= the best path using at most i edges; simple paths have <= n-1 edges, so
// n-1 rounds suffice. If round n still improves something, a negative cycle is reachable from src:
// negative_cycle then lists its nodes in edge order (last -> first closes it) and dist is not final.
// src = -1 starts every node at 0 (a virtual source with 0-edges to all): finds a negative cycle anywhere.
ShortestPaths bellman_ford(int n, const vector<Edge>& edges, int src) {
    vector<ll> dist(n, src == -1 ? 0 : INF);
    vector<int> parent(n, -1);
    if (src != -1) dist[src] = 0;
    int relaxed = -1;                            // a node improved in the latest round
    for (int round = 0; round < n; round++) {
        relaxed = -1;
        for (auto [u, v, w] : edges) {
            if (dist[u] == INF) continue;        // unreachable so far: INF + (negative w) would fake a path
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                relaxed = v;
            }
        }
        if (relaxed == -1) break;                // a quiet round: every distance is final
    }
    vector<int> cycle;
    if (relaxed != -1) {                         // round n still improved something
        int x = relaxed;
        for (int i = 0; i < n; i++) x = parent[x];   // n steps back is certainly on the cycle
        cycle.push_back(x);
        for (int v = parent[x]; v != x; v = parent[v]) cycle.push_back(v);
        reverse(cycle.begin(), cycle.end());     // parent pointers run backwards along the edges
    }
    return {dist, parent, cycle};
}
// [/snippet]

// =============================== 3. All pairs: Floyd-Warshall, transitive closure ===============================

// [snippet:floyd]
// All-pairs shortest paths, O(n^3). Input: d[i][j] = weight of edge i->j (the min over parallel edges),
// d[i][i] = 0 (or a negative self-loop), INF where there is no edge. Negative edges are fine.
// Afterwards d[i][j] = shortest distance, and some d[i][i] < 0 iff the graph has a negative cycle.
void floyd_warshall(vector<vector<ll>>& d) {
    int n = d.size();
    for (int k = 0; k < n; k++)                  // now allow k as an intermediate node: k must be outermost
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (d[i][k] < INF && d[k][j] < INF)
                    // the max(-INF, ...) clamp stops values under a negative cycle from overflowing
                    d[i][j] = min(d[i][j], max(-INF, d[i][k] + d[k][j]));
}
// [/snippet]

// [snippet:transitive_closure]
// reach[i][j] = true iff j is reachable from i (paths of >= 0 edges, so reach[i][i] is true).
// Floyd's loop with "or"/"and" in place of min/+. O(n^3); with bitset rows it is O(n^3 / 64).
vector<vector<bool>> transitive_closure(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<vector<bool>> reach(n, vector<bool>(n, false));
    for (int u = 0; u < n; u++) {
        reach[u][u] = true;
        for (int v : adj[u]) reach[u][v] = true;
    }
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            if (reach[i][k])
                for (int j = 0; j < n; j++)
                    if (reach[k][j]) reach[i][j] = true;
    return reach;
}
// [/snippet]

// =============================== 4. Minimum spanning trees ===============================

// [snippet:kruskal]
// Kruskal: scan edges from lightest to heaviest, keep each edge that joins two different components.
// O(E log E). Undirected graph. Returns a minimum spanning forest: connected iff edges.size() == n - 1.
struct MST {
    ll weight = 0;
    vector<Edge> edges;
};

MST kruskal(int n, vector<Edge> edges) {         // by value: we sort our own copy
    sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) { return a.w < b.w; });
    DSU dsu(n);
    MST mst;
    for (const Edge& e : edges) {
        if (dsu.unite(e.u, e.v)) {               // false: e would close a cycle, skip it
            mst.weight += e.w;
            mst.edges.push_back(e);
        }
    }
    return mst;
}
// [/snippet]

// [snippet:prim_dense]
// Prim for dense graphs given as a symmetric weight matrix (w[i][j] = INF: no edge). O(n^2), no heap:
// the right choice when E is close to n^2, e.g. "every pair of points is connected".
// Returns the MST weight, or INF if the graph is disconnected.
ll prim_dense(const vector<vector<ll>>& w) {
    int n = w.size();
    vector<ll> best(n, INF);                     // best[v] = cheapest edge from the tree to v so far
    vector<bool> in_tree(n, false);
    if (n > 0) best[0] = 0;
    ll total = 0;
    for (int iter = 0; iter < n; iter++) {
        int u = -1;
        for (int v = 0; v < n; v++)              // the closest vertex outside the tree
            if (!in_tree[v] && (u == -1 || best[v] < best[u])) u = v;
        if (best[u] == INF) return INF;          // nothing left is attached: disconnected
        in_tree[u] = true;
        total += best[u];
        for (int v = 0; v < n; v++)              // u joined: its edges may be cheaper links for others
            if (!in_tree[v] && w[u][v] < best[v]) best[v] = w[u][v];
    }
    return total;
}
// [/snippet]

// [snippet:prim_heap]
// Prim with a heap for sparse graphs: Dijkstra's loop, except the key is the edge weight, not the path
// length. adj must list both directions. O(E log V). Returns INF if the graph is disconnected.
ll prim_heap(const WGraph& adj) {
    int n = adj.size();
    if (n == 0) return 0;
    vector<bool> in_tree(n, false);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>> pq;   // (edge weight, node)
    pq.push({0, 0});
    ll total = 0;
    int added = 0;
    while (!pq.empty()) {
        auto [w, u] = pq.top();
        pq.pop();
        if (in_tree[u]) continue;                // stale: u already joined through a cheaper edge
        in_tree[u] = true;
        total += w;
        added++;
        for (auto [v, wv] : adj[u])
            if (!in_tree[v]) pq.push({wv, v});
    }
    return added == n ? total : INF;
}
// [/snippet]

// =============================== 5. Connectivity: SCC, bridges, articulation points ===============================

// [snippet:scc_tarjan]
// Strongly connected components (Tarjan), one DFS, O(V + E). comp[v] = component id of v.
// low[u] = smallest entry time reachable from u's DFS subtree through nodes still on the stack.
// u is the first-entered node of its SCC iff low[u] == tin[u]; that SCC is then on top of the stack.
// Ids come out in reverse topological order of the condensation: every edge u->v has comp[u] >= comp[v].
struct SCC {
    int count = 0;
    vector<int> comp;
};

SCC scc_tarjan(const vector<vector<int>>& adj) {
    int n = adj.size(), timer = 0;
    vector<int> tin(n, -1), low(n, 0), stk;
    vector<bool> on_stack(n, false);
    SCC res;
    res.comp.assign(n, -1);
    function<void(int)> dfs = [&](int u) {
        tin[u] = low[u] = timer++;
        stk.push_back(u);
        on_stack[u] = true;
        for (int v : adj[u]) {
            if (tin[v] == -1) {                  // tree edge
                dfs(v);
                low[u] = min(low[u], low[v]);
            } else if (on_stack[v]) {            // edge back into the SCC being built
                low[u] = min(low[u], tin[v]);
            }                                    // else: v's SCC is finished, ignore the edge
        }
        if (low[u] == tin[u]) {                  // u roots an SCC: pop it off the stack
            while (true) {
                int v = stk.back();
                stk.pop_back();
                on_stack[v] = false;
                res.comp[v] = res.count;
                if (v == u) break;
            }
            res.count++;
        }
    };
    for (int u = 0; u < n; u++)
        if (tin[u] == -1) dfs(u);
    return res;
}
// [/snippet]

// [snippet:bridges]
// Bridges and articulation points of an undirected multigraph, O(V + E), with low-link values:
// low[u] = smallest entry time reachable from u's DFS subtree using at most one back edge.
//   tree edge u-v is a bridge          iff low[v] >  tin[u]  (v's subtree has no way around it)
//   non-root u is an articulation point iff some child v has low[v] >= tin[u]
//   the DFS root is one                 iff it has 2+ DFS children
// We skip the parent *edge id*, not the parent node: a second edge between the same two nodes is then
// a back edge, so neither copy counts as a bridge.
struct CutInfo {
    vector<int> bridges;        // indices into the input edge list
    vector<int> articulation;   // articulation points (cut vertices), ascending
};

CutInfo bridges_and_articulation(int n, const vector<pair<int, int>>& edges) {
    vector<vector<pair<int, int>>> adj(n);        // (neighbour, edge id)
    for (int id = 0; id < (int)edges.size(); id++) {
        auto [a, b] = edges[id];
        adj[a].push_back({b, id});
        adj[b].push_back({a, id});
    }
    vector<int> tin(n, -1), low(n, 0);
    vector<bool> is_cut(n, false);
    CutInfo res;
    int timer = 0;
    function<void(int, int)> dfs = [&](int u, int parent_edge) {
        tin[u] = low[u] = timer++;
        int children = 0;
        for (auto [v, id] : adj[u]) {
            if (id == parent_edge) continue;     // don't walk back along the edge we arrived on
            if (tin[v] != -1) {                  // visited: a back edge
                low[u] = min(low[u], tin[v]);
            } else {                             // tree edge
                dfs(v, id);
                low[u] = min(low[u], low[v]);
                if (low[v] > tin[u]) res.bridges.push_back(id);
                if (parent_edge != -1 && low[v] >= tin[u]) is_cut[u] = true;
                children++;
            }
        }
        if (parent_edge == -1 && children >= 2) is_cut[u] = true;
    };
    for (int u = 0; u < n; u++)
        if (tin[u] == -1) dfs(u, -1);
    for (int u = 0; u < n; u++)
        if (is_cut[u]) res.articulation.push_back(u);
    return res;
}
// [/snippet]

// =============================== 6. Max flow ===============================

// [snippet:dinic]
// Max flow (Dinic). Each phase: BFS labels nodes by distance from s in the residual graph, then DFS pushes
// flow only along edges that go exactly one level deeper, until no such path is left (a blocking flow).
// O(V^2 E) worst case, far faster in practice; O(E sqrt V) on unit-capacity bipartite graphs.
struct Dinic {
    struct FlowEdge {
        int to;
        ll cap;                                   // remaining (residual) capacity
    };
    vector<FlowEdge> edges;                       // edges[id ^ 1] is the reverse of edges[id]
    vector<vector<int>> adj;                      // adj[u] = ids of edges leaving u
    vector<int> level, next_edge;

    explicit Dinic(int n) : adj(n), level(n), next_edge(n) {}

    void add_edge(int u, int v, ll cap) {         // directed u->v; undirected: add both ways
        adj[u].push_back((int)edges.size());
        edges.push_back({v, cap});
        adj[v].push_back((int)edges.size());
        edges.push_back({u, 0});                  // reverse edge: pushing flow on it undoes flow
    }

    void compute_levels(int s) {                  // BFS over edges that still have capacity
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int id : adj[u]) {
                int v = edges[id].to;
                if (edges[id].cap > 0 && level[v] == -1) {
                    level[v] = level[u] + 1;
                    q.push(v);
                }
            }
        }
    }

    ll augment(int u, int t, ll limit) {          // one augmenting path along the level graph
        if (u == t) return limit;
        for (int& i = next_edge[u]; i < (int)adj[u].size(); i++) {   // resume where u left off
            int id = adj[u][i], v = edges[id].to;
            if (edges[id].cap > 0 && level[v] == level[u] + 1) {
                ll got = augment(v, t, min(limit, edges[id].cap));
                if (got > 0) {
                    edges[id].cap -= got;
                    edges[id ^ 1].cap += got;
                    return got;
                }
            }
        }
        return 0;                                 // dead end: this edge pointer stays past it
    }

    ll max_flow(int s, int t) {                   // s != t
        ll flow = 0;
        while (true) {
            compute_levels(s);
            if (level[t] == -1) return flow;      // t unreachable in the residual graph: flow is maximum
            fill(next_edge.begin(), next_edge.end(), 0);
            while (ll got = augment(s, t, INF)) flow += got;
        }
    }

    // Call after max_flow(s, t): the nodes s can still reach in the residual graph are the s side of a
    // minimum cut. The original edges leaving that side are saturated, and their capacities sum to the flow.
    vector<bool> min_cut_side(int s) {
        compute_levels(s);
        vector<bool> side(adj.size());
        for (int v = 0; v < (int)adj.size(); v++) side[v] = level[v] != -1;
        return side;
    }
};
// [/snippet]

// [snippet:bipartite_matching]
// Maximum bipartite matching as max flow: source -> each left node -> its allowed right nodes -> sink,
// every capacity 1. Each unit of flow is one matched pair. pairs = (left index, right index).
int max_bipartite_matching(int left, int right, const vector<pair<int, int>>& pairs) {
    int s = left + right, t = s + 1;
    Dinic flow(left + right + 2);
    for (int l = 0; l < left; l++) flow.add_edge(s, l, 1);
    for (int r = 0; r < right; r++) flow.add_edge(left + r, t, 1);
    for (auto [l, r] : pairs) flow.add_edge(l, left + r, 1);
    return (int)flow.max_flow(s, t);
}
// [/snippet]
