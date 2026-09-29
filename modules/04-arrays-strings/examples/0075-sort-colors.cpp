// 75. Sort Colors: https://leetcode.com/problems/sort-colors/
// Pattern: Dutch national flag (three-way partition, one pass). Module 04 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    void sortColors(vector<int>& nums) {
        // Invariant:  [0, low) all 0  |  [low, mid) all 1  |  [mid, high] unknown  |  (high, n) all 2
        int low = 0, mid = 0, high = (int)nums.size() - 1;
        while (mid <= high) {                     // until the unknown region is empty
            if (nums[mid] == 0) {
                swap(nums[low], nums[mid]);       // nums[low] is a 1 (or low == mid): both regions shift
                low++;
                mid++;
            } else if (nums[mid] == 1) {
                mid++;
            } else {
                swap(nums[mid], nums[high]);      // the value that arrives at mid is unknown:
                high--;                           // look at it next round, so don't advance mid
            }
        }
    }
};
// [/snippet]

// Reference: counting sort in two passes. Also O(n), but the follow-up asks for one pass.
void sort_colors_counting(vector<int>& nums) {
    int count[3] = {};
    for (int x : nums) count[x]++;
    int i = 0;
    for (int color = 0; color < 3; color++)
        for (int c = 0; c < count[color]; c++) nums[i++] = color;
}

int main() {
    Solution sol;
    vector<int> ex1{2, 0, 2, 1, 1, 0};
    sol.sortColors(ex1);
    CHECK_EQ(ex1, vector<int>{0, 0, 1, 1, 2, 2});
    vector<int> ex2{2, 0, 1};
    sol.sortColors(ex2);
    CHECK_EQ(ex2, vector<int>{0, 1, 2});

    vector<int> single{1};
    sol.sortColors(single);
    CHECK_EQ(single, vector<int>{1});
    vector<int> twos{2, 2, 2};
    sol.sortColors(twos);
    CHECK_EQ(twos, vector<int>{2, 2, 2});
    vector<int> descending{2, 2, 1, 0, 0};
    sol.sortColors(descending);
    CHECK_EQ(descending, vector<int>{0, 0, 1, 2, 2});

    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(1, 20), 0, 2);
        auto b = a;
        sol.sortColors(a);
        sort_colors_counting(b);
        CHECK_EQ(a, b);
    }
    return t::summary("0075-sort-colors");
}
