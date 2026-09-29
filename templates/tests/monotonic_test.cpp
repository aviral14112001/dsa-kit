#include <bits/stdc++.h>
#include "test.hpp"
#include "monotonic.hpp"
using namespace std;

// O(n^2) references, written straight from the definitions.
template <class T, class Beats>
vector<int> brute_next(const vector<T>& a, Beats beats) {
    int n = (int)a.size();
    vector<int> res(n, n);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (beats(a[j], a[i])) { res[i] = j; break; }
    return res;
}

template <class T, class Beats>
vector<int> brute_prev(const vector<T>& a, Beats beats) {
    int n = (int)a.size();
    vector<int> res(n, -1);
    for (int i = 0; i < n; i++)
        for (int j = i - 1; j >= 0; j--)
            if (beats(a[j], a[i])) { res[i] = j; break; }
    return res;
}

template <class T>
vector<T> brute_window(const vector<T>& a, int k, bool want_max) {
    vector<T> res;
    for (int i = 0; k >= 1 && i + k <= (int)a.size(); i++)
        res.push_back(want_max ? *max_element(a.begin() + i, a.begin() + i + k)
                               : *min_element(a.begin() + i, a.begin() + i + k));
    return res;
}

// Sum over i of (i - L[i]) * (R[i] - i): how many (subarray, index) credits the boundaries give.
long long credits(const vector<int>& L, const vector<int>& R) {
    long long total = 0;
    for (int i = 0; i < (int)L.size(); i++) total += (long long)(i - L[i]) * (R[i] - i);
    return total;
}

int main() {
    // ---------- the four variants, hand-checked ----------
    vector<int> a{3, 1, 4, 1, 5, 9, 2, 6};  // n = 8, so "none" is 8 on the right and -1 on the left
    CHECK_EQ(next_greater(a), vector<int>{2, 2, 4, 4, 5, 8, 7, 8});
    CHECK_EQ(next_smaller(a), vector<int>{1, 8, 3, 8, 6, 6, 8, 8});
    CHECK_EQ(prev_greater(a), vector<int>{-1, 0, -1, 2, -1, -1, 5, 5});
    CHECK_EQ(prev_smaller(a), vector<int>{-1, -1, 1, -1, 3, 4, 3, 6});

    // ---------- duplicates: strict (default) vs non-strict ----------
    vector<int> d{2, 2, 1, 2, 3};
    CHECK_EQ(next_greater(d), vector<int>{4, 4, 3, 4, 5});          // a[j] > a[i]
    CHECK_EQ(next_greater(d, false), vector<int>{1, 3, 3, 4, 5});   // a[j] >= a[i]
    CHECK_EQ(next_smaller(d), vector<int>{2, 2, 5, 5, 5});          // a[j] < a[i]
    CHECK_EQ(next_smaller(d, false), vector<int>{1, 2, 5, 5, 5});   // a[j] <= a[i]
    CHECK_EQ(prev_greater(d), vector<int>{-1, -1, 1, -1, -1});
    CHECK_EQ(prev_greater(d, false), vector<int>{-1, 0, 1, 1, -1});
    CHECK_EQ(prev_smaller(d), vector<int>{-1, -1, -1, 2, 3});
    CHECK_EQ(prev_smaller(d, false), vector<int>{-1, 0, -1, 2, 3});

    // all equal: strict finds nothing, non-strict finds the neighbour
    vector<int> same{7, 7, 7};
    CHECK_EQ(next_greater(same), vector<int>{3, 3, 3});
    CHECK_EQ(next_greater(same, false), vector<int>{1, 2, 3});
    CHECK_EQ(prev_smaller(same), vector<int>{-1, -1, -1});
    CHECK_EQ(prev_smaller(same, false), vector<int>{-1, 0, 1});

    // empty and single element
    CHECK_EQ(next_greater(vector<int>{}), vector<int>{});
    CHECK_EQ(prev_smaller(vector<int>{}), vector<int>{});
    CHECK_EQ(next_smaller(vector<int>{5}), vector<int>{1});
    CHECK_EQ(prev_greater(vector<int>{5}), vector<int>{-1});

    // any comparable type works
    vector<string> words{"pear", "apple", "fig", "kiwi"};
    CHECK_EQ(next_greater(words), vector<int>{4, 2, 3, 4});
    vector<long long> big{4'000'000'000LL, -4'000'000'000LL, 4'000'000'000LL};
    CHECK_EQ(prev_greater(big, false), vector<int>{-1, 0, 0});
    CHECK_EQ(sliding_window_min(big, 2), vector<long long>{-4'000'000'000LL, -4'000'000'000LL});

    // ---------- sliding window max / min ----------
    vector<int> w{1, 3, -1, -3, 5, 3, 6, 7};
    CHECK_EQ(sliding_window_max(w, 3), vector<int>{3, 3, 5, 5, 6, 7});
    CHECK_EQ(sliding_window_min(w, 3), vector<int>{-1, -3, -3, -3, 3, 3});
    CHECK_EQ(sliding_window_max(w, 1), w);                          // k = 1: every element
    CHECK_EQ(sliding_window_max(w, 8), vector<int>{7});             // k = n: one window
    CHECK_EQ(sliding_window_min(w, 8), vector<int>{-3});
    CHECK_EQ(sliding_window_max(w, 9), vector<int>{});              // k > n: no window
    CHECK_EQ(sliding_window_max(w, 0), vector<int>{});              // k < 1: no window
    CHECK_EQ(sliding_window_max(vector<int>{}, 1), vector<int>{});
    CHECK_EQ(sliding_window_max(vector<int>{4, 4, 4, 4}, 2), vector<int>{4, 4, 4});
    CHECK_EQ(sliding_window_min(vector<int>{5, 4, 3, 2, 1}, 2), vector<int>{4, 3, 2, 1});

    // ---------- contribution technique: the tie-breaking rule ----------
    // Credit index i with the subarrays [l, r] where l is in (L[i], i] and r is in [i, R[i]).
    // On {2, 2} there are 3 subarrays. Both sides strict: [2, 2] is credited to both indices.
    // Both non-strict: [2, 2] is credited to neither. One of each: every subarray exactly once.
    vector<int> twos{2, 2};
    CHECK_EQ(credits(prev_smaller(twos), next_smaller(twos)), 4);                // over-counts
    CHECK_EQ(credits(prev_smaller(twos, false), next_smaller(twos, false)), 2);  // under-counts
    CHECK_EQ(credits(prev_smaller(twos), next_smaller(twos, false)), 3);         // exact

    // ---------- worst case for a naive scan: long monotone runs ----------
    int n = 200000;
    vector<int> down(n), up(n);
    for (int i = 0; i < n; i++) { down[i] = n - i; up[i] = i; }
    auto ng = next_greater(down);
    CHECK(all_of(ng.begin(), ng.end(), [&](int j) { return j == n; }));   // nothing bigger to the right
    auto pg = prev_greater(down);
    bool prev_is_left_neighbour = true;
    for (int i = 0; i < n; i++) prev_is_left_neighbour &= pg[i] == i - 1;
    CHECK(prev_is_left_neighbour);
    CHECK_EQ(next_smaller(up)[0], n);
    CHECK_EQ(sliding_window_max(up, 1000).back(), n - 1);
    CHECK_EQ(sliding_window_min(up, 1000).front(), 0);

    // ---------- stress: every variant vs the O(n^2) definitions ----------
    for (int iter = 0; iter < 600; iter++) {
        int len = (int)t::rand_int(0, 25);
        int hi = iter % 3 == 0 ? 1000 : 3;  // a tiny value range forces many duplicates
        vector<int> v = t::rand_vec(len, 0, hi);
        CHECK_EQ(next_greater(v), brute_next(v, greater<int>()));
        CHECK_EQ(next_greater(v, false), brute_next(v, greater_equal<int>()));
        CHECK_EQ(next_smaller(v), brute_next(v, less<int>()));
        CHECK_EQ(next_smaller(v, false), brute_next(v, less_equal<int>()));
        CHECK_EQ(prev_greater(v), brute_prev(v, greater<int>()));
        CHECK_EQ(prev_greater(v, false), brute_prev(v, greater_equal<int>()));
        CHECK_EQ(prev_smaller(v), brute_prev(v, less<int>()));
        CHECK_EQ(prev_smaller(v, false), brute_prev(v, less_equal<int>()));

        int k = (int)t::rand_int(0, len + 1);  // includes the k = 0 and k > n edge cases
        CHECK_EQ(sliding_window_max(v, k), brute_window(v, k, true));
        CHECK_EQ(sliding_window_min(v, k), brute_window(v, k, false));

        // Mixed strictness credits each subarray to exactly one index: the RIGHTMOST minimum
        // with (strict prev, non-strict next), the LEFTMOST minimum with the mirror choice.
        vector<long long> rightmost(len, 0), leftmost(len, 0);
        for (int l = 0; l < len; l++) {
            int lo = l, ro = l;  // leftmost and rightmost index of the minimum of v[l..r]
            for (int r = l; r < len; r++) {
                if (v[r] < v[lo]) lo = ro = r;
                else if (v[r] == v[lo]) ro = r;
                leftmost[lo]++;
                rightmost[ro]++;
            }
        }
        auto L = prev_smaller(v), R = next_smaller(v, false);
        auto L2 = prev_smaller(v, false), R2 = next_smaller(v);
        bool right_ok = true, left_ok = true;
        for (int i = 0; i < len; i++) {
            right_ok &= rightmost[i] == (long long)(i - L[i]) * (R[i] - i);
            left_ok &= leftmost[i] == (long long)(i - L2[i]) * (R2[i] - i);
        }
        CHECK(right_ok);
        CHECK(left_ok);
        CHECK_EQ(credits(L, R), (long long)len * (len + 1) / 2);
    }
    return t::summary("monotonic");
}
