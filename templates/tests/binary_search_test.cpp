#include <bits/stdc++.h>
#include "test.hpp"
#include "binary_search.hpp"
using namespace std;

int main() {
    // lower_bound / upper_bound expressed as first_true
    vector<int> a{1, 3, 3, 3, 7, 9};
    auto lb = [&](int x) { return first_true(0, (int)a.size() - 1, [&](int i) { return a[i] >= x; }); };
    auto ub = [&](int x) { return first_true(0, (int)a.size() - 1, [&](int i) { return a[i] > x; }); };
    CHECK_EQ(lb(3), 1);
    CHECK_EQ(ub(3), 4);
    CHECK_EQ(lb(0), 0);
    CHECK_EQ(lb(10), 6);  // not found -> hi + 1
    CHECK_EQ(last_true(0, (int)a.size() - 1, [&](int i) { return a[i] <= 3; }), 3);
    CHECK_EQ(last_true(0, 5, [](int) { return false; }), -1);

    // binary search on the answer: integer square root of 2^40 with 64-bit bounds
    long long n = 1LL << 40;
    CHECK_EQ(last_true(0LL, n, [&](long long x) { return x <= n / max(x, 1LL); }), 1LL << 20);

    CHECK_NEAR(first_true_real(0.0, 2.0, [](double x) { return x * x >= 2.0; }), sqrt(2.0), 1e-9);

    // stress: agrees with std::lower_bound / upper_bound on random sorted arrays
    for (int iter = 0; iter < 500; iter++) {
        auto v = t::rand_vec((int)t::rand_int(0, 30), -20, 20);
        sort(v.begin(), v.end());
        int x = (int)t::rand_int(-25, 25);
        int hi = (int)v.size() - 1;
        CHECK_EQ(first_true(0, hi, [&](int i) { return v[i] >= x; }), lower_bound(v.begin(), v.end(), x) - v.begin());
        CHECK_EQ(first_true(0, hi, [&](int i) { return v[i] > x; }), upper_bound(v.begin(), v.end(), x) - v.begin());
    }
    return t::summary("binary_search");
}
