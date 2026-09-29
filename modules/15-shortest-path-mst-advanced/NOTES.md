# 15 · Shortest Path + MST + Advanced Graphs
> Weighted graphs, where the naive answer is usually the wrong one.

**Time:** ~10 h of Core work ([problems](problems.md)) · **Prereqs:** modules 13 (priority queues), 14 (graph representations, BFS, DFS, DSU) · **You're done when:** (1) you can write lazy Dijkstra with path reconstruction, and 0-1 BFS, from a blank file in under 8 minutes each; (2) given only the edge weights and the constraints, you can pick BFS, 0-1 BFS, Dijkstra, Bellman-Ford or Floyd-Warshall and justify it in one sentence; (3) every Core box is ticked and the five 📖 are re-solved from blank.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Dijkstra & 0-1 BFS | Settle the closest unsettled node; with weights ≥ 0 its distance is final | `dijkstra`, `path_to`, `zero_one_bfs` in [graph.hpp](../../templates/graph.hpp) | 743, 1631, 2290 |
| 2 | Bellman-Ford & Floyd-Warshall | Relax every edge n − 1 times; all pairs by allowing one more middle node per round | `bellman_ford`, `floyd_warshall`, `transitive_closure` | 787, 1334 |
| 3 | Minimum spanning trees | Cut property: the lightest edge across any cut is safe to take | `kruskal`, `prim_dense`, `prim_heap` | 1584, 1135 |
| 4 | Advanced | Low-link values find SCCs, bridges and cut vertices in one DFS; max flow = min cut | `scc_tarjan`, `bridges_and_articulation`, `Dinic`; [scc_kosaraju.cpp](examples/scc_kosaraju.cpp), [euler_trail.cpp](examples/euler_trail.cpp) | 1192, 1568, 1520 |

**Which shortest-path algorithm?** Decide from the weights first, then the question. Rough limits assume ~10⁸ simple operations per second.

| Weights | Question | Algorithm | Time | Comfortable up to |
|---|---|---|---|---|
| all equal | one source | BFS (module 14) | O(V + E) | 10⁶ nodes and edges |
| only 0 and 1 | one source | 0-1 BFS | O(V + E) | 10⁶ |
| all ≥ 0 | one source | Dijkstra | O((V + E) log V) | ~10⁶ edges |
| negatives allowed | one source, or "is there a negative cycle?" | Bellman-Ford | O(V · E) | V · E ≈ 10⁸ |
| any | at most k edges on the path | Bellman-Ford, k rounds from a copy | O(k · E) | |
| any | every pair | Floyd-Warshall | O(V³) | V ≈ 400–500 |
| all ≥ 0, sparse | every pair | Dijkstra from every node | O(V (V + E) log V) | |

The templates share conventions: nodes 0..n−1, `WGraph adj` with `adj[u] = {(v, w), ...}`, `Edge{u, v, w}` lists, `long long` distances, and `INF = LLONG_MAX / 4` for "unreachable" (so INF + INF can't overflow).

## 1. Dijkstra & 0-1 BFS — non-negative weights, priority-queue relaxation

**Concept.** Keep a tentative distance `dist[v]` for every node (0 for the source, ∞ elsewhere). **Relaxing** edge u → v with weight w means: if `dist[u] + w < dist[v]`, a better route to v exists through u, so update `dist[v]`. Every shortest-path algorithm is a strategy for choosing which edges to relax, and in what order.

Dijkstra's strategy is BFS with a min-heap instead of a queue: repeatedly pop the unsettled node with the smallest tentative distance, declare it settled, and relax its outgoing edges.

**Why the popped node is final (weights ≥ 0).** Intuition: every unsettled node is at least as far as the one you pop, and non-negative edges can't make a detour through a farther node cheaper.

The proof: write δ(v) for the true shortest distance. Suppose u is the *first* node popped with `dist[u]` larger than δ(u). Take a true shortest path P from s to u, and let y be the first node on P that isn't settled yet, with x the settled node just before it (so `dist[x]` = δ(x), since u is the first mistake). When x was settled, edge x → y was relaxed, so `dist[y] ≤ δ(x) + w(x, y) = δ(y)`. Weights are non-negative, so the rest of P from y to u costs ≥ 0 and δ(y) ≤ δ(u) < `dist[u]`. Then y sits in the heap with a smaller key than u and would have been popped first. Contradiction. The only place non-negativity is used is "the rest of the path costs ≥ 0", and that is exactly what negative edges break.

**Lazy deletion.** C++'s `priority_queue` has no decrease-key. When `dist[v]` improves, push a new `(dist[v], v)` entry and leave the old one in the heap. When an entry pops with `d > dist[u]`, it's stale: skip it. The heap then holds up to E + 1 entries, each push and pop costing O(log E) = O(log V) (E ≤ V², so log E ≤ 2 log V), so the total is O((V + E) log V). Module 13 section 2 covers the same trick in general.

<!-- snippet: templates/graph.hpp#dijkstra -->
```cpp
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
```
<!-- /snippet -->

**Path reconstruction.** `parent[v]` is the node that last improved v. Walk back from the target and reverse:

<!-- snippet: templates/graph.hpp#path_to -->
```cpp
// Nodes of a shortest path src -> target, by walking parent pointers back and reversing.
// {} if target is unreachable. Only meaningful when there is no negative cycle.
vector<int> path_to(const ShortestPaths& sp, int target) {
    if (sp.dist[target] == INF) return {};
    vector<int> path;
    for (int v = target; v != -1; v = sp.parent[v]) path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}
```
<!-- /snippet -->

**Why negative edges break it.** Take s → a (1), s → b (2), b → a (−2). A Dijkstra that marks nodes done when popped settles a at 1 and never revisits it; the true distance is 0. The lazy template above happens to recover here, because it re-pushes a when b improves it, and on graphs with negative edges but no negative cycle it still ends with correct distances, since any node whose distance improves gets pushed and processed again (with a negative cycle it never stops). But the proof above is gone, and adversarial graphs make it re-settle nodes exponentially many times. Negative edges mean Bellman-Ford (Section 2); don't argue your way around it in an interview.

**`long long` distances.** 10⁵ nodes with weights up to 10⁹ give paths up to 10¹⁴: far past `int`'s 2.1·10⁹. Use `long long` whenever V · max(w) can pass ~2·10⁹, and a sentinel you can add to without overflow.

**Variants.** Dijkstra works for any path cost that never gets better when you extend the path (and where, of two paths to the same node, the better one stays better after both take the same edge). Change what "better" and "extend" mean, keep the loop:

| Variant | Extending by an edge of weight w | Heap order | Why it's still valid |
|---|---|---|---|
| Shortest path | d + w, with w ≥ 0 | min-heap | sums of non-negatives only grow |
| Minimax (bottleneck) path | max(d, w) | min-heap | a max can only grow as the path grows |
| Max-probability path | d × p, with 0 ≤ p ≤ 1 | max-heap | products of probabilities only shrink |
| State-augmented | d + w over states (node, extra) | min-heap | the same Dijkstra on a bigger graph |

State-augmented means the node alone doesn't capture the situation: (city, discounts left), (cell, walls you may still break), (node, parity of steps). Make `dist` a 2-D array `dist[node][extra]`, push `(d, node, extra)`, and the graph has V · K states. The cost becomes O((V + E) · K · log(V · K)).

**0-1 BFS.** When every weight is 0 or 1, a deque replaces the heap. **Invariant:** the deque holds distances D…D, D+1…D+1 in order, like a BFS queue with two levels. A 0-edge produces distance D again, so push to the front; a 1-edge produces D + 1, so push to the back. The front is always a minimum, just like the heap's top, but each operation is O(1). A node can be pushed at most twice (once at D + 1, once improved to D), so the total is O(V + E).

<!-- snippet: templates/graph.hpp#zero_one_bfs -->
```cpp
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
```
<!-- /snippet -->

**Pitfalls.**
- `priority_queue` is a max-heap. `priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>>` for a min-heap, or push negated distances.
- The pair must be `(dist, node)`: `(node, dist)` orders the heap by node id.
- Both mistakes give wrong answers if you mark nodes done on pop. With only the stale check the loop still converges to the right distances, but it re-settles nodes and loses the O((V + E) log V) bound, which is worse because small tests pass.
- Marking nodes visited when *pushed* (the BFS habit) breaks Dijkstra: the first discovery is not the cheapest one.
- No stale check: still correct, but every stale pop re-scans a whole adjacency list: TLE on dense graphs.
- `int` distances overflow; `INT_MAX + w` overflows even sooner. Check `dist[u] != INF` before adding, or use `LLONG_MAX / 4`.
- Undirected input needs both directions pushed; 1-indexed input needs size n + 1.
- 0-1 BFS with the front/back pushes swapped still ends with correct distances, but the O(V + E) guarantee is gone. The deque discipline *is* the algorithm.

**Recognize it when…**
- "minimum time / cost / effort / delay" and every weight is ≥ 0: Dijkstra.
- "the signal reaches every node", "time until all nodes receive it": Dijkstra from the source, then the maximum distance.
- "free moves and moves that cost one", "remove the fewest obstacles", "change the fewest arrows": 0-1 BFS.
- "the path whose largest step is smallest", "minimum effort where effort is the biggest jump": minimax Dijkstra (or binary search on the answer + BFS). That's 1631.
- "you may use a coupon / skip / break a wall up to k times": state-augmented Dijkstra or BFS.
- Weights present but all equal: plain BFS is enough.

### Worked example: 743. Network Delay Time
[LeetCode 743](https://leetcode.com/problems/network-delay-time/) · Medium

**Problem (paraphrased):** a directed network of n nodes (1..n) with travel times on its edges. A signal starts at node k. How long until every node has received it? −1 if some node never does.
**Signals:** weighted directed graph, non-negative times, one source, "time for all nodes": single-source shortest paths, answer = the largest distance.
**Brute force, and why it fails:** enumerate every simple path from k to every node: exponential. Bellman-Ford works (O(VE) = 100 · 6000 here) but ignores that the weights are non-negative, which is what makes the faster Dijkstra valid.
**Key insight:** each node receives the signal at its shortest-path distance from k. Run Dijkstra once; the answer is the maximum distance, or −1 if one is still ∞.
**Dry run:** times [[2,1,1],[2,3,1],[3,4,1]], n = 4, k = 2.

| Pop | Stale? | Relaxations | dist[1..4] | Heap after |
|---|---|---|---|---|
| (0, 2) | no | 1 ← 1, 3 ← 1 | 1, 0, 1, ∞ | (1,1) (1,3) |
| (1, 1) | no | no out-edges | 1, 0, 1, ∞ | (1,3) |
| (1, 3) | no | 4 ← 2 | 1, 0, 1, 2 | (2,4) |
| (2, 4) | no | – | 1, 0, 1, 2 | empty → max = 2 |

<!-- snippet: modules/15-shortest-path-mst-advanced/examples/0743-network-delay-time.cpp#solution -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** O((V + E) log V) time, O(V + E) space.
**Edge cases:**
- A node unreachable from k (edges are one-way) → −1.
- n = 1 → 0.
- Two cheap hops beat one expensive direct edge; that's why a node isn't final when first *reached*, only when *popped*.
- `int` is enough here (at most 99 edges × 100); in general use `long long`.
**Follow-ups:**
- *Print the path to the last node?* Keep `parent[v]` on each improvement and walk back (`path_to`).
- *Negative times?* Bellman-Ford (Section 2).
- *Many queries from different sources?* Rerun Dijkstra per source, or Floyd-Warshall when n is small (~100).

### Worked example: 2290. Minimum Obstacle Removal to Reach Corner
[LeetCode 2290](https://leetcode.com/problems/minimum-obstacle-removal-to-reach-corner/) · Hard

**Problem (paraphrased):** a grid of 0 (empty) and 1 (obstacle); move in 4 directions from the top-left to the bottom-right corner. Removing an obstacle lets you pass through it. Minimize the obstacles removed.
**Signals:** a shortest path where stepping onto an empty cell costs 0 and onto an obstacle costs 1: 0/1 weights. Up to 10⁵ cells.
**Brute force, and why it fails:** try every subset of obstacles to remove and BFS: 2^(obstacles). Plain BFS minimizes steps, not removals. Dijkstra works, O(mn log(mn)), but the log is unnecessary.
**Key insight:** the cost of entering cell (r, c) is `grid[r][c]` ∈ {0, 1}, so 0-1 BFS gives exact distances in O(mn): same-cost moves go to the front of the deque, cost-1 moves to the back.
**Dry run:** grid [[0,1,1],[1,1,0],[1,1,0]]. First pops (deque shown after each pop, cells with their distance):

| Pop | Pushes | Deque after |
|---|---|---|
| (0,0) d0 | (1,0)←1 back, (0,1)←1 back | (1,0)1 (0,1)1 |
| (1,0) d1 | (2,0)←2 back, (1,1)←2 back | (0,1)1 (2,0)2 (1,1)2 |
| (0,1) d1 | (0,2)←2 back | (2,0)2 (1,1)2 (0,2)2 |
| (2,0) d2 | (2,1)←3 back | (1,1)2 (0,2)2 (2,1)3 |
| (1,1) d2 | (1,2) is empty: ←2 **front** | (1,2)2 (0,2)2 (2,1)3 |
| (1,2) d2 | (2,2) is empty: ←2 **front** | (2,2)2 (0,2)2 (2,1)3 |

The corner's distance is 2 and never changes: answer 2.

<!-- snippet: modules/15-shortest-path-mst-advanced/examples/2290-minimum-obstacle-removal-to-reach-corner.cpp#solution -->
```cpp
class Solution {
public:
    int minimumObstacles(vector<vector<int>>& grid) {
        int rows = grid.size(), cols = grid[0].size();
        vector<vector<int>> dist(rows, vector<int>(cols, INT_MAX));   // fewest removals to reach each cell
        deque<pair<int, int>> dq;
        const int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
        dist[0][0] = 0;
        dq.push_back({0, 0});
        while (!dq.empty()) {
            auto [r, c] = dq.front();
            dq.pop_front();
            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d], nc = c + dc[d];
                if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
                int w = grid[nr][nc];                     // entering an obstacle = removing it = cost 1
                if (dist[r][c] + w < dist[nr][nc]) {
                    dist[nr][nc] = dist[r][c] + w;
                    if (w == 0) dq.push_front({nr, nc});  // same distance: goes before everything else
                    else dq.push_back({nr, nc});          // distance + 1: goes to the back
                }
            }
        }
        return dist[rows - 1][cols - 1];
    }
};
```
<!-- /snippet -->

**Complexity:** O(mn) time and space: each cell is pushed at most twice and checks 4 neighbours.
**Edge cases:**
- A 1 × 1 grid → 0.
- A free winding path exists → 0 however long it is: step count doesn't matter.
- A one-cell-thick wall of obstacles across the whole grid → exactly 1.
**Follow-ups:**
- *Why not Dijkstra?* It's correct too, with an extra log factor; say you'd use it if the weights weren't only 0 and 1.
- *Removals cost different amounts?* General non-negative weights: Dijkstra.
- *At most k removals allowed, minimize steps?* Now the state is (cell, removals used): BFS over (r, c, k) states.

## 2. Bellman-Ford & Floyd-Warshall — negative edges, all-pairs distances

**Concept: Bellman-Ford.** Relax *every* edge, n − 1 times over.
- **Invariant:** after round i, `dist[v]` ≤ the cost of the best path from the source to v that uses at most i edges (by induction: such a path is a best (i−1)-edge path plus one edge, and round i relaxes that edge).
- A shortest path never needs more than n − 1 edges (repeating a node would mean a cycle, which a shortest path doesn't need without negative cycles), so n − 1 rounds suffice. A round that changes nothing lets you stop early.
- **Negative cycles:** if round n still improves something, costs keep falling forever: a negative cycle is reachable from the source.
- **Reconstructing the cycle:** take a node improved in round n and walk `parent` back n steps. The parent chain from such a node never runs out (it can't end at the source), and n steps use up any lead-in, so you land on the cycle. Then follow `parent` until you return to that node.
- Starting every node at distance 0 (the template's `src = -1`) is the same as adding a virtual source with a 0-edge to everyone: it finds a negative cycle anywhere in the graph.

<!-- snippet: templates/graph.hpp#bellman_ford -->
```cpp
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
```
<!-- /snippet -->

O(V · E) time, O(V) space. The `dist[u] == INF` check matters: without it, INF + (−5) looks like a real, reachable distance.

**The k-edges variant** ("at most k stops"). If a round reads values written earlier in the *same* round, one round can chain several edges, and "round i ⇒ at most i edges" is lost. Read from a copy of the previous round and write into the new one: then after round i, `cost[v]` is exactly the best price with at most i edges. 787 below.

**Concept: Floyd-Warshall.** All-pairs shortest paths by dynamic programming over which nodes may appear in the middle of a path. Let d_k[i][j] be the best i → j path whose intermediate nodes all come from {0, …, k−1}. Allowing node k as well: either the best path avoids k, or it goes i → k → j, so d_{k+1}[i][j] = min(d_k[i][j], d_k[i][k] + d_k[k][j]).
- **k must be the outermost loop:** stage k needs stage k − 1 finished for *every* pair. With k innermost, d[i][j] is finalized from values that aren't final yet (swap the loops in graph.hpp and graph_test.cpp fails).
- **Updating in place is safe:** during stage k, row k and column k don't change, because d[i][k] = min(d[i][k], d[i][k] + d[k][k]) and d[k][k] ≥ 0 when there's no negative cycle.
- **Negative cycles:** afterwards some d[i][i] < 0 ⇔ the graph has a negative cycle.

<!-- snippet: templates/graph.hpp#floyd -->
```cpp
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
```
<!-- /snippet -->

O(V³) time, O(V²) memory: fine for V ≤ ~400, hopeless for 10⁴ (10¹² steps, and 800 MB for the matrix). Initialize with d[i][i] = 0, the minimum over parallel edges, and INF elsewhere.

**Transitive closure** ("can i reach j at all?") is the same triple loop with booleans: `reach[i][j] |= reach[i][k] && reach[k][j]`. The template is `transitive_closure` in graph.hpp; storing rows as bitsets turns the inner loop into `reach[i] |= reach[k]`, O(V³ / 64).

Other tools you should be able to name: **SPFA** (Bellman-Ford with a queue of changed nodes; often faster, same O(VE) worst case) and **Johnson's algorithm** (all pairs on a sparse graph with negative edges: one Bellman-Ford to reweight edges to ≥ 0, then Dijkstra from every node).

**Pitfalls.**
- The Floyd loop order: k outside, then i, then j.
- INF + INF or INF + w overflows: skip when either side is INF (both templates do).
- A negative *undirected* edge is a negative cycle (u → v → u), so shortest paths are undefined there.
- Forgetting d[i][i] = 0, or overwriting a cheaper parallel edge with a more expensive one when filling the matrix.
- In-place relaxation in the k-edges variant silently allows more edges than k.
- Bellman-Ford's early exit is only an optimization: to *detect* a negative cycle you need the n-th round.

**Recognize it when…**
- Negative weights: "refunds", "gains", "energy boosts", "exchange rates" (use weight −log(rate): a cycle whose rates multiply to more than 1, an arbitrage, becomes a negative cycle).
- "at most k stops / transfers / edges": Bellman-Ford with k rounds from a copy (or BFS over (node, edges used)).
- "for every pair", "for every city, how many are within distance T", n ≤ 100–400: Floyd-Warshall (1334).
- "Is there a cycle that makes the cost unbounded?": negative-cycle detection.
- "Is i a (direct or indirect) prerequisite of j?" asked many times: transitive closure.

### Worked example: 787. Cheapest Flights Within K Stops
[LeetCode 787](https://leetcode.com/problems/cheapest-flights-within-k-stops/) · Medium

**Problem (paraphrased):** n cities, directed flights with prices. Find the cheapest trip from src to dst that makes at most k intermediate stops (so at most k + 1 flights), or −1.
**Signals:** cheapest path *with an edge-count limit*. Plain Dijkstra on price alone is wrong: the cheapest route to a middle city may use too many stops, while a pricier but shorter route to it was the one you needed.
**Brute force, and why it fails:** DFS over every route with at most k + 1 flights: up to (out-degree)^(k+1) routes. With n = 100 and k up to 99 that explodes.
**Key insight:** Bellman-Ford's invariant is already "after round i, the best cost using at most i edges". Run exactly k + 1 rounds, each reading the previous round's array.
**Dry run:** n = 3, flights [[0,1,100],[1,2,100],[0,2,500]], src 0, dst 2.

| After round | cost[0], cost[1], cost[2] | Meaning |
|---|---|---|
| 0 | 0, ∞, ∞ | no flights yet |
| 1 | 0, 100, 500 | at most 1 flight: the direct 0 → 2 costs 500 |
| 2 | 0, 100, 200 | at most 2 flights: 0 → 1 → 2 costs 200 |

k = 1 allows 2 flights → 200; k = 0 allows 1 → 500. Relaxing in place breaks this: in round 1, edge 0 → 1 sets cost[1] = 100, then edge 1 → 2 reads that fresh 100 and sets cost[2] = 200, so k = 0 wrongly answers 200 (the file checks this bug on purpose):

```c++
for (auto& f : flights)                                   // WRONG: reads this round's updates
    if (cost[f[0]] != INF && cost[f[0]] + f[2] < cost[f[1]]) cost[f[1]] = cost[f[0]] + f[2];
```

<!-- snippet: modules/15-shortest-path-mst-advanced/examples/0787-cheapest-flights-within-k-stops.cpp#solution -->
```cpp
class Solution {
public:
    int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int k) {
        const int INF = INT_MAX;
        vector<int> cost(n, INF);                   // cost[v] = cheapest price using at most `round` flights
        cost[src] = 0;
        for (int round = 1; round <= k + 1; round++) {   // k stops = at most k + 1 flights
            vector<int> next = cost;                // read last round's prices, write this round's
            for (auto& f : flights) {
                int u = f[0], v = f[1], price = f[2];
                if (cost[u] != INF && cost[u] + price < next[v]) next[v] = cost[u] + price;
            }
            cost.swap(next);                        // O(1): next becomes the new "last round"
        }
        return cost[dst] == INF ? -1 : cost[dst];
    }
};
```
<!-- /snippet -->

**Complexity:** O((k + 1) · E) time, O(n) space (two arrays).
**Edge cases:**
- src == dst: the constraints rule it out, but the code would return 0.
- dst unreachable within k + 1 flights → −1, even if reachable with more.
- Cycles in the flight graph are harmless: prices are positive, so a cycle never helps.
**Follow-ups:**
- *Dijkstra version?* Heap over (cost, city, stops used), expanding a state only while stops ≤ k; the state must include the stop count, and pruning is subtle. Bellman-Ford is simpler to get right.
- *Return the route?* Keep a parent per (round, city), since the best route depends on the round.
- *Why `cost.swap(next)`?* O(1) buffer swap. (`cost = move(next)` also works, but clang warns about an unqualified `move` and this kit's `-Werror` build rejects it: write `std::move`.)

## 3. Minimum spanning trees — Kruskal, Prim, and where MSTs actually show up

**Concept.** In a connected, undirected, weighted graph, a spanning tree is a set of n − 1 edges connecting all n nodes with no cycle. A minimum spanning tree (MST) has the smallest total weight. Negative weights are fine; direction isn't (the directed version, a minimum arborescence, is a different and harder problem).

**The cut property.** Split the nodes into any two sides S and V∖S. The lightest edge e crossing the split belongs to some MST. Proof by exchange: take an MST T without e. Adding e to T creates a cycle, which crosses the split at least one more time, at some edge f. Since w(e) ≤ w(f), T − f + e is a spanning tree no heavier than T: an MST that contains e. Both algorithms below are this argument applied greedily.

**Kruskal.** Sort edges by weight; take each edge whose endpoints are in different components (a DSU from module 14 answers that in ≈ O(1)). Why it's safe: when an edge (u, v) is taken, consider the cut around u's current component. Every lighter edge was already processed and none crosses that cut (it would have merged the component), so (u, v) is the lightest crossing edge. O(E log E) for the sort.

<!-- snippet: templates/graph.hpp#kruskal -->
```cpp
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
```
<!-- /snippet -->

If the graph is disconnected, Kruskal builds a minimum spanning *forest*: check `edges.size() == n - 1`.

**Prim.** Grow one tree from any node; always add the cheapest edge leaving the tree (the cut property on the cut "tree vs rest"). Two implementations:
- **With a heap:** exactly Dijkstra's loop, except the key is the weight of the connecting edge, not the distance from the source. O(E log V); it's `prim_heap` in graph.hpp.
- **Dense, no heap:** keep `best[v]` = the cheapest edge from the tree to v, and scan all nodes to pick the next one. O(V²), with no edge list at all: the right choice when every pair of nodes is connected.

<!-- snippet: templates/graph.hpp#prim_dense -->
```cpp
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
```
<!-- /snippet -->

| | Kruskal | Prim + heap | Prim, dense |
|---|---|---|---|
| Time | O(E log E) | O(E log V) | O(V²) |
| Extra memory | the edge list | heap up to E | O(V) |
| Best for | edge lists, sparse graphs | adjacency lists | complete graphs, "every pair of points" |

**MST ≠ shortest-path tree.** In a triangle with a–b = 2, b–c = 2, a–c = 3, the MST is {a–b, b–c} (weight 4), but the shortest route from a to c is the direct edge (3), which the MST doesn't contain. The MST minimizes the total; Dijkstra minimizes each root-to-node distance.

**Where MSTs show up.**
- **Network design:** the cheapest set of cables, roads or pipes that connects every site. Tip: a "build a well at a house for cost c" option becomes an edge to a virtual node 0 of weight c.
- **Clustering:** delete the k − 1 heaviest MST edges and the remaining components are the k clusters with the largest possible minimum gap between them (single-linkage clustering).
- **Bottleneck paths:** the path between two nodes inside the MST minimizes the largest edge used. The same minimax idea as section 1, from another angle.
- **Approximation:** walk around the MST (every edge twice) and skip nodes you've already visited: when distances obey the triangle inequality, that tour is at most twice the optimal travelling-salesman tour.

**Pitfalls.**
- A disconnected graph has no spanning tree: check the edge count (Kruskal) or that every node was reached (Prim).
- Summing weights in `int` overflows long before the individual weights do.
- Kruskal on a complete graph: n = 10⁴ points means 5·10⁷ edges in memory. Use dense Prim.
- Prim with Dijkstra's key (`dist[u] + w`) instead of the edge weight `w` builds a shortest-path tree, not an MST.
- Forgetting to add both directions in Prim's adjacency lists.

**Recognize it when…**
- "Connect all the cities / computers / points at minimum total cost" (1135, and −1 if it can't be done).
- "Keep every node reachable while removing the most expensive set of edges" (the complement of an MST).
- "Points on a plane; the cost of connecting two points is their distance": a dense MST.
- "Split into k groups so the closest pair across groups is as far apart as possible": MST clustering.
- "Is this edge in every MST / some MST?": exclude it or force it and compare the MST weights.

### Worked example: 1584. Min Cost to Connect All Points
[LeetCode 1584](https://leetcode.com/problems/min-cost-to-connect-all-points/) · Medium

**Problem (paraphrased):** up to 1000 points on a plane; connecting two points costs their Manhattan distance. Find the minimum total cost that connects every point.
**Signals:** "connect all at minimum total cost" is an MST; every pair can be connected, so the graph is complete: n(n−1)/2 ≈ 5·10⁵ edges.
**Brute force, and why it fails:** enumerate spanning trees: there are n^(n−2) of them (Cayley's formula). Kruskal works: build all 5·10⁵ edges, sort them, O(n² log n) time and O(n²) memory. It's in the file as `SolutionKruskal`, and it passes, but it does more work than needed.
**Key insight:** in a complete graph, O(n²) Prim never materializes the edges: compute each distance when it's needed. n² = 10⁶ steps, no sort, O(n) memory.
**Dry run:** points (0,0), (2,2), (3,10), (5,2), (7,0), starting at point 0.

| Iteration | Picked (cost) | Total | best[] afterwards (points 1..4, – = in tree) |
|---|---|---|---|
| 1 | 0 (0) | 0 | 4, 13, 7, 7 |
| 2 | 1 (4) | 4 | –, 9, 3, 7 |
| 3 | 3 (3) | 7 | –, 9, –, 4 |
| 4 | 4 (4) | 11 | –, 9, –, – |
| 5 | 2 (9) | 20 | done → 20 |

<!-- snippet: modules/15-shortest-path-mst-advanced/examples/1584-min-cost-to-connect-all-points.cpp#solution -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** O(n²) time, O(n) space.
**Edge cases:**
- One point → 0.
- 1000 points spread over the full coordinate range: the file checks Prim against Kruskal there.
- The largest single distance is 4·10⁶. The MST total of 1000 points in a 2·10⁶ square stays far below 2³¹ (it grows like side · √n), so `int` is safe; if you can't argue that in the room, accumulate in `long long`.
**Follow-ups:**
- *Why not Kruskal?* It's O(n² log n) with 5·10⁵ edges held in memory; both pass here, but Prim is the better fit for complete graphs.
- *n = 10⁵ points?* Neither O(n²) method fits. For Manhattan distance, only O(n) candidate edges can be in the MST (the nearest point in each of 8 directional regions), then Kruskal. Name it; you won't be asked to code it.
- *Euclidean distance?* Same algorithms with `double`, or compare squared distances as `long long`.

## 4. Advanced — strongly connected components, bridges, articulation points, max-flow basics

### Strongly connected components

In a directed graph, u and v are **strongly connected** if each can reach the other. The SCCs partition the nodes, and collapsing each SCC to a single node gives the **condensation**, which is always a DAG (a cycle between SCCs would make them one bigger SCC). That's why SCCs matter: they turn any directed graph into a DAG, where topological order and DP work.

**Kosaraju (two DFS passes).**
1. DFS on G, recording nodes in finish order.
2. DFS on the reversed graph, taking start nodes in decreasing finish time. Each DFS tree of pass 2 is one SCC.

Why: the node that finishes last lies in a *source* SCC of the condensation (nothing outside it reaches in). In the reversed graph that SCC becomes a *sink*: a DFS from it can't escape, so it collects exactly its own SCC. Remove it and repeat the argument.

<!-- snippet: modules/15-shortest-path-mst-advanced/examples/scc_kosaraju.cpp#kosaraju -->
```cpp
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
```
<!-- /snippet -->

**Tarjan (one DFS, low-link values).**
- `tin[u]` ("time in") is a counter value stamped on u when the DFS first enters it.
- Visited nodes stay on a stack until their whole SCC is known.
- `low[u]` is the smallest `tin` reachable from u's DFS subtree using tree edges plus one edge back to a node still on the stack.
- If `low[u] == tin[u]`, nothing below u reaches above it: u is the first node entered in its SCC, and u together with everything above it on the stack is that SCC.

<!-- snippet: templates/graph.hpp#scc_tarjan -->
```cpp
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
```
<!-- /snippet -->

Both are O(V + E). Tarjan numbers SCCs in reverse topological order of the condensation, Kosaraju in topological order; the tests check both properties. To build the condensation, add an edge comp[u] → comp[v] for every original edge that crosses components and deduplicate (`condense` in scc_kosaraju.cpp).

### Bridges and articulation points

In an undirected graph, a **bridge** is an edge whose removal disconnects the graph, and an **articulation point** (cut vertex) is a node whose removal does. Networks call both "single points of failure".

Run one DFS, recording `tin[u]` (as for Tarjan) and `low[u]`: the smallest entry time reachable from u's subtree using tree edges plus at most one back edge.

```
    0          tin/low: 0: 0/0   1: 1/0   2: 2/0   3: 3/3
   / \         edge 1–3: low[3] = 3 > tin[1] = 1 → bridge (3 can't get around it)
  1 — 2        edge 1–2: low[2] = 0 ≤ tin[1] → not a bridge (2 reaches 0 by the back edge 2–0)
  |            node 1 has child 3 with low[3] ≥ tin[1] → articulation point
  3
```

- Tree edge u – v is a **bridge** iff `low[v] > tin[u]`: nothing in v's subtree reaches u or above without that edge.
- A non-root u is an **articulation point** iff some child v has `low[v] >= tin[u]`: v's subtree can reach u but nothing above it, so removing u strands it.
- The DFS **root** is an articulation point iff it has two or more DFS children (it has no ancestors, so the `>=` test would always fire).
- **Parallel edges:** skip the edge you arrived on by its *id*, not by its other endpoint. Then a second u – v edge acts as a back edge and neither copy is a bridge, which is correct.

<!-- snippet: templates/graph.hpp#bridges -->
```cpp
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
```
<!-- /snippet -->

O(V + E).

**Recursion depth.** These templates recurse through `std::function`, as deep as the longest DFS path. On this Mac (8 MB default stack), the recursive 1192 solution below crashes on a 10⁵-node path even at `-O2`; judges often allow more stack, but you can't see their limit. If a large test dies with no output, convert to an explicit stack of (node, parent edge, next-neighbour index), or, for SCCs, use Kosaraju: its second pass works with any traversal order, so it's easy to make iterative.

### Euler trails and circuits (in brief)

An **Euler trail** uses every edge exactly once; an **Euler circuit** also ends where it started. Existence is a degree check, plus "all edges lie in one connected piece":

| | Circuit | Trail |
|---|---|---|
| Undirected | every degree even | 0 or 2 nodes of odd degree (start at one of them) |
| Directed | in-degree = out-degree everywhere | one node with out = in + 1 (the start), one with in = out + 1 (the end), the rest equal |

**Hierholzer's algorithm**, O(E): walk along unused edges until stuck. The node where you get stuck is final at that position, so append it to the answer and back up to the previous node, which may still have unused edges (a detour that gets spliced in). Reverse the answer at the end. `examples/euler_trail.cpp` implements the undirected version with edge ids (so parallel edges and self-loops work) and tests it against a backtracking search. For a directed graph each edge lives in one adjacency list, so you can simply pop edges off `adj[u]` instead of tracking ids.

### Max flow and min cut

A **flow network** is a directed graph with a capacity on each edge, a source s and a sink t. A flow puts at most the capacity on each edge, and every node other than s and t passes on exactly what it receives. **Maximum flow** asks for the largest total that can leave s.

- **Residual graph:** an edge with capacity c carrying flow f has c − f left forwards, plus a *reverse* edge of capacity f. Pushing flow along a reverse edge cancels earlier flow, which is how the algorithm undoes bad early choices.
- **Ford–Fulkerson:** while some s → t path has positive residual capacity (an augmenting path), push the bottleneck amount along it. With integer capacities it terminates, but the number of augmentations can be as large as the flow value itself.
- **Edmonds–Karp:** always augment along a *shortest* path (BFS). O(V · E²), independent of the capacities.
- **Dinic:** BFS assigns levels (distance from s in the residual graph); DFS then pushes flow only along edges that go exactly one level deeper, until none is left (a blocking flow); repeat. O(V² · E), much faster in practice, and O(E √V) on unit-capacity bipartite graphs.
- **Max-flow = min-cut:** the maximum flow equals the minimum total capacity of edges you must cut to separate s from t. After the flow is maximal, the nodes s can still reach in the residual graph are one side of such a minimum cut.

<!-- snippet: templates/graph.hpp#dinic -->
```cpp
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
```
<!-- /snippet -->

**Reductions to flow.** Bipartite matching: source → every left node → the right nodes it may pair with → sink, all capacity 1, and the flow value is the matching size:

<!-- snippet: templates/graph.hpp#bipartite_matching -->
```cpp
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
```
<!-- /snippet -->

Also: edge-disjoint s–t paths = max flow with unit capacities; a capacity on a *node* = split it into `v_in → v_out` with that capacity; several sources or sinks = a super-source and a super-sink. In interviews, max flow is rare; recognizing "this is matching / min cut" and naming the algorithm is usually what's wanted.

**Pitfalls.**
- Tarjan's SCC update uses `tin[v]` only for nodes *still on the stack*; an edge into an already-finished SCC must be ignored.
- Bridges with parallel edges: skip the parent edge id, not the parent node.
- The articulation root rule (2+ children) is different from the non-root rule.
- Forgetting reverse edges in max flow gives a greedy that can get stuck below the maximum.
- Capacities and flow totals need `long long` when capacities go up to 10⁹.
- An undirected edge in a flow network is two directed edges, each with the full capacity.
- Dinic without the `next_edge` pointer re-scans dead edges and loses its complexity bound.

**Recognize it when…**
- "critical connection", "which link, if cut, disconnects the network": bridges.
- "which server / node, if removed, disconnects the rest": articulation points (1568 builds on this).
- "groups where everyone can reach everyone", "mutually reachable", "collapse cycles and then DP": SCCs and the condensation (1520 is this idea in disguise).
- "use every edge exactly once", "an itinerary using all tickets": Euler trail.
- "maximum throughput / how many units can get from s to t", "the maximum number of disjoint paths", "assign workers to jobs, each once": max flow or matching.
- "the cheapest set of edges to cut to separate s from t": min cut.

### Worked example: 1192. Critical Connections in a Network
[LeetCode 1192](https://leetcode.com/problems/critical-connections-in-a-network/) · Hard

**Problem (paraphrased):** n servers joined by undirected connections into one network. Return every connection whose removal disconnects some pair of servers.
**Signals:** "removing it disconnects the network" is the definition of a bridge. n up to 10⁵, so the answer must be about linear.
**Brute force, and why it fails:** remove each edge and check connectivity with BFS: O(E · (V + E)) ≈ 2·10¹⁰ at the limits.
**Key insight:** one DFS computing `tin` and `low` finds every bridge: tree edge (u, v) is a bridge iff `low[v] > tin[u]`.
**Dry run:** n = 4, [[0,1],[1,2],[2,0],[1,3]] (the diagram above). DFS order 0 → 1 → 2, then 2 sees 0 again (a back edge), so low[2] = 0; back at 1, low[1] = 0; then 1 → 3, where 3 has no other edges, so low[3] = 3 > tin[1] = 1: bridge [1, 3]. Edge (1, 2) isn't one because low[2] = 0 ≤ tin[1], and neither is (0, 1): low[1] = 0 ≤ tin[0].

<!-- snippet: modules/15-shortest-path-mst-advanced/examples/1192-critical-connections-in-a-network.cpp#solution -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** O(V + E) time, O(V + E) space.
**Edge cases:**
- Two nodes and one edge → that edge.
- A single cycle → no bridges.
- Two cycles joined by one edge → exactly that edge.
- Skipping the parent *node* is fine here only because the problem promises no repeated connections; with parallel edges, skip the parent edge id, as the template does.
**Follow-ups:**
- *Articulation points instead?* Same DFS: `low[v] >= tin[u]` for non-root u, plus the root's 2-children rule.
- *The graph might be disconnected?* Start a DFS from every unvisited node.
- *10⁵ nodes in one long path?* The recursion is 10⁵ deep, enough to overflow an 8 MB stack (see "Recursion depth" above): an iterative DFS removes the risk.

## Common mistakes

| Mistake | Symptom | Fix |
|---|---|---|
| Max-heap, or heap entries as `(node, dist)` | wrong with a done[] flag; otherwise right but slow on big tests | min-heap of `(dist, node)` via `greater<>` |
| Visited-on-push in Dijkstra | first discovery taken as final: wrong | settle on pop; skip stale entries |
| `int` distances, `INT_MAX + w` | overflow, negative "distances" | `long long`, `INF = LLONG_MAX / 4`, check before adding |
| Dijkstra with negative edges | wrong or exponential | Bellman-Ford |
| In-place relaxation with an edge limit | too many edges used | relax from a copy of the last round |
| Floyd with k not outermost | wrong distances | k, then i, then j |
| No INF check in Bellman-Ford | unreachable nodes get finite distances | skip `dist[u] == INF` |
| MST on a disconnected graph | a "tree" with fewer than n − 1 edges | check the edge count |
| Prim keyed by path length | a shortest-path tree, not an MST | key = the connecting edge's weight |
| Bridges with parent-node skipping on multigraphs | parallel edges reported as bridges | skip the parent edge id |
| Deep recursion in Tarjan / bridges | crash on the largest test only | iterative DFS, or Kosaraju |

## Say it out loud

A talk track for 787 (Cheapest Flights Within K Stops):

1. "Directed flights with prices; I need the cheapest route from src to dst with at most k stops, so at most k + 1 flights, or −1."
2. "Brute force: DFS over all routes with up to k + 1 flights, exponential in k."
3. "Plain Dijkstra on price is wrong, because the cheapest way into a middle city might use too many stops."
4. "Bellman-Ford fits exactly: after round i, cost[v] is the cheapest price using at most i flights. I run k + 1 rounds."
5. "Each round must read the previous round's array, or one round could chain several flights, so I relax into a copy."
6. "O((k + 1) · E) time and O(n) space; edge cases: src == dst is 0, and unreachable within the limit is −1."

Follow-ups interviewers like in this module:
- *Why does Dijkstra fail with negative edges?* The popped node is final only if the rest of any path costs ≥ 0.
- *Dijkstra vs 0-1 BFS vs BFS: which, and when?*
- *How do you get the actual path?* A parent array, walked backwards.
- *Detect a negative cycle and print it?* Bellman-Ford's n-th round, then n parent steps back.
- *Kruskal or Prim for this input?* Edge list → Kruskal; complete graph → dense Prim.
- *What's the recursion depth of your DFS, and what if it's 10⁵?*
- *Why is the condensation a DAG, and why do we care?*

## Self-check

1. Where exactly does Dijkstra's correctness proof use non-negative weights?
   <details><summary>Answer</summary>In "the part of the shortest path after the first unsettled node y costs ≥ 0", which gives δ(y) ≤ δ(u), so y would pop before u. With a negative edge later in the path, a node popped early could still be improved.</details>
2. What is lazy deletion, and what does it cost?
   <details><summary>Answer</summary>`priority_queue` can't decrease a key, so on every improvement you push a new (dist, node) entry and skip entries that pop with d > dist[u]. The heap holds up to E + 1 entries, so the total is O((V + E) log V) with O(E) heap memory.</details>
3. Why is 0-1 BFS correct, and what's its complexity?
   <details><summary>Answer</summary>The deque always holds distances D…D then D+1…D+1. A 0-edge yields D (front), a 1-edge yields D + 1 (back), so the front is always a minimum, exactly what Dijkstra's heap provides. Each node is pushed at most twice: O(V + E).</details>
4. Why does Bellman-Ford need n − 1 rounds, and what does an improvement in round n mean?
   <details><summary>Answer</summary>After round i every path of at most i edges has been accounted for, and without negative cycles shortest paths are simple, so at most n − 1 edges. An improvement in round n means costs can keep falling: a negative cycle is reachable from the source.</details>
5. In 787, what goes wrong if you relax in place instead of from a copy?
   <details><summary>Answer</summary>An edge processed later in the same round can read a value updated earlier in that round, so one round can extend a path by several edges and the "at most i edges after round i" invariant breaks. The example: k = 0 answers 200 via two flights instead of 500.</details>
6. Why must k be the outermost loop in Floyd-Warshall?
   <details><summary>Answer</summary>Stage k means "paths may use intermediate nodes 0..k", and computing it requires stage k − 1 to be complete for every pair. With k innermost, d[i][j] is finalized from d[i][k] and d[k][j] values that aren't final yet.</details>
7. State the cut property and use it to justify one step of Kruskal.
   <details><summary>Answer</summary>For any split of the nodes, the lightest crossing edge is in some MST (exchange: add it to an MST, remove another crossing edge on the cycle it forms). When Kruskal takes (u, v), consider the cut around u's component: no lighter edge crosses it (it would have merged the component already), so (u, v) is the lightest crossing edge.</details>
8. When is dense O(n²) Prim better than Kruskal?
   <details><summary>Answer</summary>When E ≈ n², e.g. a complete graph of points: Kruskal needs all n²/2 edges in memory and O(n² log n) to sort them, while dense Prim computes edge weights on the fly in O(n²) time and O(n) memory.</details>
9. Give the bridge and articulation-point conditions in terms of tin and low, including the root case.
   <details><summary>Answer</summary>Tree edge u–v is a bridge iff low[v] > tin[u]. A non-root u is an articulation point iff some DFS child v has low[v] ≥ tin[u]. The root is one iff it has at least two DFS children.</details>
10. Why is the condensation of a directed graph always a DAG?
   <details><summary>Answer</summary>If SCCs A and B were on a cycle in the condensation, every node of A could reach every node of B and vice versa, so A ∪ B would be one SCC, contradicting that SCCs are maximal.</details>
