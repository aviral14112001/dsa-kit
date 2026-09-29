// 54. Spiral Matrix: https://leetcode.com/problems/spiral-matrix/
// Pattern: four shrinking boundaries. Module 04 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> spiralOrder(vector<vector<int>>& matrix) {
        vector<int> out;
        int top = 0, bottom = (int)matrix.size() - 1;      // unvisited block: rows [top, bottom]
        int left = 0, right = (int)matrix[0].size() - 1;   //            and columns [left, right]
        while (top <= bottom && left <= right) {
            for (int j = left; j <= right; j++) out.push_back(matrix[top][j]);          // → top row
            top++;
            for (int i = top; i <= bottom; i++) out.push_back(matrix[i][right]);        // ↓ right column
            right--;
            if (top <= bottom) {                                                        // a bottom row remains
                for (int j = right; j >= left; j--) out.push_back(matrix[bottom][j]);   // ← bottom row
                bottom--;
            }
            if (left <= right) {                                                        // a left column remains
                for (int i = bottom; i >= top; i--) out.push_back(matrix[i][left]);     // ↑ left column
                left++;
            }
        }
        return out;
    }
};
// [/snippet]

// The classic bug: the same loop without the two `if` guards. Kept here to prove what it does.
vector<int> spiral_without_guards(const vector<vector<int>>& matrix) {
    vector<int> out;
    int top = 0, bottom = (int)matrix.size() - 1, left = 0, right = (int)matrix[0].size() - 1;
    while (top <= bottom && left <= right) {
        for (int j = left; j <= right; j++) out.push_back(matrix[top][j]);
        top++;
        for (int i = top; i <= bottom; i++) out.push_back(matrix[i][right]);
        right--;
        for (int j = right; j >= left; j--) out.push_back(matrix[bottom][j]);
        bottom--;
        for (int i = bottom; i >= top; i--) out.push_back(matrix[i][left]);
        left++;
    }
    return out;
}

// Brute force: walk like a robot. Go straight until the next cell is off the grid or already
// visited, then turn right. A different algorithm, so it is a good cross-check.
vector<int> spiral_simulate(const vector<vector<int>>& m) {
    int rows = (int)m.size(), cols = (int)m[0].size();
    vector<vector<bool>> seen(rows, vector<bool>(cols, false));
    const int dr[4] = {0, 1, 0, -1}, dc[4] = {1, 0, -1, 0};   // right, down, left, up
    vector<int> out;
    int r = 0, c = 0, d = 0;
    for (int k = 0; k < rows * cols; k++) {
        out.push_back(m[r][c]);
        seen[r][c] = true;
        int nr = r + dr[d], nc = c + dc[d];
        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols || seen[nr][nc]) {
            d = (d + 1) % 4;
            nr = r + dr[d];
            nc = c + dc[d];
        }
        r = nr;
        c = nc;
    }
    return out;
}

int main() {
    Solution sol;
    vector<vector<int>> ex1{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    CHECK_EQ(sol.spiralOrder(ex1), vector<int>{1, 2, 3, 6, 9, 8, 7, 4, 5});
    vector<vector<int>> ex2{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
    CHECK_EQ(sol.spiralOrder(ex2), vector<int>{1, 2, 3, 4, 8, 12, 11, 10, 9, 5, 6, 7});

    vector<vector<int>> row{{1, 2, 3}}, column{{1}, {2}, {3}}, cell{{7}}, tall{{1, 2}, {3, 4}, {5, 6}};
    CHECK_EQ(sol.spiralOrder(row), vector<int>{1, 2, 3});
    CHECK_EQ(sol.spiralOrder(column), vector<int>{1, 2, 3});
    CHECK_EQ(sol.spiralOrder(cell), vector<int>{7});
    CHECK_EQ(sol.spiralOrder(tall), vector<int>{1, 2, 4, 6, 5, 3});
    CHECK_EQ(spiral_without_guards(row), vector<int>{1, 2, 3, 2, 1});      // re-reads the row backwards
    CHECK_EQ(spiral_without_guards(column), vector<int>{1, 2, 3, 2});      // re-reads the column upwards
    CHECK_EQ(spiral_without_guards(ex1), sol.spiralOrder(ex1));            // square inputs hide the bug

    for (int iter = 0; iter < 300; iter++) {
        int rows = (int)t::rand_int(1, 7), cols = (int)t::rand_int(1, 7);
        vector<vector<int>> m(rows);
        for (auto& r : m) r = t::rand_vec(cols, -100, 100);
        CHECK_EQ(sol.spiralOrder(m), spiral_simulate(m));
    }
    return t::summary("0054-spiral-matrix");
}
