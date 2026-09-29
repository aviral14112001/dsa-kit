#include <bits/stdc++.h>
#include "test.hpp"
#include "segment_tree.hpp"
using namespace std;

// [snippet:max_subarray_node]
// Richer node info: the best subarray of a range lies in the left half, in the right half, or crosses
// the middle, and a crossing one = (best suffix of the left) + (best prefix of the right). So every
// node carries four numbers, and the prefix/suffix can themselves extend across the middle.
struct Info { long long sum, pref, suf, best; };

Info leaf(long long v) { return {v, v, v, v}; }

Info combine(const Info& L, const Info& R) {
    return {L.sum + R.sum,
            max(L.pref, L.sum + R.pref),               // stays in L, or takes all of L plus a prefix of R
            max(R.suf, R.sum + L.suf),                 // stays in R, or takes all of R plus a suffix of L
            max({L.best, R.best, L.suf + R.pref})};    // left, right, or across the middle
}
// [/snippet]

long long kadane(const vector<long long>& a, int l, int r) {   // brute: best non-empty subarray of a[l..r]
    long long best = LLONG_MIN;
    for (int i = l; i <= r; i++) {
        long long s = 0;
        for (int j = i; j <= r; j++) best = max(best, s += a[j]);
    }
    return best;
}

int main() {
    auto min_op = [](int x, int y) { return min(x, y); };

    // Fixed cases: iterative range min with point set.
    vector<int> a{3, 2, 4, 5, 1, 1, 5, 3};
    SegTree st(a, INT_MAX, min_op);
    CHECK_EQ(st.query(1, 3), 2);
    CHECK_EQ(st.query(0, 7), 1);
    CHECK_EQ(st.query(6, 7), 3);
    st.set(4, 9);
    st.set(5, 9);
    CHECK_EQ(st.query(3, 6), 5);
    CHECK_EQ(st.query(3, 2), INT_MAX);                // empty range: the identity

    // Recursive sum tree.
    SumSegTree sum_tree(vector<long long>{1, 3, 5});
    CHECK_EQ(sum_tree.query(0, 2), 9LL);
    sum_tree.update(1, 2);
    CHECK_EQ(sum_tree.query(0, 2), 8LL);
    CHECK_EQ(sum_tree.query(1, 1), 2LL);

    // Lazy tree on the CSES 1735 sample (0-indexed): sum [2,4]; add 2 on [1,3]; sum; assign 5 on [1,3]; sum.
    LazySegTree lazy(vector<long long>{2, 3, 1, 1, 5, 3});
    CHECK_EQ(lazy.range_sum(2, 4), 7LL);
    lazy.range_add(1, 3, 2);
    CHECK_EQ(lazy.range_sum(2, 4), 11LL);
    lazy.range_assign(1, 3, 5);
    CHECK_EQ(lazy.range_sum(2, 4), 15LL);
    // Assign then add on overlapping ranges: the add must land on top of the assigned value.
    lazy.range_assign(0, 5, 10);                      // 10 10 10 10 10 10
    lazy.range_add(2, 5, 1);                          // 10 10 11 11 11 11
    lazy.range_assign(4, 5, 0);                       // 10 10 11 11  0  0
    lazy.range_add(3, 4, 7);                          // 10 10 11 18  7  0
    CHECK_EQ(lazy.range_sum(0, 5), 56LL);
    CHECK_EQ(lazy.range_min(0, 5), 0LL);
    CHECK_EQ(lazy.range_max(0, 5), 18LL);
    CHECK_EQ(lazy.range_min(2, 4), 7LL);
    CHECK_EQ(lazy.first_at_least(0, 11), 2);
    CHECK_EQ(lazy.first_at_least(3, 11), 3);
    CHECK_EQ(lazy.first_at_least(4, 11), -1);
    CHECK_EQ(lazy.first_at_least(0, 19), -1);
    CHECK_EQ(lazy.first_at_least(5, 0), 5);

    // Single element.
    LazySegTree one(vector<long long>{-4});
    one.range_add(0, 0, 10);
    CHECK_EQ(one.range_sum(0, 0), 6LL);
    CHECK_EQ(one.first_at_least(0, 6), 0);

    // Stress 1: the iterative tree with four different monoids against brute force, for every n
    // (non-powers of two included). Concatenation and Info are not commutative, so this also checks
    // that query() keeps the left-to-right order.
    for (int iter = 0; iter < 200; iter++) {
        int n = (int)t::rand_int(1, 33);
        auto v = t::rand_vec(n, -20, 20);
        vector<long long> vl(v.begin(), v.end());
        string s = t::rand_string(n, 'a', 'f');
        vector<string> chars;
        vector<Info> infos;
        for (int i = 0; i < n; i++) {
            chars.push_back(string(1, s[i]));
            infos.push_back(leaf(vl[i]));
        }
        const long long INF = (long long)1e18;
        SegTree tmin(v, INT_MAX, min_op);
        SegTree tsum(vl, 0LL, plus<long long>());
        SegTree tcat(chars, string(), [](const string& x, const string& y) { return x + y; });
        SegTree tinfo(infos, Info{0, -INF, -INF, -INF}, combine);
        for (int op = 0; op < 40; op++) {
            int l = (int)t::rand_int(0, n - 1), r = (int)t::rand_int(l, n - 1);
            if (t::rand_int(0, 2) == 0) {
                int value = (int)t::rand_int(-20, 20);
                char c = (char)t::rand_int('a', 'f');
                v[l] = value;
                vl[l] = value;
                s[l] = c;
                tmin.set(l, value);
                tsum.set(l, value);
                tcat.set(l, string(1, c));
                tinfo.set(l, leaf(value));
            } else {
                CHECK_EQ(tmin.query(l, r), *min_element(v.begin() + l, v.begin() + r + 1));
                CHECK_EQ(tsum.query(l, r), accumulate(vl.begin() + l, vl.begin() + r + 1, 0LL));
                CHECK_EQ(tcat.query(l, r), s.substr(l, r - l + 1));
                CHECK_EQ(tinfo.query(l, r).best, kadane(vl, l, r));
            }
        }
    }

    // Stress 2: the recursive sum tree against a plain array.
    for (int iter = 0; iter < 200; iter++) {
        int n = (int)t::rand_int(1, 40);
        auto v = t::rand_vec(n, -1000, 1000);
        vector<long long> vl(v.begin(), v.end());
        SumSegTree tree(vl);
        for (int op = 0; op < 60; op++) {
            int l = (int)t::rand_int(0, n - 1), r = (int)t::rand_int(l, n - 1);
            if (t::rand_int(0, 1)) {
                vl[l] = t::rand_int(-1'000'000'000, 1'000'000'000);
                tree.update(l, vl[l]);
            } else {
                CHECK_EQ(tree.query(l, r), accumulate(vl.begin() + l, vl.begin() + r + 1, 0LL));
            }
        }
    }

    // Stress 3: the lazy tree against a plain array: random adds, assigns (often overlapping),
    // sums, mins, maxes and descents. Values reach ~1e11, so sums need 64 bits.
    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(1, 40);
        auto v = t::rand_vec(n, 0, 1'000'000'000);
        vector<long long> arr(v.begin(), v.end());
        LazySegTree tree(arr);
        for (int op = 0; op < 80; op++) {
            int l = (int)t::rand_int(0, n - 1), r = (int)t::rand_int(l, n - 1);
            int kind = (int)t::rand_int(0, 5);
            if (kind == 0) {
                long long d = t::rand_int(-1'000'000, 1'000'000);
                for (int i = l; i <= r; i++) arr[i] += d;
                tree.range_add(l, r, d);
            } else if (kind == 1) {
                long long value = t::rand_int(0, 1'000'000'000);
                for (int i = l; i <= r; i++) arr[i] = value;
                tree.range_assign(l, r, value);
            } else if (kind == 2) {
                CHECK_EQ(tree.range_sum(l, r), accumulate(arr.begin() + l, arr.begin() + r + 1, 0LL));
            } else if (kind == 3) {
                CHECK_EQ(tree.range_min(l, r), *min_element(arr.begin() + l, arr.begin() + r + 1));
                CHECK_EQ(tree.range_max(l, r), *max_element(arr.begin() + l, arr.begin() + r + 1));
            } else {
                // Descent: pick x near an existing value so both "found" and "not found" happen.
                long long x = arr[t::rand_int(0, n - 1)] + t::rand_int(-2, 2);
                int expect = -1;
                for (int i = l; i < n; i++)
                    if (arr[i] >= x) { expect = i; break; }
                CHECK_EQ(tree.first_at_least(l, x), expect);
            }
        }
    }
    return t::summary("segment_tree");
}
