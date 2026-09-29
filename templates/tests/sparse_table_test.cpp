#include <bits/stdc++.h>
#include "test.hpp"
#include "sparse_table.hpp"
using namespace std;

struct Gcd { int operator()(int a, int b) const { return gcd(a, b); } };            // idempotent
struct Plus { long long operator()(long long a, long long b) const { return a + b; } };   // not idempotent
struct Concat { string operator()(const string& a, const string& b) const { return a + b; } };  // not commutative

int main() {
    // Fixed cases (the CSES 1647 sample array, 0-indexed).
    vector<int> a{3, 2, 4, 5, 1, 1, 5, 3};
    SparseTable<int> mn(a);
    SparseTable<int, Max<int>> mx(a);
    CHECK_EQ(mn.query(1, 3), 2);
    CHECK_EQ(mn.query(4, 5), 1);
    CHECK_EQ(mn.query(0, 7), 1);
    CHECK_EQ(mn.query(2, 2), 4);
    CHECK_EQ(mx.query(0, 7), 5);
    CHECK_EQ(mx.query(0, 1), 3);
    CHECK_EQ(mn.lg[1], 0);
    CHECK_EQ(mn.lg[8], 3);
    CHECK_EQ(mn.lg[7], 2);

    // Tiny arrays: a single element, and an empty array (building must not crash).
    SparseTable<int> one(vector<int>{42});
    CHECK_EQ(one.query(0, 0), 42);
    SparseTable<int> empty(vector<int>{});
    CHECK_EQ(empty.table.size(), 1);

    // gcd is idempotent too, so the O(1) overlapping query is valid for it.
    SparseTable<int, Gcd> g(vector<int>{12, 18, 24, 7, 14});
    CHECK_EQ(g.query(0, 2), 6);
    CHECK_EQ(g.query(3, 4), 7);
    CHECK_EQ(g.query(0, 4), 1);

    // A sum through query() double-counts the overlap; fold() uses disjoint blocks and is exact.
    vector<long long> s{1, 2, 3, 4, 5};
    SparseTable<long long, Plus> sum(s);
    CHECK_EQ(fold(sum, 0, 4), 15LL);
    CHECK_EQ(sum.query(0, 4), 1 + 2 + 3 + 4 + 2 + 3 + 4 + 5);   // [0,3] + [1,4]: wrong on purpose
    CHECK_EQ(fold(sum, 1, 3), 9LL);

    // Stress: every range of random arrays of random sizes (including non-powers of two).
    for (int iter = 0; iter < 200; iter++) {
        int n = (int)t::rand_int(1, 40);
        auto v = t::rand_vec(n, -50, 50);
        SparseTable<int> smin(v);
        SparseTable<int, Max<int>> smax(v);
        vector<long long> vl(v.begin(), v.end());
        SparseTable<long long, Plus> ssum(vl);
        for (int l = 0; l < n; l++) {
            int lo = INT_MAX, hi = INT_MIN;
            long long total = 0;
            for (int r = l; r < n; r++) {
                lo = min(lo, v[r]);
                hi = max(hi, v[r]);
                total += v[r];
                CHECK_EQ(smin.query(l, r), lo);
                CHECK_EQ(smax.query(l, r), hi);
                CHECK_EQ(fold(ssum, l, r), total);
            }
        }
    }

    // Stress gcd (idempotent) and concatenation (associative, not commutative) with random queries.
    for (int iter = 0; iter < 200; iter++) {
        int n = (int)t::rand_int(1, 30);
        auto v = t::rand_vec(n, 1, 60);
        SparseTable<int, Gcd> sg(v);
        string str = t::rand_string(n, 'a', 'e');
        vector<string> chars;
        for (char c : str) chars.push_back(string(1, c));
        SparseTable<string, Concat> scat(chars);
        for (int q = 0; q < 20; q++) {
            int l = (int)t::rand_int(0, n - 1), r = (int)t::rand_int(l, n - 1);
            int expect = 0;
            for (int i = l; i <= r; i++) expect = gcd(expect, v[i]);
            CHECK_EQ(sg.query(l, r), expect);
            CHECK_EQ(fold(scat, l, r), str.substr(l, r - l + 1));
        }
    }
    return t::summary("sparse_table");
}
