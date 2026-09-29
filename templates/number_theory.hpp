// Number theory for interviews: sieves, factorization, gcd / lcm, modular arithmetic, modular
// inverses, nCr mod p. Module 18 section 3. (Free functions are `inline` because they live in a header.)
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:sieve]
// Sieve of Eratosthenes: is_prime[i] for 0 <= i <= n (n up to ~1e8), O(n log log n).
// Crossing out starts at i * i: a smaller multiple i * j with j < i has a prime factor below i, so it
// is already crossed out. For the same reason the outer loop can stop once i * i > n.
inline vector<bool> prime_sieve(int n) {
    vector<bool> is_prime(n + 1, true);
    is_prime[0] = false;
    if (n >= 1) is_prime[1] = false;
    for (int i = 2; (long long)i * i <= n; i++)
        if (is_prime[i])
            for (int j = i * i; j <= n; j += i) is_prime[j] = false;
    return is_prime;
}
// [/snippet]

// [snippet:spf_sieve]
// Linear sieve: the smallest prime factor of every x <= n, plus the list of primes, in O(n).
// Every composite c is written exactly once, as c = p * i with p = spf(c), which forces p <= spf(i).
struct Sieve {
    vector<int> spf;                              // spf[x] = smallest prime dividing x, for 2 <= x <= n
    vector<int> primes;

    explicit Sieve(int n) : spf(max(n + 1, 2), 0) {
        for (int i = 2; i <= n; i++) {
            if (spf[i] == 0) {                    // nothing smaller divides i: prime
                spf[i] = i;
                primes.push_back(i);
            }
            for (int p : primes) {                // mark p * i for each prime p <= spf(i)
                if (p > spf[i] || (long long)p * i > n) break;
                spf[p * i] = p;
            }
        }
    }

    bool is_prime(int x) const { return x >= 2 && spf[x] == x; }

    // (prime, exponent) pairs, smallest prime first, for 1 <= x <= n.
    // O(log x): every division at least halves x.
    vector<pair<int, int>> factorize(int x) const {
        vector<pair<int, int>> factors;
        while (x > 1) {
            int p = spf[x], e = 0;
            while (x % p == 0) {
                x /= p;
                e++;
            }
            factors.push_back({p, e});
        }
        return factors;
    }
};
// [/snippet]

// [snippet:gcd]
// Euclid: gcd(a, b) = gcd(b, a % b), since a % b = a - q*b keeps exactly the same common divisors.
// a % b < a / 2 whenever b <= a, so the numbers halve every two steps: O(log min(a, b)).
// std::gcd and std::lcm (<numeric>) do this for you; inputs here are >= 0.
inline long long gcd_euclid(long long a, long long b) {
    while (b != 0) {
        long long r = a % b;
        a = b;
        b = r;
    }
    return a;
}

// Divide before multiplying: a * b can overflow even when the lcm itself fits in 64 bits.
inline long long lcm_safe(long long a, long long b) {
    if (a == 0 || b == 0) return 0;
    return a / gcd_euclid(a, b) * b;
}
// [/snippet]

// [snippet:mod_pow]
// C++'s % keeps the sign of the dividend: -7 % 3 == -1, not 2. Normalize before using a residue.
inline long long mod_norm(long long a, long long m) { return (a % m + m) % m; }

// base^exp mod m by repeated squaring. Write exp in binary: base^exp is the product of base^(2^k)
// over exp's set bits, and each base^(2^k) is the square of the previous one. O(log exp).
// Products of two residues must fit in long long, so m must stay below ~3e9 (1e9 + 7 is fine).
inline long long mod_pow(long long base, long long exp, long long m) {
    long long result = 1 % m;                     // 1 % m makes m == 1 return 0
    base = mod_norm(base, m);
    while (exp > 0) {
        if (exp & 1) result = result * base % m;  // this bit is set: multiply its square in
        base = base * base % m;                   // base^(2^k) -> base^(2^(k+1))
        exp >>= 1;
    }
    return result;
}
// [/snippet]

// [snippet:mod_inv]
// Fermat: for prime p and a not divisible by p, a^(p-1) = 1 (mod p), so a * a^(p-2) = 1 (mod p).
inline long long mod_inv_prime(long long a, long long p) { return mod_pow(a, p - 2, p); }

// Extended Euclid: returns g = gcd(a, b) and sets x, y with a*x + b*y = g (Bezout), for a, b >= 0.
inline long long ext_gcd(long long a, long long b, long long& x, long long& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    long long x1, y1;
    long long g = ext_gcd(b, a % b, x1, y1);      // b*x1 + (a % b)*y1 = g
    x = y1;                                       // put a % b = a - (a / b)*b in, regroup by a and b
    y = x1 - (a / b) * y1;
    return g;
}

// Inverse of a modulo any m >= 1, prime or not. It exists iff gcd(a, m) == 1; returns -1 otherwise.
inline long long mod_inv(long long a, long long m) {
    long long x, y;
    if (ext_gcd(mod_norm(a, m), m, x, y) != 1) return -1;
    return mod_norm(x, m);                        // x can be negative
}
// [/snippet]

// [snippet:binomial]
// nCr mod a prime p, for n <= maxn < p: O(maxn + log p) setup, then O(1) per query.
//   C(n, r) = n! / (r! (n-r)!) = fact[n] * inv_fact[r] * inv_fact[n-r]   (mod p)
// One Fermat inverse for maxn!, then walk down with 1/(i-1)! = (1/i!) * i.
struct Binomial {
    long long p;
    vector<long long> fact, inv_fact;

    Binomial(int maxn, long long prime) : p(prime), fact(maxn + 1), inv_fact(maxn + 1) {
        fact[0] = 1;
        for (int i = 1; i <= maxn; i++) fact[i] = fact[i - 1] * i % p;
        inv_fact[maxn] = mod_inv_prime(fact[maxn], p);
        for (int i = maxn; i >= 1; i--) inv_fact[i - 1] = inv_fact[i] * i % p;
    }

    long long C(int n, int r) const {
        if (r < 0 || r > n) return 0;
        return fact[n] * inv_fact[r] % p * inv_fact[n - r] % p;
    }
};
// [/snippet]
