#include <bits/stdc++.h>
#include "test.hpp"
#include "number_theory.hpp"
using namespace std;

bool is_prime_brute(int x) {
    if (x < 2) return false;
    for (int d = 2; d * d <= x; d++)
        if (x % d == 0) return false;
    return true;
}

int main() {
    const long long MOD = 1'000'000'007;

    // Sieve of Eratosthenes against trial division, including the tiny n that need care.
    for (int n = 0; n <= 300; n++) {
        auto is_prime = prime_sieve(n);
        CHECK_EQ((int)is_prime.size(), n + 1);
        for (int x = 0; x <= n; x++) CHECK_EQ(is_prime[x], is_prime_brute(x));
    }
    auto big = prime_sieve(1'000'000);
    CHECK_EQ(count(big.begin(), big.end(), true), 78498);    // pi(10^6)

    // Linear sieve: spf, the prime list, and factorizations.
    Sieve sv(100'000);
    CHECK_EQ(sv.primes.size(), (size_t)9592);                // pi(10^5)
    CHECK_EQ(sv.factorize(360), vector<pair<int, int>>{{2, 3}, {3, 2}, {5, 1}});
    CHECK_EQ(sv.factorize(1), vector<pair<int, int>>{});
    CHECK_EQ(sv.factorize(99991), vector<pair<int, int>>{{99991, 1}});   // a prime
    CHECK(sv.is_prime(2) && !sv.is_prime(1) && !sv.is_prime(0) && !sv.is_prime(100'000));
    auto small = prime_sieve(100'000);
    for (int x = 0; x <= 100'000; x++) CHECK_EQ(sv.is_prime(x), (bool)small[x]);
    for (int iter = 0; iter < 2000; iter++) {
        int x = (int)t::rand_int(2, 100'000);
        int smallest = 2;
        while (x % smallest != 0) smallest++;
        CHECK_EQ(sv.spf[x], smallest);
        long long product = 1;
        int last = 0;
        for (auto [p, e] : sv.factorize(x)) {
            CHECK(is_prime_brute(p) && p > last && e >= 1);  // primes, strictly increasing
            last = p;
            for (int i = 0; i < e; i++) product *= p;
        }
        CHECK_EQ(product, (long long)x);
    }
    Sieve tiny(0);                                           // must not crash
    CHECK(tiny.primes.empty());

    // gcd / lcm.
    CHECK_EQ(gcd_euclid(12, 18), 6LL);
    CHECK_EQ(gcd_euclid(0, 7), 7LL);
    CHECK_EQ(gcd_euclid(7, 0), 7LL);
    CHECK_EQ(gcd_euclid(0, 0), 0LL);
    CHECK_EQ(gcd_euclid(832040, 514229), 1LL);               // consecutive Fibonacci numbers: Euclid's worst case
    CHECK_EQ(lcm_safe(4, 6), 12LL);
    CHECK_EQ(lcm_safe(0, 5), 0LL);
    CHECK_EQ(lcm_safe(4'000'000'000LL, 6'000'000'000LL), 12'000'000'000LL);   // a * b alone would overflow
    for (int iter = 0; iter < 2000; iter++) {
        long long a = t::rand_int(0, 1'000'000'000), b = t::rand_int(0, 1'000'000'000);
        CHECK_EQ(gcd_euclid(a, b), gcd(a, b));
        if (a > 0 && b > 0) CHECK_EQ(lcm_safe(a, b), a / gcd(a, b) * b);
    }

    // Modular arithmetic.
    CHECK_EQ(-7 % 3, -1);                                    // the C++ rule that mod_norm fixes
    CHECK_EQ(mod_norm(-7, 3), 2LL);
    CHECK_EQ(mod_norm(7, 3), 1LL);
    CHECK_EQ(mod_pow(2, 10, 1000), 24LL);
    CHECK_EQ(mod_pow(5, 0, 7), 1LL);
    CHECK_EQ(mod_pow(5, 3, 1), 0LL);
    CHECK_EQ(mod_pow(-2, 3, 7), 6LL);                        // -8 = 6 (mod 7)
    CHECK_EQ(mod_pow(2, 1'000'000'006, MOD), 1LL);           // Fermat
    for (int iter = 0; iter < 1000; iter++) {
        long long base = t::rand_int(-1'000'000'000, 1'000'000'000);
        long long exp = t::rand_int(0, 60), m = t::rand_int(1, 2'000'000'000);
        long long naive = 1 % m;
        for (int i = 0; i < exp; i++) naive = naive * mod_norm(base, m) % m;
        CHECK_EQ(mod_pow(base, exp, m), naive);
    }

    // Inverses: Fermat for a prime modulus, extended Euclid for any modulus.
    for (int iter = 0; iter < 1000; iter++) {
        long long a = t::rand_int(1, MOD - 1);
        CHECK_EQ(mod_inv_prime(a, MOD) * a % MOD, 1LL);
        CHECK_EQ(mod_inv(a, MOD), mod_inv_prime(a, MOD));
        long long b = t::rand_int(0, 1'000'000'000), x, y;
        long long g = ext_gcd(a, b, x, y);
        CHECK_EQ(g, gcd(a, b));
        CHECK_EQ(a * x + b * y, g);                          // Bezout (|x|, |y| stay below max(a, b))
    }
    CHECK_EQ(mod_inv(3, 10), 7LL);                           // 3 * 7 = 21 = 1 (mod 10)
    CHECK_EQ(mod_inv(4, 10), -1LL);                          // gcd(4, 10) = 2: no inverse
    CHECK_EQ(mod_inv(-3, 10), 3LL);                          // -3 = 7 (mod 10), and 7 * 3 = 21
    for (int m = 1; m <= 60; m++) {
        for (int a = 0; a < m; a++) {
            long long expect = -1;
            for (int cand = 0; cand < m; cand++)
                if (a * cand % m == 1 % m) { expect = cand; break; }
            CHECK_EQ(mod_inv(a, m), expect);
        }
    }

    // nCr mod p against Pascal's triangle, with a big prime and with a small prime (n < p still).
    for (long long p : {MOD, 13LL}) {
        int maxn = (p == 13) ? 12 : 60;
        Binomial binom(maxn, p);
        vector<vector<long long>> pascal(maxn + 1, vector<long long>(maxn + 1, 0));
        for (int n = 0; n <= maxn; n++) {
            pascal[n][0] = 1 % p;
            for (int r = 1; r <= n; r++) pascal[n][r] = (pascal[n - 1][r - 1] + pascal[n - 1][r]) % p;
            for (int r = 0; r <= n; r++) CHECK_EQ(binom.C(n, r), pascal[n][r]);
            CHECK_EQ(binom.C(n, n + 1), 0LL);
            CHECK_EQ(binom.C(n, -1), 0LL);
        }
    }
    Binomial large(1'000'000, MOD);
    CHECK_EQ(large.C(1'000'000, 1), 1'000'000LL);
    CHECK_EQ(large.C(1'000'000, 999'999), 1'000'000LL);
    CHECK_EQ(large.C(10, 3), 120LL);
    return t::summary("number_theory");
}
