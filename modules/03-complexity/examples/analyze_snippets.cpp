// The eight snippets from the Module 03 section 1 drill. main() measures each one and checks the answers
// given in the notes, so don't read main() until you've written your own answers.
//     make run F=modules/03-complexity/examples/analyze_snippets.cpp
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

long long snippet1(int n) {
    long long ops = 0;
    // [snippet:s1]
    for (int i = 0; i < n; i++)
        for (int j = 1; j < n; j *= 2)
            ops++;
    // [/snippet]
    return ops;
}

long long snippet2(int n) {
    long long ops = 0;
    // [snippet:s2]
    for (int i = 1; i * i <= n; i++)
        ops++;
    // [/snippet]
    return ops;
}

long long snippet3(int n) {
    long long ops = 0;
    // [snippet:s3]
    for (int i = 1; i <= n; i++)
        for (int j = i; j <= n; j += i)
            ops++;
    // [/snippet]
    return ops;
}

long long snippet4(int n) {
    long long ops = 0;
    // [snippet:s4]
    string s;
    for (int i = 0; i < n; i++) {
        s = s + 'x';
        ops++;
    }
    // [/snippet]
    return ops;
}

long long snippet5(const vector<int>& a) {
    long long ops = 0;
    int n = (int)a.size();
    // [snippet:s5]
    map<int, int> position;          // a holds n distinct values
    for (int i = 0; i < n; i++) {
        position[a[i]] = i;
        ops++;
    }
    // [/snippet]
    return ops;
}

// [snippet:s6]
int f(int n, long long& ops) {
    ops++;
    if (n == 0) return 1;
    return f(n - 1, ops) + f(n - 1, ops);
}
// [/snippet]

long long snippet7(int n) {
    long long ops = 0;
    // [snippet:s7]
    while (n > 0) {
        n /= 2;
        ops++;
    }
    // [/snippet]
    return ops;
}

long long snippet8(int n) {
    long long ops = 0;
    // [snippet:s8]
    vector<bool> composite(n + 1, false);
    for (int i = 2; i <= n; i++)
        if (!composite[i])
            for (long long j = 1LL * i * i; j <= n; j += i) {
                composite[j] = true;
                ops++;
            }
    // [/snippet]
    return ops;
}

// ---- Measuring hidden costs (test machinery only: you never write this in an interview) ----

// An allocator that counts the bytes requested, to expose the copying inside `s = s + 'x'`.
template <class T>
struct CountingAllocator {
    using value_type = T;
    static inline long long bytes = 0;
    CountingAllocator() = default;
    template <class U> CountingAllocator(const CountingAllocator<U>&) {}
    T* allocate(size_t n) {
        bytes += (long long)(n * sizeof(T));
        return allocator<T>().allocate(n);
    }
    void deallocate(T* p, size_t n) { allocator<T>().deallocate(p, n); }
    template <class U> bool operator==(const CountingAllocator<U>&) const { return true; }
};
using CountedString = basic_string<char, char_traits<char>, CountingAllocator<char>>;

long long bytes_for_concat(int n) {       // snippet 4, measured
    CountingAllocator<char>::bytes = 0;
    CountedString s;
    for (int i = 0; i < n; i++) s = s + 'x';
    return CountingAllocator<char>::bytes;
}

long long bytes_for_append(int n) {       // the fix: s += 'x'
    CountingAllocator<char>::bytes = 0;
    CountedString s;
    for (int i = 0; i < n; i++) s += 'x';
    return CountingAllocator<char>::bytes;
}

struct CountingLess {                     // a comparator that counts its calls, to expose the map's log n
    static inline long long comparisons = 0;
    bool operator()(int x, int y) const {
        comparisons++;
        return x < y;
    }
};

long long comparisons_for_map(const vector<int>& a) {   // snippet 5, measured
    CountingLess::comparisons = 0;
    map<int, int, CountingLess> position;
    for (int i = 0; i < (int)a.size(); i++) position[a[i]] = i;
    return CountingLess::comparisons;
}

int main() {
    // 1: n * ceil(log2 n) iterations -> Θ(n log n)
    for (int n : {1, 2, 8, 9, 1000, 100'000}) CHECK_EQ(snippet1(n), 1LL * n * bit_width(unsigned(n - 1)));

    // 2: floor(sqrt(n)) iterations -> Θ(√n)
    for (int n : {1, 15, 16, 17, 1'000'000, 999'999}) {
        long long r = snippet2(n);
        CHECK(r * r <= n && (r + 1) * (r + 1) > n);
    }

    // 3: sum of n / i over i = 1..n -> Θ(n log n), squeezed between n(H_n - 1) and n H_n
    CHECK_EQ(snippet3(10), 27);
    CHECK_EQ(snippet3(100), 482);
    CHECK_EQ(snippet3(1000), 7069);
    for (int n : {1000, 100'000}) {
        double harmonic = 0;
        for (int i = 1; i <= n; i++) harmonic += 1.0 / i;
        CHECK(snippet3(n) <= n * harmonic && snippet3(n) > n * (harmonic - 1));
    }

    // 4: the loop runs n times, but the bytes copied grow 4x when n doubles -> Θ(n²)
    CHECK_EQ(snippet4(1000), 1000);
    long long concat1 = bytes_for_concat(2000), concat2 = bytes_for_concat(4000);
    CHECK(concat2 > 3.5 * concat1 && concat2 < 4.5 * concat1);
    CHECK(concat1 > 2000LL * 2000 / 4);
    long long append1 = bytes_for_append(2000), append2 = bytes_for_append(4000);
    CHECK(append2 < 2.5 * append1);            // += only doubles: Θ(n)
    CHECK(append2 < 6 * 4000);                 // a few bytes per character, in total

    // 5: n map operations, each Θ(log n) comparisons -> Θ(n log n)
    for (int n : {1 << 12, 1 << 16}) {
        vector<int> a(n);
        iota(a.begin(), a.end(), 0);
        shuffle(a.begin(), a.end(), t::rng());
        CHECK_EQ(snippet5(a), n);
        long long comps = comparisons_for_map(a);
        double n_log_n = n * log2(n);
        CHECK(comps >= 0.5 * n_log_n && comps <= 4 * n_log_n);
    }

    // 6: 2^(n+1) - 1 calls -> Θ(2^n)
    for (int n = 0; n <= 20; n++) {
        long long ops = 0;
        CHECK_EQ(f(n, ops), 1 << n);
        CHECK_EQ(ops, (1LL << (n + 1)) - 1);
    }

    // 7: floor(log2 n) + 1 iterations -> Θ(log n)
    for (int n : {1, 2, 3, 1000, 1'000'000, INT_MAX}) CHECK_EQ(snippet7(n), bit_width(unsigned(n)));

    // 8: about n ln ln n marks -> Θ(n log log n): nearly linear
    for (int n : {10, 1000, 100'000, 1'000'000}) {
        long long ops = snippet8(n);
        CHECK(ops <= n * log(log(n)));
        CHECK(ops >= n / 2 - 2);               // the multiples of 2 alone
    }
    double ratio = (double)snippet8(1'000'000) / snippet8(500'000);
    CHECK(ratio > 2.0 && ratio < 2.2);         // doubling n just over doubles the work

    return t::summary("analyze_snippets");
}
