// 45. Jump Game II: https://leetcode.com/problems/jump-game-ii/
// Pattern: greedy BFS by levels; correctness by a "greedy stays ahead" argument. Module 11 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int jump(vector<int>& nums) {
        int n = nums.size();
        int jumps = 0;          // levels finished so far
        int levelEnd = 0;       // last index reachable with `jumps` jumps
        int farthest = 0;       // last index reachable with jumps + 1 jumps (from what we've scanned)
        for (int i = 0; i < n - 1; i++) {        // the last index never needs a jump out of it
            farthest = max(farthest, i + nums[i]);
            if (i == levelEnd) {                 // scanned the whole current level:
                jumps++;                         // one more jump reaches everything up to farthest
                levelEnd = farthest;
            }
        }
        return jumps;
    }
};
// [/snippet]

// Brute force: dp[j] = fewest jumps to reach j, relaxing every jump. O(n * max jump).
// Returns INT_MAX when the last index is unreachable.
int brute(const vector<int>& nums) {
    int n = nums.size();
    vector<int> dp(n, INT_MAX);
    dp[0] = 0;
    for (int i = 0; i < n; i++) {
        if (dp[i] == INT_MAX) continue;
        for (int j = i + 1; j <= min(n - 1, i + nums[i]); j++) dp[j] = min(dp[j], dp[i] + 1);
    }
    return dp[n - 1];
}

int main() {
    Solution sol;
    vector<int> a{2, 3, 1, 1, 4};
    CHECK_EQ(sol.jump(a), 2);                            // the official examples
    vector<int> b{2, 3, 0, 1, 4};
    CHECK_EQ(sol.jump(b), 2);
    vector<int> c{0};
    CHECK_EQ(sol.jump(c), 0);                            // already at the end
    vector<int> d{1, 1, 1, 1};
    CHECK_EQ(sol.jump(d), 3);                            // forced single steps
    vector<int> e{5, 0, 0, 0, 0, 0};
    CHECK_EQ(sol.jump(e), 1);                            // one jump covers everything
    vector<int> f{1, 2, 0, 1};
    CHECK_EQ(sol.jump(f), 2);                            // 0 -> 1 -> 3, stepping over the zero
    vector<int> big(10000, 1);
    CHECK_EQ(sol.jump(big), 9999);                       // the maximum length, all ones

    int tested = 0;
    while (tested < 300) {                               // stress test vs the DP
        auto nums = t::rand_vec((int)t::rand_int(1, 12), 0, 4);
        int expected = brute(nums);
        if (expected == INT_MAX) continue;               // LeetCode guarantees the end is reachable
        CHECK_EQ(sol.jump(nums), expected);
        tested++;
    }
    return t::summary("0045-jump-game-ii");
}
