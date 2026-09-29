// Binary search on the answer: two more shapes, both built on templates/binary_search.hpp.
// Module 11 section 1.
//   1. Maximise: the largest x with x^3 <= n (last_true), with a predicate that can't overflow.
//   2. Real-valued: the monthly interest rate behind an EMI (first_true_real, fixed iterations).
#include <bits/stdc++.h>
#include "test.hpp"
#include "binary_search.hpp"
using namespace std;

// [snippet:cube_root]
// Largest x >= 0 with x^3 <= n, for 0 <= n <= 10^18. ok(x) = "x^3 <= n" is true...true false...false.
long long cubeRoot(long long n) {
    // hi = n is a lazy but safe bound (only ~60 halvings). The price: mid can be ~5 * 10^17, where
    // mid * mid * mid overflows long long (UB). Divide instead: for x >= 1, x^3 <= n  <=>  x <= n / x / x.
    auto ok = [&](long long x) { return x == 0 || x <= n / x / x; };
    return last_true(0LL, n, ok);            // first_true inside computes n + 1: fine for n <= 10^18
}
// [/snippet]

// [snippet:emi_rate]
// EMI (equated monthly instalment) for principal P at monthly rate r over m months:
//     emi(r) = P * r * (1 + r)^m / ((1 + r)^m - 1)
// emi grows with r, but there is no formula for r given the EMI: binary-search it.
double emi(double principal, double rate, int months) {
    if (rate == 0) return principal / months;
    double growthMinus1 = expm1(months * log1p(rate));    // (1 + r)^m - 1, accurate even for tiny r
    return principal * rate * (growthMinus1 + 1) / growthMinus1;
}

// The monthly rate whose EMI equals `payment`: the first r in [0, 1] with emi(r) >= payment.
// Assumes principal / months <= payment <= emi(1.0), i.e. a rate between 0% and 100% a month.
double monthlyRate(double principal, double payment, int months) {
    return first_true_real(0.0, 1.0, [&](double r) { return emi(principal, r, months) >= payment; });
}
// [/snippet]

long long bruteCubeRoot(long long n) {
    long long x = 0;
    while ((x + 1) * (x + 1) * (x + 1) <= n) x++;
    return x;
}

int main() {
    CHECK_EQ(cubeRoot(0), 0LL);
    CHECK_EQ(cubeRoot(1), 1LL);
    CHECK_EQ(cubeRoot(7), 1LL);
    CHECK_EQ(cubeRoot(8), 2LL);
    CHECK_EQ(cubeRoot(26), 2LL);
    CHECK_EQ(cubeRoot(27), 3LL);
    CHECK_EQ(cubeRoot(1'000'000'000'000'000'000LL), 1'000'000LL);        // 10^18, the maximum
    CHECK_EQ(cubeRoot(999'999'999'999'999'999LL), 999'999LL);
    CHECK_EQ(cubeRoot(999'997'000'002'999'999LL), 999'999LL);            // exactly 999999^3
    CHECK_EQ(cubeRoot(999'997'000'002'999'998LL), 999'998LL);
    for (int iter = 0; iter < 300; iter++) {                             // stress vs a linear scan
        long long n = t::rand_int(0, 2'000'000);
        CHECK_EQ(cubeRoot(n), bruteCubeRoot(n));
    }

    // EMI values checked against exact decimal arithmetic.
    CHECK_NEAR(emi(100000, 0.01, 12), 8884.878867834171, 1e-6);         // 1 lakh, 1%/month, 1 year
    CHECK_NEAR(emi(5000000, 0.0075, 240), 44986.29779250865, 1e-6);     // 50 lakh, 9%/year, 20 years
    CHECK_NEAR(emi(1200, 0.0, 12), 100.0, 1e-12);                       // 0%: principal / months

    // Recover the rate from the EMI.
    CHECK_NEAR(monthlyRate(100000, 8884.878867834171, 12), 0.01, 1e-9);
    CHECK_NEAR(monthlyRate(5000000, 44986.29779250865, 240), 0.0075, 1e-9);
    CHECK_NEAR(monthlyRate(1000000, 19332.80152942792, 60), 0.005, 1e-9);
    CHECK_NEAR(monthlyRate(1200, 100.0, 12), 0.0, 1e-9);                // tiny rates stay finite
    for (int iter = 0; iter < 200; iter++) {                            // round trip on random loans
        double principal = (double)t::rand_int(1'000, 10'000'000);
        double rate = t::rand_int(1, 5000) / 100000.0;                  // 0.001% .. 5% a month
        int months = (int)t::rand_int(1, 360);
        CHECK_NEAR(monthlyRate(principal, emi(principal, rate, months), months), rate, 1e-9);
    }
    return t::summary("answer_search_demos");
}
