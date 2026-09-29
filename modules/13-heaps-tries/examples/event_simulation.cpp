// Event simulation with a heap: "who frees up first?" answered in O(log k) per event. Module 13 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:simulation]
// k identical printers serve jobs first come, first served. jobs[i] = {arrival, duration}, sorted
// by arrival. Returns each job's finish time. O(n log k).
vector<long long> finish_times(const vector<pair<int, int>>& jobs, int k) {
    // One entry per printer: the time it becomes free. Min-heap: the soonest-free printer on top.
    priority_queue<long long, vector<long long>, greater<long long>> free_at;
    for (int i = 0; i < k; i++) free_at.push(0);
    vector<long long> finish;
    for (auto [arrival, duration] : jobs) {
        // Start when the job has arrived AND a printer is free. If a printer is already idle, the
        // max() jumps straight to the arrival: the clock moves event to event, never tick by tick.
        long long start = max<long long>(arrival, free_at.top());
        free_at.pop();
        long long done = start + duration;     // long long: n durations of up to 1e9 overflow int
        finish.push_back(done);
        free_at.push(done);
    }
    return finish;
}
// [/snippet]

// Brute force: an array of printer free times, scanned for the minimum. O(n k).
vector<long long> brute(const vector<pair<int, int>>& jobs, int k) {
    vector<long long> free_at(k, 0), finish;
    for (auto [arrival, duration] : jobs) {
        auto soonest = min_element(free_at.begin(), free_at.end());
        long long start = max<long long>(arrival, *soonest);
        *soonest = start + duration;
        finish.push_back(*soonest);
    }
    return finish;
}

int main() {
    // 2 printers. Jobs at t=0 (5 long) and t=1 (3 long) start at once; the t=2 job waits for the
    // printer that frees at t=4; the t=20 job finds both printers idle and starts on arrival.
    vector<pair<int, int>> jobs{{0, 5}, {1, 3}, {2, 4}, {20, 1}};
    CHECK_EQ(finish_times(jobs, 2), vector<long long>{5, 4, 8, 21});
    CHECK_EQ(finish_times(jobs, 1), vector<long long>{5, 8, 12, 21});
    CHECK_EQ(finish_times({}, 3), vector<long long>{});
    // durations that overflow int when added up
    vector<pair<int, int>> long_jobs(3, {0, 1'000'000'000});
    CHECK_EQ(finish_times(long_jobs, 1), vector<long long>{1'000'000'000, 2'000'000'000, 3'000'000'000});

    // stress against the brute force
    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(0, 30), k = (int)t::rand_int(1, 5);
        vector<pair<int, int>> js(n);
        for (auto& [arrival, duration] : js) arrival = (int)t::rand_int(0, 50), duration = (int)t::rand_int(1, 20);
        sort(js.begin(), js.end());
        CHECK_EQ(finish_times(js, k), brute(js, k));
    }
    return t::summary("event_simulation");
}
