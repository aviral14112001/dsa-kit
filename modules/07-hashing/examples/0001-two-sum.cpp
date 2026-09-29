// 1. Two Sum: https://leetcode.com/problems/two-sum/
// Pattern: complement lookup in a hash map of values seen so far. Module 07 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> index_of;   // value -> its index, for every value left of i
        for (int i = 0; i < (int)nums.size(); i++) {
            int need = target - nums[i];    // |need| <= 2*10^9 < 2^31 - 1: fits in int, barely
            auto it = index_of.find(need);  // find, not []: [] would insert `need` with index 0
            if (it != index_of.end()) return {it->second, i};
            index_of[nums[i]] = i;          // AFTER the lookup, so i is never paired with itself
        }
        return {};                          // unreachable: exactly one answer is guaranteed
    }
};
// [/snippet]

// Brute force for the stress test: every pair, O(n^2).
vector<int> brute(const vector<int>& nums, int target) {
    for (int i = 0; i < (int)nums.size(); i++)
        for (int j = i + 1; j < (int)nums.size(); j++)
            if (nums[i] + nums[j] == target) return {i, j};
    return {};
}

int main() {
    Solution sol;
    vector<int> ex1{2, 7, 11, 15}, ex2{3, 2, 4}, ex3{3, 3};
    CHECK_EQ(sol.twoSum(ex1, 9), vector<int>{0, 1});
    CHECK_EQ(sol.twoSum(ex2, 6), vector<int>{1, 2});   // not {0, 0}: 3 can't pair with itself
    CHECK_EQ(sol.twoSum(ex3, 6), vector<int>{0, 1});   // duplicates: the map holds the first 3 when the second arrives

    vector<int> negatives{-1, -2, -3, -4, -5}, zeros{0, 4, 3, 0}, extremes{1000000000, -1000000000, 7};
    CHECK_EQ(sol.twoSum(negatives, -8), vector<int>{2, 4});
    CHECK_EQ(sol.twoSum(zeros, 0), vector<int>{0, 3});
    CHECK_EQ(sol.twoSum(extremes, 0), vector<int>{0, 1});
    vector<int> far{1000000000, 5, -1000000000};
    CHECK_EQ(sol.twoSum(far, -999999995), vector<int>{1, 2});

    // Stress test: a random array may have several valid pairs (or none), so check that the answer
    // is VALID, and that one is found exactly when the brute force finds one.
    for (int iter = 0; iter < 500; iter++) {
        vector<int> v = t::rand_vec((int)t::rand_int(2, 12), -10, 10);
        int target = (int)t::rand_int(-20, 20);
        vector<int> got = sol.twoSum(v, target), expected = brute(v, target);
        CHECK_EQ(got.empty(), expected.empty());
        if (got.size() == 2) CHECK(got[0] < got[1] && v[got[0]] + v[got[1]] == target);
    }
    return t::summary("0001-two-sum");
}
