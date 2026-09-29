// 239. Sliding Window Maximum: https://leetcode.com/problems/sliding-window-maximum/
// Pattern: monotonic deque of indices (expire from the front, drop dominated from the back).
// Module 09 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
#include "monotonic.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        vector<int> result;
        deque<int> dq;  // indices in the window, values strictly decreasing front to back
        for (int i = 0; i < (int)nums.size(); i++) {
            // nums[i] is at least as big and leaves later: smaller-or-equal older values are dead
            while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
            dq.push_back(i);
            if (dq.front() == i - k) dq.pop_front();  // the front index just slid out on the left
            if (i >= k - 1) result.push_back(nums[dq.front()]);  // a full window: front is its max
        }
        return result;
    }
};
// [/snippet]

// Brute force: take the max of every window directly. O(n k).
vector<int> brute(const vector<int>& nums, int k) {
    vector<int> res;
    for (int i = 0; i + k <= (int)nums.size(); i++)
        res.push_back(*max_element(nums.begin() + i, nums.begin() + i + k));
    return res;
}

int main() {
    Solution sol;
    // official examples
    vector<int> ex1{1, 3, -1, -3, 5, 3, 6, 7};
    CHECK_EQ(sol.maxSlidingWindow(ex1, 3), vector<int>{3, 3, 5, 5, 6, 7});
    vector<int> ex2{1};
    CHECK_EQ(sol.maxSlidingWindow(ex2, 1), vector<int>{1});
    // edge cases
    vector<int> dup{4, 4, 4, 4};
    CHECK_EQ(sol.maxSlidingWindow(dup, 2), vector<int>{4, 4, 4});
    vector<int> falling{9, 7, 5, 3, 1};           // the max expires from the front every step
    CHECK_EQ(sol.maxSlidingWindow(falling, 2), vector<int>{9, 7, 5, 3});
    vector<int> rising{1, 3, 5, 7, 9};            // each new value clears the whole deque
    CHECK_EQ(sol.maxSlidingWindow(rising, 3), vector<int>{5, 7, 9});
    CHECK_EQ(sol.maxSlidingWindow(ex1, 8), vector<int>{7});   // k = n
    CHECK_EQ(sol.maxSlidingWindow(ex1, 1), ex1);              // k = 1
    vector<int> neg{-7, -8, 7, 5, 7, 1, 6, 0};
    CHECK_EQ(sol.maxSlidingWindow(neg, 4), vector<int>{7, 7, 7, 7, 7});

    // stress vs brute force and vs the template's sliding_window_max
    for (int iter = 0; iter < 500; iter++) {
        vector<int> nums = t::rand_vec((int)t::rand_int(1, 30), -5, iter % 2 ? 5 : 1000);
        int k = (int)t::rand_int(1, (long long)nums.size());
        vector<int> expected = brute(nums, k);
        CHECK_EQ(sol.maxSlidingWindow(nums, k), expected);
        CHECK_EQ(sliding_window_max(nums, k), expected);
    }
    return t::summary("0239-sliding-window-maximum");
}
