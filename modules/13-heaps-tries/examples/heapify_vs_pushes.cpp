// Checks the "heapify is O(n), n pushes are O(n log n)" claim by counting comparisons with
// templates/heap.hpp. Module 13 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
#include "heap.hpp"
using namespace std;

// [snippet:counting]
// A comparator that counts its own calls: the quickest way to test a complexity claim.
struct CountingLess {
    long long* calls;
    bool operator()(int x, int y) const {
        ++*calls;
        return x < y;
    }
};
// [/snippet]

int main() {
    for (int n : {1 << 10, 1 << 14, 1 << 17}) {
        // Ascending input is the worst case for pushes into a max-heap: each new value is the
        // largest so far, so it climbs all the way to the root.
        vector<int> ascending(n);
        iota(ascending.begin(), ascending.end(), 0);
        long long by_heapify = 0, by_pushes = 0;
        BinaryHeap<int, CountingLess> built(ascending, CountingLess{&by_heapify});
        BinaryHeap<int, CountingLess> pushed(CountingLess{&by_pushes});
        for (int x : ascending) pushed.push(x);
        CHECK(by_heapify < 2LL * n);                            // sum over nodes of 2 * height < 2n
        int log_n = bit_width((unsigned)n) - 1;
        CHECK(by_pushes >= (long long)n * (log_n - 2));         // about n log2 n
        CHECK(built.top() == n - 1 && pushed.top() == n - 1);
        if (n == 1 << 17) {                                     // the numbers quoted in Notes section 1
            CHECK_EQ(by_heapify, 262110LL);
            CHECK_EQ(by_pushes, 1966099LL);
        }
    }
    // On random input the gap shrinks: an average push climbs only a level or two. Heapify's O(n)
    // is a worst-case guarantee; n pushes are O(n) only on average.
    long long by_heapify = 0, by_pushes = 0;
    vector<int> random_input = t::rand_vec(1 << 17, 0, 1'000'000);
    BinaryHeap<int, CountingLess> built(random_input, CountingLess{&by_heapify});
    BinaryHeap<int, CountingLess> pushed(CountingLess{&by_pushes});
    for (int x : random_input) pushed.push(x);
    CHECK(by_heapify < 2LL * (1 << 17));
    CHECK(by_pushes < 3LL * (1 << 17));
    return t::summary("heapify_vs_pushes");
}
