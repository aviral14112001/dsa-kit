// 0/1 and unbounded knapsack: the 2D table, the 1D row with a DOWNWARD loop, the 1D row with an UPWARD loop.
// Pattern: DP over (items, capacity). Module 16 section 3. Stress-tested against exhaustive search.
// All weights are >= 1 (a weight-0 item with unlimited copies would make the value infinite).
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:knapsack_2d]
// 0/1 knapsack: each item at most once, total weight <= W, maximise the total value.
int knapsack01Table(const vector<int>& weight, const vector<int>& value, int W) {
    int n = weight.size();
    // best[i][c] = max value using only the first i items with total weight <= c
    vector<vector<int>> best(n + 1, vector<int>(W + 1, 0));       // row 0: no items -> 0
    for (int i = 1; i <= n; i++) {
        int w = weight[i - 1], v = value[i - 1];
        for (int c = 0; c <= W; c++) {
            best[i][c] = best[i - 1][c];                             // skip item i-1
            if (w <= c) best[i][c] = max(best[i][c], best[i - 1][c - w] + v);   // or take it, once
        }
    }
    return best[n][W];
}
// [/snippet]

// [snippet:knapsack_1d]
int knapsack01(const vector<int>& weight, const vector<int>& value, int W) {
    vector<int> best(W + 1, 0);                    // one row, overwritten in place: row i-1 becomes row i
    for (size_t i = 0; i < weight.size(); i++)
        for (int c = W; c >= weight[i]; c--)       // DOWNWARD: best[c - w] still holds row i-1 (item unused)
            best[c] = max(best[c], best[c - weight[i]] + value[i]);
    return best[W];
}
// [/snippet]

// [snippet:unbounded]
int knapsackUnbounded(const vector<int>& weight, const vector<int>& value, int W) {
    vector<int> best(W + 1, 0);
    for (size_t i = 0; i < weight.size(); i++)
        for (int c = weight[i]; c <= W; c++)       // UPWARD: best[c - w] may already contain item i -> reuse
            best[c] = max(best[c], best[c - weight[i]] + value[i]);
    return best[W];
}
// [/snippet]

// Brute force (0/1): every subset of the items. n <= ~15.
int bruteZeroOne(const vector<int>& weight, const vector<int>& value, int W) {
    int n = weight.size(), best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        int w = 0, v = 0;
        for (int i = 0; i < n; i++)
            if (mask >> i & 1) w += weight[i], v += value[i];
        if (w <= W) best = max(best, v);
    }
    return best;
}

// Brute force (unbounded): every count of every item, item by item.
int bruteUnbounded(const vector<int>& weight, const vector<int>& value, size_t i, int remaining) {
    if (i == weight.size()) return 0;
    int best = 0;
    for (int k = 0; k * weight[i] <= remaining; k++)
        best = max(best, k * value[i] + bruteUnbounded(weight, value, i + 1, remaining - k * weight[i]));
    return best;
}

int main() {
    // Weights {1, 2, 3}, values {6, 10, 12}, capacity 5: once each -> items 2 + 3 = 22; unlimited -> five 1s = 30.
    vector<int> w = {1, 2, 3}, v = {6, 10, 12};
    CHECK_EQ(knapsack01Table(w, v, 5), 22);
    CHECK_EQ(knapsack01(w, v, 5), 22);
    CHECK_EQ(knapsackUnbounded(w, v, 5), 30);
    CHECK_EQ(knapsack01(w, v, 0), 0);                     // capacity 0
    CHECK_EQ(knapsackUnbounded({4}, {9}, 3), 0);          // nothing fits
    CHECK_EQ(knapsack01({1}, {10}, 3), 10);               // one item: once...
    CHECK_EQ(knapsackUnbounded({1}, {10}, 3), 30);        // ...or as often as it fits
    CHECK_EQ(knapsack01Table({}, {}, 7), 0);              // no items

    for (int iter = 0; iter < 300; iter++) {              // stress test: 0/1 vs every subset
        int n = (int)t::rand_int(0, 12), W = (int)t::rand_int(0, 40);
        vector<int> weight = t::rand_vec(n, 1, 15), value = t::rand_vec(n, 0, 100);
        int expected = bruteZeroOne(weight, value, W);
        CHECK_EQ(knapsack01Table(weight, value, W), expected);
        CHECK_EQ(knapsack01(weight, value, W), expected);
    }
    for (int iter = 0; iter < 300; iter++) {              // stress test: unbounded vs every count vector
        int n = (int)t::rand_int(0, 4), W = (int)t::rand_int(0, 30);
        vector<int> weight = t::rand_vec(n, 1, 10), value = t::rand_vec(n, 0, 50);
        CHECK_EQ(knapsackUnbounded(weight, value, W), bruteUnbounded(weight, value, 0, W));
    }
    return t::summary("knapsack");
}
