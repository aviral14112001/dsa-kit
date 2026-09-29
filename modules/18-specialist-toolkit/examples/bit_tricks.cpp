// Bit manipulation idioms, each checked against std::bitset or a brute force. Module 18 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:basics]
// Bit k of x, k = 0 being the least significant. & | ^ bind LOOSER than == and <, so parenthesize.
bool test_bit(int x, int k)  { return (x >> k) & 1; }
int set_bit(int x, int k)    { return x | (1 << k); }       // 1 << k is an int: fine for k <= 30
int clear_bit(int x, int k)  { return x & ~(1 << k); }
int toggle_bit(int x, int k) { return x ^ (1 << k); }
int lowbit(int x)            { return x & -x; }             // the lowest set bit, as a value (x != INT_MIN)
int drop_lowbit(int x)       { return x & (x - 1); }        // x with its lowest set bit cleared
bool is_power_of_two(long long x) { return x > 0 && (x & (x - 1)) == 0; }   // exactly one bit set
// [/snippet]

// [snippet:submasks]
// Every submask of mask, from mask itself down to 0. s - 1 clears s's lowest set bit and sets all
// bits below it; & mask keeps only bits that belong to mask: the result is the next smaller submask.
vector<int> submasks(int mask) {
    vector<int> result;
    for (int s = mask; ; s = (s - 1) & mask) {
        result.push_back(s);
        if (s == 0) break;                    // 0 is a submask too, and (0 - 1) & mask would wrap to mask
    }
    return result;
}
// [/snippet]

// [snippet:gray]
// Gray code: g(i) = i ^ (i >> 1). Consecutive codes differ in exactly one bit: going from i to i + 1
// flips a block of trailing bits of i, and after XOR-ing with the shifted copy only the top bit of
// that block survives.
int gray(int i) { return i ^ (i >> 1); }

// Inverse: bit k of i is the XOR of all bits of g at positions >= k, a prefix XOR from the top.
int gray_inverse(int g) {
    int i = 0;
    for (; g != 0; g >>= 1) i ^= g;
    return i;
}
// [/snippet]

int main() {
    // Basics against std::bitset.
    CHECK(test_bit(5, 0) && !test_bit(5, 1) && test_bit(5, 2));
    CHECK_EQ(set_bit(5, 1), 7);
    CHECK_EQ(clear_bit(7, 0), 6);
    CHECK_EQ(toggle_bit(6, 3), 14);
    CHECK_EQ(lowbit(12), 4);                  // 1100 -> 0100
    CHECK_EQ(drop_lowbit(12), 8);             // 1100 -> 1000
    CHECK_EQ(lowbit(-12), 4);                 // two's complement: same lowest set bit
    CHECK(is_power_of_two(1) && is_power_of_two(1LL << 62) && !is_power_of_two(0) && !is_power_of_two(6));
    CHECK(!is_power_of_two(LLONG_MIN));       // negative: the x > 0 guard matters
    for (int iter = 0; iter < 2000; iter++) {
        int x = (int)t::rand_int(1, INT_MAX), k = (int)t::rand_int(0, 30);
        bitset<32> b(x);
        CHECK_EQ(test_bit(x, k), (bool)b[k]);
        CHECK_EQ(set_bit(x, k), (int)bitset<32>(b).set(k).to_ulong());
        CHECK_EQ(clear_bit(x, k), (int)bitset<32>(b).reset(k).to_ulong());
        CHECK_EQ(toggle_bit(x, k), (int)bitset<32>(b).flip(k).to_ulong());
        int low = 0;
        while (!b[low]) low++;                // brute: position of the lowest set bit
        CHECK_EQ(lowbit(x), 1 << low);
        CHECK_EQ(drop_lowbit(x), x - (1 << low));
        CHECK_EQ(is_power_of_two(x), b.count() == 1);
    }

    // The builtins table in Notes section 2: GCC/Clang builtins vs C++20 <bit> vs std::bitset.
    for (int iter = 0; iter < 2000; iter++) {
        unsigned x = (unsigned)t::rand_int(1, UINT_MAX);
        unsigned long long y = (unsigned long long)t::rand_int(1, LLONG_MAX) * 2 + t::rand_int(0, 1);
        int highest = 31;
        while (!((x >> highest) & 1)) highest--;
        CHECK_EQ(__builtin_popcount(x), (int)bitset<32>(x).count());
        CHECK_EQ(std::popcount(x), (int)bitset<32>(x).count());
        CHECK_EQ(__builtin_popcountll(y), (int)bitset<64>(y).count());
        CHECK_EQ(__builtin_ctz(x), std::countr_zero(x));          // index of the lowest set bit
        CHECK_EQ(__builtin_clz(x), std::countl_zero(x));
        CHECK_EQ(31 - __builtin_clz(x), highest);                  // index of the highest set bit
        CHECK_EQ((int)std::bit_width(x) - 1, highest);             // = floor(log2 x)
        CHECK_EQ(std::has_single_bit(x), is_power_of_two(x));
    }
    CHECK_EQ(std::countr_zero(0u), 32);       // the <bit> versions are defined at 0; the builtins are not

    // Submasks: exactly the s with (s & mask) == s, in decreasing order; and summed over every mask
    // of n bits, the loop runs 3^n times (each bit: outside mask, in mask not s, in both).
    for (int mask = 0; mask < 256; mask++) {
        vector<int> expect;
        for (int s = mask; s >= 0; s--)
            if ((s & mask) == s) expect.push_back(s);
        CHECK_EQ(submasks(mask), expect);
    }
    for (int n = 0; n <= 10; n++) {
        long long total = 0, three_n = 1;
        for (int mask = 0; mask < (1 << n); mask++) total += (long long)submasks(mask).size();
        for (int i = 0; i < n; i++) three_n *= 3;
        CHECK_EQ(total, three_n);
    }

    // Gray code: a permutation of 0 .. 2^n - 1, one bit between neighbours (wrapping around), invertible.
    for (int n = 1; n <= 10; n++) {
        int size = 1 << n;
        vector<bool> seen(size, false);
        for (int i = 0; i < size; i++) {
            int g = gray(i);
            CHECK(g >= 0 && g < size && !seen[g]);
            seen[g] = true;
            CHECK_EQ(std::popcount((unsigned)(g ^ gray((i + 1) % size))), 1);
            CHECK_EQ(gray_inverse(g), i);
        }
    }
    return t::summary("bit_tricks");
}
