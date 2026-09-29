// Module 04 section 4: enumerating subarrays in O(n^2), and the contribution technique that avoids it.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:enumerate]
// Visit every subarray a[l..r] with its sum in O(n^2): fix l, extend r, keep a running sum.
// (Re-adding a[l..r] from scratch for each pair would be O(n^3).)
// Example: the total of the sums of all n(n+1)/2 subarrays.
long long sum_of_subarray_sums_quadratic(const vector<int>& a) {
    long long total = 0;
    int n = (int)a.size();
    for (int l = 0; l < n; l++) {
        long long sum = 0;
        for (int r = l; r < n; r++) {
            sum += a[r];   // now sum == a[l] + ... + a[r]
            total += sum;
        }
    }
    return total;
}
// [/snippet]

// [snippet:contribution]
// Contribution technique: instead of "what does each subarray add up to?", ask "how many
// subarrays contain a[i]?". a[i] is in a[l..r] exactly when l <= i <= r: (i + 1) choices for l
// times (n - i) choices for r. O(n). (Fits in long long while |a[i]| * n^3 / 6 < 9.2e18.)
long long sum_of_subarray_sums(const vector<int>& a) {
    long long total = 0, n = (long long)a.size();
    for (long long i = 0; i < n; i++) total += a[i] * (i + 1) * (n - i);
    return total;
}
// [/snippet]

long long sum_of_subarray_sums_cubic(const vector<int>& a) {
    long long total = 0;
    int n = (int)a.size();
    for (int l = 0; l < n; l++)
        for (int r = l; r < n; r++)
            for (int k = l; k <= r; k++) total += a[k];
    return total;
}

int main() {
    CHECK_EQ(sum_of_subarray_sums({1, 2, 3}), 20);   // 1+2+3 + (1+2)+(2+3) + (1+2+3)
    CHECK_EQ(sum_of_subarray_sums({}), 0);
    CHECK_EQ(sum_of_subarray_sums({-5}), -5);

    for (int n = 0; n <= 50; n++) {                   // the double loop visits n(n+1)/2 subarrays
        long long visited = 0;
        for (int l = 0; l < n; l++)
            for (int r = l; r < n; r++) visited++;
        CHECK_EQ(visited, (long long)n * (n + 1) / 2);
    }

    vector<int> big(100000, 10000);                   // the contribution version at full scale
    long long n = 100000;
    CHECK_EQ(sum_of_subarray_sums(big), 10000 * (n * (n + 1) * (n + 2) / 6));

    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(0, 15), -20, 20);
        long long expected = sum_of_subarray_sums_cubic(a);
        CHECK_EQ(sum_of_subarray_sums_quadratic(a), expected);
        CHECK_EQ(sum_of_subarray_sums(a), expected);
    }
    return t::summary("04-subarrays");
}
