// 15. 3Sum: https://leetcode.com/problems/3sum/
// Pattern: sort + fix one element + converging pointers, skipping duplicates. Module 06 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
        sort(nums.begin(), nums.end());
        int n = (int)nums.size();
        vector<vector<int>> result;
        for (int i = 0; i + 2 < n; i++) {
            if (nums[i] > 0) break;                              // the smallest of the three is positive
            if (i > 0 && nums[i] == nums[i - 1]) continue;       // same first value: same triplets again
            int l = i + 1, r = n - 1;
            while (l < r) {                                      // 167 on nums[i+1..n-1], target -nums[i]
                int sum = nums[i] + nums[l] + nums[r];           // |sum| <= 3 * 10^5: fits in int
                if (sum < 0) {
                    l++;
                } else if (sum > 0) {
                    r--;
                } else {
                    result.push_back({nums[i], nums[l], nums[r]});
                    l++;
                    r--;
                    while (l < r && nums[l] == nums[l - 1]) l++; // skip equal second values; an equal
                }                                                // third value then overshoots and r-- skips it
            }
        }
        return result;
    }
};
// [/snippet]

// Brute force: all triples, deduplicated through a set of sorted triplets. O(n^3 log n).
set<vector<int>> brute(vector<int> nums) {
    set<vector<int>> found;
    int n = (int)nums.size();
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            for (int k = j + 1; k < n; k++)
                if (nums[i] + nums[j] + nums[k] == 0) {
                    vector<int> triplet{nums[i], nums[j], nums[k]};
                    sort(triplet.begin(), triplet.end());
                    found.insert(triplet);
                }
    return found;
}

int main() {
    Solution sol;
    vector<int> ex1{-1, 0, 1, 2, -1, -4}, ex2{0, 1, 1}, ex3{0, 0, 0};
    CHECK_EQ(sol.threeSum(ex1), vector<vector<int>>{{-1, -1, 2}, {-1, 0, 1}});
    CHECK_EQ(sol.threeSum(ex2), vector<vector<int>>{});
    CHECK_EQ(sol.threeSum(ex3), vector<vector<int>>{{0, 0, 0}});

    vector<int> zeros(3000, 0);                                  // lots of duplicates, one triplet
    CHECK_EQ(sol.threeSum(zeros), vector<vector<int>>{{0, 0, 0}});
    vector<int> pairsOfDupes{-2, -2, 0, 0, 2, 2};
    CHECK_EQ(sol.threeSum(pairsOfDupes), vector<vector<int>>{{-2, 0, 2}});

    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(3, 14), -6, 6);
        auto got = sol.threeSum(a);
        set<vector<int>> asSet(got.begin(), got.end());
        CHECK_EQ(asSet.size(), got.size());                      // no triplet reported twice
        CHECK_EQ(asSet, brute(a));
    }
    return t::summary("0015-3sum");
}
