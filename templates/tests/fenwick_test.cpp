#include <bits/stdc++.h>
#include "test.hpp"
#include "fenwick.hpp"
using namespace std;

int main() {
    // Fixed cases.
    Fenwick<long long> f(vector<long long>{5, 2, 9, -3, 4});
    CHECK_EQ(f.prefix(0), 0LL);
    CHECK_EQ(f.prefix(1), 5LL);
    CHECK_EQ(f.prefix(5), 17LL);
    CHECK_EQ(f.range_sum(1, 3), 8LL);
    f.add(3, 10);                                    // a = {5, 2, 9, 7, 4}
    CHECK_EQ(f.range_sum(3, 3), 7LL);
    CHECK_EQ(f.range_sum(0, 4), 27LL);

    // lower_bound on non-negative values: the first index where the running sum reaches target.
    Fenwick<int> cnt(vector<int>{0, 2, 0, 1, 3});    // prefix sums 0, 2, 2, 3, 6
    CHECK_EQ(cnt.lower_bound(1), 1);
    CHECK_EQ(cnt.lower_bound(2), 1);
    CHECK_EQ(cnt.lower_bound(3), 3);
    CHECK_EQ(cnt.lower_bound(4), 4);
    CHECK_EQ(cnt.lower_bound(6), 4);
    CHECK_EQ(cnt.lower_bound(7), 5);                 // more than the total: n
    CHECK_EQ(cnt.lower_bound(0), 0);                 // nothing needed: index 0
    CHECK_EQ(Fenwick<int>(0).lower_bound(1), 0);     // empty tree
    CHECK_EQ(Fenwick<int>(1).prefix(1), 0);

    // RangeFenwick fixed case.
    RangeFenwick<long long> rf(6);
    rf.range_add(1, 3, 5);                           // 0 5 5 5 0 0
    rf.range_add(2, 5, -2);                          // 0 5 3 3 -2 -2
    CHECK_EQ(rf.range_sum(0, 5), 7LL);
    CHECK_EQ(rf.range_sum(2, 4), 4LL);
    CHECK_EQ(rf.point(1), 5LL);
    CHECK_EQ(rf.point(5), -2LL);

    // Stress 1: the O(n) build produces exactly the tree that n add() calls produce.
    for (int iter = 0; iter < 200; iter++) {
        int n = (int)t::rand_int(0, 50);
        auto v = t::rand_vec(n, -1000, 1000);
        vector<long long> a(v.begin(), v.end());
        Fenwick<long long> built(a), added(n);
        for (int i = 0; i < n; i++) added.add(i, a[i]);
        CHECK_EQ(built.tree, added.tree);
    }

    // Stress 2: random point adds and range sums against a plain array.
    for (int iter = 0; iter < 200; iter++) {
        int n = (int)t::rand_int(1, 40);
        vector<long long> a(n, 0);
        Fenwick<long long> fw(n);
        for (int op = 0; op < 100; op++) {
            if (t::rand_int(0, 1)) {
                int i = (int)t::rand_int(0, n - 1);
                long long d = t::rand_int(-1'000'000'000, 1'000'000'000);
                a[i] += d;
                fw.add(i, d);
            } else {
                int l = (int)t::rand_int(0, n - 1), r = (int)t::rand_int(l, n - 1);
                CHECK_EQ(fw.range_sum(l, r), accumulate(a.begin() + l, a.begin() + r + 1, 0LL));
            }
        }
    }

    // Stress 3: order statistics. A multiset of values in [0, m) as a count per value; the k-th
    // smallest (1-based) is lower_bound(k). Compare with a sorted vector.
    for (int iter = 0; iter < 200; iter++) {
        int m = (int)t::rand_int(1, 33);
        Fenwick<int> counts(m);
        vector<int> sorted_vals;
        for (int op = 0; op < 60; op++) {
            if (sorted_vals.empty() || t::rand_int(0, 2)) {
                int v = (int)t::rand_int(0, m - 1);
                counts.add(v, 1);
                sorted_vals.insert(upper_bound(sorted_vals.begin(), sorted_vals.end(), v), v);
            } else {                                 // erase the k-th smallest
                int k = (int)t::rand_int(1, (int)sorted_vals.size());
                int v = counts.lower_bound(k);
                CHECK_EQ(v, sorted_vals[k - 1]);
                counts.add(v, -1);
                sorted_vals.erase(sorted_vals.begin() + (k - 1));
            }
            int total = (int)sorted_vals.size();
            for (int k = 1; k <= total; k++) CHECK_EQ(counts.lower_bound(k), sorted_vals[k - 1]);
            CHECK_EQ(counts.lower_bound(total + 1), m);
        }
    }

    // Stress 4: range add + range sum + point query against a plain array.
    for (int iter = 0; iter < 200; iter++) {
        int n = (int)t::rand_int(1, 40);
        vector<long long> a(n, 0);
        RangeFenwick<long long> rfw(n);
        for (int op = 0; op < 100; op++) {
            int l = (int)t::rand_int(0, n - 1), r = (int)t::rand_int(l, n - 1);
            int kind = (int)t::rand_int(0, 2);
            if (kind == 0) {
                long long d = t::rand_int(-1'000'000, 1'000'000);
                for (int i = l; i <= r; i++) a[i] += d;
                rfw.range_add(l, r, d);
            } else if (kind == 1) {
                CHECK_EQ(rfw.range_sum(l, r), accumulate(a.begin() + l, a.begin() + r + 1, 0LL));
            } else {
                CHECK_EQ(rfw.point(l), a[l]);
            }
        }
    }
    return t::summary("fenwick");
}
