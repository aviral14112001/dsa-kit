// 137. Single Number II: https://leetcode.com/problems/single-number-ii/
// Pattern: count each bit mod 3, then the same counter as a two-variable state machine. Module 18 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace bit_count {
// [snippet:bit_count]
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        unsigned result = 0;
        for (int b = 0; b < 32; b++) {
            int ones = 0;
            for (int x : nums) ones += ((unsigned)x >> b) & 1;   // unsigned: well-defined for negatives
            if (ones % 3 != 0) result |= 1u << b;               // triples add a multiple of 3 to every bit
        }
        return (int)result;
    }
};
// [/snippet]
}  // namespace bit_count

// [snippet:solution]
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        // Per bit, (twos, ones) holds that bit's count mod 3 in binary: 00 -> 01 -> 10 -> 00.
        // These two lines are that 3-state counter, run on all 32 bits at once.
        int ones = 0, twos = 0;
        for (int x : nums) {
            ones = (ones ^ x) & ~twos;         // flip "ones" where x has a 1, unless the count is at 2
            twos = (twos ^ x) & ~ones;         // uses the NEW ones: 01 + 1 -> 10, and 10 + 1 -> 00
        }
        return ones;                           // bits whose count is 1 (mod 3): the single number
    }
};
// [/snippet]

int main() {
    Solution sol;
    bit_count::Solution counting;
    vector<int> ex1{2, 2, 3, 2}, ex2{0, 1, 0, 1, 0, 1, 99};
    CHECK_EQ(sol.singleNumber(ex1), 3);
    CHECK_EQ(sol.singleNumber(ex2), 99);
    CHECK_EQ(counting.singleNumber(ex1), 3);
    CHECK_EQ(counting.singleNumber(ex2), 99);

    // Edge cases: a lone element, negatives, and the extremes of int.
    vector<int> lone{-5}, neg{-2, -2, 1, -2}, mins{INT_MIN, 7, INT_MIN, 7, INT_MIN, 7, INT_MAX};
    vector<int> single_min{INT_MIN, 4, 4, 4};
    CHECK_EQ(sol.singleNumber(lone), -5);
    CHECK_EQ(sol.singleNumber(neg), 1);
    CHECK_EQ(sol.singleNumber(mins), INT_MAX);
    CHECK_EQ(sol.singleNumber(single_min), INT_MIN);
    CHECK_EQ(counting.singleNumber(neg), 1);
    CHECK_EQ(counting.singleNumber(single_min), INT_MIN);

    // Stress: random triples plus one single, shuffled. The brute force is a frequency map.
    for (int iter = 0; iter < 300; iter++) {
        int groups = (int)t::rand_int(0, 12);
        vector<int> v;
        for (int g = 0; g < groups; g++) {
            int x = (int)t::rand_int(INT_MIN, INT_MAX);
            v.insert(v.end(), {x, x, x});
        }
        v.push_back((int)t::rand_int(INT_MIN, INT_MAX));
        shuffle(v.begin(), v.end(), t::rng());
        map<int, int> freq;
        for (int x : v) freq[x]++;
        int expect = 0;
        for (auto [x, c] : freq)
            if (c % 3 == 1) expect = x;          // the single (a triple may reuse its value: 4 copies)
        CHECK_EQ(sol.singleNumber(v), expect);
        CHECK_EQ(counting.singleNumber(v), expect);
    }
    return t::summary("0137-single-number-ii");
}
