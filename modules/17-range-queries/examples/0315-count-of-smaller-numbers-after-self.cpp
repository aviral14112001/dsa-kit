// 315. Count of Smaller Numbers After Self: https://leetcode.com/problems/count-of-smaller-numbers-after-self/
// Pattern: Fenwick tree over compressed values, scanning right to left. Module 17 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> countSmaller(vector<int>& nums) {
        int n = (int)nums.size();
        // Coordinate compression: each value -> its rank among the distinct values (0 .. m-1).
        vector<int> sorted_vals(nums);
        sort(sorted_vals.begin(), sorted_vals.end());
        sorted_vals.erase(unique(sorted_vals.begin(), sorted_vals.end()), sorted_vals.end());
        int m = (int)sorted_vals.size();

        vector<int> tree(m + 1, 0);          // Fenwick over ranks: how many of each rank lie right of i
        vector<int> result(n);
        for (int i = n - 1; i >= 0; i--) {
            int r = (int)(lower_bound(sorted_vals.begin(), sorted_vals.end(), nums[i]) - sorted_vals.begin());
            int smaller = 0;
            for (int j = r; j > 0; j -= j & -j) smaller += tree[j];    // count of ranks 0 .. r-1 seen
            result[i] = smaller;
            for (int j = r + 1; j <= m; j += j & -j) tree[j]++;       // now nums[i] is "to the right"
        }
        return result;
    }
};
// [/snippet]

// Brute force for the stress test: O(n^2).
vector<int> brute(const vector<int>& nums) {
    int n = (int)nums.size();
    vector<int> res(n, 0);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) res[i] += nums[j] < nums[i];
    return res;
}

int main() {
    Solution sol;
    vector<int> ex1{5, 2, 6, 1}, ex2{-1}, ex3{-1, -1};
    CHECK_EQ(sol.countSmaller(ex1), vector<int>{2, 1, 1, 0});
    CHECK_EQ(sol.countSmaller(ex2), vector<int>{0});
    CHECK_EQ(sol.countSmaller(ex3), vector<int>{0, 0});               // equal is not smaller

    // Edge cases: strictly decreasing / increasing, extreme values.
    vector<int> dec{5, 4, 3, 2, 1}, inc{1, 2, 3}, ext{10000, -10000, 10000, -10000};
    CHECK_EQ(sol.countSmaller(dec), vector<int>{4, 3, 2, 1, 0});
    CHECK_EQ(sol.countSmaller(inc), vector<int>{0, 0, 0});
    CHECK_EQ(sol.countSmaller(ext), vector<int>{2, 0, 1, 0});

    // The largest input: n = 1e5, non-increasing in runs of 5 equal values (10000 down to -9999).
    // Everything right of i is smaller except the rest of i's run.
    vector<int> big(100000);
    for (int i = 0; i < 100000; i++) big[i] = 10000 - i / 5;
    auto big_res = sol.countSmaller(big);
    CHECK_EQ(big_res[0], 100000 - 5);
    CHECK_EQ(big_res[12], 100000 - 15);
    CHECK_EQ(big_res[99999], 0);

    // Stress against the O(n^2) brute force, with many duplicates.
    for (int iter = 0; iter < 300; iter++) {
        vector<int> v = t::rand_vec((int)t::rand_int(1, 40), -6, 6);
        CHECK_EQ(sol.countSmaller(v), brute(v));
    }
    return t::summary("0315-count-of-smaller-numbers-after-self");
}
