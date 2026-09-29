// Scheduling unit-time jobs with deadlines to maximise profit (the classic "job sequencing" problem).
// Pattern: sort by deadline + a min-heap of the jobs kept so far; when over capacity, drop the
// cheapest. Module 11 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

struct Job { int deadline, profit; };   // takes one time unit; must run in some slot 1..deadline

// [snippet:deadline_heap]
// Max total profit: one job per time slot 1, 2, 3, ..., each job finished by its deadline.
long long maxProfit(vector<Job> jobs) {
    sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b) { return a.deadline < b.deadline; });
    priority_queue<int, vector<int>, greater<int>> kept;   // min-heap: the cheapest kept job on top
    long long total = 0;
    // Invariant: `kept` is a most profitable set of the jobs seen so far that fits in the slots.
    for (const Job& j : jobs) {
        kept.push(j.profit);
        total += j.profit;
        if ((int)kept.size() > j.deadline) {                // more jobs than slots 1..deadline:
            total -= kept.top();                            // drop the least profitable one
            kept.pop();
        }
    }
    return total;
}
// [/snippet]

// A set of unit jobs fits iff, in deadline order, the i-th job (1-based) has deadline >= i.
bool fits(vector<Job> chosen) {
    sort(chosen.begin(), chosen.end(), [](const Job& a, const Job& b) { return a.deadline < b.deadline; });
    for (int i = 0; i < (int)chosen.size(); i++)
        if (chosen[i].deadline < i + 1) return false;
    return true;
}

// Brute force: every subset of jobs that fits. O(2^n * n log n).
long long brute(const vector<Job>& jobs) {
    int n = jobs.size();
    long long best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        vector<Job> chosen;
        long long profit = 0;
        for (int i = 0; i < n; i++)
            if ((mask >> i) & 1) {
                chosen.push_back(jobs[i]);
                profit += jobs[i].profit;
            }
        if (fits(chosen)) best = max(best, profit);
    }
    return best;
}

int main() {
    // (deadline, profit). Best: 40 in slot 1, 30 in slot 2, 25 in slot 3.
    CHECK_EQ(maxProfit({{1, 20}, {2, 15}, {2, 30}, {3, 5}, {3, 25}, {1, 40}}), 95LL);
    CHECK_EQ(maxProfit({{1, 10}, {1, 20}, {1, 30}}), 30LL);          // one slot: keep the best
    CHECK_EQ(maxProfit({{5, 1}, {5, 2}, {5, 3}}), 6LL);              // room for all of them
    CHECK_EQ(maxProfit({{2, 50}, {1, 10}, {2, 20}}), 70LL);          // the deadline-1 job loses its slot
    CHECK_EQ(maxProfit({}), 0LL);
    for (int iter = 0; iter < 300; iter++) {                         // stress test vs every subset
        vector<Job> jobs((int)t::rand_int(1, 10));
        for (Job& j : jobs) j = {(int)t::rand_int(1, 6), (int)t::rand_int(1, 50)};
        CHECK_EQ(maxProfit(jobs), brute(jobs));
    }
    return t::summary("deadline_scheduling");
}
