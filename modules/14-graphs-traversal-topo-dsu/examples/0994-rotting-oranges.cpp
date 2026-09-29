// 994. Rotting Oranges: https://leetcode.com/problems/rotting-oranges/
// Pattern: multi-source BFS, one level per minute. Module 14 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
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
// [/snippet]

// Brute force: simulate minute by minute from a snapshot until nothing changes.
int brute(vector<vector<int>> grid) {
    int rows = grid.size(), cols = grid[0].size();
    for (int minute = 0;; minute++) {
        auto next = grid;
        bool changed = false;
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                if (grid[r][c] != 1) continue;
                bool touches_rotten = (r > 0 && grid[r - 1][c] == 2) || (r + 1 < rows && grid[r + 1][c] == 2) ||
                                      (c > 0 && grid[r][c - 1] == 2) || (c + 1 < cols && grid[r][c + 1] == 2);
                if (touches_rotten) next[r][c] = 2, changed = true;
            }
        if (!changed) {
            for (auto& row : grid)
                for (int cell : row)
                    if (cell == 1) return -1;
            return minute;
        }
        grid = next;
    }
}

int main() {
    Solution sol;
    vector<vector<int>> ex1{{2, 1, 1}, {1, 1, 0}, {0, 1, 1}};
    CHECK_EQ(sol.orangesRotting(ex1), 4);
    vector<vector<int>> ex2{{2, 1, 1}, {0, 1, 1}, {1, 0, 1}};
    CHECK_EQ(sol.orangesRotting(ex2), -1);   // the bottom-left orange is cut off
    vector<vector<int>> ex3{{0, 2}};
    CHECK_EQ(sol.orangesRotting(ex3), 0);    // nothing fresh: zero minutes
    vector<vector<int>> empty{{0}};
    CHECK_EQ(sol.orangesRotting(empty), 0);
    vector<vector<int>> no_rotten{{1, 1}};
    CHECK_EQ(sol.orangesRotting(no_rotten), -1);
    vector<vector<int>> two_sources{{2, 1, 1, 1, 2}};
    CHECK_EQ(sol.orangesRotting(two_sources), 2);   // both ends rot inward at the same time

    for (int iter = 0; iter < 400; iter++) {
        int rows = (int)t::rand_int(1, 6), cols = (int)t::rand_int(1, 6);
        vector<vector<int>> g(rows, vector<int>(cols));
        for (auto& row : g)
            for (int& cell : row) cell = (int)t::rand_int(0, 2);   // empty, fresh or rotten
        int expect = brute(g);
        CHECK_EQ(sol.orangesRotting(g), expect);
    }
    return t::summary("0994-rotting-oranges");
}
