// 34. Find First and Last Position of Element in Sorted Array: https://leetcode.com/problems/find-first-and-last-position-of-element-in-sorted-array/
// Pattern: two boundary searches, lower_bound and upper_bound. Module 05 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> searchRange(vector<int>& nums, int target) {
        int first = firstPassing(nums, [&](int x) { return x >= target; });  // = lower_bound
        if (first == (int)nums.size() || nums[first] != target) return {-1, -1};
        int last = firstPassing(nums, [&](int x) { return x > target; }) - 1;  // = upper_bound - 1
        return {first, last};
    }

private:
    // First index whose value passes `test`, or nums.size() if none does. Along a sorted array
    // the test must read false...false true...true.
    // Invariant: every index < lo fails, every index >= hi passes; [lo, hi) is still unknown.
    template <class Test>
    int firstPassing(const vector<int>& nums, Test test) {
        int lo = 0, hi = nums.size();
        while (lo < hi) {                   // unknown range non-empty
            int mid = lo + (hi - lo) / 2;   // lo <= mid < hi, so both branches shrink [lo, hi)
            if (test(nums[mid])) hi = mid;  // mid passes: the boundary is mid or to its left
            else lo = mid + 1;              // mid fails: the boundary is right of mid
        }
        return lo;                          // lo == hi: the first passing index
    }
};
// [/snippet]

class SolutionSTL {
public:
    // [snippet:stl]
    vector<int> searchRange(vector<int>& nums, int target) {
        auto [lo, hi] = equal_range(nums.begin(), nums.end(), target);  // [lower_bound, upper_bound)
        if (lo == hi) return {-1, -1};                                   // empty range: absent
        return {int(lo - nums.begin()), int(hi - nums.begin()) - 1};
    }
    // [/snippet]
};

// Brute force for the stress test: a linear scan.
vector<int> brute(const vector<int>& nums, int target) {
    int first = -1, last = -1;
    for (int i = 0; i < (int)nums.size(); i++)
        if (nums[i] == target) {
            if (first == -1) first = i;
            last = i;
        }
    return {first, last};
}

int main() {
    Solution sol;
    SolutionSTL stl;
    vector<int> ex{5, 7, 7, 8, 8, 10}, empty;
    CHECK_EQ(sol.searchRange(ex, 8), vector<int>{3, 4});
    CHECK_EQ(sol.searchRange(ex, 6), vector<int>{-1, -1});
    CHECK_EQ(sol.searchRange(empty, 0), vector<int>{-1, -1});

    vector<int> one{1}, same{2, 2, 2, 2}, extremes{INT_MIN, INT_MIN, 0, INT_MAX, INT_MAX};
    CHECK_EQ(sol.searchRange(one, 1), vector<int>{0, 0});
    CHECK_EQ(sol.searchRange(one, 0), vector<int>{-1, -1});
    CHECK_EQ(sol.searchRange(same, 2), vector<int>{0, 3});          // the whole array
    CHECK_EQ(sol.searchRange(ex, 5), vector<int>{0, 0});            // first element
    CHECK_EQ(sol.searchRange(ex, 10), vector<int>{5, 5});           // last element
    CHECK_EQ(sol.searchRange(ex, 11), vector<int>{-1, -1});         // past the end
    CHECK_EQ(sol.searchRange(ex, 4), vector<int>{-1, -1});          // before the start
    CHECK_EQ(sol.searchRange(extremes, INT_MAX), vector<int>{3, 4});  // no target + 1 overflow
    CHECK_EQ(sol.searchRange(extremes, INT_MIN), vector<int>{0, 1});

    for (int iter = 0; iter < 500; iter++) {        // stress test vs a linear scan
        vector<int> v = t::rand_vec((int)t::rand_int(0, 25), -6, 6);
        sort(v.begin(), v.end());
        int target = (int)t::rand_int(-8, 8);
        CHECK_EQ(sol.searchRange(v, target), brute(v, target));
        CHECK_EQ(stl.searchRange(v, target), brute(v, target));
    }
    return t::summary("0034-first-last-position");
}
