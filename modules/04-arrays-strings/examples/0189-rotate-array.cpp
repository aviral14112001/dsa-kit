// 189. Rotate Array: https://leetcode.com/problems/rotate-array/
// Pattern: triple reverse (in place, O(1) extra space). Module 04 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    void rotate(vector<int>& nums, int k) {
        int n = (int)nums.size();
        k %= n;                                       // rotating by n changes nothing, and k may exceed n
        reverse(nums.begin(), nums.end());            // A B  ->  B^R A^R      (B = the last k elements)
        reverse(nums.begin(), nums.begin() + k);      // B^R  ->  B
        reverse(nums.begin() + k, nums.end());        // A^R  ->  A
    }
};
// [/snippet]

// [snippet:extra_array]
// The easy O(n)-space version: the element at index i belongs at (i + k) % n.
void rotate_copy(vector<int>& nums, int k) {
    int n = (int)nums.size();
    vector<int> out(n);
    for (int i = 0; i < n; i++) out[(i + k) % n] = nums[i];
    nums = out;
}
// [/snippet]

// Brute force: shift everything one step right, k times. O(n·k): up to 10^10 moves at the limits.
void rotate_brute(vector<int>& nums, int k) {
    for (int step = 0; step < k; step++) {
        int last = nums.back();
        for (int i = (int)nums.size() - 1; i > 0; i--) nums[i] = nums[i - 1];
        nums[0] = last;
    }
}

int main() {
    Solution sol;
    vector<int> ex1{1, 2, 3, 4, 5, 6, 7};
    sol.rotate(ex1, 3);
    CHECK_EQ(ex1, vector<int>{5, 6, 7, 1, 2, 3, 4});
    vector<int> ex2{-1, -100, 3, 99};
    sol.rotate(ex2, 2);
    CHECK_EQ(ex2, vector<int>{3, 99, -1, -100});

    vector<int> one{42};
    sol.rotate(one, 100000);                          // k far larger than n
    CHECK_EQ(one, vector<int>{42});
    vector<int> full{1, 2, 3};
    sol.rotate(full, 3);                              // k == n: unchanged
    CHECK_EQ(full, vector<int>{1, 2, 3});
    vector<int> zero{1, 2, 3};
    sol.rotate(zero, 0);
    CHECK_EQ(zero, vector<int>{1, 2, 3});
    vector<int> wrap{1, 2, 3};
    sol.rotate(wrap, 4);                              // same as k = 1
    CHECK_EQ(wrap, vector<int>{3, 1, 2});

    for (int iter = 0; iter < 300; iter++) {          // stress: all three versions agree
        auto a = t::rand_vec((int)t::rand_int(1, 12), -9, 9);
        int k = (int)t::rand_int(0, 30);
        auto b = a, c = a;
        sol.rotate(a, k);
        rotate_brute(b, k);
        rotate_copy(c, k);
        CHECK_EQ(a, b);
        CHECK_EQ(c, b);
    }
    return t::summary("0189-rotate-array");
}
