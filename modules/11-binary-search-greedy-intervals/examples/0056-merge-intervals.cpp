// 56. Merge Intervals: https://leetcode.com/problems/merge-intervals/
// Pattern: sort by start, then one pass that extends or starts a block. Module 11 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        sort(intervals.begin(), intervals.end());               // by start (ties: by end)
        vector<vector<int>> merged;
        // Invariant: merged holds disjoint blocks, sorted; only the last one can still grow,
        // because every interval still to come starts at or after its start.
        for (const auto& iv : intervals) {
            if (!merged.empty() && iv[0] <= merged.back()[1])   // closed intervals: touching overlaps
                merged.back()[1] = max(merged.back()[1], iv[1]); // max: iv may lie entirely inside
            else
                merged.push_back(iv);                            // a gap: start a new block
        }
        return merged;
    }
};
// [/snippet]

// Brute force on coordinates 0..20: mark every covered half-unit (x2 keeps [1,2] and [3,4] apart
// while [1,2] and [2,3] still touch), then read off the maximal runs.
vector<vector<int>> brute(const vector<vector<int>>& intervals) {
    const int MAX2 = 40;
    vector<bool> covered(MAX2 + 1, false);
    for (const auto& iv : intervals)
        for (int x = 2 * iv[0]; x <= 2 * iv[1]; x++) covered[x] = true;
    vector<vector<int>> out;
    for (int x = 0; x <= MAX2; x++) {
        if (!covered[x]) continue;
        int start = x;
        while (x + 1 <= MAX2 && covered[x + 1]) x++;
        out.push_back({start / 2, x / 2});                     // runs start and end on even marks
    }
    return out;
}

int main() {
    Solution sol;
    vector<vector<int>> a{{1, 3}, {2, 6}, {8, 10}, {15, 18}};
    CHECK_EQ(sol.merge(a), vector<vector<int>>{{1, 6}, {8, 10}, {15, 18}});   // the official examples
    vector<vector<int>> b{{1, 4}, {4, 5}};
    CHECK_EQ(sol.merge(b), vector<vector<int>>{{1, 5}});                      // touching counts
    vector<vector<int>> c{{4, 7}, {1, 4}};
    CHECK_EQ(sol.merge(c), vector<vector<int>>{{1, 7}});                      // unsorted input
    vector<vector<int>> d{{1, 10}, {2, 3}, {4, 5}};
    CHECK_EQ(sol.merge(d), vector<vector<int>>{{1, 10}});                     // containment
    vector<vector<int>> e{{1, 2}, {3, 4}};
    CHECK_EQ(sol.merge(e), vector<vector<int>>{{1, 2}, {3, 4}});              // a gap of 1: separate
    vector<vector<int>> f{{2, 2}, {2, 2}, {0, 0}};
    CHECK_EQ(sol.merge(f), vector<vector<int>>{{0, 0}, {2, 2}});              // points, duplicates
    vector<vector<int>> g{{5, 5}};
    CHECK_EQ(sol.merge(g), vector<vector<int>>{{5, 5}});

    for (int iter = 0; iter < 300; iter++) {                                   // stress vs brute force
        int n = (int)t::rand_int(1, 8);
        vector<vector<int>> ivs(n);
        for (auto& iv : ivs) {
            int s = (int)t::rand_int(0, 20), len = (int)t::rand_int(0, 6);
            iv = {s, min(20, s + len)};
        }
        auto expected = brute(ivs);
        CHECK_EQ(sol.merge(ivs), expected);
    }
    return t::summary("0056-merge-intervals");
}
