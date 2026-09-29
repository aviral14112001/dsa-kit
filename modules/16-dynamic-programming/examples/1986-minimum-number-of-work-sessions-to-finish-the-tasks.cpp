// 1986. Minimum Number of Work Sessions to Finish the Tasks:
//       https://leetcode.com/problems/minimum-number-of-work-sessions-to-finish-the-tasks/
// Pattern: bitmask DP over the set of finished tasks; O(3^n) submask version as a follow-up. Module 16 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int minSessions(vector<int>& tasks, int sessionTime) {
        int n = tasks.size();
        // best[mask] = smallest {sessions opened, minutes used in the current session} over every
        // order of finishing exactly the tasks in mask (pairs compare lexicographically)
        vector<pair<int, int>> best(1 << n, {INT_MAX, INT_MAX});
        best[0] = {0, sessionTime};                     // no session yet: a "full" one, so task 1 opens one
        for (int mask = 0; mask < (1 << n); mask++) {   // a task only ever makes the mask bigger
            auto [sessions, used] = best[mask];
            for (int i = 0; i < n; i++) {
                if (mask >> i & 1) continue;            // task i already done
                pair<int, int> next = used + tasks[i] <= sessionTime
                                          ? pair{sessions, used + tasks[i]}   // fits in the current session
                                          : pair{sessions + 1, tasks[i]};     // open a new one for it
                best[mask | 1 << i] = min(best[mask | 1 << i], next);
            }
        }
        return best[(1 << n) - 1].first;
    }
};
// [/snippet]

namespace submask {
// [snippet:submask]
class Solution {
public:
    int minSessions(vector<int>& tasks, int sessionTime) {
        int n = tasks.size(), full = (1 << n) - 1;
        vector<int> total(1 << n, 0);                   // total[mask] = minutes of the tasks in mask
        for (int mask = 1; mask <= full; mask++)
            total[mask] = total[mask & (mask - 1)] + tasks[__builtin_ctz(mask)];   // drop the lowest task
        vector<int> fewest(1 << n, INT_MAX);            // fewest[mask] = fewest sessions to finish mask
        fewest[0] = 0;
        for (int mask = 1; mask <= full; mask++)
            for (int last = mask; last > 0; last = (last - 1) & mask)   // every non-empty submask of mask
                if (total[last] <= sessionTime)                          // ... that fits in one session
                    fewest[mask] = min(fewest[mask], fewest[mask ^ last] + 1);
        return fewest[full];
    }
};
// [/snippet]
}  // namespace submask

// Brute force: every way to split the tasks into sessions (task i joins an open session or opens one).
int brute(const vector<int>& tasks, int sessionTime, size_t i, vector<int>& load) {
    if (i == tasks.size()) return load.size();
    int best = INT_MAX;
    for (size_t s = 0; s < load.size(); s++)
        if (load[s] + tasks[i] <= sessionTime) {
            load[s] += tasks[i];
            best = min(best, brute(tasks, sessionTime, i + 1, load));
            load[s] -= tasks[i];
        }
    load.push_back(tasks[i]);
    best = min(best, brute(tasks, sessionTime, i + 1, load));
    load.pop_back();
    return best;
}

int main() {
    Solution sol;
    submask::Solution sub;
    vector<int> ex1 = {1, 2, 3}, ex2 = {3, 1, 3, 1, 1}, ex3 = {1, 2, 3, 4, 5};
    CHECK_EQ(sol.minSessions(ex1, 3), 2);        // official examples
    CHECK_EQ(sol.minSessions(ex2, 8), 2);
    CHECK_EQ(sol.minSessions(ex3, 15), 1);
    CHECK_EQ(sub.minSessions(ex1, 3), 2);
    CHECK_EQ(sub.minSessions(ex2, 8), 2);
    CHECK_EQ(sub.minSessions(ex3, 15), 1);

    vector<int> one = {10}, allFull(14, 10), allTiny(14, 1), ffdTrap = {3, 3, 2, 2, 2, 2};
    CHECK_EQ(sol.minSessions(one, 10), 1);
    CHECK_EQ(sol.minSessions(allFull, 10), 14);  // every task fills a session
    CHECK_EQ(sub.minSessions(allFull, 10), 14);
    CHECK_EQ(sol.minSessions(allTiny, 15), 1);   // n = 14: 2^14 masks
    CHECK_EQ(sol.minSessions(ffdTrap, 7), 2);    // {3,2,2} + {3,2,2}; "largest first, first fit" uses 3
    CHECK_EQ(sub.minSessions(ffdTrap, 7), 2);
    vector<int> tieBreak = {4, 3, 4, 7, 1};      // {3,7} + {4,4,1}; comparing only the session count
    CHECK_EQ(sol.minSessions(tieBreak, 10), 2);  // (not the minutes used) returns 3 here

    for (int iter = 0; iter < 1500; iter++) {    // stress test vs every split into sessions
        vector<int> tasks = t::rand_vec((int)t::rand_int(4, 9), 1, 10);   // n >= 4: enough tasks for ties to matter
        int sessionTime = (int)t::rand_int(*max_element(tasks.begin(), tasks.end()), 15);
        vector<int> load;
        int expected = brute(tasks, sessionTime, 0, load);
        CHECK_EQ(sol.minSessions(tasks, sessionTime), expected);
        CHECK_EQ(sub.minSessions(tasks, sessionTime), expected);
    }
    for (int iter = 0; iter < 5; iter++) {       // full size: the two formulations must agree
        vector<int> tasks = t::rand_vec(14, 1, 10);
        int sessionTime = (int)t::rand_int(10, 15);
        CHECK_EQ(sol.minSessions(tasks, sessionTime), sub.minSessions(tasks, sessionTime));
    }
    return t::summary("1986-minimum-number-of-work-sessions");
}
