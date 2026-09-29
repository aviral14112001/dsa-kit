// 84. Largest Rectangle in Histogram: https://leetcode.com/problems/largest-rectangle-in-histogram/
// Pattern: previous / next smaller element (monotonic stack). Module 09 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
#include "monotonic.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int largestRectangleArea(vector<int>& heights) {
        int n = (int)heights.size(), best = 0;
        vector<int> st;  // indices of bars with strictly increasing heights
        for (int i = 0; i <= n; i++) {
            int h = i == n ? 0 : heights[i];  // a height-0 bar at the end flushes the stack
            while (!st.empty() && heights[st.back()] >= h) {
                // Bar `top` can't extend right past i. Its left limit is the bar below it on the
                // stack (the previous smaller one), so its widest rectangle spans (left, i).
                int top = st.back();
                st.pop_back();
                int left = st.empty() ? -1 : st.back();
                best = max(best, heights[top] * (i - left - 1));
            }
            st.push_back(i);
        }
        return best;
    }
};
// [/snippet]

// [snippet:boundaries]
// The same answer from templates/monotonic.hpp: bar i's widest rectangle of height heights[i]
// spans the open interval (previous smaller, next smaller).
int largestRectangleBoundaries(const vector<int>& heights) {
    vector<int> left = prev_smaller(heights), right = next_smaller(heights);
    int best = 0;
    for (int i = 0; i < (int)heights.size(); i++)
        best = max(best, heights[i] * (right[i] - left[i] - 1));
    return best;
}
// [/snippet]

// Brute force: every range [l, r] with its minimum. O(n^2).
long long brute(const vector<int>& h) {
    long long best = 0;
    for (int l = 0; l < (int)h.size(); l++) {
        int lowest = INT_MAX;
        for (int r = l; r < (int)h.size(); r++) {
            lowest = min(lowest, h[r]);
            best = max(best, (long long)lowest * (r - l + 1));
        }
    }
    return best;
}

int main() {
    Solution sol;
    // official examples
    vector<int> ex1{2, 1, 5, 6, 2, 3};
    CHECK_EQ(sol.largestRectangleArea(ex1), 10);
    vector<int> ex2{2, 4};
    CHECK_EQ(sol.largestRectangleArea(ex2), 4);
    // edge cases
    vector<int> one{7};
    CHECK_EQ(sol.largestRectangleArea(one), 7);
    vector<int> zero{0};
    CHECK_EQ(sol.largestRectangleArea(zero), 0);
    vector<int> flat{3, 3, 3};                     // equal heights: one rectangle spans them all
    CHECK_EQ(sol.largestRectangleArea(flat), 9);
    vector<int> rising{1, 2, 3, 4, 5};             // nothing pops until the flush at the end
    CHECK_EQ(sol.largestRectangleArea(rising), 9);
    vector<int> falling{5, 4, 3, 2, 1};
    CHECK_EQ(sol.largestRectangleArea(falling), 9);
    CHECK_EQ(largestRectangleBoundaries(ex1), 10);

    // LeetCode's largest case: 10^5 bars of height 10^4. The area 10^9 still fits in an int.
    vector<int> tall(100000, 10000);
    CHECK_EQ(sol.largestRectangleArea(tall), 1'000'000'000);
    CHECK_EQ(largestRectangleBoundaries(tall), 1'000'000'000);

    // stress vs the O(n^2) brute force; a small height range makes ties common
    for (int iter = 0; iter < 500; iter++) {
        vector<int> h = t::rand_vec((int)t::rand_int(1, 25), 0, iter % 2 ? 4 : 50);
        long long expected = brute(h);
        CHECK_EQ(sol.largestRectangleArea(h), expected);
        CHECK_EQ(largestRectangleBoundaries(h), expected);
    }
    return t::summary("0084-largest-rectangle-in-histogram");
}
