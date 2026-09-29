// 560. Subarray Sum Equals K: https://leetcode.com/problems/subarray-sum-equals-k/
// Pattern: prefix sum + hash map of prefix counts, seeded with {0: 1}. Module 07 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int subarraySum(vector<int>& nums, int k) {
        // |prefix| <= 2*10^4 * 1000 = 2*10^7 here, so int is safe; in general use long long.
        unordered_map<int, int> seen;   // prefix value -> how many earlier prefixes have it
        seen[0] = 1;                    // the empty prefix: lets a subarray start at index 0
        int prefix = 0, count = 0;
        for (int x : nums) {
            prefix += x;                // prefix = sum of everything up to and including x
            // A subarray ending here sums to k  <=>  some earlier prefix equals prefix - k.
            auto it = seen.find(prefix - k);
            if (it != seen.end()) count += it->second;
            seen[prefix]++;             // AFTER the lookup: a subarray needs at least one element
        }
        return count;
    }
};
// [/snippet]

// Brute force for the stress test: every start, a running sum over every end. O(n^2).
int brute(const vector<int>& nums, int k) {
    int count = 0;
    for (size_t i = 0; i < nums.size(); i++) {
        int sum = 0;
        for (size_t j = i; j < nums.size(); j++) {
            sum += nums[j];
            count += sum == k;
        }
    }
    return count;
}

int main() {
    Solution sol;
    vector<int> ex1{1, 1, 1}, ex2{1, 2, 3};
    CHECK_EQ(sol.subarraySum(ex1, 2), 2);
    CHECK_EQ(sol.subarraySum(ex2, 3), 2);

    vector<int> dry{2, -1, 1, 2}, zeros{0, 0, 0}, alternating{3, -3, 3, -3}, one{1}, negative{-1, -1, 1};
    CHECK_EQ(sol.subarraySum(dry, 2), 4);            // the NOTES dry run
    CHECK_EQ(sol.subarraySum(zeros, 0), 6);          // every one of the 3*4/2 subarrays
    CHECK_EQ(sol.subarraySum(alternating, 0), 4);
    CHECK_EQ(sol.subarraySum(one, 0), 0);            // the empty subarray doesn't count
    CHECK_EQ(sol.subarraySum(one, 1), 1);
    CHECK_EQ(sol.subarraySum(negative, 0), 1);
    CHECK_EQ(sol.subarraySum(negative, -2), 1);

    for (int iter = 0; iter < 500; iter++) {         // stress test vs brute force: negatives, zeros
        vector<int> v = t::rand_vec((int)t::rand_int(1, 30), -3, 3);
        int k = (int)t::rand_int(-5, 5);
        CHECK_EQ(sol.subarraySum(v, k), brute(v, k));
    }
    return t::summary("0560-subarray-sum-equals-k");
}
