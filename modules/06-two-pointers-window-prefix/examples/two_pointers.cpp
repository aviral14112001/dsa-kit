// Module 06 section 1: the same-direction two-pointer template, tested against brute force.
// (The converging template is modules/02-patterns/examples/skeletons.cpp#converging and 167's solution.)
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:same_direction]
// How many pairs i < j have a[j] - a[i] <= d?  `a` sorted ascending, d >= 0.
// For a right end j, the valid left ends are exactly [i, j): a suffix of the indices before j.
// When j moves right, a[j] grows, so i never has to move back: both pointers only advance, and the
// whole loop is O(n) (after an O(n log n) sort, if the input isn't sorted yet).
long long count_close_pairs(const vector<int>& a, long long d) {
    long long count = 0;
    int i = 0;
    for (int j = 0; j < (int)a.size(); j++) {
        while ((long long)a[j] - a[i] > d) i++;   // a[i] is too far from a[j], and from every later a[j]
        count += j - i;                           // pairs (i, j), (i+1, j), ..., (j-1, j)
    }
    return count;
}
// [/snippet]

long long brute(const vector<int>& a, long long d) {
    long long count = 0;
    for (size_t i = 0; i < a.size(); i++)
        for (size_t j = i + 1; j < a.size(); j++) count += (long long)a[j] - a[i] <= d;
    return count;
}

int main() {
    CHECK_EQ(count_close_pairs({1, 3, 4, 8}, 3), 3);             // (1,3) (1,4) (3,4)
    CHECK_EQ(count_close_pairs({5, 5, 5}, 0), 3);                // equal values: difference 0
    CHECK_EQ(count_close_pairs({}, 10), 0);
    CHECK_EQ(count_close_pairs({INT_MIN, INT_MAX}, 1), 0);       // the difference needs 64 bits
    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(0, 15), -20, 20);
        sort(a.begin(), a.end());
        long long d = t::rand_int(0, 15);
        CHECK_EQ(count_close_pairs(a, d), brute(a, d));
    }
    return t::summary("06-two-pointers");
}
