// Coordinate compression + counting inversions with a Fenwick tree (templates/fenwick.hpp). Module 17 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
#include "fenwick.hpp"
using namespace std;

// [snippet:compress]
// Coordinate compression: replace each value by its rank among the distinct values (0 .. m-1).
// Order is kept, so "smaller than" questions have the same answers, but a Fenwick tree over values
// now needs m <= n slots instead of one per possible value (1e9, or negative values).
vector<int> compress(const vector<int>& a) {
    vector<int> sorted_vals(a);
    sort(sorted_vals.begin(), sorted_vals.end());
    sorted_vals.erase(unique(sorted_vals.begin(), sorted_vals.end()), sorted_vals.end());
    vector<int> ranks(a.size());
    for (size_t i = 0; i < a.size(); i++)
        ranks[i] = (int)(lower_bound(sorted_vals.begin(), sorted_vals.end(), a[i]) - sorted_vals.begin());
    return ranks;
}
// [/snippet]

// [snippet:inversions]
// Inversions: pairs i < j with a[i] > a[j]. Scan left to right; before recording a[j], count how
// many earlier values are greater: j values seen so far, minus those <= a[j]. O(n log n).
long long count_inversions(const vector<int>& a) {
    vector<int> r = compress(a);
    int n = (int)r.size();
    Fenwick<int> seen(n);                         // ranks are < n
    long long inversions = 0;                     // up to n(n-1)/2: about 5e9 for n = 1e5
    for (int j = 0; j < n; j++) {
        inversions += j - seen.prefix(r[j] + 1);  // prefix(r + 1) = earlier values with rank <= r[j]
        seen.add(r[j], 1);
    }
    return inversions;
}
// [/snippet]

long long brute(const vector<int>& a) {
    long long count = 0;
    for (size_t i = 0; i < a.size(); i++)
        for (size_t j = i + 1; j < a.size(); j++) count += a[i] > a[j];
    return count;
}

int main() {
    CHECK_EQ(compress({100, -5, 100, 7}), vector<int>{2, 0, 2, 1});
    CHECK_EQ(compress({}), vector<int>{});
    CHECK_EQ(count_inversions({2, 4, 1, 3, 5}), 3LL);      // (2,1) (4,1) (4,3)
    CHECK_EQ(count_inversions({1, 1, 1}), 0LL);            // equal values are not inversions
    CHECK_EQ(count_inversions({}), 0LL);
    CHECK_EQ(count_inversions({1'000'000'000, -1'000'000'000}), 1LL);

    // n = 1e5 strictly decreasing: n(n-1)/2 = 4,999,950,000 inversions, which overflows int.
    vector<int> dec(100000);
    for (int i = 0; i < 100000; i++) dec[i] = 100000 - i;
    CHECK_EQ(count_inversions(dec), 4'999'950'000LL);

    for (int iter = 0; iter < 300; iter++) {
        vector<int> v = t::rand_vec((int)t::rand_int(0, 40), -8, 8);
        CHECK_EQ(count_inversions(v), brute(v));
    }
    return t::summary("inversions");
}
