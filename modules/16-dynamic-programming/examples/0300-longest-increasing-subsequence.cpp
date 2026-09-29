// 300. Longest Increasing Subsequence: https://leetcode.com/problems/longest-increasing-subsequence/
// Pattern: LIS, O(n^2) "ending at i" DP, then O(n log n) tails + lower_bound, plus reconstruction. Module 16 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace quadratic {
// [snippet:quadratic]
class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        int n = nums.size();
        vector<int> endingAt(n, 1);          // endingAt[i] = longest strictly increasing subsequence ending at i
        for (int i = 0; i < n; i++)
            for (int j = 0; j < i; j++)      // the element before nums[i] is some smaller nums[j], j < i
                if (nums[j] < nums[i]) endingAt[i] = max(endingAt[i], endingAt[j] + 1);
        return *max_element(endingAt.begin(), endingAt.end());   // the LIS may end anywhere
    }
};
// [/snippet]
}  // namespace quadratic

// [snippet:solution]
class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        vector<int> tails;      // tails[k] = smallest tail of any increasing subsequence of length k+1 so far
        for (int x : nums) {
            auto it = lower_bound(tails.begin(), tails.end(), x);   // first tail >= x
            if (it == tails.end()) tails.push_back(x);               // x extends the longest one
            else *it = x;                                            // x: a smaller tail for that length
        }
        return tails.size();
    }
};
// [/snippet]

// [snippet:reconstruct]
// One longest strictly increasing subsequence (its values), in O(n log n).
vector<int> longestIncreasingSubsequence(const vector<int>& nums) {
    vector<int> tailIdx;                     // tailIdx[k] = index of the smallest tail for length k+1
    vector<int> parent(nums.size(), -1);     // parent[i] = the index before i in its subsequence
    for (int i = 0; i < (int)nums.size(); i++) {
        int k = lower_bound(tailIdx.begin(), tailIdx.end(), nums[i],
                            [&](int idx, int value) { return nums[idx] < value; }) - tailIdx.begin();
        if (k > 0) parent[i] = tailIdx[k - 1];           // extend the best subsequence of length k
        if (k == (int)tailIdx.size()) tailIdx.push_back(i);
        else tailIdx[k] = i;
    }
    vector<int> lis;
    for (int i = tailIdx.empty() ? -1 : tailIdx.back(); i != -1; i = parent[i]) lis.push_back(nums[i]);
    reverse(lis.begin(), lis.end());
    return lis;
}
// [/snippet]

// [snippet:brute]
// Brute force: module 10's choose / skip recursion, no memo. O(2^n): fine for n <= ~15.
// (Memoise it on (i, prev) and you have the O(n^2) DP.)
int brute(const vector<int>& nums, int i, int prev) {      // prev = index of the last element taken, -1 if none
    if (i == (int)nums.size()) return 0;
    int best = brute(nums, i + 1, prev);                                       // skip nums[i]
    if (prev == -1 || nums[prev] < nums[i]) best = max(best, 1 + brute(nums, i + 1, i));   // take it
    return best;
}
// [/snippet]

bool isIncreasingSubsequence(const vector<int>& sub, const vector<int>& nums) {
    for (size_t k = 1; k < sub.size(); k++)
        if (sub[k - 1] >= sub[k]) return false;
    size_t k = 0;
    for (int x : nums)
        if (k < sub.size() && sub[k] == x) k++;
    return k == sub.size();
}

int main() {
    Solution sol;
    quadratic::Solution quad;
    vector<int> ex1 = {10, 9, 2, 5, 3, 7, 101, 18}, ex2 = {0, 1, 0, 3, 2, 3}, ex3 = {7, 7, 7, 7, 7, 7, 7};
    CHECK_EQ(sol.lengthOfLIS(ex1), 4);           // official examples
    CHECK_EQ(sol.lengthOfLIS(ex2), 4);
    CHECK_EQ(sol.lengthOfLIS(ex3), 1);           // equal values never extend a strictly increasing run
    CHECK_EQ(quad.lengthOfLIS(ex1), 4);
    CHECK_EQ(quad.lengthOfLIS(ex3), 1);
    CHECK_EQ(longestIncreasingSubsequence(ex1).size(), 4);
    CHECK(isIncreasingSubsequence(longestIncreasingSubsequence(ex1), ex1));

    vector<int> one = {5}, down = {5, 4, 3, 2, 1}, up = {-3, -1, 0, 4, 9}, trap = {3, 4, 1};
    CHECK_EQ(sol.lengthOfLIS(one), 1);
    CHECK_EQ(sol.lengthOfLIS(down), 1);
    CHECK_EQ(sol.lengthOfLIS(up), 5);
    CHECK_EQ(sol.lengthOfLIS(trap), 2);          // tails ends as [1, 4]: right length, not a subsequence
    CHECK_EQ(longestIncreasingSubsequence(trap), vector<int>{3, 4});
    CHECK_EQ(longestIncreasingSubsequence({}), vector<int>{});

    // [snippet:stress]
    for (int iter = 0; iter < 300; iter++) {    // stress test: random tiny arrays vs the brute force
        vector<int> nums = t::rand_vec((int)t::rand_int(1, 12), -5, 5);   // tiny range forces duplicates
        int expected = brute(nums, 0, -1);
        CHECK_EQ(sol.lengthOfLIS(nums), expected);
        CHECK_EQ(quad.lengthOfLIS(nums), expected);
        vector<int> lis = longestIncreasingSubsequence(nums);
        CHECK_EQ((int)lis.size(), expected);
        CHECK(isIncreasingSubsequence(lis, nums));
    }
    // [/snippet]

    for (int iter = 0; iter < 5; iter++) {      // larger inputs: the two fast versions must agree
        vector<int> nums = t::rand_vec(1000, -10000, 10000);
        CHECK_EQ(sol.lengthOfLIS(nums), quad.lengthOfLIS(nums));
        CHECK_EQ(longestIncreasingSubsequence(nums).size(), (size_t)sol.lengthOfLIS(nums));
    }
    return t::summary("0300-longest-increasing-subsequence");
}
