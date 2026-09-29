// Graph representations: edge list -> adjacency list / matrix, reading 1-indexed input, grids and state
// spaces as implicit graphs. Module 14 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:build_adj]
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
// [/snippet]

// [snippet:read_graph]
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
// [/snippet]

// [snippet:grid]
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
// [/snippet]

// [snippet:state_space]
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
// [/snippet]

// Brute force for jug_moves: grow the set of states reachable in <= k moves, one round at a time,
// until it contains a target state or stops growing. No queue, no dist array: obviously correct.
int jug_moves_brute(int a, int b, int target) {
    set<pair<int, int>> reached{{0, 0}};
    for (int k = 0;; k++) {
        for (auto [x, y] : reached)
            if (x == target || y == target) return k;
        set<pair<int, int>> next = reached;
        for (auto [x, y] : reached) {
            int x_to_y = min(x, b - y), y_to_x = min(y, a - x);
            for (auto s : {pair{a, y}, pair{x, b}, pair{0, y}, pair{x, 0}, pair{x - x_to_y, y + x_to_y},
                           pair{x + y_to_x, y - y_to_x}})
                next.insert(s);
        }
        if (next == reached) return -1;   // nothing new: target is unreachable
        reached = next;
    }
}

int main() {
    // ---- the three representations agree ----
    vector<pair<int, int>> edges{{0, 1}, {0, 2}, {1, 2}, {2, 3}};
    auto adj = build_undirected(5, edges);
    CHECK_EQ(adj, vector<vector<int>>{{1, 2}, {0, 2}, {0, 1, 3}, {2}, {}});
    auto mat = build_matrix(5, edges);
    for (int u = 0; u < 5; u++)
        for (int v = 0; v < 5; v++)
            CHECK_EQ(mat[u][v] == 1, count(adj[u].begin(), adj[u].end(), v) == 1);
    int degree_sum = 0;
    for (auto& nb : adj) degree_sum += nb.size();
    CHECK_EQ(degree_sum, 2 * (int)edges.size());   // handshake lemma: every undirected edge counted twice

    auto wadj = build_directed_weighted(3, {{0, 1, 5}, {1, 2, 7}, {0, 2, 20}});
    CHECK_EQ(wadj[0], vector<pair<int, int>>{{1, 5}, {2, 20}});
    CHECK(wadj[2].empty());   // directed: no edge back

    istringstream input("4 3\n1 2\n2 3\n4 1\n");
    CHECK_EQ(read_graph(input), vector<vector<int>>{{1, 3}, {0, 2}, {1}, {0}});

    // ---- grids ----
    vector<string> grid{"..#",
                        ".#.",
                        "..."};
    CHECK_EQ(open_neighbours(grid, 0, 0), vector<pair<int, int>>{{1, 0}, {0, 1}});
    CHECK_EQ(open_neighbours(grid, 0, 1), vector<pair<int, int>>{{0, 0}});   // wall below and right
    CHECK_EQ(open_neighbours(grid, 2, 2), vector<pair<int, int>>{{1, 2}, {2, 1}});
    int cols = 3, id = 2 * cols + 1;
    CHECK_EQ(make_pair(id / cols, id % cols), make_pair(2, 1));

    // ---- state space ----
    CHECK_EQ(jug_moves(3, 5, 4), 6);   // fill 5, pour, empty 3, pour, fill 5, pour
    CHECK_EQ(jug_moves(3, 5, 0), 0);
    CHECK_EQ(jug_moves(2, 4, 3), -1);  // every reachable amount is even
    CHECK_EQ(jug_moves(3, 5, 5), 1);
    for (int iter = 0; iter < 300; iter++) {
        int a = (int)t::rand_int(1, 7), b = (int)t::rand_int(1, 7), target = (int)t::rand_int(0, max(a, b) + 1);
        int got = jug_moves(a, b, target);
        CHECK_EQ(got, jug_moves_brute(a, b, target));
        // number theory cross-check: a jug can hold target iff target <= max(a, b) and gcd(a, b) divides it
        CHECK_EQ(got != -1, target <= max(a, b) && target % gcd(a, b) == 0);
    }
    return t::summary("14 representations");
}
