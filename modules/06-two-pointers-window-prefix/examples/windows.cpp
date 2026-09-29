// Module 06 section 2: the shortest-window, counting and exactly-K templates, each tested against brute force,
// plus a proof by counterexample that windows break on negative numbers.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:shortest]
// Length of the shortest subarray that contains at least k distinct values (k >= 1); 0 if none does.
// Shortest-window template: grow r; WHILE the window is valid, record it and shrink from the left.
// Adding elements never removes a distinct value, so for each r the loop ends with the shortest
// valid window ending at r already recorded.
int shortest_with_k_distinct(const vector<int>& a, int k) {
    unordered_map<int, int> count;              // value -> occurrences in a[l..r]
    int best = INT_MAX;
    for (int l = 0, r = 0; r < (int)a.size(); r++) {
        count[a[r]]++;
        while ((int)count.size() >= k) {        // valid: record, then try a shorter one
            best = min(best, r - l + 1);
            if (--count[a[l]] == 0) count.erase(a[l]);
            l++;
        }
    }
    return best == INT_MAX ? 0 : best;
}
// [/snippet]

// [snippet:count_at_most]
// Number of substrings in which no character appears more than K times (K >= 0).
// The rule is shrink-closed: if s[l..r] obeys it, so does every substring of it. So once l is the
// smallest valid left end for r, EVERY start in [l, r] is valid: r - l + 1 substrings end at r.
long long count_at_most(const string& s, int K) {
    int freq[256] = {};
    long long total = 0;
    for (int l = 0, r = 0; r < (int)s.size(); r++) {
        unsigned char c = s[r];
        freq[c]++;
        while (freq[c] > K) freq[(unsigned char)s[l++]]--;   // only s[r]'s count can break the rule
        total += r - l + 1;
    }
    return total;
}
// [/snippet]

// [snippet:exactly]
// Number of substrings whose most frequent character appears exactly K times (K >= 1).
// {max frequency <= K} = {max frequency == K} + {max frequency <= K - 1}, and the two parts don't
// overlap, so:  exactly(K) = atMost(K) - atMost(K - 1).
long long count_exactly(const string& s, int K) {
    return count_at_most(s, K) - count_at_most(s, K - 1);
}
// [/snippet]

// ---------- brute forces ----------
int shortest_brute(const vector<int>& a, int k) {
    int n = (int)a.size();
    for (int len = 1; len <= n; len++)
        for (int start = 0; start + len <= n; start++)
            if ((int)set<int>(a.begin() + start, a.begin() + start + len).size() >= k) return len;
    return 0;
}

int max_frequency(const string& s, int start, int len) {
    map<char, int> f;
    int best = 0;
    for (int k = start; k < start + len; k++) best = max(best, ++f[s[k]]);
    return best;
}

long long count_brute(const string& s, int K, bool exact) {
    long long total = 0;
    int n = (int)s.size();
    for (int start = 0; start < n; start++)
        for (int len = 1; start + len <= n; len++) {
            int m = max_frequency(s, start, len);
            total += exact ? m == K : m <= K;
        }
    return total;
}

// The module 02 longest-window skeleton, applied where its precondition (all values >= 0) is false.
int longest_within_budget_window(const vector<int>& a, long long budget) {
    long long sum = 0;
    int best = 0;
    for (int l = 0, r = 0; r < (int)a.size(); r++) {
        sum += a[r];
        while (sum > budget && l <= r) sum -= a[l++];
        best = max(best, r - l + 1);
    }
    return best;
}

int longest_within_budget_brute(const vector<int>& a, long long budget) {
    int best = 0;
    for (int l = 0; l < (int)a.size(); l++) {
        long long sum = 0;
        for (int r = l; r < (int)a.size(); r++) {
            sum += a[r];
            if (sum <= budget) best = max(best, r - l + 1);
        }
    }
    return best;
}

int main() {
    // ---- shortest ----
    CHECK_EQ(shortest_with_k_distinct({1, 2, 2, 3, 1}, 3), 3);   // [2, 3, 1]
    CHECK_EQ(shortest_with_k_distinct({4, 4, 4}, 2), 0);         // never valid
    CHECK_EQ(shortest_with_k_distinct({7}, 1), 1);
    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(1, 14), 0, 5);
        int k = (int)t::rand_int(1, 5);
        CHECK_EQ(shortest_with_k_distinct(a, k), shortest_brute(a, k));
    }

    // ---- counting and exactly(K) ----
    CHECK_EQ(count_at_most("abca", 1), 9);          // K = 1 means "no repeats": all 10 but "abca"
    CHECK_EQ(count_at_most("aaa", 0), 0);
    CHECK_EQ(count_at_most("", 3), 0);
    CHECK_EQ(count_exactly("aab", 2), 2);           // "aa", "aab"
    CHECK_EQ(count_exactly("abca", 1), 9);          // same as atMost(1): nothing has max frequency 0
    for (int iter = 0; iter < 300; iter++) {
        string s = t::rand_string((int)t::rand_int(0, 12), 'a', 'c');
        int K = (int)t::rand_int(1, 4);
        CHECK_EQ(count_at_most(s, K), count_brute(s, K, false));
        CHECK_EQ(count_exactly(s, K), count_brute(s, K, true));
    }

    // ---- windows fail with negatives ----
    // At r = 1 the sum is 4 > 3, so the window drops index 0 for good. But the -10 at index 2 would
    // have made the whole array (sum -4) valid: the true answer is 4, the window reports 3.
    vector<int> negatives{2, 2, -10, 2};
    CHECK_EQ(longest_within_budget_window(negatives, 3), 3);
    CHECK_EQ(longest_within_budget_brute(negatives, 3), 4);
    int disagreements = 0;                           // and random arrays with negatives disagree often
    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(1, 10), -10, 10);
        disagreements += longest_within_budget_window(a, 3) != longest_within_budget_brute(a, 3);
    }
    CHECK(disagreements > 0);
    return t::summary("06-windows");
}
