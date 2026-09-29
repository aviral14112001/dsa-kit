// 312. Burst Balloons: https://leetcode.com/problems/burst-balloons/
// Pattern: interval DP by increasing length; split on the LAST balloon burst in (l, r). Module 16 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int maxCoins(vector<int>& nums) {
        int n = nums.size();
        vector<int> val(n + 2, 1);                   // pad with a virtual 1-balloon at each end
        for (int i = 0; i < n; i++) val[i + 1] = nums[i];
        // best[l][r] = most coins from bursting every balloon strictly between l and r (l and r stay)
        vector<vector<int>> best(n + 2, vector<int>(n + 2, 0));   // r = l + 1: nothing inside -> 0
        for (int len = 2; len <= n + 1; len++)       // by increasing gap r - l: inner intervals are final
            for (int l = 0; l + len <= n + 1; l++) {
                int r = l + len;
                for (int k = l + 1; k < r; k++)      // k = the LAST balloon burst inside (l, r)
                    best[l][r] = max(best[l][r], best[l][k] + val[l] * val[k] * val[r] + best[k][r]);
            }
        return best[0][n + 1];
    }
};
// [/snippet]

// Brute force: try every burst order (n! of them), n <= ~7.
int brute(const vector<int>& balloons) {
    int best = 0;
    for (size_t i = 0; i < balloons.size(); i++) {
        int left = i > 0 ? balloons[i - 1] : 1;
        int right = i + 1 < balloons.size() ? balloons[i + 1] : 1;
        vector<int> rest = balloons;
        rest.erase(rest.begin() + i);
        best = max(best, left * balloons[i] * right + brute(rest));
    }
    return best;
}

int main() {
    Solution sol;
    vector<int> ex1 = {3, 1, 5, 8}, ex2 = {1, 5};
    CHECK_EQ(sol.maxCoins(ex1), 167);            // official examples
    CHECK_EQ(sol.maxCoins(ex2), 10);

    vector<int> one = {7}, zero = {0}, two = {2, 3}, withZero = {4, 0, 6};
    CHECK_EQ(sol.maxCoins(one), 7);              // 1 * 7 * 1
    CHECK_EQ(sol.maxCoins(zero), 0);
    CHECK_EQ(sol.maxCoins(two), 9);              // burst 2 first (6), then 3 (3)
    CHECK_EQ(sol.maxCoins(withZero), brute(withZero));
    vector<int> maxed(300, 100);                 // largest input: < 300 * 10^6 coins, fits in int
    CHECK_EQ(sol.maxCoins(maxed), 298 * 1000000 + 100 * 100 + 100);

    for (int iter = 0; iter < 200; iter++) {     // stress test vs every burst order
        vector<int> nums = t::rand_vec((int)t::rand_int(1, 7), 0, 9);
        CHECK_EQ(sol.maxCoins(nums), brute(nums));
    }
    return t::summary("0312-burst-balloons");
}
