// The combinations template: every k-element subset exactly once, with the loop bound that stops
// the search from entering branches that can't collect k elements. Module 10 section 2.
// The tests also count calls, checking the node-count formulas quoted in Notes section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:combinations]
// All k-element combinations of items, each once, in index order (C(n, k) of them).
void combine(const vector<int>& items, int k, int start, vector<int>& cur, vector<vector<int>>& out) {
    if ((int)cur.size() == k) {                                 // a full combination
        out.push_back(cur);
        return;
    }
    int need = k - (int)cur.size();                             // elements still to pick
    for (int i = start; i <= (int)items.size() - need; i++) {   // beyond this, too few elements remain
        cur.push_back(items[i]);
        combine(items, k, i + 1, cur, out);                     // later elements only: no reorderings
        cur.pop_back();
    }
}
// [/snippet]

vector<vector<int>> combinations(const vector<int>& items, int k) {
    vector<vector<int>> out;
    vector<int> cur;
    combine(items, k, 0, cur, out);
    return out;
}

// The same tree without the data: how many calls does the search make, with or without the bound?
long long countCalls(int n, int k, int start, int size, bool pruned) {
    long long calls = 1;
    if (size == k) return calls;
    int last = pruned ? n - (k - size) : n - 1;
    for (int i = start; i <= last; i++) calls += countCalls(n, k, i + 1, size + 1, pruned);
    return calls;
}

long long binom(int n, int k) {
    if (k < 0 || k > n) return 0;
    long long r = 1;
    for (int i = 1; i <= k; i++) r = r * (n - k + i) / i;   // exact at every step
    return r;
}

// Brute force: every bitmask with exactly k bits set.
vector<vector<int>> bruteCombinations(const vector<int>& items, int k) {
    int n = items.size();
    vector<vector<int>> out;
    for (int mask = 0; mask < (1 << n); mask++) {
        if (__builtin_popcount(mask) != k) continue;
        vector<int> c;
        for (int j = 0; j < n; j++)
            if ((mask >> j) & 1) c.push_back(items[j]);
        out.push_back(c);
    }
    sort(out.begin(), out.end());
    return out;
}

int main() {
    CHECK_EQ(combinations({1, 2, 3, 4}, 2),
             vector<vector<int>>{{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
    CHECK_EQ(combinations({1, 2, 3}, 0), vector<vector<int>>{{}});        // one empty combination
    CHECK_EQ(combinations({1, 2, 3}, 3), vector<vector<int>>{{1, 2, 3}});
    CHECK_EQ(combinations({7}, 1), vector<vector<int>>{{7}});

    for (int iter = 0; iter < 200; iter++) {                               // stress vs bitmasks
        int n = (int)t::rand_int(0, 10);
        int k = (int)t::rand_int(0, n);
        vector<int> items(n);
        iota(items.begin(), items.end(), 1);                               // sorted, distinct
        auto got = combinations(items, k);
        CHECK_EQ((long long)got.size(), binom(n, k));
        CHECK_EQ(got, bruteCombinations(items, k));                        // same set, same (sorted) order
    }

    // Node counts. With the bound, every node can still be completed: C(n+1, k) calls in total.
    // Without it, the search visits every prefix of length <= k: sum of C(n, d) for d = 0..k.
    for (int n = 0; n <= 16; n++) {
        for (int k = 0; k <= n; k++) {
            long long unpruned = 0;
            for (int d = 0; d <= k; d++) unpruned += binom(n, d);
            CHECK_EQ(countCalls(n, k, 0, 0, true), binom(n + 1, k));
            CHECK_EQ(countCalls(n, k, 0, 0, false), unpruned);
        }
    }
    CHECK_EQ(countCalls(20, 18, 0, 0, true), 1330LL);        // the bound matters when k is close to n
    CHECK_EQ(countCalls(20, 18, 0, 0, false), 1048555LL);
    return t::summary("combinations_template");
}
