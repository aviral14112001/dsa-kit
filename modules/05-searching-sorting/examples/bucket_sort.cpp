// Bucket sort for doubles spread over [0, 1) (module 05 section 2): scatter into n buckets by value,
// sort each (small) bucket, concatenate.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:bucket_sort]
// Bucket i holds the values in [i/n, (i+1)/n). Uniform input puts O(1) values in each bucket
// on average, so the whole sort is O(n) expected. Skewed input can pile everything into one
// bucket: then it is just std::sort, O(n log n).
void bucket_sort(vector<double>& a) {
    int n = a.size();
    vector<vector<double>> buckets(n);
    for (double x : a) buckets[min(n - 1, (int)(x * n))].push_back(x);  // min(): guard x*n rounding up to n
    a.clear();
    for (auto& bucket : buckets) {
        sort(bucket.begin(), bucket.end());
        a.insert(a.end(), bucket.begin(), bucket.end());   // buckets are already in value order
    }
}
// [/snippet]

double rand_unit() { return uniform_real_distribution<double>(0.0, 1.0)(t::rng()); }

int main() {
    vector<double> empty, one{0.5}, few{0.42, 0.32, 0.99, 0.0, 0.32, 0.5};
    bucket_sort(empty);
    bucket_sort(one);
    bucket_sort(few);
    CHECK(empty.empty());
    CHECK_EQ(one, vector<double>{0.5});
    CHECK_EQ(few, vector<double>{0.0, 0.32, 0.32, 0.42, 0.5, 0.99});

    vector<double> edge{nextafter(1.0, 0.0), 0.0, nextafter(1.0, 0.0), 0.5};   // the largest double below 1
    vector<double> edge_sorted = edge;
    sort(edge_sorted.begin(), edge_sorted.end());
    bucket_sort(edge);
    CHECK_EQ(edge, edge_sorted);

    for (int iter = 0; iter < 300; iter++) {        // stress test vs std::sort, uniform and skewed
        int n = (int)t::rand_int(0, 200);
        vector<double> v(n);
        for (auto& x : v) x = (iter % 3 == 0) ? rand_unit() * 0.001 : rand_unit();   // skewed: one bucket
        vector<double> expected = v;
        sort(expected.begin(), expected.end());
        bucket_sort(v);
        CHECK_EQ(v, expected);
    }
    return t::summary("bucket_sort");
}
