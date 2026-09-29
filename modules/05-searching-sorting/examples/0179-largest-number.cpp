// 179. Largest Number: https://leetcode.com/problems/largest-number/
// Pattern: sort with a custom comparator (concatenation order). Module 05 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    string largestNumber(vector<int>& nums) {
        vector<string> parts;
        for (int x : nums) parts.push_back(to_string(x));
        // a goes before b iff "a then b" beats "b then a". Both concatenations have the same
        // length, so comparing the strings compares the numbers.
        sort(parts.begin(), parts.end(), [](const string& a, const string& b) {
            return a + b > b + a;
        });
        if (parts[0] == "0") return "0";     // the best first part is "0": every part is "0"
        string result;
        for (const string& p : parts) result += p;
        return result;
    }
};
// [/snippet]

// Brute force for the stress test: try every order (n <= 6), keep the largest string.
// All orders give strings of the same length, so the lexicographic max is the numeric max.
string brute(vector<int> nums) {
    vector<string> parts;
    for (int x : nums) parts.push_back(to_string(x));
    sort(parts.begin(), parts.end());
    string best;
    do {
        string s;
        for (auto& p : parts) s += p;
        best = max(best, s);
    } while (next_permutation(parts.begin(), parts.end()));
    size_t nonzero = best.find_first_not_of('0');
    return nonzero == string::npos ? "0" : best.substr(nonzero);
}

// The comparator, and the key it secretly sorts by: a before b iff a/(10^len(a) - 1) is larger.
// (a·10^len(b) + b > b·10^len(a) + a  <=>  a·(10^len(b) - 1) > b·(10^len(a) - 1).)
bool concat_before(int a, int b) { return to_string(a) + to_string(b) > to_string(b) + to_string(a); }
unsigned long long pow10_len(int x) {           // 10^(number of digits of x); x = 0 has one digit
    unsigned long long p = 10;
    while (x >= 10) { x /= 10; p *= 10; }
    return p;
}
bool key_before(int a, int b) {                  // a, b <= 10^9: every product fits in 64 bits
    return (unsigned long long)a * (pow10_len(b) - 1) > (unsigned long long)b * (pow10_len(a) - 1);
}

int main() {
    Solution sol;
    vector<int> ex1{10, 2}, ex2{3, 30, 34, 5, 9};
    CHECK_EQ(sol.largestNumber(ex1), "210");
    CHECK_EQ(sol.largestNumber(ex2), "9534330");

    vector<int> zeros{0, 0, 0}, zero{0}, ten{10}, tricky{432, 43243}, prefix{111311, 1113}, big{1000000000, 999999999};
    CHECK_EQ(sol.largestNumber(zeros), "0");          // not "000"
    CHECK_EQ(sol.largestNumber(zero), "0");
    CHECK_EQ(sol.largestNumber(ten), "10");
    CHECK_EQ(sol.largestNumber(tricky), "43243432");  // 43243|432 beats 432|43243
    CHECK_EQ(sol.largestNumber(prefix), "1113111311");
    CHECK_EQ(sol.largestNumber(big), "9999999991000000000");

    // The comparator is a strict weak ordering: it IS "sort by the key a/(10^len(a) - 1)".
    vector<int> pool{0, 1, 3, 9, 10, 11, 30, 33, 34, 99, 110, 121, 303, 330, 333, 1000000000, 999999999};
    for (int iter = 0; iter < 300; iter++) pool.push_back((int)t::rand_int(0, iter < 150 ? 500 : 1000000000));
    for (int iter = 0; iter < 3000; iter++) {
        int a = pool[t::rand_int(0, (int)pool.size() - 1)];
        int b = pool[t::rand_int(0, (int)pool.size() - 1)];
        int c = pool[t::rand_int(0, (int)pool.size() - 1)];
        CHECK_EQ(concat_before(a, b), key_before(a, b));
        CHECK(!concat_before(a, a));                                            // irreflexive
        CHECK(!(concat_before(a, b) && concat_before(b, a)));                   // asymmetric
        if (concat_before(a, b) && concat_before(b, c)) CHECK(concat_before(a, c));  // transitive
    }

    for (int iter = 0; iter < 300; iter++) {        // stress test vs trying every order
        int len = (int)t::rand_int(1, 6);
        vector<int> v = t::rand_vec(len, 0, iter % 3 == 0 ? 3 : 1000);
        CHECK_EQ(sol.largestNumber(v), brute(v));
    }
    return t::summary("0179-largest-number");
}
