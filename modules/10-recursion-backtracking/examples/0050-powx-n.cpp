// 50. Pow(x, n): https://leetcode.com/problems/powx-n/
// Pattern: halving recursion (x^n from x^(n/2)), then the same idea as a loop over the bits of n.
// Module 10 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace recursive {
// [snippet:solution]
class Solution {
public:
    double myPow(double x, int n) {
        long long N = n;                   // -n overflows int when n == INT_MIN; -N is fine in 64 bits
        if (N < 0) {
            x = 1 / x;
            N = -N;
        }
        return power(x, N);
    }

private:
    // x^n for n >= 0. Leap of faith: assume power(x, n / 2) is correct, and build x^n from it.
    double power(double x, long long n) {
        if (n == 0) return 1.0;            // base case: every call chain ends here
        double half = power(x, n / 2);     // ONE recursive call, reused: that's what makes it O(log n)
        return n % 2 == 0 ? half * half : half * half * x;
    }
};
// [/snippet]
}  // namespace recursive

namespace iterative {
// [snippet:iterative]
class Solution {
public:
    double myPow(double x, int n) {
        long long N = n;
        if (N < 0) {
            x = 1 / x;
            N = -N;
        }
        double result = 1.0;
        // Invariant: result * x^N == the answer. Each step moves the lowest bit of N into result.
        while (N > 0) {
            if (N & 1) result *= x;        // this bit is set: the current power of x is a factor
            x *= x;                        // x, x^2, x^4, x^8, ...
            N >>= 1;
        }
        return result;
    }
};
// [/snippet]
}  // namespace iterative

// Brute force: |n| multiplications. Obviously correct, O(|n|); only for small n.
double brute(double x, int n) {
    double r = 1.0;
    for (int i = 0; i < abs(n); i++) r *= x;
    return n < 0 ? 1 / r : r;
}

// Doubles: compare with a relative tolerance, since results range from 1e-9 to 1e4.
double tol(double expected) { return 1e-9 * max(1.0, fabs(expected)); }

template <class S>
void run(S& sol) {
    CHECK_NEAR(sol.myPow(2.0, 10), 1024.0, tol(1024.0));   // the official examples
    CHECK_NEAR(sol.myPow(2.1, 3), 9.261, tol(9.261));
    CHECK_NEAR(sol.myPow(2.0, -2), 0.25, tol(0.25));
    CHECK_NEAR(sol.myPow(5.0, 0), 1.0, tol(1.0));          // n = 0
    CHECK_NEAR(sol.myPow(0.0, 7), 0.0, tol(0.0));          // x = 0 (allowed when n > 0)
    CHECK_NEAR(sol.myPow(-2.0, 3), -8.0, tol(8.0));        // negative base, odd power
    CHECK_NEAR(sol.myPow(-2.0, -3), -0.125, tol(0.125));
    CHECK_NEAR(sol.myPow(1.0, INT_MIN), 1.0, tol(1.0));    // the INT_MIN trap
    CHECK_NEAR(sol.myPow(-1.0, INT_MIN), 1.0, tol(1.0));   // INT_MIN is even
    CHECK_NEAR(sol.myPow(-1.0, INT_MAX), -1.0, tol(1.0));  // INT_MAX is odd
    CHECK_NEAR(sol.myPow(2.0, INT_MIN), 0.0, 1e-300);      // underflows to 0, no crash, no hang
    CHECK_NEAR(sol.myPow(1.00001, 123456), pow(1.00001, 123456), tol(pow(1.00001, 123456)));

    for (int iter = 0; iter < 300; iter++) {               // stress test vs brute force
        double x = t::rand_int(-2000, 2000) / 1000.0;
        int n = (int)t::rand_int(-20, 20);
        if (x == 0 && n <= 0) continue;                     // excluded by the constraints
        double expected = brute(x, n);
        CHECK_NEAR(sol.myPow(x, n), expected, tol(expected));
    }
}

int main() {
    recursive::Solution rec;
    iterative::Solution it;
    run(rec);
    run(it);
    return t::summary("0050-powx-n");
}
