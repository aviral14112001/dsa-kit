// Every question you can ask a sorted array is one of two boundaries: lower_bound (first >= x)
// and upper_bound (first > x). Module 05 section 4. Checked against first_true and linear scans.
#include <bits/stdc++.h>
#include "test.hpp"
#include "binary_search.hpp"
using namespace std;

// [snippet:bounds]
// a is sorted ascending. Results are indices; "none" is a.size() for first_* and -1 for last_*.
int first_ge(const vector<int>& a, int x) { return lower_bound(a.begin(), a.end(), x) - a.begin(); }
int first_gt(const vector<int>& a, int x) { return upper_bound(a.begin(), a.end(), x) - a.begin(); }
int last_le(const vector<int>& a, int x) { return first_gt(a, x) - 1; }
int last_lt(const vector<int>& a, int x) { return first_ge(a, x) - 1; }
int count_equal(const vector<int>& a, int x) { return first_gt(a, x) - first_ge(a, x); }
int count_between(const vector<int>& a, int lo, int hi) {   // how many v with lo <= v <= hi (lo <= hi)
    return first_gt(a, hi) - first_ge(a, lo);
}
// [/snippet]

int main() {
    // ---- the four boundaries, on a small example ----
    vector<int> a{1, 3, 3, 3, 7, 9};
    CHECK_EQ(first_ge(a, 3), 1);
    CHECK_EQ(first_gt(a, 3), 4);
    CHECK_EQ(last_le(a, 3), 3);
    CHECK_EQ(last_lt(a, 3), 0);
    CHECK_EQ(count_equal(a, 3), 3);
    CHECK_EQ(count_equal(a, 4), 0);
    CHECK_EQ(count_between(a, 2, 7), 4);
    CHECK_EQ(first_ge(a, 10), 6);       // none: a.size()
    CHECK_EQ(last_lt(a, 1), -1);        // none: -1

    // ---- set / map: use the member functions ----
    // [snippet:set_bounds]
    set<int> s{10, 20, 30};
    auto it = s.lower_bound(15);                            // MEMBER lower_bound: O(log n)
    int successor = it == s.end() ? -1 : *it;               // smallest element >= 15: 20
    int predecessor = it == s.begin() ? -1 : *prev(it);     // largest element < 15: 10
    // std::lower_bound(s.begin(), s.end(), 15) compiles and returns the same iterator, in O(n):
    // set iterators step one node at a time, so the "binary" search walks the whole range.
    // [/snippet]
    CHECK_EQ(successor, 20);
    CHECK_EQ(predecessor, 10);
    CHECK_EQ(*lower_bound(s.begin(), s.end(), 15), 20);

    // ---- descending arrays: pass the order the array is sorted by ----
    // [snippet:descending]
    vector<int> desc{9, 7, 7, 4, 1};
    // "Sorted" now means sorted by greater<int>, and the bounds follow that order:
    int first_le_7 = lower_bound(desc.begin(), desc.end(), 7, greater<int>()) - desc.begin();  // 1: first v <= 7
    int first_lt_7 = upper_bound(desc.begin(), desc.end(), 7, greater<int>()) - desc.begin();  // 3: first v < 7
    // [/snippet]
    CHECK_EQ(first_le_7, 1);
    CHECK_EQ(first_lt_7, 3);

    // ---- any monotone predicate works ----
    // [snippet:predicate]
    // partition_point wants the shape true...true false...false (first_true's mirror image)
    // and returns the first false.
    vector<string> words{"a", "be", "cat", "door", "eagle"};          // sorted by length
    auto first_long = partition_point(words.begin(), words.end(),
                                      [](const string& w) { return w.size() < 3; });   // -> "cat"
    // The same boundary with the kit's template, over indices instead of iterators:
    int idx = first_true(0, (int)words.size() - 1, [&](int i) { return words[i].size() >= 3; });   // 2
    // [/snippet]
    CHECK_EQ(*first_long, "cat");
    CHECK_EQ(idx, 2);

    // ---- stress: STL expressions == first_true predicates == linear scans ----
    for (int iter = 0; iter < 500; iter++) {
        vector<int> v = t::rand_vec((int)t::rand_int(0, 25), -8, 8);
        sort(v.begin(), v.end());
        int n = v.size(), x = (int)t::rand_int(-10, 10), y = (int)t::rand_int(x, 11);
        int ge = n, gt = n, le = -1, lt = -1, eq = 0, between = 0;       // linear scans
        for (int i = n - 1; i >= 0; i--) {
            if (v[i] >= x) ge = i;
            if (v[i] > x) gt = i;
        }
        for (int i = 0; i < n; i++) {
            if (v[i] <= x) le = i;
            if (v[i] < x) lt = i;
            eq += v[i] == x;
            between += x <= v[i] && v[i] <= y;
        }
        CHECK_EQ(first_ge(v, x), ge);
        CHECK_EQ(first_gt(v, x), gt);
        CHECK_EQ(last_le(v, x), le);
        CHECK_EQ(last_lt(v, x), lt);
        CHECK_EQ(count_equal(v, x), eq);
        CHECK_EQ(count_between(v, x, y), between);
        CHECK_EQ(first_true(0, n - 1, [&](int i) { return v[i] >= x; }), ge);
        CHECK_EQ(first_true(0, n - 1, [&](int i) { return v[i] > x; }), gt);
        CHECK_EQ(last_true(0, n - 1, [&](int i) { return v[i] <= x; }), le);
        CHECK_EQ(last_true(0, n - 1, [&](int i) { return v[i] < x; }), lt);
    }
    return t::summary("sorted_queries");
}
