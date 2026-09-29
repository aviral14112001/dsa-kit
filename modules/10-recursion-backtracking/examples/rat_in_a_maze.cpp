// Rat in a maze: list every path from the top-left to the bottom-right cell of an n x n grid
// (1 = open, 0 = wall), moving D/L/R/U and never revisiting a cell within one path.
// Pattern: grid backtracking with direction arrays + in-place marking and restoring. Module 10 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:rat_in_a_maze]
// Directions listed in alphabetical order of their letters, so paths come out sorted.
const int DR[4] = {1, 0, 0, -1};
const int DC[4] = {0, -1, 1, 0};
const char DIR[4] = {'D', 'L', 'R', 'U'};

void explore(vector<vector<int>>& grid, int r, int c, string& path, vector<string>& paths) {
    int n = grid.size();
    if (r == n - 1 && c == n - 1) {                  // reached the exit
        paths.push_back(path);
        return;
    }
    grid[r][c] = 0;                                  // mark: this cell is on the current path
    for (int d = 0; d < 4; d++) {
        int nr = r + DR[d], nc = c + DC[d];
        if (nr < 0 || nr >= n || nc < 0 || nc >= n || grid[nr][nc] == 0) continue;   // bounds first
        path.push_back(DIR[d]);
        explore(grid, nr, nc, path, paths);
        path.pop_back();
    }
    grid[r][c] = 1;                                  // restore: other paths may pass through here
}

vector<string> ratInAMaze(vector<vector<int>> grid) {   // by value: we mark cells in our own copy
    int n = grid.size();
    vector<string> paths;
    if (n == 0 || grid[0][0] == 0 || grid[n - 1][n - 1] == 0) return paths;
    string path;
    explore(grid, 0, 0, path, paths);
    return paths;
}
// [/snippet]

// Independent check: an iterative DFS whose states carry (cell, visited bitmask, path), so there is
// no marking to undo at all. Different code, same answer.
vector<string> brute(const vector<vector<int>>& grid) {
    int n = grid.size();
    vector<string> out;
    if (grid[0][0] == 0 || grid[n - 1][n - 1] == 0) return out;
    struct State { int r, c; long long visited; string path; };
    vector<State> stack = {{0, 0, 1LL, ""}};
    const int dr[4] = {1, 0, 0, -1}, dc[4] = {0, -1, 1, 0};
    const string letters = "DLRU";
    while (!stack.empty()) {
        State s = stack.back();
        stack.pop_back();
        if (s.r == n - 1 && s.c == n - 1) {
            out.push_back(s.path);
            continue;
        }
        for (int d = 0; d < 4; d++) {
            int nr = s.r + dr[d], nc = s.c + dc[d];
            if (nr < 0 || nr >= n || nc < 0 || nc >= n || grid[nr][nc] == 0) continue;
            long long bit = 1LL << (nr * n + nc);
            if (s.visited & bit) continue;
            stack.push_back({nr, nc, s.visited | bit, s.path + letters[d]});
        }
    }
    sort(out.begin(), out.end());
    return out;
}

int main() {
    vector<vector<int>> maze = {{1, 1, 0},
                                {1, 1, 1},
                                {0, 1, 1}};
    CHECK_EQ(ratInAMaze(maze), vector<string>{"DRDR", "DRRD", "RDDR", "RDRD"});

    vector<vector<int>> maze2 = {{1, 0, 0, 0},
                                 {1, 1, 1, 0},
                                 {0, 0, 1, 0},
                                 {0, 1, 1, 1}};
    CHECK_EQ(ratInAMaze(maze2), vector<string>{"DRRDDR"});        // a single corridor

    CHECK(ratInAMaze({{0, 1}, {1, 1}}).empty());                   // start blocked
    CHECK(ratInAMaze({{1, 1}, {1, 0}}).empty());                   // exit blocked
    CHECK(ratInAMaze({{1, 0}, {0, 1}}).empty());                   // no route
    CHECK_EQ(ratInAMaze({{1}}), vector<string>{""});               // already at the exit: one empty path

    // Open n x n grids: the number of self-avoiding corner-to-corner paths is 1, 2, 12, 184, 8512.
    const size_t openCount[6] = {0, 1, 2, 12, 184, 8512};
    for (int n = 1; n <= 5; n++) {
        auto paths = ratInAMaze(vector<vector<int>>(n, vector<int>(n, 1)));
        CHECK_EQ(paths.size(), openCount[n]);
        CHECK(is_sorted(paths.begin(), paths.end()));             // DLRU order gives sorted output
    }

    // The restore invariant: after explore() returns, the grid is exactly as it was.
    auto grid = maze;
    string path;
    vector<string> paths;
    explore(grid, 0, 0, path, paths);
    CHECK_EQ(grid, maze);
    CHECK(path.empty());

    for (int iter = 0; iter < 300; iter++) {                       // stress test vs the iterative DFS
        int n = (int)t::rand_int(1, 4);
        vector<vector<int>> g(n, vector<int>(n));
        for (auto& row : g)
            for (int& cell : row) cell = t::rand_int(1, 10) <= 7 ? 1 : 0;
        CHECK_EQ(ratInAMaze(g), brute(g));
    }
    return t::summary("rat_in_a_maze");
}
