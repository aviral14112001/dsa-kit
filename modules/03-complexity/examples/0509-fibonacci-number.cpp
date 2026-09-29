// 509. Fibonacci Number: https://leetcode.com/problems/fibonacci-number/
// Pattern: recursion tree -> memoization -> two variables; count the calls to see the cost. Module 03 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

long long calls = 0;   // every version bumps this on entry, so main() can measure the work

// [snippet:naive]
int fib_naive(int n) {                  // T(n) = T(n-1) + T(n-2) + O(1): Θ(φ^n) calls, φ ≈ 1.618
    calls++;
    if (n < 2) return n;
    return fib_naive(n - 1) + fib_naive(n - 2);
}
// [/snippet]

// [snippet:memo]
int fib_memo(int n, vector<int>& memo) {   // n + 1 distinct states × O(1) work each = Θ(n)
    calls++;
    if (n < 2) return n;
    if (memo[n] != -1) return memo[n];     // solved before: answer without recursing
    return memo[n] = fib_memo(n - 1, memo) + fib_memo(n - 2, memo);
}
// [/snippet]

// [snippet:solution]
class Solution {
public:
    int fib(int n) {
        if (n < 2) return n;
        int prev = 0, cur = 1;             // F(0), F(1)
        for (int i = 2; i <= n; i++) {     // after each step: cur = F(i), prev = F(i - 1)
            int sum = prev + cur;
            prev = cur;
            cur = sum;
        }
        return cur;
    }
};
// [/snippet]

int main() {
    Solution sol;
    CHECK_EQ(sol.fib(2), 1);               // the official examples
    CHECK_EQ(sol.fib(3), 2);
    CHECK_EQ(sol.fib(4), 3);
    CHECK_EQ(sol.fib(0), 0);               // base cases
    CHECK_EQ(sol.fib(1), 1);
    CHECK_EQ(sol.fib(30), 832'040);        // the constraint's maximum
    CHECK_EQ(sol.fib(46), 1'836'311'903);  // the largest Fibonacci number that fits in an int

    // [snippet:count_calls]
    calls = 0;
    CHECK_EQ(fib_naive(20), 6765);
    CHECK_EQ(calls, 21891);                // = 2·F(21) − 1: the recursion tree has 21891 nodes

    calls = 0;
    vector<int> memo(21, -1);
    CHECK_EQ(fib_memo(20, memo), 6765);
    CHECK_EQ(calls, 39);                   // = 2·20 − 1: states 2..20 expand once (19 calls); the other 20 return at once
    // [/snippet]

    // The whole input range (0..30) is tiny, so test all of it instead of sampling: the three
    // versions agree, and the call counts follow their formulas exactly.
    for (int n = 0; n <= 30; n++) {
        calls = 0;
        int naive = fib_naive(n);
        long long naive_calls = calls;

        calls = 0;
        vector<int> table(n + 1, -1);
        int memoized = fib_memo(n, table);
        long long memo_calls = calls;

        CHECK_EQ(sol.fib(n), naive);
        CHECK_EQ(memoized, naive);
        CHECK_EQ(naive_calls, 2LL * sol.fib(n + 1) - 1);
        CHECK_EQ(memo_calls, max(1, 2 * n - 1));
    }
    return t::summary("0509-fibonacci-number");
}
