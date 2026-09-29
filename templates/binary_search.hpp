// Binary search on a monotone predicate: the one template behind lower_bound, "first bad version"
// and every "binary search on the answer" problem (modules 05 and 11).
//
// Templates in this folder are written interview-style (unqualified std names), so each header
// opens with `using namespace std;` and snippets can be copied straight into a solution.
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:first_true]
// Smallest x in [lo, hi] with ok(x) true, for ok shaped false...false true...true.
// Returns hi + 1 when ok is false everywhere. Keep hi + 1 representable (use long long if unsure).
template <class T, class Pred>
T first_true(T lo, T hi, Pred ok) {
    hi++;                               // search the half-open range [lo, hi)
    while (lo < hi) {
        T mid = lo + (hi - lo) / 2;     // not (lo + hi) / 2, which can overflow
        if (ok(mid)) hi = mid;          // mid works: the answer is mid or left of it
        else lo = mid + 1;              // mid fails: the answer is right of it
    }
    return lo;
}
// [/snippet]

// [snippet:last_true]
// Largest x in [lo, hi] with ok(x) true, for ok shaped true...true false...false.
// Returns lo - 1 when ok is false everywhere.
template <class T, class Pred>
T last_true(T lo, T hi, Pred ok) {
    return first_true(lo, hi, [&](T x) { return !ok(x); }) - 1;
}
// [/snippet]

// [snippet:first_true_real]
// Real-valued answers: a fixed number of halvings instead of an epsilon loop.
// 100 halvings shrink the range by 2^100 ≈ 1e30, far beyond the ~16 digits a double resolves.
template <class Pred>
double first_true_real(double lo, double hi, Pred ok, int iters = 100) {
    for (int i = 0; i < iters; i++) {
        double mid = (lo + hi) / 2;
        if (ok(mid)) hi = mid;
        else lo = mid;
    }
    return hi;
}
// [/snippet]
