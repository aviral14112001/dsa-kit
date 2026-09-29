#include <bits/stdc++.h>
#include "test.hpp"
#include "sorting.hpp"
using namespace std;

using SortFn = function<void(vector<int>&)>;

// Every int sort in the header behind one signature, so each case runs through all of them.
// Lomuto is kept out of the big adversarial inputs: it is O(n^2) on sorted and all-equal input.
const vector<pair<string, SortFn>> kFastSorts = {
    {"merge_sort", [](vector<int>& a) { merge_sort(a); }},
    {"quick_sort", [](vector<int>& a) { quick_sort(a); }},
    {"heap_sort", [](vector<int>& a) { heap_sort(a); }},
    {"quick_sort_hoare", [](vector<int>& a) { quick_sort_hoare(a, 0, (int)a.size() - 1); }},
    {"counting_sort", [](vector<int>& a) { counting_sort(a); }},
};
const pair<string, SortFn> kLomuto = {"quick_sort_lomuto",
                                      [](vector<int>& a) { quick_sort_lomuto(a, 0, (int)a.size() - 1); }};

// Sorts a copy of `input` with `fn` and compares with std::sort. The name is part of the checked
// value, so a failure message says which algorithm broke.
void check_sort(const pair<string, SortFn>& algo, vector<int> input) {
    vector<int> expected = input;
    sort(expected.begin(), expected.end());
    algo.second(input);
    CHECK_EQ(make_pair(algo.first, input), make_pair(algo.first, expected));
}

int main() {
    // ---- deterministic small cases, every algorithm ----
    vector<vector<int>> small = {
        {}, {1}, {2, 1}, {1, 2}, {3, 1, 2}, {5, -1, 3, -1, 0}, {4, 4, 4, 4}, {2, 1, 2, 1, 2, 1},
        {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {-5, 3, -5, 3, 0, 0, 7, -2},
    };
    for (const auto& v : small) {
        for (const auto& algo : kFastSorts) check_sort(algo, v);
        check_sort(kLomuto, v);
    }
    CHECK_EQ(counting_sort(vector<int>{}, 5, [](int x) { return x; }), vector<int>{});

    // Extreme values: comparison sorts only (counting sort would need a 2^32-entry count array).
    vector<int> extremes{INT_MAX, INT_MIN, 0, -1, 1, INT_MAX, INT_MIN};
    for (const auto& algo : kFastSorts)
        if (algo.first != "counting_sort") check_sort(algo, extremes);
    check_sort(kLomuto, extremes);

    // ---- adversarial inputs, n = 20000: an O(n^2) sort would take ~2*10^8 steps here ----
    const int n = 20000;
    vector<int> sorted_in(n), reversed_in(n), equal_in(n, 42), organ_pipe(n), few_distinct(n), sawtooth(n);
    iota(sorted_in.begin(), sorted_in.end(), 0);
    reversed_in.assign(sorted_in.rbegin(), sorted_in.rend());
    for (int i = 0; i < n; i++) {
        organ_pipe[i] = min(i, n - 1 - i);              // 0 1 2 ... 2 1 0
        few_distinct[i] = (int)t::rand_int(0, 3);
        sawtooth[i] = i % 100;
    }
    vector<int> random_in = t::rand_vec(n, 0, 1000000);
    for (const auto* input : {&sorted_in, &reversed_in, &equal_in, &organ_pipe, &few_distinct, &sawtooth, &random_in})
        for (const auto& algo : kFastSorts) check_sort(algo, *input);

    // ---- stress against std::sort: many duplicates, then wide values ----
    // Counting sort only gets the narrow range: on a 2*10^6-wide range it would allocate a
    // 2*10^6-entry count array for 60 numbers, which is exactly how not to use it.
    for (int iter = 0; iter < 400; iter++) {
        int len = (int)t::rand_int(0, 60);
        bool narrow = iter % 2 == 0;
        vector<int> v = narrow ? t::rand_vec(len, -5, 5) : t::rand_vec(len, -1000000, 1000000);
        for (const auto& algo : kFastSorts)
            if (narrow || algo.first != "counting_sort") check_sort(algo, v);
        check_sort(kLomuto, v);
    }

    // ---- custom comparators: descending, and a non-int type ----
    for (int iter = 0; iter < 100; iter++) {
        vector<int> v = t::rand_vec((int)t::rand_int(0, 50), -100, 100);
        vector<int> expected = v;
        sort(expected.begin(), expected.end(), greater<int>());
        vector<int> a = v, b = v, c = v, d = v, e = v;
        merge_sort(a, greater<int>());
        quick_sort(b, greater<int>());
        heap_sort(c, greater<int>());
        quick_sort_hoare(d, 0, (int)d.size() - 1, greater<int>());
        quick_sort_lomuto(e, 0, (int)e.size() - 1, greater<int>());
        CHECK_EQ(a, expected);
        CHECK_EQ(b, expected);
        CHECK_EQ(c, expected);
        CHECK_EQ(d, expected);
        CHECK_EQ(e, expected);
    }
    vector<string> words{"pear", "fig", "apple", "fig", "banana", "kiwi", ""};
    vector<string> words_sorted = words;
    sort(words_sorted.begin(), words_sorted.end());
    for (int which = 0; which < 3; which++) {
        vector<string> w = words;
        if (which == 0) merge_sort(w);
        if (which == 1) quick_sort(w);
        if (which == 2) heap_sort(w);
        CHECK_EQ(w, words_sorted);
    }

    // ---- stability, with (key, original index) records compared by key only ----
    // A stable sort leaves equal keys in increasing index order, so its output must equal the
    // records sorted by (key, index), which is exactly std::pair's default order.
    auto by_key = [](const pair<int, int>& x, const pair<int, int>& y) { return x.first < y.first; };
    auto key_of = [](const pair<int, int>& p) { return p.first; };
    for (int iter = 0; iter < 300; iter++) {
        int len = (int)t::rand_int(0, 50);
        vector<pair<int, int>> recs(len);
        for (int i = 0; i < len; i++) recs[i] = {(int)t::rand_int(0, 4), i};
        vector<pair<int, int>> expected = recs;
        sort(expected.begin(), expected.end());
        vector<pair<int, int>> merged = recs;
        merge_sort(merged, by_key);
        CHECK_EQ(merged, expected);
        CHECK_EQ(counting_sort(recs, 4, key_of), expected);
    }
    // Heap sort is not stable: two equal keys come out swapped.
    vector<pair<int, char>> two{{1, 'a'}, {1, 'b'}};
    heap_sort(two, [](const pair<int, char>& x, const pair<int, char>& y) { return x.first < y.first; });
    CHECK_EQ(two, vector<pair<int, char>>{{1, 'b'}, {1, 'a'}});

    // ---- radix sort: several bases, values up to INT_MAX ----
    for (int base : {2, 10, 256, 1 << 16}) {
        for (int iter = 0; iter < 60; iter++) {
            vector<int> v = t::rand_vec((int)t::rand_int(0, 60), 0, (iter % 3 == 0) ? 20 : INT_MAX);
            vector<int> expected = v;
            sort(expected.begin(), expected.end());
            radix_sort(v, base);
            CHECK_EQ(v, expected);
        }
        vector<int> edge{INT_MAX, 0, 1, INT_MAX - 1, 0, 255, 256, 65535, 65536};
        vector<int> edge_sorted = edge;
        sort(edge_sorted.begin(), edge_sorted.end());
        radix_sort(edge, base);
        CHECK_EQ(edge, edge_sorted);
    }
    vector<int> zeros(10, 0), big = random_in;
    radix_sort(zeros);
    CHECK_EQ(zeros, vector<int>(10, 0));
    radix_sort(big);
    CHECK(is_sorted(big.begin(), big.end()));

    // ---- partitions keep their promises ----
    for (int iter = 0; iter < 300; iter++) {
        vector<int> v = t::rand_vec((int)t::rand_int(1, 30), 0, 6);
        int hi = (int)v.size() - 1;
        vector<int> a = v;
        int p = lomuto_partition(a, 0, hi);
        bool ok = true;
        for (int i = 0; i < p; i++) ok &= a[i] < a[p];
        for (int i = p + 1; i <= hi; i++) ok &= a[i] >= a[p];
        CHECK(ok);
        if (hi == 0) continue;                           // Hoare needs at least two elements
        vector<int> b = v;
        int j = hoare_partition(b, 0, hi);
        CHECK(0 <= j && j < hi);                         // both sides non-empty: recursion shrinks
        CHECK(*max_element(b.begin(), b.begin() + j + 1) <= *min_element(b.begin() + j + 1, b.end()));
    }
    return t::summary("sorting");
}
