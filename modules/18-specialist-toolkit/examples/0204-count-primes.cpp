// 204. Count Primes: https://leetcode.com/problems/count-primes/
// Pattern: sieve of Eratosthenes, crossing out from i * i. Module 18 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int countPrimes(int n) {                          // primes strictly less than n
        if (n < 3) return 0;
        vector<bool> composite(n, false);             // index = number; n bits, not n ints
        for (long long i = 2; i * i < n; i++) {       // long long: i * i must not overflow
            if (composite[i]) continue;
            for (long long j = i * i; j < n; j += i)  // smaller multiples of i are already crossed out
                composite[j] = true;
        }
        int count = 0;
        for (int i = 2; i < n; i++) count += !composite[i];
        return count;
    }
};
// [/snippet]

// Brute force: trial division up to sqrt(x).
int brute(int n) {
    int count = 0;
    for (int x = 2; x < n; x++) {
        bool prime = true;
        for (int d = 2; d * d <= x && prime; d++) prime = x % d != 0;
        count += prime;
    }
    return count;
}

int main() {
    Solution sol;
    CHECK_EQ(sol.countPrimes(10), 4);                 // 2, 3, 5, 7
    CHECK_EQ(sol.countPrimes(0), 0);
    CHECK_EQ(sol.countPrimes(1), 0);
    CHECK_EQ(sol.countPrimes(2), 0);                  // strictly less than 2: none
    CHECK_EQ(sol.countPrimes(3), 1);
    CHECK_EQ(sol.countPrimes(1'000'000), 78498);      // pi(10^6)
    CHECK_EQ(sol.countPrimes(5'000'000), 348513);     // the largest n allowed

    for (int n = 0; n <= 2000; n++) CHECK_EQ(sol.countPrimes(n), brute(n));
    return t::summary("0204-count-primes");
}
