#include <bits/stdc++.h>
#include "test.hpp"
#include "prefix_sum.hpp"
using namespace std;
using ll = long long;

int main() {
    // ---------- PrefixSum (1D) ----------
    vector<int> a{3, -1, 4, 1, -5, 9};
    PrefixSum ps(a);
    CHECK_EQ(ps.sum(0, 5), 11);
    CHECK_EQ(ps.sum(2, 3), 5);
    CHECK_EQ(ps.sum(4, 4), -5);
    CHECK_EQ(ps.sum(0, 0), 3);
    CHECK_EQ(ps.sum(3, 2), 0);                         // l == r + 1: the empty range
    CHECK_EQ(PrefixSum(vector<int>{}).sum(0, -1), 0);  // empty input
    vector<int> big(100000, 2'000'000'000);            // an int running sum would overflow at i = 1
    CHECK_EQ(PrefixSum(big).sum(0, 99999), 200'000'000'000'000LL);
    CHECK_EQ(PrefixSum(big).sum(500, 501), 4'000'000'000LL);

    // ---------- PrefixSum2D ----------
    vector<vector<int>> g{{1, 2, 3},
                          {4, 5, 6},
                          {7, 8, 9}};
    PrefixSum2D p2(g);
    CHECK_EQ(p2.sum(0, 0, 2, 2), 45);
    CHECK_EQ(p2.sum(1, 1, 2, 2), 28);                  // 5 + 6 + 8 + 9
    CHECK_EQ(p2.sum(0, 1, 1, 2), 16);                  // 2 + 3 + 5 + 6
    CHECK_EQ(p2.sum(2, 0, 2, 0), 7);                   // one cell
    CHECK_EQ(p2.sum(0, 2, 2, 2), 18);                  // one column
    CHECK_EQ(PrefixSum2D(vector<vector<int>>{{-4}}).sum(0, 0, 0, 0), -4);

    // ---------- DiffArray (1D) ----------
    DiffArray d(5);
    d.add(0, 2, 10);
    d.add(1, 4, 5);
    d.add(4, 4, -3);
    CHECK_EQ(d.build(), vector<ll>{10, 15, 15, 5, 2});
    DiffArray from(vector<int>{4, 4, 7, 1});            // start from existing values
    CHECK_EQ(from.build(), vector<ll>{4, 4, 7, 1});
    from.add(1, 2, -4);
    CHECK_EQ(from.build(), vector<ll>{4, 0, 3, 1});
    CHECK_EQ(DiffArray(0).build(), vector<ll>{});

    // ---------- DiffArray2D ----------
    DiffArray2D d2(3, 4);
    d2.add(0, 0, 1, 1, 1);
    d2.add(1, 1, 2, 3, 2);
    CHECK_EQ(d2.build(), vector<vector<ll>>{{1, 1, 0, 0},
                                            {1, 3, 2, 2},
                                            {0, 2, 2, 2}});

    // ---------- stress tests against brute force ----------
    for (int iter = 0; iter < 300; iter++) {           // 1D range sums
        int n = (int)t::rand_int(1, 25);
        auto v = t::rand_vec(n, -1'000'000'000, 1'000'000'000);
        PrefixSum p(v);
        for (int q = 0; q < 10; q++) {
            int l = (int)t::rand_int(0, n - 1), r = (int)t::rand_int(l, n - 1);
            ll brute = 0;
            for (int i = l; i <= r; i++) brute += v[i];
            CHECK_EQ(p.sum(l, r), brute);
        }
    }
    for (int iter = 0; iter < 200; iter++) {           // 2D rectangle sums
        int rows = (int)t::rand_int(1, 7), cols = (int)t::rand_int(1, 7);
        vector<vector<int>> m(rows);
        for (auto& row : m) row = t::rand_vec(cols, -100, 100);
        PrefixSum2D p(m);
        for (int q = 0; q < 10; q++) {
            int r1 = (int)t::rand_int(0, rows - 1), r2 = (int)t::rand_int(r1, rows - 1);
            int c1 = (int)t::rand_int(0, cols - 1), c2 = (int)t::rand_int(c1, cols - 1);
            ll brute = 0;
            for (int i = r1; i <= r2; i++)
                for (int j = c1; j <= c2; j++) brute += m[i][j];
            CHECK_EQ(p.sum(r1, c1, r2, c2), brute);
        }
    }
    for (int iter = 0; iter < 300; iter++) {           // 1D range adds
        int n = (int)t::rand_int(1, 20);
        auto init = t::rand_vec(n, -50, 50);
        DiffArray diff(init);
        vector<ll> brute(init.begin(), init.end());
        for (int u = (int)t::rand_int(0, 12); u > 0; u--) {
            int l = (int)t::rand_int(0, n - 1), r = (int)t::rand_int(l, n - 1);
            ll v = t::rand_int(-1'000'000'000, 1'000'000'000);
            diff.add(l, r, v);
            for (int i = l; i <= r; i++) brute[i] += v;
        }
        CHECK_EQ(diff.build(), brute);
    }
    for (int iter = 0; iter < 200; iter++) {           // 2D rectangle adds
        int rows = (int)t::rand_int(1, 6), cols = (int)t::rand_int(1, 6);
        DiffArray2D diff(rows, cols);
        vector<vector<ll>> brute(rows, vector<ll>(cols, 0));
        for (int u = (int)t::rand_int(0, 8); u > 0; u--) {
            int r1 = (int)t::rand_int(0, rows - 1), r2 = (int)t::rand_int(r1, rows - 1);
            int c1 = (int)t::rand_int(0, cols - 1), c2 = (int)t::rand_int(c1, cols - 1);
            ll v = t::rand_int(-100, 100);
            diff.add(r1, c1, r2, c2, v);
            for (int i = r1; i <= r2; i++)
                for (int j = c1; j <= c2; j++) brute[i][j] += v;
        }
        CHECK_EQ(diff.build(), brute);
    }
    return t::summary("prefix_sum");
}
