// 62. Unique Paths: https://leetcode.com/problems/unique-paths/
// Pattern: grid DP, the same recurrence three ways: memo -> table -> one row. Module 16 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace memo {
// [snippet:memo]
class Solution {
    vector<vector<int>> memo;                  // memo[r][c] = paths from (0,0) to (r,c); -1 = not computed yet

    int paths(int r, int c) {
        if (r == 0 || c == 0) return 1;        // first row or column: one straight path
        int& cached = memo[r][c];              // safe: memo is never resized during the recursion
        if (cached != -1) return cached;
        return cached = paths(r - 1, c) + paths(r, c - 1);   // the last move came from above or from the left
    }

public:
    int uniquePaths(int m, int n) {
        memo.assign(m, vector<int>(n, -1));
        return paths(m - 1, n - 1);
    }
};
// [/snippet]
}  // namespace memo

namespace table {
// [snippet:table]
class Solution {
public:
    int uniquePaths(int m, int n) {
        vector<vector<int>> paths(m, vector<int>(n, 1));    // row 0 and column 0 stay 1: the base cases
        for (int r = 1; r < m; r++)
            for (int c = 1; c < n; c++)                      // row by row: up and left are already final
                paths[r][c] = paths[r - 1][c] + paths[r][c - 1];
        return paths[m - 1][n - 1];
    }
};
// [/snippet]
}  // namespace table

// [snippet:row]
class Solution {
public:
    int uniquePaths(int m, int n) {
        vector<int> row(n, 1);                 // row 0
        for (int r = 1; r < m; r++)
            for (int c = 1; c < n; c++)
                row[c] += row[c - 1];          // before: row[c] = up (last row), row[c-1] = left (this row)
        return row[n - 1];
    }
};
// [/snippet]

// Brute force: the recursion with no memo, walking every path (exponential; m, n <= ~10).
long long brute(int r, int c) {
    if (r == 0 || c == 0) return 1;
    return brute(r - 1, c) + brute(r, c - 1);
}

// Closed form C(m+n-2, m-1), or -1 when it exceeds LeetCode's 2e9 guarantee (such inputs aren't tested).
// Every intermediate res is itself a binomial C(N-k+i, i) <= the final value, so nothing overflows.
long long binomialOrSkip(int m, int n) {
    int N = m + n - 2, k = min(m, n) - 1;
    long long res = 1;
    for (int i = 1; i <= k; i++) {
        res = res * (N - k + i) / i;
        if (res > 2'000'000'000LL) return -1;
    }
    return res;
}

int main() {
    memo::Solution mem;
    table::Solution tab;
    Solution row;
    CHECK_EQ(mem.uniquePaths(3, 7), 28);       // official examples
    CHECK_EQ(tab.uniquePaths(3, 7), 28);
    CHECK_EQ(row.uniquePaths(3, 7), 28);
    CHECK_EQ(row.uniquePaths(3, 2), 3);
    CHECK_EQ(row.uniquePaths(1, 1), 1);        // edge cases: a single cell, a single row / column
    CHECK_EQ(mem.uniquePaths(1, 100), 1);
    CHECK_EQ(tab.uniquePaths(100, 1), 1);
    CHECK_EQ(row.uniquePaths(10, 10), 48620);

    for (int m = 1; m <= 10; m++)              // exhaustive small grids vs walking every path
        for (int n = 1; n <= 10; n++) {
            long long expected = brute(m - 1, n - 1);
            CHECK_EQ(mem.uniquePaths(m, n), expected);
            CHECK_EQ(tab.uniquePaths(m, n), expected);
            CHECK_EQ(row.uniquePaths(m, n), expected);
        }
    for (int m = 1; m <= 100; m++)             // every in-range grid vs the closed form
        for (int n = 1; n <= 100; n++) {
            long long expected = binomialOrSkip(m, n);
            if (expected == -1) continue;
            CHECK_EQ(mem.uniquePaths(m, n), expected);
            CHECK_EQ(tab.uniquePaths(m, n), expected);
            CHECK_EQ(row.uniquePaths(m, n), expected);
        }
    return t::summary("0062-unique-paths");
}
