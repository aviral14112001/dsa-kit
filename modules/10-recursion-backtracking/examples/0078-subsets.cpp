// 78. Subsets: https://leetcode.com/problems/subsets/
// Pattern: backtracking over the choose / skip tree (2^n leaves). Module 10 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace choose_skip {
// [snippet:solution]
class Solution {
public:
    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> cur;                         // the path: elements chosen so far
        build(nums, 0, cur, result);
        return result;
    }

private:
    // Decide nums[i..]; cur holds the elements chosen from nums[0..i).
    void build(const vector<int>& nums, int i, vector<int>& cur, vector<vector<int>>& result) {
        if (i == (int)nums.size()) {             // every element decided: cur is one subset
            result.push_back(cur);               // copies cur: a snapshot, not a reference
            return;
        }
        build(nums, i + 1, cur, result);         // skip nums[i]
        cur.push_back(nums[i]);                  // choose nums[i]
        build(nums, i + 1, cur, result);
        cur.pop_back();                          // un-choose: hand cur back exactly as we got it
    }
};
// [/snippet]
}  // namespace choose_skip

namespace start_index {
// [snippet:start_index]
class Solution {
public:
    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> cur;
        build(nums, 0, cur, result);
        return result;
    }

private:
    // Every node of this tree is a subset: record it, then extend it by each later element.
    void build(const vector<int>& nums, int start, vector<int>& cur, vector<vector<int>>& result) {
        result.push_back(cur);
        for (int i = start; i < (int)nums.size(); i++) {
            cur.push_back(nums[i]);
            build(nums, i + 1, cur, result);     // i + 1, not start + 1: only later elements may follow
            cur.pop_back();
        }
    }
};
// [/snippet]
}  // namespace start_index

// [snippet:bitmask]
// No recursion: bit j of mask says whether nums[j] is in the subset. (The tests use it as the brute force.)
vector<vector<int>> subsetsBitmask(const vector<int>& nums) {
    int n = nums.size();
    vector<vector<int>> result;
    for (int mask = 0; mask < (1 << n); mask++) {
        vector<int> subset;
        for (int j = 0; j < n; j++)
            if ((mask >> j) & 1) subset.push_back(nums[j]);
        result.push_back(subset);
    }
    return result;
}
// [/snippet]

// LeetCode accepts any order, so compare after sorting each subset and then the list.
vector<vector<int>> canonical(vector<vector<int>> sets) {
    for (auto& s : sets) sort(s.begin(), s.end());
    sort(sets.begin(), sets.end());
    return sets;
}

// n distinct values from [-10, 10], as the constraints promise.
vector<int> distinctValues(int n) {
    vector<int> pool;
    for (int v = -10; v <= 10; v++) pool.push_back(v);
    shuffle(pool.begin(), pool.end(), t::rng());
    return vector<int>(pool.begin(), pool.begin() + n);
}

template <class S>
void run(S& sol) {
    vector<int> a{1, 2, 3};
    CHECK_EQ(canonical(sol.subsets(a)), canonical({{}, {1}, {2}, {1, 2}, {3}, {1, 3}, {2, 3}, {1, 2, 3}}));
    vector<int> b{0};
    CHECK_EQ(canonical(sol.subsets(b)), canonical({{}, {0}}));
    vector<int> c{-10, 10};                                   // extreme values
    CHECK_EQ(canonical(sol.subsets(c)), canonical({{}, {-10}, {10}, {-10, 10}}));

    for (int iter = 0; iter < 200; iter++) {                 // stress test vs bitmask enumeration
        auto nums = distinctValues((int)t::rand_int(1, 10));
        auto got = sol.subsets(nums);
        CHECK_EQ(got.size(), size_t{1} << nums.size());       // exactly 2^n subsets
        CHECK_EQ(canonical(got), canonical(subsetsBitmask(nums)));
    }
}

int main() {
    choose_skip::Solution s1;
    start_index::Solution s2;
    run(s1);
    run(s2);
    vector<int> order{1, 2, 3};                               // the start-index tree lists subsets in
    CHECK_EQ(s2.subsets(order),                               // preorder: [], [1], [1,2], [1,2,3], ...
             vector<vector<int>>{{}, {1}, {1, 2}, {1, 2, 3}, {1, 3}, {2}, {2, 3}, {3}});
    return t::summary("0078-subsets");
}
