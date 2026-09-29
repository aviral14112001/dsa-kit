// Module 04 section 2: index maths on grids (ids, bounds, direction arrays, diagonals, rings),
// each checked against a brute force.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:index_math]
// Row-major numbering: cell (i, j) of a grid with `cols` columns gets the id i * cols + j.
// One int per cell is handy for visited arrays, DSU, hash keys, or treating a sorted matrix as
// one sorted array.
int to_id(int i, int j, int cols) { return i * cols + j; }
pair<int, int> to_cell(int id, int cols) { return {id / cols, id % cols}; }

bool in_bounds(int i, int j, int rows, int cols) { return 0 <= i && i < rows && 0 <= j && j < cols; }

// The 4 neighbours, clockwise from up: (i + DR[d], j + DC[d]) for d = 0..3.
const int DR[4] = {-1, 0, 1, 0};
const int DC[4] = {0, 1, 0, -1};

// Example: count the cells that are strictly greater than every neighbour that exists.
int count_peaks(const vector<vector<int>>& g) {
    int rows = (int)g.size(), cols = (int)g[0].size(), peaks = 0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            bool peak = true;
            for (int d = 0; d < 4; d++) {
                int ni = i + DR[d], nj = j + DC[d];
                if (in_bounds(ni, nj, rows, cols) && g[ni][nj] >= g[i][j]) peak = false;
            }
            peaks += peak;
        }
    return peaks;
}
// [/snippet]

// [snippet:diagonals]
// Along a "\" diagonal i - j is constant; along a "/" anti-diagonal i + j is constant.
// i + j runs 0 .. rows + cols - 2. i - j runs -(cols - 1) .. rows - 1, so shift it by cols - 1
// before using it as an index.
pair<vector<long long>, vector<long long>> diagonal_sums(const vector<vector<int>>& g) {
    int rows = (int)g.size(), cols = (int)g[0].size();
    vector<long long> anti(rows + cols - 1, 0), diag(rows + cols - 1, 0);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            anti[i + j] += g[i][j];
            diag[i - j + cols - 1] += g[i][j];
        }
    return {anti, diag};
}

// Which spiral ring (layer) a cell is on: its distance to the nearest edge.
int ring_of(int i, int j, int rows, int cols) { return min({i, j, rows - 1 - i, cols - 1 - j}); }
// [/snippet]

// ---------- brute forces ----------
int count_peaks_brute(const vector<vector<int>>& g) {
    int rows = (int)g.size(), cols = (int)g[0].size(), peaks = 0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            bool peak = true;
            if (i > 0 && g[i - 1][j] >= g[i][j]) peak = false;
            if (i + 1 < rows && g[i + 1][j] >= g[i][j]) peak = false;
            if (j > 0 && g[i][j - 1] >= g[i][j]) peak = false;
            if (j + 1 < cols && g[i][j + 1] >= g[i][j]) peak = false;
            peaks += peak;
        }
    return peaks;
}

pair<vector<long long>, vector<long long>> diagonal_sums_brute(const vector<vector<int>>& g) {
    // Walk each diagonal from its first cell: "/" ones start on the top row or the right column,
    // "\" ones start on the top row or the left column.
    int rows = (int)g.size(), cols = (int)g[0].size();
    vector<long long> anti, diag;
    for (int s = 0; s < rows + cols - 1; s++) {                 // anti-diagonal i + j == s
        long long sum = 0;
        int i = max(0, s - (cols - 1)), j = s - i;
        for (; i < rows && j >= 0; i++, j--) sum += g[i][j];
        anti.push_back(sum);
    }
    for (int start = cols - 1; start >= 0; start--) {           // starts on the top row, right to left
        long long sum = 0;
        for (int i = 0, j = start; i < rows && j < cols; i++, j++) sum += g[i][j];
        diag.push_back(sum);
    }
    for (int start = 1; start < rows; start++) {                // then down the left column
        long long sum = 0;
        for (int i = start, j = 0; i < rows && j < cols; i++, j++) sum += g[i][j];
        diag.push_back(sum);
    }
    return {anti, diag};
}

vector<vector<int>> rings_brute(int rows, int cols) {          // peel boundary layers one at a time
    vector<vector<int>> ring(rows, vector<int>(cols, -1));
    for (int layer = 0;; layer++) {
        int top = layer, left = layer, bottom = rows - 1 - layer, right = cols - 1 - layer;
        if (top > bottom || left > right) break;
        for (int i = top; i <= bottom; i++)
            for (int j = left; j <= right; j++)
                if ((i == top || i == bottom || j == left || j == right) && ring[i][j] == -1) ring[i][j] = layer;
    }
    return ring;
}

int main() {
    CHECK_EQ(to_id(2, 3, 5), 13);
    CHECK_EQ(to_cell(13, 5), make_pair(2, 3));
    for (int rows = 1; rows <= 5; rows++)
        for (int cols = 1; cols <= 5; cols++)
            for (int id = 0; id < rows * cols; id++) {
                auto [i, j] = to_cell(id, cols);
                CHECK(in_bounds(i, j, rows, cols));
                CHECK_EQ(to_id(i, j, cols), id);
            }
    CHECK(!in_bounds(-1, 0, 3, 3));
    CHECK(!in_bounds(0, 3, 3, 3));

    vector<vector<int>> g{{1, 5, 2},
                          {4, 3, 6}};
    CHECK_EQ(count_peaks(g), 3);                                 // the 5, the 4 and the 6
    auto [anti, diag] = diagonal_sums(g);
    CHECK_EQ(anti, vector<long long>{1, 9, 5, 6});               // i+j = 0: 1 | 1: 5+4 | 2: 2+3 | 3: 6
    CHECK_EQ(diag, vector<long long>{2, 11, 4, 4});              // i-j = -2: 2 | -1: 5+6 | 0: 1+3 | 1: 4
    CHECK_EQ(ring_of(1, 1, 3, 3), 1);
    CHECK_EQ(ring_of(0, 2, 3, 3), 0);

    for (int iter = 0; iter < 300; iter++) {
        int rows = (int)t::rand_int(1, 7), cols = (int)t::rand_int(1, 7);
        vector<vector<int>> m(rows);
        for (auto& row : m) row = t::rand_vec(cols, 0, 9);
        CHECK_EQ(count_peaks(m), count_peaks_brute(m));
        CHECK_EQ(diagonal_sums(m), diagonal_sums_brute(m));
        auto ring = rings_brute(rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++) CHECK_EQ(ring_of(i, j, rows, cols), ring[i][j]);
    }
    return t::summary("04-matrix-basics");
}
