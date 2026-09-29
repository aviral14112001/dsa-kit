// BFS and DFS building blocks: recursive DFS, iterative components, cycle detection (undirected and
// directed), and the grid BFS template. Module 14 section 2. The plain BFS template is bfs_dist() in graph.hpp.
#include <bits/stdc++.h>
#include "test.hpp"
#include "graph.hpp"   // bfs_dist and DSU, used as independent cross-checks below
using namespace std;

// [snippet:dfs_recursive]
// Visit everything reachable from u. Each node is entered once and each adjacency entry scanned once,
// so a full traversal is O(V + E). Recursion depth = length of the current path (up to V).
void dfs(int u, const vector<vector<int>>& adj, vector<bool>& visited, vector<int>& order) {
    visited[u] = true;
    order.push_back(u);                           // pre-order: record on the way in
    for (int v : adj[u])
        if (!visited[v]) dfs(v, adj, visited, order);
}
// [/snippet]

// [snippet:components]
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
// [/snippet]

// [snippet:cycle_undirected]
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
// [/snippet]

// [snippet:cycle_directed]
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
// [/snippet]

// [snippet:grid_bfs]
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
// [/snippet]

// ---------------------------------------------------------------- test helpers

vector<vector<int>> random_simple_undirected(int n, int m) {
    set<pair<int, int>> used;
    vector<vector<int>> adj(n);
    for (int tries = 0; tries < 4 * m && (int)used.size() < m; tries++) {
        int u = (int)t::rand_int(0, n - 1), v = (int)t::rand_int(0, n - 1);
        if (u == v || used.count({min(u, v), max(u, v)})) continue;
        used.insert({min(u, v), max(u, v)});
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    return adj;
}

// Directed cycle brute force: some node reaches itself by a path of >= 1 edge (Floyd-style closure).
bool has_cycle_directed_brute(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<vector<bool>> path(n, vector<bool>(n, false));   // path[i][j]: i reaches j with >= 1 edge
    for (int u = 0; u < n; u++)
        for (int v : adj[u]) path[u][v] = true;
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (path[i][k] && path[k][j]) path[i][j] = true;
    for (int i = 0; i < n; i++)
        if (path[i][i]) return true;
    return false;
}

int main() {
    // ---- DFS order and components on a fixed graph ----
    //  0 - 1 - 2      3 - 4      5
    vector<vector<int>> adj{{1}, {0, 2}, {1}, {4}, {3}, {}};
    vector<bool> visited(6, false);
    vector<int> order;
    dfs(1, adj, visited, order);
    CHECK_EQ(order, vector<int>{1, 0, 2});
    vector<int> comp;
    CHECK_EQ(count_components(adj, comp), 3);
    CHECK_EQ(comp, vector<int>{0, 0, 0, 1, 1, 2});

    // ---- cycles ----
    CHECK(!has_cycle_undirected(adj));                         // a forest
    CHECK(has_cycle_undirected({{1, 2}, {0, 2}, {0, 1}}));     // triangle
    CHECK(has_cycle_undirected({{0, 0}}));                     // a self-loop appears twice in adj[0]
    CHECK(!has_cycle_directed({{1, 2}, {3}, {3}, {}}));        // diamond: 3 is reached twice, no cycle
    CHECK(has_cycle_directed({{1}, {2}, {0}}));
    CHECK(has_cycle_directed({{0}}));
    CHECK(!has_cycle_directed({}));

    // ---- grid BFS ----
    vector<string> grid{"S..#",
                        ".#..",
                        "...#"};
    auto dist = grid_bfs(grid, 0, 0);
    CHECK_EQ(dist, vector<vector<int>>{{0, 1, 2, -1}, {1, -1, 3, 4}, {2, 3, 4, -1}});
    auto from_right = grid_bfs(grid, 1, 3);
    CHECK_EQ(from_right, vector<vector<int>>{{4, 3, 2, -1}, {5, -1, 1, 0}, {4, 3, 2, -1}});   // around the wall
    CHECK_EQ(grid_bfs({"."}, 0, 0), vector<vector<int>>{{0}});

    // ---- stress ----
    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(1, 12);
        auto g = random_simple_undirected(n, (int)t::rand_int(0, 18));
        int m = 0;
        for (auto& nb : g) m += nb.size();
        m /= 2;
        // components vs DSU over the same edges
        DSU dsu(n);
        for (int u = 0; u < n; u++)
            for (int v : g[u]) dsu.unite(u, v);
        vector<int> c;
        int k = count_components(g, c);
        CHECK_EQ(k, dsu.components);
        for (int u = 0; u < n; u++)
            for (int v = 0; v < n; v++) CHECK_EQ(c[u] == c[v], dsu.same(u, v));
        // recursive DFS reaches exactly the start's component
        vector<bool> seen(n, false);
        vector<int> reached;
        int s = (int)t::rand_int(0, n - 1);
        dfs(s, g, seen, reached);
        CHECK_EQ((int)reached.size(), dsu.size_of(s));
        // simple undirected graph: acyclic iff it is a forest, i.e. m == n - components
        CHECK_EQ(has_cycle_undirected(g), m > n - k);
    }
    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(1, 9);
        vector<vector<int>> g(n);
        int m = (int)t::rand_int(0, 14);
        for (int i = 0; i < m; i++) g[t::rand_int(0, n - 1)].push_back((int)t::rand_int(0, n - 1));
        CHECK_EQ(has_cycle_directed(g), has_cycle_directed_brute(g));
    }
    // grid BFS vs graph.hpp's bfs_dist on the explicit graph (cell id = r * cols + c)
    for (int iter = 0; iter < 200; iter++) {
        int rows = (int)t::rand_int(1, 7), cols = (int)t::rand_int(1, 7);
        vector<string> g(rows, string(cols, '.'));
        for (auto& row : g)
            for (auto& cell : row)
                if (t::rand_int(0, 3) == 0) cell = '#';
        int sr = (int)t::rand_int(0, rows - 1), sc = (int)t::rand_int(0, cols - 1);
        g[sr][sc] = '.';                            // the start is never a wall
        vector<vector<int>> cells(rows * cols);
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                if (g[r][c] == '#') continue;
                if (r + 1 < rows && g[r + 1][c] == '.') {
                    cells[r * cols + c].push_back((r + 1) * cols + c);
                    cells[(r + 1) * cols + c].push_back(r * cols + c);
                }
                if (c + 1 < cols && g[r][c + 1] == '.') {
                    cells[r * cols + c].push_back(r * cols + c + 1);
                    cells[r * cols + c + 1].push_back(r * cols + c);
                }
            }
        auto expect = bfs_dist(cells, {sr * cols + sc});
        auto got = grid_bfs(g, sr, sc);
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) CHECK_EQ(got[r][c], expect[r * cols + c]);
    }
    // a 1000 x 1000 open grid: 10^6 nodes, fine for the iterative versions
    {
        int n = 1000;
        vector<string> big(n, string(n, '.'));
        auto d = grid_bfs(big, 0, 0);
        CHECK_EQ(d[n - 1][n - 1], 2 * (n - 1));
        vector<vector<int>> path(n * 10);   // a path graph with 10^4 nodes, one component
        for (int i = 0; i + 1 < n * 10; i++) {
            path[i].push_back(i + 1);
            path[i + 1].push_back(i);
        }
        vector<int> c;
        CHECK_EQ(count_components(path, c), 1);
    }
    return t::summary("14 bfs_dfs");
}
