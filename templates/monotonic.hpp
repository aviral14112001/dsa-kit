// Monotonic stack + monotonic deque (module 09 section 2 and section 4).
//   next_greater / next_smaller / prev_greater / prev_smaller: the nearest index to the right or
//   left holding a bigger or smaller value, for every index at once, in O(n) total.
//   sliding_window_max / sliding_window_min: the max or min of every length-k window in O(n).
//
// Conventions (the part that decides whether duplicates are handled right):
//   * The stack functions return INDICES, not values, so you can compute distances and widths.
//   * "None" is n for next_* and -1 for prev_*. Then i - prev[i] and next[i] - i are the numbers
//     of choices for a subarray's left end and right end, with no special case at the borders.
//   * Comparisons are STRICT by default: next_greater looks for a[j] > a[i], and a value equal to
//     a[i] does not count. Pass strict = false to accept equal values too (a[j] >= a[i]).
//     Finding a max or min? Either choice works. Counting or summing over subarrays? Make exactly
//     one side strict so each subarray is credited to exactly one index (module 09 section 2).
//
// Templates in this folder are written interview-style (unqualified std names), so each header
// opens with `using namespace std;` and snippets can be copied straight into a solution.
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:core]
// next_index(a, beats)[i] = smallest j > i with beats(a[j], a[i]), or n if there is none.
// prev_index(a, beats)[i] = largest  j < i with beats(a[j], a[i]), or -1 if there is none.
// beats is one of greater<T>(), greater_equal<T>(), less<T>(), less_equal<T>().
template <class T, class Beats>
vector<int> next_index(const vector<T>& a, Beats beats) {
    int n = (int)a.size();
    vector<int> res(n, n);
    vector<int> waiting;  // indices still waiting for their answer, oldest at the bottom
    for (int j = 0; j < n; j++) {
        // No waiting value beats the one below it, so the indices a[j] beats sit together on
        // top: pop and answer them, and stop at the first one a[j] doesn't beat.
        while (!waiting.empty() && beats(a[j], a[waiting.back()])) {
            res[waiting.back()] = j;
            waiting.pop_back();
        }
        waiting.push_back(j);
    }
    return res;  // indices left waiting keep the default n: nothing to their right beats them
}

template <class T, class Beats>
vector<int> prev_index(const vector<T>& a, Beats beats) {
    int n = (int)a.size();
    vector<int> res(n, -1);
    vector<int> candidates;  // indices that can still be the answer for a later index
    for (int i = 0; i < n; i++) {
        // A candidate that doesn't beat a[i] is useless from now on: i is closer to every later
        // index, and beats whatever that candidate would have beaten.
        while (!candidates.empty() && !beats(a[candidates.back()], a[i])) candidates.pop_back();
        if (!candidates.empty()) res[i] = candidates.back();
        candidates.push_back(i);
    }
    return res;
}
// [/snippet]

// [snippet:four]
// The four variants. strict = true (default): strictly greater / smaller.
// strict = false: greater-or-equal / smaller-or-equal (an equal value counts).
template <class T>
vector<int> next_greater(const vector<T>& a, bool strict = true) {
    return strict ? next_index(a, greater<T>()) : next_index(a, greater_equal<T>());
}
template <class T>
vector<int> next_smaller(const vector<T>& a, bool strict = true) {
    return strict ? next_index(a, less<T>()) : next_index(a, less_equal<T>());
}
template <class T>
vector<int> prev_greater(const vector<T>& a, bool strict = true) {
    return strict ? prev_index(a, greater<T>()) : prev_index(a, greater_equal<T>());
}
template <class T>
vector<int> prev_smaller(const vector<T>& a, bool strict = true) {
    return strict ? prev_index(a, less<T>()) : prev_index(a, less_equal<T>());
}
// [/snippet]

// [snippet:window]
// window_best(a, k, better)[i] = the best value of a[i .. i+k-1], for i = 0 .. n-k.
// better = greater<T>() gives the max, less<T>() the min. Returns {} unless 1 <= k <= n.
template <class T, class Better>
vector<T> window_best(const vector<T>& a, int k, Better better) {
    int n = (int)a.size();
    if (k < 1 || k > n) return {};
    vector<T> res;
    res.reserve(n - k + 1);
    deque<int> dq;  // indices in the window, oldest at the front; each value strictly better
                    // than every value behind it, so the front is the window's best
    for (int i = 0; i < n; i++) {
        // An older index that isn't better than a[i] can never be the best again: a[i] is at
        // least as good and stays in the window longer. (Ties: the newer index wins.)
        while (!dq.empty() && !better(a[dq.back()], a[i])) dq.pop_back();
        dq.push_back(i);
        if (dq.front() == i - k) dq.pop_front();  // exactly one index (i - k) leaves per step
        if (i >= k - 1) res.push_back(a[dq.front()]);
    }
    return res;
}
template <class T>
vector<T> sliding_window_max(const vector<T>& a, int k) { return window_best(a, k, greater<T>()); }
template <class T>
vector<T> sliding_window_min(const vector<T>& a, int k) { return window_best(a, k, less<T>()); }
// [/snippet]
