// 2406. Divide Intervals Into Minimum Number of Groups:
//       https://leetcode.com/problems/divide-intervals-into-minimum-number-of-groups/
// Pattern: minimum rooms = maximum overlap; a min-heap of end times, or a sweep line. Module 11 section 3.
// The free twin of 253. Meeting Rooms II (Premium), with closed intervals instead of half-open.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace heap {
// [snippet:solution]
class Solution {
public:
    int minGroups(vector<vector<int>>& intervals) {
        sort(intervals.begin(), intervals.end());                    // by start
        priority_queue<int, vector<int>, greater<int>> groupEnds;    // min-heap: the group that frees up first
        for (const auto& iv : intervals) {
            if (!groupEnds.empty() && groupEnds.top() < iv[0])       // closed: must end strictly before
                groupEnds.pop();                                     // reuse that group
            groupEnds.push(iv[1]);                                   // iv is now that group's last interval
        }
        return groupEnds.size();                                     // one heap entry per group
    }
};
// [/snippet]
}  // namespace heap

namespace sweep {
// [snippet:sweep]
class Solution {
public:
    int minGroups(vector<vector<int>>& intervals) {
        vector<pair<int, int>> events;                   // (coordinate, +1 = starts, -1 = has ended)
        for (const auto& iv : intervals) {
            events.push_back({iv[0], +1});
            events.push_back({iv[1] + 1, -1});           // [l, r] covers the integers l..r: gone at r + 1
        }
        sort(events.begin(), events.end());              // same coordinate: -1 before +1 (leave, then enter)
        int active = 0, most = 0;
        for (const auto& e : events) {
            active += e.second;
            most = max(most, active);                    // the most intervals covering one point
        }
        return most;
    }
};
// [/snippet]
}  // namespace sweep

// Brute force: the answer is the most intervals covering any single integer point.
int brute(const vector<vector<int>>& intervals) {
    int most = 0;
    for (int x = 1; x <= 30; x++) {
        int covering = 0;
        for (const auto& iv : intervals) covering += iv[0] <= x && x <= iv[1];
        most = max(most, covering);
    }
    return most;
}

template <class S>
void run(S& sol) {
    vector<vector<int>> a{{5, 10}, {6, 8}, {1, 5}, {2, 3}, {1, 10}};
    CHECK_EQ(sol.minGroups(a), 3);                               // the official examples
    vector<vector<int>> b{{1, 3}, {5, 6}, {8, 10}, {11, 13}};
    CHECK_EQ(sol.minGroups(b), 1);
    vector<vector<int>> c{{1, 5}, {5, 8}};
    CHECK_EQ(sol.minGroups(c), 2);                               // sharing an endpoint is a clash
    vector<vector<int>> d{{1, 5}, {6, 8}};
    CHECK_EQ(sol.minGroups(d), 1);                               // 5 < 6: no shared point
    vector<vector<int>> e{{2, 4}, {2, 4}, {2, 4}, {2, 4}};
    CHECK_EQ(sol.minGroups(e), 4);                               // all identical
    vector<vector<int>> f{{1, 1}};
    CHECK_EQ(sol.minGroups(f), 1);
    vector<vector<int>> g{{1, 1000000}, {1000000, 1000000}};
    CHECK_EQ(sol.minGroups(g), 2);                               // the largest coordinates

    for (int iter = 0; iter < 300; iter++) {                     // stress test vs brute force
        int n = (int)t::rand_int(1, 10);
        vector<vector<int>> ivs(n);
        for (auto& iv : ivs) {
            int l = (int)t::rand_int(1, 25), len = (int)t::rand_int(0, 5);
            iv = {l, l + len};
        }
        int expected = brute(ivs);
        CHECK_EQ(sol.minGroups(ivs), expected);
    }
}

int main() {
    heap::Solution s1;
    sweep::Solution s2;
    run(s1);
    run(s2);
    return t::summary("2406-divide-intervals-into-minimum-number-of-groups");
}
