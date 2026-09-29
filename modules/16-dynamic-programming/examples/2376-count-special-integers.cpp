// 2376. Count Special Integers: https://leetcode.com/problems/count-special-integers/
// Pattern: digit DP on (pos, tight, started, used-digit mask). Module 16 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
    string digits;        // n in decimal, most significant digit first
    vector<int> memo;     // memo[state index] = ways, -1 = not computed yet

    // Ways to fill positions pos.. so that the whole number is <= n and its digits are distinct.
    //   tight:   the digits so far equal n's prefix, so this digit may not exceed digits[pos]
    //   started: a non-zero digit has been placed (leading zeros are not real digits)
    //   used:    bitmask of the real digits placed so far
    int countFrom(int pos, bool tight, bool started, int used) {
        if (pos == (int)digits.size()) return started ? 1 : 0;   // "all zeros" is 0, which isn't in [1, n]
        int& cached = memo[((pos * 2 + tight) * 2 + started) * 1024 + used];
        if (cached != -1) return cached;
        int limit = tight ? digits[pos] - '0' : 9;
        int ways = 0;
        for (int d = 0; d <= limit; d++) {
            bool nextTight = tight && d == limit;
            if (!started && d == 0)
                ways += countFrom(pos + 1, nextTight, false, used);         // another leading zero
            else if (!(used >> d & 1))
                ways += countFrom(pos + 1, nextTight, true, used | 1 << d);  // a real digit, still unused
        }
        return cached = ways;
    }

public:
    int countSpecialNumbers(int n) {
        digits = to_string(n);
        memo.assign(digits.size() * 2 * 2 * 1024, -1);   // fresh memo: pos is counted from n's first digit
        return countFrom(0, true, false, 0);
    }
};
// [/snippet]

bool distinctDigits(int x) {
    int seen = 0;
    for (; x > 0; x /= 10) {
        if (seen >> (x % 10) & 1) return false;
        seen |= 1 << (x % 10);
    }
    return true;
}

int main() {
    Solution sol;
    CHECK_EQ(sol.countSpecialNumbers(20), 19);          // official examples
    CHECK_EQ(sol.countSpecialNumbers(5), 5);
    CHECK_EQ(sol.countSpecialNumbers(135), 110);

    CHECK_EQ(sol.countSpecialNumbers(1), 1);            // edge cases
    CHECK_EQ(sol.countSpecialNumbers(10), 10);          // 10 itself is special
    CHECK_EQ(sol.countSpecialNumbers(11), 10);          // 11 is not
    CHECK_EQ(sol.countSpecialNumbers(2'000'000'000), 5974650);   // largest n (checked by combinatorics)
    CHECK_EQ(sol.countSpecialNumbers(987654321), 5611770);        // every special number with <= 9 digits

    // Brute force: a prefix count of special numbers, then compare every n up to 3000
    // and random n up to 200000.
    const int LIMIT = 200000;
    vector<int> upTo(LIMIT + 1, 0);                     // upTo[x] = special integers in [1, x]
    for (int x = 1; x <= LIMIT; x++) upTo[x] = upTo[x - 1] + distinctDigits(x);
    for (int n = 1; n <= 3000; n++) CHECK_EQ(sol.countSpecialNumbers(n), upTo[n]);
    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(1, LIMIT);
        CHECK_EQ(sol.countSpecialNumbers(n), upTo[n]);
    }
    return t::summary("2376-count-special-integers");
}
