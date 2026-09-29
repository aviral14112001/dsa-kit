# 14 · Graphs + BFS + DFS + Topological Sort + DSU
> Model the problem as nodes and edges and half the work is already done.

**Time:** ~15 h of Core work ([problems](problems.md)) · **Prereqs:** modules 09 (queues, stacks), 10 (recursion), 12 (tree DFS/BFS), 13 (heaps, for section 3's smallest-order variant) · **You're done when:** (1) you can write grid BFS, Kahn's algorithm and the DSU struct from a blank file in under 5 minutes each; (2) given a statement, you can say in one sentence whether it needs fewest hops (BFS), reachability or components (DFS/BFS/DSU), an ordering (topological sort) or connectivity under merges (DSU), and why; (3) every Core box is ticked and the four 📖 are re-solved from blank.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Representations | Adjacency list by default; matrix only when dense or given; grids and states are graphs nobody builds | [representations.cpp](examples/representations.cpp) | 1971, 547 |
| 2 | BFS & DFS | BFS = layers = fewest edges; DFS = go deep, backtrack; mark on push; colours find directed cycles | `bfs_dist` in [graph.hpp](../../templates/graph.hpp), [bfs_dfs.cpp](examples/bfs_dfs.cpp) | 733, 200, 994, 542, 130, 694, 785, 127, 126 |
| 3 | Topological sort | Output nodes whose prerequisites are all done; whatever is left sits on or behind a cycle | `topo_sort` in graph.hpp, [topo_sort.cpp](examples/topo_sort.cpp) | 207, 210, 269 |
| 4 | Disjoint set union | Forest of parent pointers; union by size + path compression make every operation ≈ O(1) | [dsu.hpp](../../templates/dsu.hpp), [dsu_variants.cpp](examples/dsu_variants.cpp) | 684, 1319, 827, 305 |

## 1. Representations — adjacency list vs matrix, weighted and directed variants

**Concept.** A graph is a set of nodes (vertices) joined by edges. V and E stand for the node and edge counts in every complexity below. The vocabulary you need:

- **Directed** (u → v only: "follows", "is a prerequisite of") vs **undirected** (u – v both ways: roads, friendships).
- **Weighted** (each edge has a cost) vs **unweighted** (every edge counts as 1).
- **Degree** = edges at a node; directed graphs split it into in-degree and out-degree.
- **Path**, **cycle**, **connected component** (a maximal set of mutually reachable nodes), **DAG** (directed acyclic graph).
- **Tree** = connected and acyclic, which forces exactly n − 1 edges.
- **Sparse** (E ≈ V: 10⁵ nodes, 2·10⁵ edges, the usual input) vs **dense** (E ≈ V²).

Three ways to store one:

| Representation | Memory | Is u–v an edge? | Neighbours of u | Use when |
|---|---|---|---|---|
| Adjacency list `vector<vector<int>>` | O(V + E) | O(deg u) | O(deg u) | The default: BFS, DFS, topo sort, Dijkstra |
| Adjacency matrix `n × n` | O(V²) | O(1) | O(V) | Dense graphs, input given as a matrix, Floyd-Warshall; n up to a few thousand |
| Edge list `vector<array<int, 3>>` | O(E) | O(E) | O(E) | Sorting edges (Kruskal), relaxing all edges (Bellman-Ford), the raw input |

A matrix for n = 10⁴ is 10⁸ cells (400 MB of ints): dead before you start. Traversing a matrix also costs O(V²) however few edges there are.

C# bridge: `vector<vector<int>> adj(n)` is `List<int>[]`, an array of neighbour lists. When nodes are strings (words, emails), `unordered_map<string, vector<string>>` is `Dictionary<string, List<string>>`, but mapping each string to an int id once and using vectors is faster and simpler. And remember `vector` is a value type: a parameter `vector<vector<int>> adj` copies the whole graph on every call. Take `const vector<vector<int>>&`.

Weighted graphs store `(neighbour, weight)` pairs. Directed graphs add one direction; undirected graphs add both, so every edge appears twice and the degrees sum to 2E (the handshake lemma).

**Template.** Build once from the edge list, then every algorithm reads the adjacency list:

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/representations.cpp#build_adj -->
```cpp
// Input usually arrives as an edge list. Build the adjacency list once, then every algorithm reads it.
vector<vector<int>> build_undirected(int n, const vector<pair<int, int>>& edges) {
    vector<vector<int>> adj(n);
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);                            // undirected: store both directions
    }
    return adj;
}

vector<vector<pair<int, int>>> build_directed_weighted(int n, const vector<array<int, 3>>& edges) {
    vector<vector<pair<int, int>>> adj(n);
    for (auto [u, v, w] : edges) adj[u].push_back({v, w});   // (neighbour, weight), one direction only
    return adj;
}

vector<vector<int>> build_matrix(int n, const vector<pair<int, int>>& edges) {
    vector<vector<int>> mat(n, vector<int>(n, 0));      // n^2 cells no matter how few edges
    for (auto [u, v] : edges) mat[u][v] = mat[v][u] = 1;
    return mat;
}
```
<!-- /snippet -->

Online assessments usually send "n m" and then m lines of 1-indexed edges on stdin. Shift to 0-based while reading:

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/representations.cpp#read_graph -->
```cpp
// The usual online-assessment input: "n m", then m lines "u v" numbered from 1. Shift to 0-based on the way in.
// In a real solution: auto adj = read_graph(cin);
vector<vector<int>> read_graph(istream& in) {
    int n, m;
    in >> n >> m;
    vector<vector<int>> adj(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        in >> u >> v;
        u--, v--;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    return adj;
}
```
<!-- /snippet -->

Building is O(V + E) time and memory for lists, O(V²) for the matrix.

**Implicit graphs.** Often nobody hands you a graph: nodes and edges are rules.

- **Grids.** Cell (r, c) is a node; its neighbours are the in-bounds cells at the offsets in `DR`/`DC`. Compute them on the fly instead of building lists:

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/representations.cpp#grid -->
```cpp
// A grid is a graph nobody builds: cell (r, c) is a node and its neighbours are computed on the fly.
const int DR[4] = {-1, 1, 0, 0};   // up, down, left, right
const int DC[4] = {0, 0, -1, 1};   // 8 directions: add the four diagonals (+-1, +-1)

vector<pair<int, int>> open_neighbours(const vector<string>& grid, int r, int c) {
    int rows = grid.size(), cols = grid[0].size();
    vector<pair<int, int>> out;
    for (int d = 0; d < 4; d++) {
        int nr = r + DR[d], nc = c + DC[d];
        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;   // bounds first, then index
        if (grid[nr][nc] == '#') continue;                             // wall
        out.push_back({nr, nc});
    }
    return out;
}
// Need one int per cell (a visited array, a DSU)? id = r * cols + c, and back: r = id / cols, c = id % cols.
```
<!-- /snippet -->

- **State spaces.** A node is a whole configuration (a word, a lock combination, jug contents, (cell, keys held)); an edge is one legal move. BFS over states finds the fewest moves, and only reachable states are ever created:

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/representations.cpp#state_space -->
```cpp
// A state-space graph: jugs of capacity a and b start empty; a move fills one, empties one, or pours one
// into the other until it is empty or the other is full. Fewest moves until either jug holds target?
// Nodes are states (x, y), edges are moves, and BFS finds the fewest moves. -1 if impossible.
int jug_moves(int a, int b, int target) {
    vector<vector<int>> dist(a + 1, vector<int>(b + 1, -1));
    queue<pair<int, int>> q;
    dist[0][0] = 0;
    q.push({0, 0});
    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        if (x == target || y == target) return dist[x][y];
        int x_to_y = min(x, b - y), y_to_x = min(y, a - x);
        pair<int, int> moves[] = {{a, y}, {x, b}, {0, y}, {x, 0},       // fill, empty
                                  {x - x_to_y, y + x_to_y}, {x + y_to_x, y - y_to_x}};   // pour
        for (auto [nx, ny] : moves) {
            if (dist[nx][ny] != -1) continue;
            dist[nx][ny] = dist[x][y] + 1;
            q.push({nx, ny});
        }
    }
    return -1;
}
```
<!-- /snippet -->

`jug_moves(3, 5, 4)` is 6 moves. The cost is bounded by the state count: (a + 1)(b + 1) states × 6 moves each. When you design a state, ask "what do I need to know to decide the next move?"; everything else stays out of the state.

**Pitfalls.**
- An undirected edge stored in one direction only: BFS silently misses half the graph.
- 1-indexed input in a size-n vector: `adj[n]` is out of bounds. ASan reports it here; a judge may just crash or, worse, pass by luck. `u--` on read, or size n + 1.
- `vector<vector<int>> adj;` without the size: `adj[u].push_back(v)` on an empty outer vector is undefined behaviour.
- Passing the graph by value into a recursive function copies it at every call: TLE and memory blowup.
- `unordered_map<int, vector<int>>` when nodes are 0..n−1: slower than a vector, and `adj[u]` on a missing key inserts one.
- Grids: check bounds before indexing. `grid[nr][nc] == '1' && nr >= 0` reads out of bounds first; `&&` short-circuits left to right.
- Duplicate edges and self-loops: harmless for BFS/DFS, but they inflate degree and edge counts, and a duplicated edge (a 2-cycle) slips past the parent check in section 2's undirected cycle test.

**Recognize it when…**
- "n cities and m roads", "connections[i] = [a, b]", "prerequisites[i] = [a, b]": an edge list. Build adjacency lists first.
- An n × n 0/1 matrix like `isConnected[i][j]`: you were handed an adjacency matrix (n is small, ≤ ~200). Row i lists i's neighbours.
- `graph[i]` = list of i's neighbours: already an adjacency list.
- A grid where "adjacent" means up/down/left/right: an implicit grid graph.
- "Transform A into B one step at a time", "fewest operations" with a small state: an implicit state graph and BFS.
- Nodes that are strings or pairs: map them to int ids first.

Section 1 has no worked example: its two practice problems, 1971 and 547, are a build step plus one traversal from section 2. Do them after reading section 2 if BFS and DFS are new to you.

## 2. BFS & DFS — shortest hops, components, cycle detection, bipartite check

**Concept: BFS.** Breadth-first search explores in layers: every node 1 edge away, then every node 2 away, and so on. A FIFO queue does this.

- **Invariant:** the queue holds nodes in nondecreasing distance, with at most two distinct values (d and d + 1). So nodes leave the queue in distance order.
- **Why the first discovery is a shortest path:** any shorter route to v passes through some node with a smaller distance, which left the queue earlier and would have discovered v first.
- This needs every edge to cost the same. With weights, "fewest edges" is not "cheapest": that's module 15.

**Mark visited when you push, not when you pop.** Marked on pop, a node can be queued once by every neighbour that gets popped before it: up to O(E) queue entries instead of O(V) (up to 4 copies of each grid cell), and distances overwritten if you're careless.

**Multi-source BFS:** push every source at distance 0 before the loop. It's BFS from a virtual super-source with an edge to each real source, so dist[v] becomes the distance to the *nearest* source. The template takes a list of sources for exactly this reason:

<!-- snippet: templates/graph.hpp#bfs -->
```cpp
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
```
<!-- /snippet -->

O(V + E) time, O(V) extra space. On a grid there are no adjacency lists; the same loop reads neighbours from direction arrays:

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/bfs_dfs.cpp#grid_bfs -->
```cpp
// Fewest steps from (sr, sc) to every cell of a grid ('#' = wall), moving in 4 directions.
// dist is -1 for walls and unreachable cells. O(rows * cols): each cell is queued at most once.
vector<vector<int>> grid_bfs(const vector<string>& grid, int sr, int sc) {
    int rows = grid.size(), cols = grid[0].size();
    const int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
    vector<vector<int>> dist(rows, vector<int>(cols, -1));
    queue<pair<int, int>> q;
    dist[sr][sc] = 0;
    q.push({sr, sc});
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        for (int d = 0; d < 4; d++) {
            int nr = r + dr[d], nc = c + dc[d];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
            if (grid[nr][nc] == '#' || dist[nr][nc] != -1) continue;   // wall, or reached already
            dist[nr][nc] = dist[r][c] + 1;
            q.push({nr, nc});
        }
    }
    return dist;
}
```
<!-- /snippet -->

O(rows · cols). When you need the level number without a dist array ("how many minutes"), snapshot `int level_size = q.size();` and process exactly that many nodes per level (994 below).

**Concept: DFS.** Depth-first search goes as deep as it can, then backtracks. Recursion is the natural form: the call stack remembers where to resume.

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/bfs_dfs.cpp#dfs_recursive -->
```cpp
// Visit everything reachable from u. Each node is entered once and each adjacency entry scanned once,
// so a full traversal is O(V + E). Recursion depth = length of the current path (up to V).
void dfs(int u, const vector<vector<int>>& adj, vector<bool>& visited, vector<int>& order) {
    visited[u] = true;
    order.push_back(u);                           // pre-order: record on the way in
    for (int v : adj[u])
        if (!visited[v]) dfs(v, adj, visited, order);
}
```
<!-- /snippet -->

- Pre-order (record on entry) vs post-order (record on exit). Post-order drives topological sort (Section 3) and low-link algorithms (module 15).
- DFS gives reachability, components, cycles and orderings, but not shortest paths.

**Recursive vs iterative.** Recursion depth equals the length of the current DFS path, up to V. On a 1000 × 1000 all-land grid a DFS path can run through most of the 10⁶ cells; at tens of bytes per frame that overflows the default stack (a few MB). The symptom is a crash with no output, only on the largest tests. The fix is an explicit stack:

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/bfs_dfs.cpp#components -->
```cpp
// Connected components of an undirected graph: comp[v] = component id, returns the count.
// Iterative, with an explicit stack instead of recursion: safe even on a 1000 x 1000 grid.
int count_components(const vector<vector<int>>& adj, vector<int>& comp) {
    int n = adj.size(), count = 0;
    comp.assign(n, -1);
    for (int s = 0; s < n; s++) {
        if (comp[s] != -1) continue;              // already swallowed by an earlier component
        vector<int> stk{s};
        comp[s] = count;
        while (!stk.empty()) {
            int u = stk.back();
            stk.pop_back();
            for (int v : adj[u])
                if (comp[v] == -1) {              // mark on push: nobody enters the stack twice
                    comp[v] = count;
                    stk.push_back(v);
                }
        }
        count++;
    }
    return count;
}
```
<!-- /snippet -->

This stack version visits the same nodes as recursive DFS, which is all that components and flood fill need, but in a different order and with no post-order. When you need true DFS order iteratively, keep (node, index of the next neighbour) on the stack. The outer loop over every start node is the whole trick for **connected components**: each unvisited node starts a new one, and the total stays O(V + E) because each node is pushed once.

**Cycle detection, undirected.** DFS meets an already-visited neighbour that isn't the node it came from: that's a second route, so a cycle.

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/bfs_dfs.cpp#cycle_undirected -->
```cpp
// Undirected graph without parallel edges: there is a cycle iff DFS reaches an already-visited node
// through an edge other than the one it arrived on.
bool cycle_from(int u, int parent, const vector<vector<int>>& adj, vector<bool>& visited) {
    visited[u] = true;
    for (int v : adj[u]) {
        if (v == parent) continue;                // the edge we came in on is not a cycle
        if (visited[v]) return true;              // a second route to v
        if (cycle_from(v, u, adj, visited)) return true;
    }
    return false;
}

bool has_cycle_undirected(const vector<vector<int>>& adj) {
    vector<bool> visited(adj.size(), false);
    for (int s = 0; s < (int)adj.size(); s++)     // start from every component
        if (!visited[s] && cycle_from(s, -1, adj, visited)) return true;
    return false;
}
```
<!-- /snippet -->

With parallel edges (two u–v edges), skipping "the parent node" skips both, so the 2-cycle is missed. Track the parent *edge id* instead (module 15's bridge template does), or use a DSU (Section 4).

**Cycle detection, directed.** A visited array isn't enough: in the diamond below, DFS reaches 3 twice and there is no cycle. Colour nodes GREY while they are on the current recursion path and BLACK once finished; only an edge into a GREY node points back up the path.

```
0 → 1 → 3      dfs(0) grey → dfs(1) grey → dfs(3) grey, then black; 1 black
 ↘ 2 ↗         dfs(2) grey → edge 2→3 hits BLACK: already explored, not on the path, no cycle
```

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/bfs_dfs.cpp#cycle_directed -->
```cpp
// Directed graph, three colours: WHITE = unvisited, GREY = on the current DFS path, BLACK = finished.
// An edge into a GREY node leads back up the current path: that is a cycle. BLACK nodes are safe to
// meet again (a diamond 0->1->3, 0->2->3 reaches 3 twice without any cycle).
enum Colour { WHITE, GREY, BLACK };

bool cycle_from_directed(int u, const vector<vector<int>>& adj, vector<Colour>& colour) {
    colour[u] = GREY;
    for (int v : adj[u]) {
        if (colour[v] == GREY) return true;       // back edge
        if (colour[v] == WHITE && cycle_from_directed(v, adj, colour)) return true;
    }
    colour[u] = BLACK;                            // everything below u is explored and cycle-free
    return false;
}

bool has_cycle_directed(const vector<vector<int>>& adj) {
    vector<Colour> colour(adj.size(), WHITE);
    for (int s = 0; s < (int)adj.size(); s++)
        if (colour[s] == WHITE && cycle_from_directed(s, adj, colour)) return true;
    return false;
}
```
<!-- /snippet -->

**Bipartite check.** A graph is *bipartite* if its nodes split into two sides with every edge crossing between them (two teams, no rivals on the same team). Theorem: bipartite ⇔ no odd-length cycle.

- **Method:** BFS or DFS with a colour instead of a visited flag. The start gets colour 0; every uncoloured neighbour gets the opposite of the current node's colour; a neighbour that already has the *same* colour is a conflict: not bipartite.
- **Why a conflict proves an odd cycle:** BFS colours by depth parity (even depth = 0, odd = 1). An edge between two same-parity nodes, plus the two tree paths down to them from their common ancestor, closes a cycle of odd length.
- **Disconnected graphs:** start a fresh colouring from every uncoloured node.

785 is where you write it.

**Pitfalls.**
- Marking visited on pop: duplicated queue entries, TLE or MLE on big grids.
- `for (int i = 0; i < q.size(); i++)` to process a level re-reads a growing size (and compares signed with unsigned). Snapshot `level_size` first.
- Recursion on big grids: depth up to rows · cols.
- Forgetting the outer loop over all nodes: only the first component gets handled, and cycles elsewhere go unseen.
- Marking the input grid as visited is fine on LeetCode; say so in an interview ("I'll mark in place; if the input must stay intact, a `visited` array").
- Parent-node skipping with parallel edges misses 2-cycles; two colours instead of three report cycles on diamonds.
- `vector<bool>` packs bits: `auto& x = seen[i];` doesn't compile. Fine for flags, `vector<char>` if you need references.
- BFS on a weighted graph returns the fewest edges, not the cheapest route.

**Recognize it when…**

| The statement says | Reach for |
|---|---|
| "minimum number of steps / moves / transformations", every move costs the same | BFS with a dist array |
| "each minute it spreads to neighbours", several starting cells | multi-source BFS, count levels |
| "distance to the nearest X", for every cell | multi-source BFS from all X at once |
| "number of islands / provinces / groups / regions" | components: DFS, BFS or DSU |
| "regions surrounded by …", "connected to the border" | flood fill that starts from the border |
| "count distinct shapes" | a canonical signature per component (relative coordinates or the DFS move string) |
| "is there a cycle", "can everything finish" (directed) | three colours, or topological sort (Section 3) |
| "split into two groups with no conflicts", "odd cycle" | bipartite 2-colouring |
| word or state transformations, "fewest operations" | BFS over an implicit state graph |
| grid up to 10³ × 10³ | iterative BFS or DFS |

### Worked example: 200. Number of Islands
[LeetCode 200](https://leetcode.com/problems/number-of-islands/) · Medium

**Problem (paraphrased):** a grid of '1' (land) and '0' (water). Count the islands: maximal groups of land cells joined up, down, left or right.
**Signals:** "count groups of connected cells" = connected components on an implicit grid graph. Up to 300 × 300 cells, so linear in the cells.
**Brute force, and why it fails:** for every land cell, BFS its island and check whether it matches an island already found: O((rc)²), 8·10⁹ steps at the limit.
**Key insight:** every still-unvisited land cell starts a new island; flood-fill it at once so none of its cells is ever counted again. Each cell is handled once.
**Dry run:**

```
1 1 0     (0,0) land → island 1; the flood sinks (0,0), (0,1), (1,1)
0 1 0     (0,1), (1,1) already sunk: skipped
1 0 1     (2,0) → island 2    (2,2) → island 3    answer 3 (diagonals don't connect)
```

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/0200-number-of-islands.cpp#solution -->
```cpp
class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        int rows = grid.size(), cols = grid[0].size();
        const int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
        int islands = 0;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (grid[r][c] != '1') continue;
                islands++;                                  // unvisited land: a new island starts here
                grid[r][c] = '0';                           // "sink" visited land so it's never counted again
                vector<pair<int, int>> stk{{r, c}};
                while (!stk.empty()) {
                    auto [cr, cc] = stk.back();
                    stk.pop_back();
                    for (int d = 0; d < 4; d++) {
                        int nr = cr + dr[d], nc = cc + dc[d];
                        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols || grid[nr][nc] != '1') continue;
                        grid[nr][nc] = '0';                 // mark on push: no cell enters the stack twice
                        stk.push_back({nr, nc});
                    }
                }
            }
        }
        return islands;
    }
};
```
<!-- /snippet -->

**Complexity:** O(rows · cols) time: each cell is pushed at most once and checks 4 neighbours. O(rows · cols) space for the stack in the worst case.
**Edge cases:**
- All water → 0; a single cell.
- Diagonal land cells are separate islands; a ring of land around water is one island.
- A 300 × 300 all-land grid: the recursive flood fill (also in the file, as `sink`) recurses about 90,000 deep there. The explicit stack has no depth limit.
**Follow-ups:**
- *Don't modify the input?* A `vector<vector<bool>> seen`; same complexity.
- *Land appears one cell at a time; report the count after each?* That's 305: think DSU (Section 4), not a flood fill per query.
- *The largest island?* Count cells during each flood fill and keep the maximum.

### Worked example: 994. Rotting Oranges
[LeetCode 994](https://leetcode.com/problems/rotting-oranges/) · Medium

**Problem (paraphrased):** a grid of 0 (empty), 1 (fresh) and 2 (rotten). Every minute, each rotten orange rots its fresh 4-neighbours. Return the minutes until nothing is fresh, or −1 if some orange never rots.
**Signals:** "every minute, spreads to adjacent cells" from *several* starting cells at once: multi-source BFS, and minutes are BFS levels.
**Brute force, and why it fails:** simulate minute by minute, rescanning the whole grid each time: up to rc minutes × rc cells = O((rc)²). It passes the 10 × 10 limits, but it's the wrong tool as soon as the grid grows.
**Key insight:** all initially rotten oranges form level 0 of one BFS. The answer is the number of levels needed to reach the last fresh orange; a fresh orange never reached means −1.
**Dry run** (example 1):

```
minute 0     minute 1     minute 2     minute 3     minute 4
2 1 1        2 2 1        2 2 2        2 2 2        2 2 2
1 1 0        2 1 0        2 2 0        2 2 0        2 2 0
0 1 1        0 1 1        0 1 1        0 2 1        0 2 2      fresh = 0 → 4
```

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/0994-rotting-oranges.cpp#solution -->
```cpp
class Solution {
public:
    int orangesRotting(vector<vector<int>>& grid) {
        int rows = grid.size(), cols = grid[0].size();
        queue<pair<int, int>> q;
        int fresh = 0;
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                if (grid[r][c] == 2) q.push({r, c});    // every rotten orange is a source at minute 0
                if (grid[r][c] == 1) fresh++;
            }
        const int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
        int minutes = 0;
        while (!q.empty() && fresh > 0) {
            int level_size = q.size();                  // exactly the oranges that rotted last minute
            for (int i = 0; i < level_size; i++) {
                auto [r, c] = q.front();
                q.pop();
                for (int d = 0; d < 4; d++) {
                    int nr = r + dr[d], nc = c + dc[d];
                    if (nr < 0 || nr >= rows || nc < 0 || nc >= cols || grid[nr][nc] != 1) continue;
                    grid[nr][nc] = 2;                   // rot it now = mark visited on push
                    fresh--;
                    q.push({nr, nc});
                }
            }
            minutes++;                                  // one BFS level = one minute
        }
        return fresh == 0 ? minutes : -1;               // someone fresh was never reached
    }
};
```
<!-- /snippet -->

**Complexity:** O(rows · cols) time and space: each cell enters the queue at most once.
**Edge cases:**
- No fresh orange at the start → 0: the `fresh > 0` condition stops the loop before any minute is counted.
- Fresh oranges but no rotten ones, or a fresh orange walled off by empty cells → −1.
- The last level that rots the final orange counts; no extra minute is added afterwards, again thanks to `fresh > 0`.
**Follow-ups:**
- *When does each cell rot?* Keep a dist grid instead of the level counter.
- *Some cells slow the rot down (different costs per step)?* Weighted: Dijkstra, module 15.
- *Why not DFS from each rotten orange?* DFS explores one path deeply, so the first time it reaches a cell is not the earliest minute; you'd need repeated re-visits. BFS levels are minutes by construction.

## 3. Topological sort — Kahn's algorithm, DFS ordering, dependency resolution

**Concept.** A topological order lists the nodes of a directed graph so that every edge u → v has u before v: every task after its prerequisites. It exists iff the graph is a DAG (a cycle has no node that can go first), and usually there are many.

**Kahn's algorithm.** A node's in-degree counts its unfinished prerequisites. Start with every in-degree-0 node. Repeatedly output one and "finish" it: decrement each successor's in-degree; any that reach 0 become ready.
- **Invariant:** the queue holds exactly the not-yet-output nodes whose prerequisites are all output, so each node comes out after everything it depends on.
- **Cycle detection for free:** a node on a cycle always keeps an in-edge from another unfinished node of the same cycle, so its in-degree never reaches 0, and nothing downstream of it does either. Hence `order.size() < n` ⇔ there is a cycle.

<!-- snippet: templates/graph.hpp#topo_kahn -->
```cpp
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
```
<!-- /snippet -->

O(V + E): each node is queued once and each edge decrements once.

**DFS post-order, reversed.** Push u when its DFS call finishes, then reverse the list. For an edge u → v seen while exploring u: if v is white it finishes inside u's call, so before u; if v is black it finished earlier; if v is grey there is a cycle. In every acyclic case v finishes before u, so after reversing, u comes first.

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/topo_sort.cpp#topo_dfs -->
```cpp
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
```
<!-- /snippet -->

**Lexicographically smallest order.** Swap Kahn's queue for a min-heap. Exchange argument: among the nodes available right now, the smallest must come next, because any order that starts with a larger available node is lexicographically larger, and taking the smallest never blocks the others (they stay available). O((V + E) log V).

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/topo_sort.cpp#topo_lex -->
```cpp
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
```
<!-- /snippet -->

A FIFO queue is *not* the smallest in general: with 1 → 0 and a lone 2, FIFO outputs 1, 2, 0 but the smallest order is 1, 0, 2 (checked in topo_sort.cpp).

| You need | Use |
|---|---|
| Any valid order, or "is it possible?" | Kahn: iterative, the cycle check is a count |
| Work in rounds ("semesters", "minimum number of batches") | Kahn, one queue snapshot per round |
| A DFS you're writing anyway, or post-order | three-colour DFS |
| The smallest (largest) order | Kahn with a min-heap (max-heap) |

**Where it shows up.** Build systems (compile after your imports), package managers (install dependencies first), spreadsheet recalculation, database migrations, CI stages, course planning. C# bridge: Visual Studio refuses a project reference that would create a circular dependency, which is this cycle check. Durations on tasks turn "earliest finish time" into the longest path in a DAG: DP in topological order.

**Pitfalls.**
- Edge direction: `[a, b]` = "b before a" gives edge b → a and increments a's in-degree. Backwards, you sort the reversed graph and return orders in reverse.
- Isolated nodes have in-degree 0 and still belong in the order: seed the queue by looping over all n nodes.
- Returning a partial order on a cycle: check `order.size() == n`.
- DFS topo sort without the grey check returns garbage on cyclic input instead of failing.
- DFS topo sort on a 10⁵-long chain of prerequisites recurses 10⁵ deep; Kahn doesn't recurse at all.
- Duplicate edges are fine in Kahn: each copy increments and later decrements once.
- `priority_queue<int>` is a max-heap: `greater<int>` for the smallest order.

**Recognize it when…**
- "prerequisites", "dependencies", "must happen before", "build order", "schedule the tasks".
- "Is it possible to finish all?" → cycle check. "Return an order" → the order itself.
- "The order of letters in an unknown alphabet, from sorted words" → edges from comparing adjacent words, then a topological sort.
- "Minimum time / semesters to finish everything" → Kahn in rounds, or the longest path in the DAG.
- A DAG plus "longest path" or "number of paths" → DP in topological order.

### Worked example: 207. Course Schedule
[LeetCode 207](https://leetcode.com/problems/course-schedule/) · Medium

**Problem (paraphrased):** `numCourses` courses and pairs [a, b] meaning b must be taken before a. Can every course be completed?
**Signals:** "prerequisites" → a directed graph; "can you finish all of them" → is it acyclic?
**Brute force, and why it fails:** try all n! orderings; or DFS from every node looking for a path back to itself, O(V · (V + E)), about 1.4·10⁷ with n = 2000 and 5000 edges. The first is hopeless and the second is wasteful.
**Key insight:** every course is finishable ⇔ no cycle ⇔ Kahn's algorithm outputs all n nodes. Count the outputs; no order needs storing.
**Dry run:** 4 courses, [[1,0],[2,0],[3,1],[3,2]], so edges 0→1, 0→2, 1→3, 2→3.

| Step | Pop | In-degrees of 0, 1, 2, 3 afterwards | Queue | Taken |
|---|---|---|---|---|
| start | – | 0, 1, 1, 2 | [0] | 0 |
| 1 | 0 | 0, 0, 0, 2 | [1, 2] | 1 |
| 2 | 1 | 0, 0, 0, 1 | [2] | 2 |
| 3 | 2 | 0, 0, 0, 0 | [3] | 3 |
| 4 | 3 | – | [] | 4 = n → true |

Add [0, 3] (edge 3→0) and no course starts at in-degree 0: taken stays 0 → false.

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/0207-course-schedule.cpp#solution -->
```cpp
class Solution {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> adj(numCourses);
        vector<int> indegree(numCourses, 0);
        for (auto& p : prerequisites) {        // [a, b] = "take b before a": edge b -> a
            adj[p[1]].push_back(p[0]);
            indegree[p[0]]++;
        }
        queue<int> ready;                      // courses with every prerequisite already taken
        for (int c = 0; c < numCourses; c++)
            if (indegree[c] == 0) ready.push(c);
        int taken = 0;
        while (!ready.empty()) {
            int c = ready.front();
            ready.pop();
            taken++;
            for (int next : adj[c])
                if (--indegree[next] == 0) ready.push(next);
        }
        return taken == numCourses;            // courses on or behind a cycle never become ready
    }
};
```
<!-- /snippet -->

**Complexity:** O(V + E) time and space.
**Edge cases:**
- No prerequisites → true.
- A course that requires itself, [0, 0] → false (its in-degree never drops).
- A cycle plus courses that depend on it: those are blocked too, and the count exposes them.
- 10⁵ courses in one chain: Kahn has no recursion to overflow (tested in the file).
**Follow-ups:**
- *Return an order?* Output the pop order instead of counting: that's 210.
- *DFS instead?* section 2's three colours; the file's `dfs` region is the same answer recursively.
- *Which courses are stuck?* Exactly the ones Kahn never outputs; to print an actual cycle, use the three-colour DFS and read the grey path when you hit a grey node.

## 4. Disjoint set union — union by rank, path compression, connectivity queries

**Concept.** A DSU (union-find) keeps elements 0..n−1 partitioned into sets under two operations: `unite(a, b)` merges two sets, and `find(x)` returns the representative (root) of x's set, so a and b are together iff `find(a) == find(b)`. Each set is a tree of parent pointers.

1. **Naive:** link one root under the other, either way round. A bad sequence builds a path, and find becomes O(n):

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/dsu_variants.cpp#naive -->
```cpp
// Naive union-find: link one root under the other, whichever way round. A bad sequence of unions
// builds a single long path, and then every find walks all of it: O(n) per operation.
struct NaiveDSU {
    vector<int> parent;
    explicit NaiveDSU(int n) : parent(n) { iota(parent.begin(), parent.end(), 0); }
    int find(int x) {
        while (parent[x] != x) x = parent[x];
        return x;
    }
    void unite(int a, int b) { parent[find(a)] = find(b); }
};
```
<!-- /snippet -->

2. **Union by size (or rank):** hang the smaller tree under the larger. A node only gets deeper when its tree is merged into one at least as large, so its tree size at least doubles each time that happens: depth ≤ log₂ n. dsu_variants.cpp measures it: the union sequence that gives the naive version depth 1999 (n = 2000) gives depth 1 here, and the worst case (merging equal halves) reaches exactly log₂ 1024 = 10. Rank is an upper bound on height instead of a node count, with the same guarantee:

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/dsu_variants.cpp#union_by_rank -->
```cpp
// Union by rank: rank[r] is an upper bound on the height of r's tree (path compression can only
// lower heights, so ranks are never updated by find).
bool unite(int a, int b) {
    a = find(a);
    b = find(b);
    if (a == b) return false;
    if (rnk[a] < rnk[b]) swap(a, b);          // the shorter tree goes under the taller one
    parent[b] = a;
    if (rnk[a] == rnk[b]) rnk[a]++;           // only equal ranks make the result taller
    return true;
}
```
<!-- /snippet -->

3. **Path compression:** once find(x) reaches the root, point every node on the way straight at it. Later finds from those nodes take one hop.

**α(n).** With both tricks, any sequence of m operations costs O(m · α(n)), where α is the inverse Ackermann function. It grows so slowly that α(n) ≤ 4 for any n you could ever store, so treat each operation as O(1) and say "amortized inverse-Ackermann" if pressed. Tarjan proved the bound in 1975, and it is tight for this structure. Either trick alone already gives O(log n) per operation (amortized, for path compression alone).

**Template.** Size is more useful than rank because answers often need component sizes:

<!-- snippet: templates/dsu.hpp#dsu -->
```cpp
struct DSU {
    vector<int> parent, sz;   // sz[r] is only meaningful while r is a root
    int components;           // number of disjoint sets right now

    explicit DSU(int n) : parent(n), sz(n, 1), components(n) {
        iota(parent.begin(), parent.end(), 0);   // every element starts as its own root
    }

    // Root of x's set. Path compression: every node on the walk is re-pointed straight at the root.
    // Union by size keeps trees O(log n) deep, so this recursion is always shallow.
    int find(int x) {
        if (parent[x] == x) return x;
        return parent[x] = find(parent[x]);
    }

    // Merge the sets containing a and b. Returns false if they were already one set
    // (for an edge a-b, that means the edge closes a cycle).
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) swap(a, b);   // hang the smaller tree under the larger root
        parent[b] = a;
        sz[a] += sz[b];
        components--;
        return true;
    }

    bool same(int a, int b) { return find(a) == find(b); }
    int size_of(int x) { return sz[find(x)]; }
};
```
<!-- /snippet -->

`find` recurses, but union by size keeps depth ≤ log₂ n (17 for 10⁵), so that's safe. O(n) space.

**What it's for.**
- **Component counting:** `components` drops by one on every successful `unite`.
- **Cycle detection (undirected):** `unite` returning false means the endpoints were already connected, so the edge closes a cycle (684 below).
- **Kruskal's MST** (module 15 section 3): sort edges by weight, keep each edge whose `unite` succeeds.
- **Grouping by equivalence:** things that share a key (an email, a row, a letter) join one set.
- **Online connectivity:** edges or cells arrive one at a time, with queries in between.

**Extra data per set.** Keep a summary at the root and combine two summaries in `unite`. Size is the built-in example; minimum, maximum, sum and edge counts work the same way. Only the root's value means anything.

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/dsu_variants.cpp#dsu_min -->
```cpp
// Extra data per set lives at the root and is combined in unite. Here: the smallest element of
// each set. Sums, counts, edge totals and max values work exactly the same way.
bool unite(int a, int b) {
    a = find(a);
    b = find(b);
    if (a == b) return false;
    if (sz[a] < sz[b]) swap(a, b);
    parent[b] = a;
    sz[a] += sz[b];
    min_of[a] = min(min_of[a], min_of[b]);   // the surviving root now summarizes both sets
    return true;
}
int smallest_in_set(int x) { return min_of[find(x)]; }
```
<!-- /snippet -->

**DSU or BFS/DFS?** A fixed graph where you need paths or distances: BFS/DFS. Edges arriving over time, many "connected?" queries, or merging groups by shared keys: DSU. A DSU can't split sets (no edge deletion); the usual workaround is to process deletions offline, in reverse, as additions.

**Pitfalls.**
- `parent[a] == parent[b]` instead of `find(a) == find(b)`: parents aren't necessarily roots.
- Reading `sz[x]` for a non-root: call `size_of(x)`, which finds the root first.
- 1-indexed nodes: `DSU dsu(n + 1)`, and remember `components` then counts the unused node 0.
- Keys that aren't small ints (emails, names, coordinates): map them to ids with an `unordered_map` first.
- Path compression without union by size can recurse O(n) deep on the first find of a long chain.
- A DSU tells you *whether* two nodes are connected, never the route between them.

**Recognize it when…**
- "Are x and y in the same group?" asked many times, with merges in between.
- "Merge accounts / groups that share …", "equivalent", "a == b and b == c".
- "Edges (or land cells) are added one at a time; after each addition report …".
- "Which edge creates the cycle / is redundant?"
- "Minimum operations or cables to connect everything" → count components.
- "Minimum cost to connect everything" → Kruskal, module 15.

### Worked example: 684. Redundant Connection
[LeetCode 684](https://leetcode.com/problems/redundant-connection/) · Medium

**Problem (paraphrased):** a tree on nodes 1..n got one extra edge, so there are n edges. Return an edge whose removal leaves a tree; if several work, the one that appears last in the input.
**Signals:** undirected graph with exactly one cycle; "remove one edge to get a tree"; the input order matters.
**Brute force, and why it fails:** remove each edge from the last one backwards and check whether the rest is still connected with BFS: O(n²). With n ≤ 1000 that actually passes, but it doesn't scale and misses the idea being taught.
**Key insight:** add edges to a DSU in input order. The first edge whose endpoints are already connected closes the cycle, and it is the answer: every cycle edge is a valid removal, and this one is the last cycle edge in the input (the others came before it, which is why its endpoints were already joined).
**Dry run:** [[1,2],[1,3],[2,3]]

| Edge | find(a), find(b) before | Action | Sets after |
|---|---|---|---|
| [1,2] | 1, 2 | unite | {1,2} {3} |
| [1,3] | 1, 3 | unite | {1,2,3} |
| [2,3] | 1, 1 | same root → return [2,3] | |

<!-- snippet: modules/14-graphs-traversal-topo-dsu/examples/0684-redundant-connection.cpp#solution -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** O(n · α(n)) ≈ O(n) time, O(n) space.
**Edge cases:**
- The cycle closes before the last input edge: [[1,2],[2,3],[3,1],[3,4]] → [3,1].
- Nodes are 1-indexed: arrays of size n + 1.
**Follow-ups:**
- *Return every edge on the cycle?* After the closing edge (u, v), walk the tree edges from u to v (BFS with parent pointers) and add (u, v).
- *Directed version, where one node may end up with two parents?* Case analysis first. No node has two parents: the answer is the first edge that closes a cycle, as here. Otherwise the two edges into that node are the candidates: run the DSU pass without the later one; if a cycle still appears the answer is the earlier one, otherwise the later one.
- *The edges arrive as a stream too large to store?* The DSU needs O(n) memory, whatever the number of edges.

## Common mistakes

| Mistake | Symptom | Fix |
|---|---|---|
| Marking visited on pop in BFS | TLE/MLE on large grids, repeated work | mark (and set dist) when pushing |
| Recursive DFS on a 10³ × 10³ grid | crashes only on the biggest tests | explicit stack, or BFS |
| Undirected edge stored once | connected nodes reported unreachable | push both directions |
| 1-indexed input in a size-n vector | ASan error, random crash | `u--` on read, or size n + 1 |
| Two colours for directed cycles | "cycle" reported on a diamond | GREY / BLACK |
| No loop over all start nodes | only the first component handled | `for s in 0..n-1: if unvisited, traverse` |
| Prerequisite edges reversed | wrong order, wrong in-degrees | [a, b] means b → a |
| `i < q.size()` for a BFS level | levels bleed into each other | snapshot `level_size` |
| `parent[a] == parent[b]` in a DSU | one group reported as two | `find(a) == find(b)` |
| Graph passed by value into recursion | TLE, memory blowup | `const vector<vector<int>>&` |
| BFS for weighted shortest paths | wrong answer | Dijkstra (module 15) |

## Say it out loud

A talk track for 994 (Rotting Oranges):

1. "The grid has empty, fresh and rotten cells; every minute rot spreads to the 4 neighbours; I need the minutes until nothing is fresh, or −1."
2. "Brute force: simulate minute by minute, rescanning the grid: O((mn)²) in the worst case."
3. "The spread is a BFS where every rotten orange is a source at time 0, so a multi-source BFS; minutes are levels."
4. "I queue all rotten cells and count the fresh ones, then process level by level, rotting and queueing fresh neighbours."
5. "Each cell enters the queue at most once: O(mn) time and space."
6. "Edge cases: no fresh oranges gives 0, an unreachable fresh orange gives −1, and I stop as soon as fresh hits 0 so I never count an extra minute."

Follow-ups interviewers like in this module:
- *BFS or DFS, and why?* BFS for fewest steps; either for reachability; DFS when you need post-order or cycles.
- *What's the recursion depth, and how would you avoid a stack overflow?*
- *Can you do it without modifying the input?*
- *What if edges arrive one at a time?* DSU.
- *Return the actual path / order / cycle, not just a yes or no.* A parent array; Kahn's pop order; the grey path.
- *What changes for a directed graph?* Edge insertion, and the cycle test (three colours or Kahn).
- *State the complexity in V and E, and for grids in rows and cols.*

## Self-check

1. Why must BFS mark a node when it's pushed rather than when it's popped?
   <details><summary>Answer</summary>Marked on pop, a node can be pushed once by every already-visited neighbour before its first pop: O(E) queue entries instead of O(V), and repeated work. Marking on push guarantees each node enters the queue at most once, and its dist is set by the first (shortest) discovery.</details>
2. Why does BFS find shortest paths only when every edge costs the same?
   <details><summary>Answer</summary>Its correctness rests on processing nodes in order of distance, which the FIFO queue delivers only when each edge adds exactly 1. With weights, a path with more edges can be cheaper, and BFS would already have fixed the node's distance by the fewer-edges route.</details>
3. Adjacency matrix or list for n = 10⁵, m = 2·10⁵? What does each cost?
   <details><summary>Answer</summary>A list: O(n + m), about 4·10⁵ neighbour entries (each undirected edge is stored twice). A matrix needs n² = 10¹⁰ cells (tens of GB), and scanning a node's neighbours costs O(n) even though the average degree is 4.</details>
4. Why doesn't a plain visited array detect cycles in a directed graph? What's the fix?
   <details><summary>Answer</summary>Reaching a visited node can just mean two paths lead to it (the diamond 0→1→3, 0→2→3). The fix is three colours: only an edge into a GREY node (one still on the current DFS path) closes a cycle. Or run Kahn's algorithm and check that it outputs all n nodes.</details>
5. In Kahn's algorithm, why can a node on a cycle never be output?
   <details><summary>Answer</summary>Every node on the cycle has an in-edge from its predecessor on the cycle. None of them can be output first, so each keeps in-degree ≥ 1 forever; nodes downstream of the cycle are stuck behind them too.</details>
6. Why is reversed DFS finish order a topological order?
   <details><summary>Answer</summary>For any edge u → v, v finishes before u: if v was unvisited it finishes inside u's call; if it was finished, it finished earlier; if it were on the current path (grey), there would be a cycle. Reversing the finish order therefore puts u before v.</details>
7. With union by size, why is every tree at most log₂ n deep?
   <details><summary>Answer</summary>A node's depth grows only when its tree is hung under a root whose tree is at least as large, so the size of the tree containing it at least doubles each time. Sizes can't exceed n, so that happens at most log₂ n times.</details>
8. What do union by size and path compression give together, and what is α(n)?
   <details><summary>Answer</summary>O(α(n)) amortized per operation, where α is the inverse Ackermann function, at most 4 for any realistic n. Effectively constant; either technique alone gives O(log n).</details>
9. Name two ways to detect a cycle in an undirected graph. What breaks the DFS one?
   <details><summary>Answer</summary>DFS that finds a visited neighbour other than the parent, or a DSU where unite(u, v) finds u and v already connected. With parallel edges, skipping the parent node skips both copies and misses the 2-cycle; skip the parent edge id instead.</details>
10. A 1000 × 1000 grid needs a flood fill. Recursive or iterative, and why?
   <details><summary>Answer</summary>Iterative (an explicit stack or a BFS queue). A DFS path can snake through most of the 10⁶ cells, and that many stack frames overflows the default stack, crashing with no useful error.</details>
