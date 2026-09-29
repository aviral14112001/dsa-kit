// 200. Number of Islands: https://leetcode.com/problems/number-of-islands/
// Pattern: connected components on a grid (flood fill with an explicit stack). Module 14 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
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
// [/snippet]

// [snippet:recursive]
// The same flood fill, recursively. Shorter, but a 300 x 300 all-land grid recurses 90,000 deep.
void sink(vector<vector<char>>& grid, int r, int c) {
    if (r < 0 || r >= (int)grid.size() || c < 0 || c >= (int)grid[0].size() || grid[r][c] != '1') return;
    grid[r][c] = '0';
    sink(grid, r + 1, c);
    sink(grid, r - 1, c);
    sink(grid, r, c + 1);
    sink(grid, r, c - 1);
}
// [/snippet]

int islands_recursive(vector<vector<char>> grid) {
    int count = 0;
    for (int r = 0; r < (int)grid.size(); r++)
        for (int c = 0; c < (int)grid[0].size(); c++)
            if (grid[r][c] == '1') {
                count++;
                sink(grid, r, c);
            }
    return count;
}

// Brute force for the stress test: every land cell starts with its own label; repeatedly take the
// minimum label over 4-neighbours until nothing changes; count distinct labels. Slow, obviously correct.
int brute(const vector<vector<char>>& grid) {
    int rows = grid.size(), cols = grid[0].size();
    vector<vector<int>> label(rows, vector<int>(cols, -1));
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (grid[r][c] == '1') label[r][c] = r * cols + c;
    for (bool changed = true; changed;) {
        changed = false;
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                if (label[r][c] < 0) continue;
                int best = label[r][c];
                if (r > 0 && label[r - 1][c] >= 0) best = min(best, label[r - 1][c]);
                if (r + 1 < rows && label[r + 1][c] >= 0) best = min(best, label[r + 1][c]);
                if (c > 0 && label[r][c - 1] >= 0) best = min(best, label[r][c - 1]);
                if (c + 1 < cols && label[r][c + 1] >= 0) best = min(best, label[r][c + 1]);
                if (best < label[r][c]) label[r][c] = best, changed = true;
            }
    }
    set<int> distinct;
    for (auto& row : label)
        for (int l : row)
            if (l >= 0) distinct.insert(l);
    return distinct.size();
}

vector<vector<char>> to_grid(const vector<string>& rows) {
    vector<vector<char>> g;
    for (auto& s : rows) g.push_back(vector<char>(s.begin(), s.end()));
    return g;
}

int main() {
    Solution sol;
    auto ex1 = to_grid({"11110",
                        "11010",
                        "11000",
                        "00000"});
    CHECK_EQ(sol.numIslands(ex1), 1);
    auto ex2 = to_grid({"11000",
                        "11000",
                        "00100",
                        "00011"});
    CHECK_EQ(sol.numIslands(ex2), 3);
    auto zero = to_grid({"0"});
    CHECK_EQ(sol.numIslands(zero), 0);
    auto one = to_grid({"1"});
    CHECK_EQ(sol.numIslands(one), 1);
    auto diagonal = to_grid({"101",
                             "010",
                             "101"});             // diagonal cells are not connected
    CHECK_EQ(sol.numIslands(diagonal), 5);
    auto ring = to_grid({"111",
                         "101",
                         "111"});                 // a ring around water is still one island
    CHECK_EQ(sol.numIslands(ring), 1);
    vector<vector<char>> all_land(300, vector<char>(300, '1'));   // the largest grid LeetCode allows
    CHECK_EQ(sol.numIslands(all_land), 1);

    for (int iter = 0; iter < 300; iter++) {
        int rows = (int)t::rand_int(1, 8), cols = (int)t::rand_int(1, 8);
        vector<vector<char>> g(rows, vector<char>(cols));
        for (auto& row : g)
            for (auto& cell : row) cell = t::rand_int(0, 1) ? '1' : '0';
        int expect = brute(g);
        CHECK_EQ(islands_recursive(g), expect);
        CHECK_EQ(sol.numIslands(g), expect);    // last: it sinks the grid
    }
    return t::summary("0200-number-of-islands");
}
