// 416. Partition Equal Subset Sum: https://leetcode.com/problems/partition-equal-subset-sum/
// Pattern: 0/1 knapsack over sums (subset sum), downward loop; bitset follow-up. Module 16 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    bool canPartition(vector<int>& nums) {
        int total = accumulate(nums.begin(), nums.end(), 0);
        if (total % 2 != 0) return false;            // an odd total can't split into two equal halves
        int target = total / 2;
        vector<bool> reachable(target + 1, false);   // reachable[s]: some subset of the items so far sums to s
        reachable[0] = true;                         // the empty subset
        for (int x : nums)
            for (int s = target; s >= x; s--)        // downward, so each item is used at most once
                if (reachable[s - x]) reachable[s] = true;
        return reachable[target];
    }
};
// [/snippet]

namespace bitset_version {
// [snippet:bitset]
class Solution {
public:
    bool canPartition(vector<int>& nums) {
        int total = accumulate(nums.begin(), nums.end(), 0);
        if (total % 2 != 0) return false;
        bitset<10001> reachable;                     // bit s <=> sum s reachable; 10001 > max target 200*100/2
        reachable[0] = 1;
        for (int x : nums) reachable |= reachable << x;   // every old sum s also gives s + x, 64 sums per word
        return reachable[total / 2];
    }
};
// [/snippet]
}  // namespace bitset_version

// Brute force: every subset's sum, built incrementally (sum[mask] = sum[mask without its lowest bit] + that item).
bool brute(const vector<int>& nums) {
    int n = nums.size(), total = accumulate(nums.begin(), nums.end(), 0);
    vector<int> sum(1 << n, 0);
    for (int mask = 1; mask < (1 << n); mask++) {
        sum[mask] = sum[mask & (mask - 1)] + nums[__builtin_ctz(mask)];
        if (2 * sum[mask] == total) return true;
    }
    return false;
}

int main() {
    Solution sol;
    bitset_version::Solution bits;
    vector<int> ex1 = {1, 5, 11, 5}, ex2 = {1, 2, 3, 5};
    CHECK_EQ(sol.canPartition(ex1), true);     // official examples
    CHECK_EQ(sol.canPartition(ex2), false);
    CHECK_EQ(bits.canPartition(ex1), true);
    CHECK_EQ(bits.canPartition(ex2), false);

    vector<int> single = {100}, pair2 = {2, 2}, trap = {1, 2, 5}, big = {3, 3, 3, 4, 5};
    CHECK_EQ(sol.canPartition(single), false);  // one item: the other half would be empty
    CHECK_EQ(sol.canPartition(pair2), true);
    CHECK_EQ(sol.canPartition(trap), false);    // an upward loop would wrongly say true (reuses the 1)
    CHECK_EQ(bits.canPartition(trap), false);
    CHECK_EQ(sol.canPartition(big), true);      // 4 + 5 = 3 + 3 + 3
    vector<int> maxed(200, 100);                // largest input: target = 10000, the bitset's last bit
    CHECK_EQ(sol.canPartition(maxed), true);
    CHECK_EQ(bits.canPartition(maxed), true);
    maxed[0] = 99;                              // total becomes odd
    CHECK_EQ(sol.canPartition(maxed), false);

    for (int iter = 0; iter < 300; iter++) {    // stress test vs every subset, n <= 15
        int hi = iter % 2 ? 20 : 100;           // small values make "true" common
        vector<int> nums = t::rand_vec((int)t::rand_int(1, 15), 1, hi);
        bool expected = brute(nums);
        CHECK_EQ(sol.canPartition(nums), expected);
        CHECK_EQ(bits.canPartition(nums), expected);
    }
    return t::summary("0416-partition-equal-subset-sum");
}
