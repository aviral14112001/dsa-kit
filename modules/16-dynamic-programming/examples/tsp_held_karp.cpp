// Travelling salesman by bitmask DP (Held-Karp): O(2^n * n^2) instead of O(n!).
// Pattern: DP over (set of visited cities, last city). Module 16 section 4. Stress-tested against all permutations.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:tsp]
// Shortest tour that starts at city 0, visits every city exactly once and returns to 0.
// dist is an n x n matrix (it may be asymmetric); n <= ~16.
long long shortestTour(const vector<vector<int>>& dist) {
    int n = dist.size();
    if (n == 1) return 0;
    const long long INF = LLONG_MAX / 4;
    // best[mask][last] = shortest path from 0 through exactly the cities in mask, ending at last
    vector<vector<long long>> best(1 << n, vector<long long>(n, INF));
    best[1][0] = 0;                                   // visited {0}, standing on 0
    for (int mask = 1; mask < (1 << n); mask++)       // supersets come later numerically
        for (int last = 0; last < n; last++) {
            if (best[mask][last] == INF) continue;    // unreachable (e.g. mask without city 0)
            for (int next = 0; next < n; next++) {
                if (mask >> next & 1) continue;       // already visited
                int grown = mask | 1 << next;
                best[grown][next] = min(best[grown][next], best[mask][last] + dist[last][next]);
            }
        }
    long long tour = INF;
    for (int last = 1; last < n; last++)              // close the cycle back to city 0
        tour = min(tour, best[(1 << n) - 1][last] + dist[last][0]);
    return tour;
}
// [/snippet]

// Brute force: fix city 0 first, try every order of the others ((n-1)! tours).
long long brute(const vector<vector<int>>& dist) {
    int n = dist.size();
    vector<int> order(n);
    iota(order.begin(), order.end(), 0);
    long long best = LLONG_MAX;
    do {
        long long len = 0;
        for (int i = 0; i < n; i++) len += dist[order[i]][order[(i + 1) % n]];
        best = min(best, len);
    } while (next_permutation(order.begin() + 1, order.end()));
    return best;
}

int main() {
    vector<vector<int>> classic = {{0, 10, 15, 20}, {10, 0, 35, 25}, {15, 35, 0, 30}, {20, 25, 30, 0}};
    CHECK_EQ(shortestTour(classic), 80);              // 0 -> 1 -> 3 -> 2 -> 0
    CHECK_EQ(shortestTour({{0}}), 0);                 // one city: no travel
    CHECK_EQ(shortestTour({{0, 3}, {5, 0}}), 8);      // two cities: there and back (asymmetric)

    for (int iter = 0; iter < 200; iter++) {          // stress test vs every permutation, n <= 8
        int n = (int)t::rand_int(1, 8);
        vector<vector<int>> dist(n, vector<int>(n, 0));
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (i != j) dist[i][j] = (int)t::rand_int(1, 100);
        CHECK_EQ(shortestTour(dist), brute(dist));
    }
    return t::summary("tsp_held_karp");
}
