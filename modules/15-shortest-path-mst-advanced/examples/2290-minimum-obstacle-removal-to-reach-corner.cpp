// 2290. Minimum Obstacle Removal to Reach Corner: https://leetcode.com/problems/minimum-obstacle-removal-to-reach-corner/
// Pattern: 0-1 BFS on a grid (stepping onto an obstacle costs 1, onto an empty cell costs 0). Module 15 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
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
// [/snippet]

// Brute force: relax every cell from its neighbours, sweep after sweep, until nothing changes
// (Bellman-Ford on the grid graph). Slow, obviously correct.
int brute(const vector<vector<int>>& grid) {
    int rows = grid.size(), cols = grid[0].size();
    const int BIG = 1'000'000;
    vector<vector<int>> dist(rows, vector<int>(cols, BIG));
    dist[0][0] = 0;
    for (bool changed = true; changed;) {
        changed = false;
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                if (r == 0 && c == 0) continue;
                int best = dist[r][c];
                if (r > 0) best = min(best, dist[r - 1][c] + grid[r][c]);
                if (r + 1 < rows) best = min(best, dist[r + 1][c] + grid[r][c]);
                if (c > 0) best = min(best, dist[r][c - 1] + grid[r][c]);
                if (c + 1 < cols) best = min(best, dist[r][c + 1] + grid[r][c]);
                if (best < dist[r][c]) dist[r][c] = best, changed = true;
            }
    }
    return dist[rows - 1][cols - 1];
}

int main() {
    Solution sol;
    vector<vector<int>> ex1{{0, 1, 1}, {1, 1, 0}, {1, 1, 0}};
    CHECK_EQ(sol.minimumObstacles(ex1), 2);
    vector<vector<int>> ex2{{0, 1, 0, 0, 0}, {0, 1, 0, 1, 0}, {0, 0, 0, 1, 0}};
    CHECK_EQ(sol.minimumObstacles(ex2), 0);       // a free path winds around the obstacles
    vector<vector<int>> single{{0}};
    CHECK_EQ(sol.minimumObstacles(single), 0);
    vector<vector<int>> wall{{0, 1, 0}, {0, 1, 0}, {0, 1, 0}};
    CHECK_EQ(sol.minimumObstacles(wall), 1);      // a full column of obstacles: remove exactly one
    vector<vector<int>> detour{{0, 1, 0, 0}, {0, 1, 0, 1}, {0, 0, 0, 1}, {1, 1, 1, 0}};
    CHECK_EQ(sol.minimumObstacles(detour), 1);    // more steps, fewer removals: step count doesn't matter

    for (int iter = 0; iter < 400; iter++) {
        int rows = (int)t::rand_int(1, 7), cols = (int)t::rand_int(1, 7);
        vector<vector<int>> g(rows, vector<int>(cols));
        for (auto& row : g)
            for (int& cell : row) cell = (int)t::rand_int(0, 1);
        g[0][0] = g[rows - 1][cols - 1] = 0;      // the problem guarantees both corners are empty
        CHECK_EQ(sol.minimumObstacles(g), brute(g));
    }
    // the upper limit: m * n <= 10^5 cells
    vector<vector<int>> big(316, vector<int>(316));
    for (int r = 0; r < 316; r++)
        for (int c = 0; c < 316; c++) big[r][c] = (r + c) % 2;   // checkerboard: every other step costs 1
    big[315][315] = 0;
    CHECK_EQ(sol.minimumObstacles(big), 315);
    return t::summary("2290-minimum-obstacle-removal-to-reach-corner");
}
