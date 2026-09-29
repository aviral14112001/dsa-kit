// 787. Cheapest Flights Within K Stops: https://leetcode.com/problems/cheapest-flights-within-k-stops/
// Pattern: Bellman-Ford limited to k + 1 rounds, each round relaxing from a copy of the last. Module 15 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int k) {
        const int INF = INT_MAX;
        vector<int> cost(n, INF);                   // cost[v] = cheapest price using at most `round` flights
        cost[src] = 0;
        for (int round = 1; round <= k + 1; round++) {   // k stops = at most k + 1 flights
            vector<int> next = cost;                // read last round's prices, write this round's
            for (auto& f : flights) {
                int u = f[0], v = f[1], price = f[2];
                if (cost[u] != INF && cost[u] + price < next[v]) next[v] = cost[u] + price;
            }
            cost.swap(next);                        // O(1): next becomes the new "last round"
        }
        return cost[dst] == INF ? -1 : cost[dst];
    }
};
// [/snippet]

// The classic bug: relaxing in place lets one round chain several flights (u -> v, then v -> w
// using the v just updated), so "round i" no longer means "at most i flights".
int buggy_in_place(int n, const vector<vector<int>>& flights, int src, int dst, int k) {
    vector<int> cost(n, INT_MAX);
    cost[src] = 0;
    for (int round = 1; round <= k + 1; round++)
        for (auto& f : flights)
            if (cost[f[0]] != INT_MAX && cost[f[0]] + f[2] < cost[f[1]]) cost[f[1]] = cost[f[0]] + f[2];
    return cost[dst] == INT_MAX ? -1 : cost[dst];
}

// Brute force: DFS over every route from src with at most k + 1 flights.
void explore(int u, int flights_left, int spent, int dst, const vector<vector<int>>& flights, int& best) {
    if (u == dst) best = min(best, spent);
    if (flights_left == 0) return;
    for (auto& f : flights)
        if (f[0] == u) explore(f[1], flights_left - 1, spent + f[2], dst, flights, best);
}

int brute(int n, const vector<vector<int>>& flights, int src, int dst, int k) {
    int best = INT_MAX;
    explore(src, k + 1, 0, dst, flights, best);
    return best == INT_MAX ? -1 : best;
}

int main() {
    Solution sol;
    vector<vector<int>> ex1{{0, 1, 100}, {1, 2, 100}, {2, 0, 100}, {1, 3, 600}, {2, 3, 200}};
    CHECK_EQ(sol.findCheapestPrice(4, ex1, 0, 3, 1), 700);   // 0 -> 1 -> 3; the 400 route needs 2 stops
    vector<vector<int>> ex2{{0, 1, 100}, {1, 2, 100}, {0, 2, 500}};
    CHECK_EQ(sol.findCheapestPrice(3, ex2, 0, 2, 1), 200);
    CHECK_EQ(sol.findCheapestPrice(3, ex2, 0, 2, 0), 500);   // no stops allowed: only the direct flight
    CHECK_EQ(buggy_in_place(3, ex2, 0, 2, 0), 200);          // wrong: chained 0 -> 1 -> 2 in one round
    vector<vector<int>> none{{0, 1, 5}};
    CHECK_EQ(sol.findCheapestPrice(3, none, 0, 2, 5), -1);   // unreachable
    CHECK_EQ(sol.findCheapestPrice(3, none, 1, 1, 0), 0);    // already there

    int buggy_wrong = 0;
    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(2, 6);
        vector<vector<int>> flights;
        set<pair<int, int>> used;                            // no self-loops, no repeated (u, v)
        int m = (int)t::rand_int(0, 12);
        for (int i = 0; i < m; i++) {
            int u = (int)t::rand_int(0, n - 1), v = (int)t::rand_int(0, n - 1);
            if (u == v || used.count({u, v})) continue;
            used.insert({u, v});
            flights.push_back({u, v, (int)t::rand_int(1, 50)});
        }
        int src = (int)t::rand_int(0, n - 1), dst = (int)t::rand_int(0, n - 1), k = (int)t::rand_int(0, 4);
        int expect = brute(n, flights, src, dst, k);
        CHECK_EQ(sol.findCheapestPrice(n, flights, src, dst, k), expect);
        buggy_wrong += buggy_in_place(n, flights, src, dst, k) != expect;
    }
    CHECK(buggy_wrong > 0);                                   // the in-place version really does fail
    return t::summary("0787-cheapest-flights-within-k-stops");
}
