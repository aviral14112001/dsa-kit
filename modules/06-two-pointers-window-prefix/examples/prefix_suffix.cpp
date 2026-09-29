// Module 06 section 3: using templates/prefix_sum.hpp, and the running-sum + total trick that needs no array.
#include <bits/stdc++.h>
#include "test.hpp"
#include "prefix_sum.hpp"
using namespace std;

// [snippet:using_prefix]
// Average of each query range [l, r], after one O(n) build: O(n + q) instead of O(n * q).
vector<double> range_averages(const vector<int>& a, const vector<pair<int, int>>& queries) {
    PrefixSum ps(a);
    vector<double> out;
    for (auto [l, r] : queries) out.push_back((double)ps.sum(l, r) / (r - l + 1));
    return out;
}
// [/snippet]

// [snippet:split_points]
// How many split points i (left = a[0..i], right = a[i+1..n-1], both non-empty) have
// left sum >= right sum? right = total - left, so a running sum and the total are enough: O(1) space.
int count_heavy_left_splits(const vector<int>& a) {
    long long total = accumulate(a.begin(), a.end(), 0LL);   // 0LL, not 0: the sum is done in the init's type
    long long left = 0;
    int count = 0;
    for (int i = 0; i + 1 < (int)a.size(); i++) {
        left += a[i];
        if (left >= total - left) count++;
    }
    return count;
}
// [/snippet]

int count_heavy_left_splits_brute(const vector<int>& a) {
    int count = 0, n = (int)a.size();
    for (int i = 0; i + 1 < n; i++) {
        long long left = 0, right = 0;
        for (int k = 0; k <= i; k++) left += a[k];
        for (int k = i + 1; k < n; k++) right += a[k];
        count += left >= right;
    }
    return count;
}

int main() {
    auto avg = range_averages({2, 4, 6, 8}, {{0, 3}, {1, 2}, {3, 3}});
    CHECK_NEAR(avg[0], 5.0, 1e-9);
    CHECK_NEAR(avg[1], 5.0, 1e-9);
    CHECK_NEAR(avg[2], 8.0, 1e-9);

    CHECK_EQ(count_heavy_left_splits({10, 4, -8, 7}), 2);    // splits after 10 and after 4
    CHECK_EQ(count_heavy_left_splits({5}), 0);               // no split leaves both sides non-empty
    vector<int> big(100000, 100000);                         // total 10^10: an int total would overflow
    CHECK_EQ(count_heavy_left_splits(big), 50000);           // left >= right from i = 49999 on
    CHECK_EQ(accumulate(big.begin(), big.end(), 0LL), 10'000'000'000LL);

    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(1, 15), -20, 20);
        CHECK_EQ(count_heavy_left_splits(a), count_heavy_left_splits_brute(a));
    }
    return t::summary("06-prefix-suffix");
}
