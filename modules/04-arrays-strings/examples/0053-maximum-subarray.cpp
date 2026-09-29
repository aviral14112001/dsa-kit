// 53. Maximum Subarray: https://leetcode.com/problems/maximum-subarray/
// Pattern: Kadane (best sum ending here). Module 04 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:cubic]
// O(n^3): every subarray, each re-summed from scratch.
int max_subarray_cubic(const vector<int>& a) {
    int n = (int)a.size(), best = INT_MIN;
    for (int l = 0; l < n; l++)
        for (int r = l; r < n; r++) {
            int sum = 0;
            for (int k = l; k <= r; k++) sum += a[k];
            best = max(best, sum);
        }
    return best;
}
// [/snippet]

// [snippet:quadratic]
// O(n^2): sum(l..r) = sum(l..r-1) + a[r], so extend a running sum instead of re-adding.
int max_subarray_quadratic(const vector<int>& a) {
    int n = (int)a.size(), best = INT_MIN;
    for (int l = 0; l < n; l++) {
        int sum = 0;
        for (int r = l; r < n; r++) {
            sum += a[r];
            best = max(best, sum);
        }
    }
    return best;
}
// [/snippet]

// [snippet:solution]
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int endingHere = nums[0];   // best sum of a subarray that ends exactly at index i
        int best = nums[0];         // best sum seen anywhere so far
        for (int i = 1; i < (int)nums.size(); i++) {
            endingHere = max(nums[i], endingHere + nums[i]);   // start fresh at i, or extend
            best = max(best, endingHere);
        }
        return best;
    }
};
// [/snippet]

int main() {
    Solution sol;
    vector<int> ex1{-2, 1, -3, 4, -1, 2, 1, -5, 4}, ex2{1}, ex3{5, 4, -1, 7, 8};
    CHECK_EQ(sol.maxSubArray(ex1), 6);
    CHECK_EQ(sol.maxSubArray(ex2), 1);
    CHECK_EQ(sol.maxSubArray(ex3), 23);

    vector<int> allNegative{-3, -1, -2}, bigSums(100000, 10000);
    CHECK_EQ(sol.maxSubArray(allNegative), -1);        // not 0: the subarray can't be empty
    CHECK_EQ(sol.maxSubArray(bigSums), 1'000'000'000); // the largest total the constraints allow: fits in int

    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(1, 15), -10, 10);
        int expected = max_subarray_cubic(a);
        CHECK_EQ(max_subarray_quadratic(a), expected);
        CHECK_EQ(sol.maxSubArray(a), expected);
    }
    return t::summary("0053-maximum-subarray");
}
