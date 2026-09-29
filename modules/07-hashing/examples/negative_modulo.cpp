// Remainders as hash-map keys (module 07 section 4): C++'s % keeps the sign of the dividend, so a
// negative prefix sum gets a negative remainder. Normalize before using it as a key.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:mod]
// C++'s % truncates toward zero, so -7 % 3 == -1 (Python says 2). As map keys, -1 and 2 are the
// same remainder class but different keys: normalize every remainder into [0, m).
long long mod(long long a, long long m) { return (a % m + m) % m; }   // needs m > 0
// [/snippet]

int main() {
    CHECK_EQ(-7 % 3, -1);                       // the C++ behaviour that needs fixing
    CHECK_EQ(7 % -3, 1);                        // the sign follows the dividend, not the divisor
    CHECK_EQ(mod(-7, 3), 2);
    CHECK_EQ(mod(7, 3), 1);
    CHECK_EQ(mod(-6, 3), 0);
    CHECK_EQ(mod(0, 5), 0);
    CHECK_EQ(mod(-1'000'000'000'000'000LL, 7), (7 - 1'000'000'000'000'000LL % 7) % 7);

    // -1 and 1 differ by 2, so they are the same class mod 2. Raw % files them under two keys.
    unordered_map<long long, int> raw, normalized;
    for (long long prefix : {-1LL, 1LL}) {
        raw[prefix % 2]++;
        normalized[mod(prefix, 2)]++;
    }
    CHECK_EQ(raw.size(), 2);                    // keys -1 and 1: the pair is missed
    CHECK_EQ(normalized.size(), 1);             // key 1, seen twice
    CHECK_EQ(normalized[1], 2);

    for (int iter = 0; iter < 500; iter++) {    // mod() lands in [0, m) and keeps the class
        long long x = t::rand_int(-1'000'000'000'000LL, 1'000'000'000'000LL), m = t::rand_int(1, 1000);
        long long r = mod(x, m);
        CHECK(0 <= r && r < m);
        CHECK_EQ((x - r) % m, 0);
        long long y = x + m * t::rand_int(-1000, 1000);   // same class as x
        CHECK_EQ(mod(y, m), r);
    }
    return t::summary("negative_modulo");
}
