// 621. Task Scheduler: https://leetcode.com/problems/task-scheduler/
// Pattern: counting argument (the most frequent task fixes a frame; two lower bounds, both
// achievable). Module 11 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int leastInterval(vector<char>& tasks, int n) {
        int freq[26] = {};
        for (char c : tasks) freq[c - 'A']++;
        int maxFreq = *max_element(freq, freq + 26);
        int tiedForMax = count(freq, freq + 26, maxFreq);
        // Lower bound 1: the most frequent task needs maxFreq - 1 gaps of n slots between its copies:
        // maxFreq - 1 frames of n + 1 slots, then a last frame with every task tied for the max.
        int frames = (maxFreq - 1) * (n + 1) + tiedForMax;
        // Lower bound 2: every task takes a slot. The answer is the larger bound, and it's achievable.
        return max(frames, (int)tasks.size());
    }
};
// [/snippet]

// Brute force: BFS over schedules. A state is (copies left of each task, slots each task must still
// wait); every time slot runs one ready task or idles. The first state with nothing left is optimal.
int brute(const vector<char>& tasks, int n) {
    map<char, int> cnt;
    for (char c : tasks) cnt[c]++;
    int kinds = cnt.size();
    vector<int> start;                                   // [left_0..left_{k-1}, wait_0..wait_{k-1}]
    for (auto& kv : cnt) start.push_back(kv.second);
    start.resize(2 * kinds, 0);
    map<vector<int>, int> dist{{start, 0}};
    queue<vector<int>> q;
    q.push(start);
    while (!q.empty()) {
        vector<int> s = q.front();
        q.pop();
        int d = dist[s];
        if (all_of(s.begin(), s.begin() + kinds, [](int left) { return left == 0; })) return d;
        for (int pick = -1; pick < kinds; pick++) {      // -1 = idle
            if (pick >= 0 && (s[pick] == 0 || s[kinds + pick] > 0)) continue;
            vector<int> next = s;
            for (int i = 0; i < kinds; i++) next[kinds + i] = max(0, next[kinds + i] - 1);
            if (pick >= 0) {
                next[pick]--;
                next[kinds + pick] = n;                  // skip the next n slots
            }
            if (!dist.count(next)) {
                dist[next] = d + 1;
                q.push(next);
            }
        }
    }
    return -1;
}

vector<char> chars(const string& s) { return vector<char>(s.begin(), s.end()); }

int main() {
    Solution sol;
    auto a = chars("AAABBB");
    CHECK_EQ(sol.leastInterval(a, 2), 8);                 // A B _ A B _ A B
    auto b = chars("ACABDB");
    CHECK_EQ(sol.leastInterval(b, 1), 6);                 // no idle needed
    CHECK_EQ(sol.leastInterval(a, 3), 10);                // A B _ _ A B _ _ A B
    CHECK_EQ(sol.leastInterval(a, 0), 6);                 // n = 0: no cooldown at all
    auto c = chars("AAAA");
    CHECK_EQ(sol.leastInterval(c, 3), 13);                // one task type: (4-1)*4 + 1
    auto d = chars("AAABBBCCCDDE");
    CHECK_EQ(sol.leastInterval(d, 2), 12);                // frames = 2*3 + 3 = 9 < 12 tasks: no idles
    auto e = chars("ABCDEFG");
    CHECK_EQ(sol.leastInterval(e, 2), 7);
    vector<char> big(10000, 'A');
    CHECK_EQ(sol.leastInterval(big, 100), 9999 * 101 + 1);   // the largest answer the limits allow

    for (int iter = 0; iter < 300; iter++) {              // stress test vs the BFS
        string s = t::rand_string((int)t::rand_int(1, 8), 'A', 'C');
        int n = (int)t::rand_int(0, 3);
        auto v = chars(s);
        CHECK_EQ(sol.leastInterval(v, n), brute(v, n));
    }
    return t::summary("0621-task-scheduler");
}
