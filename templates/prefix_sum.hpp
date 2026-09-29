// Prefix sums and difference arrays (module 06 section 3–4).
//
//   PrefixSum     O(n) build, O(1) sum of any range              static data, many range-sum queries
//   PrefixSum2D   O(R·C) build, O(1) sum of any sub-rectangle     inclusion–exclusion over four corners
//   DiffArray     O(1) range add, O(n) rebuild of every value     many updates first, read everything once
//   DiffArray2D   O(1) rectangle add, O(R·C) rebuild
//
// Every running total is long long: 10^5 values of 10^9 overflow int long before the end.
// Written interview-style (unqualified std names), like every header in templates/.
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:prefix1d]
// pre[i] = a[0] + ... + a[i-1]: pre[0] = 0 (the empty prefix), and pre has n + 1 entries.
// a[l] + ... + a[r] = pre[r + 1] - pre[l]: "everything up to r" minus "everything before l".
struct PrefixSum {
    vector<long long> pre;

    explicit PrefixSum(const vector<int>& a) : pre(a.size() + 1, 0) {
        for (size_t i = 0; i < a.size(); i++) pre[i + 1] = pre[i] + a[i];
    }
    // Sum of a[l..r], inclusive. Needs 0 <= l <= r + 1 <= n; l == r + 1 is the empty range (0).
    long long sum(int l, int r) const { return pre[r + 1] - pre[l]; }
};
// [/snippet]

// [snippet:prefix2d]
// P[i][j] = sum of the top-left block a[0..i-1][0..j-1]; row 0 and column 0 of P stay 0.
// Build: the block above plus the block to the left count their overlap twice: subtract it once.
// Query: the big block, minus the strip above, minus the strip to the left, plus the corner
//        that both strips removed.
struct PrefixSum2D {
    vector<vector<long long>> P;

    explicit PrefixSum2D(const vector<vector<int>>& a) {
        int rows = (int)a.size(), cols = rows ? (int)a[0].size() : 0;
        P.assign(rows + 1, vector<long long>(cols + 1, 0));
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                P[i + 1][j + 1] = a[i][j] + P[i][j + 1] + P[i + 1][j] - P[i][j];
    }
    // Sum of the rectangle with top-left (r1, c1) and bottom-right (r2, c2), both inclusive.
    long long sum(int r1, int c1, int r2, int c2) const {
        return P[r2 + 1][c2 + 1] - P[r1][c2 + 1] - P[r2 + 1][c1] + P[r1][c1];
    }
};
// [/snippet]

// [snippet:diff1d]
// d[i] = value[i] - value[i-1] (with value[-1] = 0), so value[i] = d[0] + ... + d[i].
// Adding v to value[l..r] raises the step INTO l by v and lowers the step OUT OF r by v:
// only d[l] and d[r + 1] change. One running sum over d rebuilds every value.
struct DiffArray {
    vector<long long> d;   // n + 1 slots, so d[r + 1] exists even when r = n - 1

    explicit DiffArray(int n) : d(n + 1, 0) {}
    explicit DiffArray(const vector<int>& initial) : d(initial.size() + 1, 0) {
        for (size_t i = 0; i < initial.size(); i++) {
            long long previous = i ? initial[i - 1] : 0;
            d[i] = initial[i] - previous;
        }
    }
    void add(int l, int r, long long v) {   // value[l..r] += v, for 0 <= l <= r < n
        d[l] += v;
        d[r + 1] -= v;
    }
    vector<long long> build() const {       // O(n): value[i] is the running sum of d[0..i]
        vector<long long> value(d.size() - 1);
        long long running = 0;
        for (size_t i = 0; i < value.size(); i++) {
            running += d[i];
            value[i] = running;
        }
        return value;
    }
};
// [/snippet]

// [snippet:diff2d]
// Add v to a whole rectangle with four O(1) corner updates; a 2D prefix sum over d rebuilds the grid.
// +v at (r1, c1) switches v on for everything below-right of it; the -v at (r1, c2+1) and (r2+1, c1)
// switch it off right of the rectangle and below it; the +v at (r2+1, c2+1) cancels the region that
// got switched off twice.
struct DiffArray2D {
    vector<vector<long long>> d;   // (rows + 1) x (cols + 1)

    DiffArray2D(int rows, int cols) : d(rows + 1, vector<long long>(cols + 1, 0)) {}
    void add(int r1, int c1, int r2, int c2, long long v) {   // inclusive corners
        d[r1][c1] += v;
        d[r1][c2 + 1] -= v;
        d[r2 + 1][c1] -= v;
        d[r2 + 1][c2 + 1] += v;
    }
    vector<vector<long long>> build() const {                // 2D running sum: same recurrence as P
        int rows = (int)d.size() - 1, cols = (int)d[0].size() - 1;
        vector<vector<long long>> grid(rows, vector<long long>(cols, 0));
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++) {
                long long up = i ? grid[i - 1][j] : 0, left = j ? grid[i][j - 1] : 0;
                long long overlap = (i && j) ? grid[i - 1][j - 1] : 0;
                grid[i][j] = d[i][j] + up + left - overlap;
            }
        return grid;
    }
};
// [/snippet]
