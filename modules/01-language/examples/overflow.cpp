// Numeric pitfalls: overflow, unsigned wrap-around, char arithmetic, integer division, floating point.
// Module 01 section 2. The build runs UBSan, so the BAD lines can't run here (signed overflow would abort the
// program, which is the point). They sit in comments; the CHECKs run the fixed versions and prove
// that the bad ones would have overflowed.
//     make run F=modules/01-language/examples/overflow.cpp
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

int main() {
    {
        // [snippet:limits]
        CHECK_EQ(INT_MAX, 2'147'483'647);                    // ≈ 2.1e9 (10 digits)
        CHECK_EQ(LLONG_MAX, 9'223'372'036'854'775'807LL);    // ≈ 9.2e18 (19 digits)
        CHECK_EQ(sizeof(int), 4);
        CHECK_EQ(sizeof(long long), 8);
        CHECK_EQ(numeric_limits<size_t>::max(), 18'446'744'073'709'551'615ULL);   // unsigned 64-bit
        // [/snippet]
    }
    {
        // [snippet:widen_first]
        int a = 100'000, b = 100'000;
        // long long bad = a * b;       // int * int overflows BEFORE the result is widened: UB
        long long good = 1LL * a * b;   // 1LL makes the whole product 64-bit from the first step
        CHECK_EQ(good, 10'000'000'000LL);
        CHECK(good > INT_MAX);          // proof that a * b could never have fit in an int

        // int bad_mask = 1 << 40;      // shifting a 32-bit int by 40 bits: UB
        long long mask = 1LL << 40;
        CHECK_EQ(mask, 1'099'511'627'776LL);
        // [/snippet]
    }
    {
        // [snippet:midpoint]
        int lo = 2'000'000'000, hi = 2'100'000'000;
        // int bad = (lo + hi) / 2;     // lo + hi = 4.1e9: overflow, UB
        int mid = lo + (hi - lo) / 2;   // hi - lo fits whenever 0 <= lo <= hi (true for indices)
        CHECK_EQ(mid, 2'050'000'000);
        CHECK(1LL * lo + hi > INT_MAX);
        CHECK_EQ(midpoint(lo, hi), 2'050'000'000);   // C++20 <numeric> does it safely for any ints
        // [/snippet]
    }
    {
        // [snippet:accumulate]
        vector<int> big(3, 1'000'000'000);
        // accumulate(big.begin(), big.end(), 0)   // the running sum is an int (the init's type): UB
        long long total = accumulate(big.begin(), big.end(), 0LL);
        CHECK_EQ(total, 3'000'000'000LL);

        vector<double> halves{0.5, 0.5, 0.5};
        CHECK_EQ(accumulate(halves.begin(), halves.end(), 0), 0);   // int init: every step truncates
        CHECK_NEAR(accumulate(halves.begin(), halves.end(), 0.0), 1.5, 1e-12);
        // [/snippet]
    }
    {
        // [snippet:size_minus_one]
        vector<int> empty;
        CHECK_EQ(empty.size() - 1, SIZE_MAX);   // size() is unsigned: 0 - 1 wraps to 18446744073709551615
        // for (size_t i = 0; i < v.size() - 1; i++)   on an empty v: ~forever, reading garbage
        int iterations = 0;
        for (int i = 0; i + 1 < (int)empty.size(); i++) iterations++;   // cast once, stay signed
        CHECK_EQ(iterations, 0);
        // [/snippet]
    }
    {
        // [snippet:signed_unsigned]
        vector<int> v{1, 2, 3};
        int i = -1;
        // if (i < v.size())            // -1 converts to SIZE_MAX, so this is FALSE (-Wsign-compare warns)
        CHECK(!(static_cast<size_t>(i) < v.size()));   // that is what the comparison really computes
        CHECK(i < (int)v.size());       // cast the size to int once...
        CHECK(cmp_less(i, v.size()));   // ...or use C++20 cmp_less, which compares true values
        // [/snippet]
    }
    {
        // [snippet:char_math]
        string word = "banana";
        int count[26] = {};             // all zeros; index = letter - 'a'
        for (char c : word) count[c - 'a']++;
        CHECK_EQ(count['a' - 'a'], 3);
        CHECK_EQ(count['n' - 'a'], 2);
        CHECK_EQ('7' - '0', 7);         // digit character -> its value
        char third = char('a' + 2);     // 'a' + 2 is the int 99: cast back to get a char
        CHECK_EQ(third, 'c');
        CHECK_EQ(string(1, char('0' + 5)), "5");
        // [/snippet]
    }
    {
        // [snippet:division]
        CHECK_EQ(7 / 2, 3);
        CHECK_EQ(-7 / 2, -3);           // division truncates toward zero (same as C#)
        CHECK_EQ(-7 % 3, -1);           // so % can be negative: it takes the dividend's sign
        int m = 3;
        CHECK_EQ(((-7 % m) + m) % m, 2);   // always lands in [0, m)
        int total = 7, per_box = 2;
        CHECK_EQ((total + per_box - 1) / per_box, 4);   // ceil(a / b) for positive ints, no floating point
        // [/snippet]
    }
    {
        // [snippet:floating]
        double sum = 0.1 + 0.2;
        CHECK(sum != 0.3);                    // 0.1 has no exact binary representation
        CHECK(fabs(sum - 0.3) < 1e-9);        // compare with a tolerance instead

        long long x = 1'000'000'000'000'000'000LL - 1;   // 10^18 - 1: its root is 999'999'999.99...
        long long r = (long long)sqrt((double)x);       // but (double)x rounds up to exactly 1e18...
        CHECK_EQ(r, 1'000'000'000LL);                    // ...so the "floor of the root" is one too big
        CHECK(r * r > x);                                // an exact integer check exposes it
        CHECK((double)(1LL << 53) == (double)((1LL << 53) + 1));   // doubles are exact only up to 2^53
        // [/snippet]
    }
    return t::summary("overflow");
}
