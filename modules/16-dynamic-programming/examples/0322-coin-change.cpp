// 322. Coin Change: https://leetcode.com/problems/coin-change/
// Pattern: unbounded knapsack (min count), "which coin is last". Module 16 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        const int INF = amount + 1;                  // more coins than any real answer, and INF + 1 can't overflow
        vector<int> fewest(amount + 1, INF);         // fewest[a] = fewest coins summing to exactly a
        fewest[0] = 0;
        for (int a = 1; a <= amount; a++)            // upward: every fewest[a - c] is final before fewest[a]
            for (int c : coins)
                if (c <= a) fewest[a] = min(fewest[a], fewest[a - c] + 1);   // c is the last coin
        return fewest[amount] == INF ? -1 : fewest[amount];
    }
};
// [/snippet]

// Brute force: try every count of every coin (small amounts only). INT_MAX = impossible.
int brute(const vector<int>& coins, size_t i, int remaining) {
    if (remaining == 0) return 0;
    if (i == coins.size()) return INT_MAX;
    int best = INT_MAX;
    for (int k = 0; (long long)k * coins[i] <= remaining; k++) {
        int rest = brute(coins, i + 1, remaining - k * coins[i]);
        if (rest != INT_MAX) best = min(best, rest + k);
    }
    return best;
}

int main() {
    Solution sol;
    vector<int> c125 = {1, 2, 5}, c2 = {2}, c1 = {1};
    CHECK_EQ(sol.coinChange(c125, 11), 3);       // official examples
    CHECK_EQ(sol.coinChange(c2, 3), -1);
    CHECK_EQ(sol.coinChange(c1, 0), 0);

    vector<int> greedyTrap = {1, 3, 4}, huge = {2147483647}, mixed = {186, 419, 83, 408};
    CHECK_EQ(sol.coinChange(greedyTrap, 6), 2);  // greedy takes 4 + 1 + 1; the DP finds 3 + 3
    CHECK_EQ(sol.coinChange(huge, 2), -1);       // a coin bigger than any amount is simply skipped
    CHECK_EQ(sol.coinChange(huge, 0), 0);
    CHECK_EQ(sol.coinChange(mixed, 6249), 20);   // no small coin: most amounts are unreachable
    vector<int> ones = {1};
    CHECK_EQ(sol.coinChange(ones, 10000), 10000);  // largest amount

    for (int iter = 0; iter < 300; iter++) {     // stress test vs trying every count of every coin
        vector<int> coins = t::rand_vec((int)t::rand_int(1, 4), 1, 12);
        int amount = (int)t::rand_int(0, 40);
        int expected = brute(coins, 0, amount);
        CHECK_EQ(sol.coinChange(coins, amount), expected == INT_MAX ? -1 : expected);
    }
    return t::summary("0322-coin-change");
}
