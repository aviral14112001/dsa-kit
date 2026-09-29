// 198. House Robber: https://leetcode.com/problems/house-robber/
// Pattern: 1D DP, take / skip the last house. Module 16 section 1 (the five-step recipe).
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace table {
// [snippet:table]
class Solution {
public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        // 1. State: best[i] = the most money from the first i houses (houses 0..i-1).
        vector<int> best(n + 1);
        // 3. Base cases: no houses -> 0; one house -> rob it.
        best[0] = 0;
        best[1] = nums[0];
        // 4. Order: best[i] reads best[i-1] and best[i-2], so fill left to right.
        for (int i = 2; i <= n; i++) {
            // 2. Transition, by the last decision: skip house i-1, or rob it (then house i-2 is off limits).
            best[i] = max(best[i - 1], best[i - 2] + nums[i - 1]);
        }
        // 5. Answer: the state that covers all n houses.
        return best[n];
    }
};
// [/snippet]
}  // namespace table

// [snippet:solution]
class Solution {
public:
    int rob(vector<int>& nums) {
        int twoBack = 0, oneBack = 0;          // best[i-2], best[i-1]: the only cells ever read
        for (int money : nums) {
            int cur = max(oneBack, twoBack + money);   // skip this house, or rob it
            twoBack = oneBack;
            oneBack = cur;
        }
        return oneBack;
    }
};
// [/snippet]

// Brute force for the stress test: every subset with no two adjacent houses (n <= ~15).
int brute(const vector<int>& nums) {
    int n = nums.size(), best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        if (mask & (mask >> 1)) continue;      // two neighbouring houses chosen
        int total = 0;
        for (int i = 0; i < n; i++)
            if (mask >> i & 1) total += nums[i];
        best = max(best, total);
    }
    return best;
}

int main() {
    Solution sol;
    table::Solution tab;
    vector<int> ex1 = {1, 2, 3, 1}, ex2 = {2, 7, 9, 3, 1};
    CHECK_EQ(sol.rob(ex1), 4);
    CHECK_EQ(sol.rob(ex2), 12);
    CHECK_EQ(tab.rob(ex1), 4);
    CHECK_EQ(tab.rob(ex2), 12);

    vector<int> one = {5}, two = {2, 1}, zeros = {0, 0, 0}, gap = {2, 1, 1, 2}, empty;
    CHECK_EQ(sol.rob(one), 5);                 // n = 1: the table's loop never runs
    CHECK_EQ(tab.rob(one), 5);
    CHECK_EQ(sol.rob(two), 2);
    CHECK_EQ(tab.rob(two), 2);
    CHECK_EQ(sol.rob(zeros), 0);
    CHECK_EQ(sol.rob(gap), 4);                 // skips two houses in a row: "every other house" is wrong
    CHECK_EQ(tab.rob(gap), 4);
    CHECK_EQ(sol.rob(empty), 0);               // outside the constraints; the rolling version copes anyway

    for (int iter = 0; iter < 300; iter++) {   // stress test vs brute force
        vector<int> nums = t::rand_vec((int)t::rand_int(1, 12), 0, 400);
        int expected = brute(nums);
        CHECK_EQ(sol.rob(nums), expected);
        CHECK_EQ(tab.rob(nums), expected);
    }
    return t::summary("0198-house-robber");
}
