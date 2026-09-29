// 167. Two Sum II - Input Array Is Sorted: https://leetcode.com/problems/two-sum-ii-input-array-is-sorted/
// Pattern: converging pointers on a sorted array. Module 06 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> twoSum(vector<int>& numbers, int target) {
        int l = 0, r = (int)numbers.size() - 1;
        while (l < r) {
            int sum = numbers[l] + numbers[r];          // |values| <= 1000: no overflow
            if (sum == target) return {l + 1, r + 1};   // the answer is 1-indexed
            if (sum < target) l++;                      // numbers[l] is too small even with the largest partner
            else r--;                                   // numbers[r] is too big even with the smallest partner
        }
        return {};                                      // unreachable: a solution is guaranteed
    }
};
// [/snippet]

// Brute force: every pair. O(n^2) = 9 * 10^8 pair checks at n = 3 * 10^4: too slow, and it
// ignores the one thing the problem gives you (sortedness).
bool has_pair(const vector<int>& a, int target) {
    for (size_t i = 0; i < a.size(); i++)
        for (size_t j = i + 1; j < a.size(); j++)
            if (a[i] + a[j] == target) return true;
    return false;
}

int main() {
    Solution sol;
    vector<int> ex1{2, 7, 11, 15}, ex2{2, 3, 4}, ex3{-1, 0};
    CHECK_EQ(sol.twoSum(ex1, 9), vector<int>{1, 2});
    CHECK_EQ(sol.twoSum(ex2, 6), vector<int>{1, 3});
    CHECK_EQ(sol.twoSum(ex3, -1), vector<int>{1, 2});

    vector<int> dup{3, 3}, farApart{-1000, -5, 0, 7, 1000};
    CHECK_EQ(sol.twoSum(dup, 6), vector<int>{1, 2});          // equal values, two different indices
    CHECK_EQ(sol.twoSum(farApart, 0), vector<int>{1, 5});     // the two ends

    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(2, 15), -30, 30);
        sort(a.begin(), a.end());
        int i = (int)t::rand_int(0, (int)a.size() - 2), j = (int)t::rand_int(i + 1, (int)a.size() - 1);
        int target = a[i] + a[j];                             // plant a solution (maybe not the only one)
        auto got = sol.twoSum(a, target);
        CHECK(has_pair(a, target));
        CHECK_EQ(got.size(), 2);
        if (got.size() == 2) {
            CHECK(1 <= got[0] && got[0] < got[1] && got[1] <= (int)a.size());
            CHECK_EQ(a[got[0] - 1] + a[got[1] - 1], target);
        }
    }
    return t::summary("0167-two-sum-ii");
}
