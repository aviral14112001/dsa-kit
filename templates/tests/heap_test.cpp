// Tests for templates/heap.hpp: fixed cases, the heap property after heapify, and random
// operation sequences replayed against std::priority_queue (the brute force here is the STL).
#include <bits/stdc++.h>
#include "test.hpp"
#include "heap.hpp"
using namespace std;

// The heap property, checked directly: no element outranks its parent.
template <class T, class C>
bool heap_property_holds(const BinaryHeap<T, C>& h) {
    for (int i = 1; i < h.size(); i++)
        if (h.outranks(i, (i - 1) / 2)) return false;
    return true;
}

template <class T, class C>
vector<T> drain(BinaryHeap<T, C> h) {           // pops everything: the order a heap sort would produce
    vector<T> out;
    while (!h.empty()) { out.push_back(h.top()); h.pop(); }
    return out;
}

// Replays the same random pushes and pops on BinaryHeap and priority_queue, comparing after each.
template <class T, class C, class Gen>
void stress_against_stl(BinaryHeap<T, C> mine, priority_queue<T, vector<T>, C> stl, int ops, Gen random_value) {
    for (int op = 0; op < ops; op++) {
        if (stl.empty() || t::rand_int(0, 99) < 55) {   // slightly more pushes than pops, so it grows
            T x = random_value();
            mine.push(x);
            stl.push(x);
        } else {
            mine.pop();
            stl.pop();
        }
        CHECK_EQ(mine.size(), stl.size());
        if (!stl.empty()) CHECK_EQ(mine.top(), stl.top());
    }
    CHECK(heap_property_holds(mine));
}

struct CountingLess {                           // counts comparisons, to check the O(n) heapify claim
    long long* calls;
    bool operator()(int x, int y) const { ++*calls; return x < y; }
};

int main() {
    // fixed cases: max-heap by default, duplicates kept
    BinaryHeap<int> mx;
    CHECK(mx.empty());
    CHECK_EQ(mx.size(), 0);
    for (int x : {5, 1, 8, 3, 8}) mx.push(x);
    CHECK_EQ(mx.top(), 8);
    CHECK_EQ(mx.size(), 5);
    CHECK_EQ(drain(mx), vector<int>{8, 8, 5, 3, 1});

    // min-heap via greater<int>
    BinaryHeap<int, greater<int>> mn;
    for (int x : {5, 1, 8, 3}) mn.push(x);
    CHECK_EQ(mn.top(), 1);
    mn.pop();
    CHECK_EQ(mn.top(), 3);

    // heapify from a vector, then drain: a heap sort (descending for a max-heap)
    BinaryHeap<int> built(vector<int>{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5});
    CHECK(heap_property_holds(built));
    CHECK_EQ(built.top(), 9);
    CHECK_EQ(drain(built), vector<int>{9, 6, 5, 5, 5, 4, 3, 3, 2, 1, 1});
    CHECK(BinaryHeap<int>(vector<int>{}).empty());
    CHECK_EQ(BinaryHeap<int>(vector<int>{7}).top(), 7);

    // pairs order lexicographically: a min-heap of (dist, node) pops the smallest dist, ties by node
    BinaryHeap<pair<int, int>, greater<>> by_dist;
    for (auto p : vector<pair<int, int>>{{4, 1}, {2, 7}, {2, 3}, {9, 0}}) by_dist.push(p);
    CHECK_EQ(drain(by_dist), vector<pair<int, int>>{{2, 3}, {2, 7}, {4, 1}, {9, 0}});

    // heaps are not stable: with a comparator that sees only the length, equal-length strings may
    // come out in any order. Compare against priority_queue by rank (length), not by value.
    auto shorter = [](const string& x, const string& y) { return x.size() > y.size(); };
    BinaryHeap<string, decltype(shorter)> by_len(shorter);
    priority_queue<string, vector<string>, decltype(shorter)> stl_by_len(shorter);
    for (int i = 0; i < 300; i++) {
        string s = t::rand_string((int)t::rand_int(0, 6), 'a', 'c');
        by_len.push(s);
        stl_by_len.push(s);
        if (t::rand_int(0, 2) == 0) { by_len.pop(); stl_by_len.pop(); }
        CHECK_EQ(by_len.top().size(), stl_by_len.top().size());
    }

    // heapify on random inputs: the heap property holds and draining sorts the input
    for (int iter = 0; iter < 300; iter++) {
        auto v = t::rand_vec((int)t::rand_int(0, 60), -20, 20);
        BinaryHeap<int> h(v);
        CHECK(heap_property_holds(h));
        CHECK(is_heap(h.a.begin(), h.a.end()));      // std::is_heap uses the same convention
        sort(v.rbegin(), v.rend());
        CHECK_EQ(drain(h), v);
    }

    // heapify is O(n): fewer than 2n comparisons, on random and on sorted inputs
    for (int n : {1, 2, 3, 10, 1000, 1 << 15}) {
        vector<int> sorted_input(n), random_input = t::rand_vec(n, 0, 1'000'000);
        iota(sorted_input.begin(), sorted_input.end(), 0);
        for (auto& input : {sorted_input, random_input}) {
            long long calls = 0;
            BinaryHeap<int, CountingLess> h(input, CountingLess{&calls});
            CHECK(calls < 2LL * n);
            CHECK(heap_property_holds(h));
        }
    }

    // stress: random operations against std::priority_queue
    for (int trial = 0; trial < 3; trial++) {
        stress_against_stl(BinaryHeap<int>(), priority_queue<int>(), 1500,
                           [] { return (int)t::rand_int(0, 30); });                 // many duplicates
        stress_against_stl(BinaryHeap<int, greater<int>>(), priority_queue<int, vector<int>, greater<int>>(), 1500,
                           [] { return (int)t::rand_int(INT_MIN, INT_MAX); });      // full int range
        stress_against_stl(BinaryHeap<pair<int, int>, greater<>>(),
                           priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>>(), 1500,
                           [] { return make_pair((int)t::rand_int(0, 5), (int)t::rand_int(0, 5)); });
    }
    // start from a heapified vector, then keep going with random operations
    for (int trial = 0; trial < 20; trial++) {
        auto v = t::rand_vec((int)t::rand_int(0, 200), -1000, 1000);
        stress_against_stl(BinaryHeap<int>(v), priority_queue<int>(v.begin(), v.end()), 300,
                           [] { return (int)t::rand_int(-1000, 1000); });
    }
    return t::summary("heap");
}
