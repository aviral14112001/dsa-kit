// Module 06 section 4: sweep lines over sorted events, and a map-based sweep for huge, sparse coordinates.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:union_length]
// Total length covered by at least one interval. Intervals are half-open [start, end), start < end.
// Each interval becomes two events; sort them by position and walk left to right, keeping
// `active` = how many intervals cover the stretch since the previous event.
long long union_length(const vector<pair<int, int>>& intervals) {
    vector<pair<int, int>> events;                   // (position, +1 = an interval starts, -1 = one ends)
    for (auto [start, end] : intervals) {
        events.push_back({start, +1});
        events.push_back({end, -1});
    }
    sort(events.begin(), events.end());
    long long covered = 0;
    int active = 0;
    for (size_t k = 0; k < events.size(); k++) {
        if (k > 0 && active > 0) covered += (long long)events[k].first - events[k - 1].first;
        active += events[k].second;
    }
    return covered;
}
// [/snippet]

// [snippet:covered_by_k]
// Total length covered by at least k intervals (k >= 1), coordinates up to 1e9.
// A difference ARRAY would need 1e9 slots; a MAP stores only the positions where coverage changes,
// and iterates them in sorted order, which is exactly the sweep. O(n log n).
long long covered_by_at_least(const vector<pair<int, int>>& intervals, int k) {
    map<int, int> delta;                             // position -> change in coverage at that position
    for (auto [start, end] : intervals) {            // half-open [start, end)
        delta[start]++;                              // operator[] creates the key at 0 first: wanted here
        delta[end]--;
    }
    long long covered = 0;
    int coverage = 0, prev = 0;
    for (auto [pos, change] : delta) {
        if (coverage >= k) covered += (long long)pos - prev;   // coverage was constant on [prev, pos)
        coverage += change;
        prev = pos;
    }
    return covered;
}
// [/snippet]

// Brute force on small coordinates: count the intervals covering each unit cell [x, x + 1).
long long covered_brute(const vector<pair<int, int>>& intervals, int k) {
    long long covered = 0;
    for (int x = -30; x < 30; x++) {
        int c = 0;
        for (auto [start, end] : intervals) c += start <= x && x < end;
        covered += c >= k;
    }
    return covered;
}

int main() {
    CHECK_EQ(union_length({{1, 4}, {2, 6}, {8, 9}}), 6);                  // [1, 6) and [8, 9)
    CHECK_EQ(union_length({{0, 2}, {2, 5}}), 5);                          // touching intervals
    CHECK_EQ(union_length({}), 0);
    CHECK_EQ(covered_by_at_least({{1, 4}, {2, 6}, {8, 9}}, 2), 2);        // only [2, 4) is doubled
    CHECK_EQ(covered_by_at_least({{0, 1'000'000'000}, {500'000'000, 1'000'000'000}}, 2), 500'000'000);
    CHECK_EQ(covered_by_at_least({{-1'000'000'000, 1'000'000'000}}, 1), 2'000'000'000LL);   // needs 64 bits

    for (int iter = 0; iter < 300; iter++) {
        vector<pair<int, int>> intervals;
        for (int m = (int)t::rand_int(0, 7); m > 0; m--) {
            int start = (int)t::rand_int(-25, 24), end = (int)t::rand_int(start + 1, 25);
            intervals.push_back({start, end});
        }
        int k = (int)t::rand_int(1, 3);
        CHECK_EQ(union_length(intervals), covered_brute(intervals, 1));
        CHECK_EQ(covered_by_at_least(intervals, k), covered_brute(intervals, k));
    }
    return t::summary("06-sweep");
}
