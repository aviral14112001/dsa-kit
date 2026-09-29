// 1636. Sort Array by Increasing Frequency: https://leetcode.com/problems/sort-array-by-increasing-frequency/
// Pattern: count with a hash map, then one sort with a two-key comparator lambda. Module 01 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> frequencySort(vector<int>& nums) {
        unordered_map<int, int> freq;
        for (int x : nums) freq[x]++;
        // [&] captures freq by reference: no copy, and the lambda sees the finished counts.
        sort(nums.begin(), nums.end(), [&](int a, int b) {
            if (freq[a] != freq[b]) return freq[a] < freq[b];   // rarer values first
            return a > b;                                       // same count: larger value first
        });
        return nums;
    }
};
// [/snippet]

// Brute force for the stress test: count each value by rescanning the array (O(n^2)), then let pair
// ordering do the two-key sort: (count ascending, -value ascending = value descending).
vector<int> brute(const vector<int>& nums) {
    vector<pair<int, int>> keyed;
    for (int x : nums) keyed.push_back({(int)count(nums.begin(), nums.end(), x), -x});
    sort(keyed.begin(), keyed.end());
    vector<int> out;
    for (auto [cnt, negated] : keyed) out.push_back(-negated);
    return out;
}

int main() {
    Solution sol;
    auto run = [&](vector<int> nums) { return sol.frequencySort(nums); };   // takes a copy to sort

    CHECK_EQ(run({1, 1, 2, 2, 2, 3}), vector<int>{3, 1, 1, 2, 2, 2});      // the official examples
    CHECK_EQ(run({2, 3, 1, 3, 2}), vector<int>{1, 3, 3, 2, 2});
    CHECK_EQ(run({-1, 1, -6, 4, 5, -6, 1, 4, 1}), vector<int>{5, -1, 4, 4, -6, -6, 1, 1, 1});

    CHECK_EQ(run({7}), vector<int>{7});                                     // a single element
    CHECK_EQ(run({4, 4, 4}), vector<int>{4, 4, 4});                         // one value only
    CHECK_EQ(run({1, 2, 3}), vector<int>{3, 2, 1});                         // all counts tie: descending
    CHECK_EQ(run({-100, 100, 100}), vector<int>{-100, 100, 100});           // the value bounds

    for (int iter = 0; iter < 300; iter++) {                                // stress test vs brute force
        int n = (int)t::rand_int(1, 100);
        int spread = iter % 2 ? 100 : 5;          // a narrow range forces many equal counts
        vector<int> nums = t::rand_vec(n, -spread, spread);
        CHECK_EQ(run(nums), brute(nums));
    }
    return t::summary("1636-sort-array-by-increasing-frequency");
}
