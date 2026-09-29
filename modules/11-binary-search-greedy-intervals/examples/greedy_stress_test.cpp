// Testing greedy ideas against brute force. Module 11 section 2.
//   1. Scheduling to minimise total weighted completion time: Smith's rule (proved by the exchange
//      argument in Notes section 2) survives the stress test; two plausible rules don't.
//   2. Coin change with {1, 3, 4} and 0/1 knapsack by value density: greedy gives wrong answers.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// One machine, jobs run back to back. A job's completion time is when it finishes.
struct Job {
    int time, weight;
    auto operator<=>(const Job&) const = default;   // lets sort / next_permutation order Jobs
};

// Sum of weight * completion time, running the jobs in this order.
long long weightedCompletion(const vector<Job>& order) {
    long long clock = 0, total = 0;
    for (const Job& j : order) {
        clock += j.time;
        total += j.weight * clock;
    }
    return total;
}

// [snippet:greedy]
// Smith's rule: run a before b when a.time / a.weight < b.time / b.weight. Cross-multiplied in
// 64 bits, so there's no floating-point error, and `<` (never `<=`) keeps it a strict weak ordering.
long long smithsRule(vector<Job> jobs) {
    sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b) {
        return (long long)a.time * b.weight < (long long)b.time * a.weight;
    });
    return weightedCompletion(jobs);
}
// [/snippet]

// Plausible but wrong: shortest job first (ignores weights), heaviest job first (ignores times).
long long shortestFirst(vector<Job> jobs) {
    sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b) { return a.time < b.time; });
    return weightedCompletion(jobs);
}
long long heaviestFirst(vector<Job> jobs) {
    sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b) { return a.weight > b.weight; });
    return weightedCompletion(jobs);
}

// Brute force: every order. O(n! * n), fine for n <= 7.
long long bruteForce(vector<Job> jobs) {
    sort(jobs.begin(), jobs.end());
    long long best = LLONG_MAX;
    do best = min(best, weightedCompletion(jobs));
    while (next_permutation(jobs.begin(), jobs.end()));
    return best;
}

// [snippet:stress]
// The stress test: random tiny inputs, greedy vs brute force. Returns the first input where they
// disagree, i.e. a counterexample. Print it, shrink it by hand, and you'll see why the rule fails.
template <class Greedy>
optional<vector<Job>> findCounterexample(Greedy greedy, int iterations) {
    for (int iter = 0; iter < iterations; iter++) {
        int n = (int)t::rand_int(1, 6);                  // tiny, so the brute force stays fast
        vector<Job> jobs(n);
        for (Job& j : jobs) j = {(int)t::rand_int(1, 10), (int)t::rand_int(1, 10)};
        if (greedy(jobs) != bruteForce(jobs)) return jobs;
    }
    return nullopt;                                      // no counterexample found (not a proof!)
}
// [/snippet]

// ---------- coin change: fewest coins that sum to `amount` ----------
int greedyCoins(vector<int> coins, int amount) {        // always take the largest coin that fits
    sort(coins.rbegin(), coins.rend());
    int count = 0;
    for (int c : coins) {
        count += amount / c;
        amount %= c;
    }
    return amount == 0 ? count : -1;
}

// Exhaustive: every count of every coin (coins sorted descending; small amounts only).
int bruteCoins(const vector<int>& coins, int amount, size_t i = 0) {
    if (i + 1 == coins.size()) return amount % coins[i] == 0 ? amount / coins[i] : INT_MAX;
    int best = INT_MAX;
    for (int count = 0; count * coins[i] <= amount; count++) {
        int rest = bruteCoins(coins, amount - count * coins[i], i + 1);
        if (rest != INT_MAX) best = min(best, count + rest);
    }
    return best;
}

// ---------- 0/1 knapsack: each item whole or not at all ----------
struct Item { int weight, value; };

int greedyKnapsack(vector<Item> items, int capacity) {  // best value per unit weight first
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return (long long)a.value * b.weight > (long long)b.value * a.weight;
    });
    int total = 0;
    for (const Item& it : items)
        if (it.weight <= capacity) {
            capacity -= it.weight;
            total += it.value;
        }
    return total;
}

int bruteKnapsack(const vector<Item>& items, int capacity) {   // every subset
    int n = items.size(), best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        int weight = 0, value = 0;
        for (int i = 0; i < n; i++)
            if ((mask >> i) & 1) {
                weight += items[i].weight;
                value += items[i].value;
            }
        if (weight <= capacity) best = max(best, value);
    }
    return best;
}

int main() {
    // Smith's rule: hand cases, then 500 random ones with no counterexample.
    CHECK_EQ(smithsRule({{3, 1}, {1, 1}, {2, 1}}), 10LL);           // equal weights: shortest first, 1+3+6
    CHECK_EQ(smithsRule({{1, 1}, {2, 5}}), 13LL);                   // ratios 1 vs 0.4: the heavy job first
    CHECK_EQ(smithsRule({{2, 2}, {1, 1}, {4, 4}}), bruteForce({{2, 2}, {1, 1}, {4, 4}}));   // all tied
    CHECK(!findCounterexample(smithsRule, 500).has_value());

    // The wrong rules: known counterexamples, and the stress test finds some on its own.
    CHECK_EQ(shortestFirst({{1, 1}, {2, 5}}), 16LL);                // vs 13
    CHECK_EQ(heaviestFirst({{10, 2}, {1, 1}}), 31LL);               // vs 23
    CHECK_EQ(bruteForce({{10, 2}, {1, 1}}), 23LL);
    for (auto rule : {shortestFirst, heaviestFirst}) {
        auto bad = findCounterexample(rule, 500);
        CHECK(bad.has_value());
        if (bad) CHECK(rule(*bad) > bruteForce(*bad));
    }

    // Coin change: largest-coin-first fails for {1, 3, 4}; the first failing amount is 6 (4+1+1 vs 3+3).
    vector<int> odd{4, 3, 1};
    CHECK_EQ(greedyCoins(odd, 6), 3);
    CHECK_EQ(bruteCoins(odd, 6), 2);
    int firstBad = -1;
    for (int amount = 1; amount <= 50 && firstBad < 0; amount++)
        if (greedyCoins(odd, amount) != bruteCoins(odd, amount)) firstBad = amount;
    CHECK_EQ(firstBad, 6);
    vector<int> rupees{50, 20, 10, 5, 2, 1};                          // a "canonical" system:
    bool greedyOk = true;                                             // greedy is optimal for it
    for (int amount = 0; amount <= 100; amount++)
        greedyOk = greedyOk && greedyCoins(rupees, amount) == bruteCoins(rupees, amount);
    CHECK(greedyOk);

    // 0/1 knapsack: by value density, greedy takes the 6 kg item (5 per kg) and then nothing fits.
    vector<Item> items{{6, 30}, {5, 20}, {5, 20}};
    CHECK_EQ(greedyKnapsack(items, 10), 30);
    CHECK_EQ(bruteKnapsack(items, 10), 40);                           // the two 5 kg items
    int mismatches = 0;
    for (int iter = 0; iter < 300; iter++) {                          // greedy is never better,
        vector<Item> rnd((int)t::rand_int(1, 8));                     // and often worse
        for (Item& it : rnd) it = {(int)t::rand_int(1, 10), (int)t::rand_int(1, 30)};
        int cap = (int)t::rand_int(1, 25);
        int g = greedyKnapsack(rnd, cap), b = bruteKnapsack(rnd, cap);
        CHECK(g <= b);
        mismatches += g < b;
    }
    CHECK(mismatches > 0);
    return t::summary("greedy_stress_test");
}
