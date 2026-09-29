// Amortized analysis, measured: how often does push_back reallocate, and how many element moves do
// all those reallocations cost in total? Module 03 section 1.
//     make run F=modules/03-complexity/examples/amortized_vector.cpp    (-DLOCAL also prints capacities)
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

struct GrowthStats {
    int reallocations = 0;   // how many times the capacity changed
    long long moves = 0;     // elements carried over to a new buffer, summed over every reallocation
};

// [snippet:count_reallocations]
GrowthStats push_n(int n) {
    GrowthStats stats;
    vector<int> v;
    size_t cap = v.capacity();
    for (int i = 0; i < n; i++) {
        if (v.size() == v.capacity()) stats.moves += (long long)v.size();   // full: growing moves them all
        v.push_back(i);
        if (v.capacity() != cap) {
            stats.reallocations++;
            cap = v.capacity();
        }
    }
    return stats;
}
// [/snippet]

// [snippet:additive_growth]
// A hypothetical vector that grows by a fixed `step` slots instead of by a factor.
long long moves_with_additive_growth(long long n, long long step) {
    long long moves = 0, cap = 0;
    for (long long size = 0; size < n; size++) {
        if (size == cap) {        // full: allocate cap + step slots and carry over all `size` elements
            moves += size;
            cap += step;
        }
    }
    return moves;
}
// [/snippet]

// An element type that counts every copy or move the vector makes of it: a direct measurement to
// cross-check the moves that push_n() infers from size() at each reallocation.
struct Tracked {
    static inline long long relocations = 0;
    int value;
    Tracked(int v) : value(v) {}
    Tracked(const Tracked& o) : value(o.value) { relocations++; }
    Tracked(Tracked&& o) noexcept : value(o.value) { relocations++; }
    Tracked& operator=(const Tracked&) = default;
    Tracked& operator=(Tracked&&) = default;
};

int main() {
    // Doubling (libc++, libstdc++) gives about log2(n) reallocations; MSVC grows by 1.5x, about
    // log_1.5(n). Every mainstream library uses a factor >= 1.5, so check against log base 1.5.
    for (int n : {1, 10, 1000, 100'000, 1'000'000}) {
        GrowthStats stats = push_n(n);
        CHECK(stats.reallocations <= log(n) / log(1.5) + 3);   // O(log n) reallocations...
        CHECK(stats.moves <= 3LL * n);                          // ...and O(n) moves: O(1) amortized per push

        Tracked::relocations = 0;
        vector<Tracked> tracked;
        for (int i = 0; i < n; i++) tracked.emplace_back(i);   // built in place, so only growth moves count
        CHECK_EQ(Tracked::relocations, stats.moves);           // inference == measurement
    }

    {
        // [snippet:reserve]
        vector<int> v;
        v.reserve(1000);                   // one allocation up front...
        size_t cap = v.capacity();
        for (int i = 0; i < 1000; i++) v.push_back(i);
        CHECK(cap >= 1000);
        CHECK_EQ(v.capacity(), cap);       // ...so no push_back ever reallocated
        // [/snippet]
    }

    // Growing by a constant is quadratic: ~n^2 / (2 * step) moves, here ~780 per element.
    CHECK(moves_with_additive_growth(100'000, 64) > 100LL * 100'000);
    CHECK(push_n(100'000).moves < 3LL * 100'000);

#ifdef LOCAL
    vector<int> v;
    size_t cap = v.capacity();
    cout << "capacities over 1000 push_backs:";
    for (int i = 0; i < 1000; i++) {
        v.push_back(i);
        if (v.capacity() != cap) cout << ' ' << (cap = v.capacity());
    }
    cout << '\n';
#endif
    return t::summary("amortized_vector");
}
