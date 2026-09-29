// 421. Maximum XOR of Two Numbers in an Array: https://leetcode.com/problems/maximum-xor-of-two-numbers-in-an-array/
// Pattern: bitwise trie, greedy from the top bit. Module 18 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int findMaximumXOR(vector<int>& nums) {
        const int B = 31;                                 // 0 <= nums[i] < 2^31: bits 30..0
        vector<array<int, 2>> child(1, {0, 0});           // node 0 = root; child id 0 = "none"
        child.reserve(1 + nums.size() * B);               // pool: at most 1 + 31n nodes, no reallocation
        int best = 0;
        for (int x : nums) {
            int v = 0;                                    // 1) insert x
            for (int b = B - 1; b >= 0; b--) {
                int bit = (x >> b) & 1;
                if (child[v][bit] == 0) {
                    child[v][bit] = (int)child.size();
                    child.push_back({0, 0});
                }
                v = child[v][bit];
            }
            int u = 0, cur = 0;                           // 2) best partner for x among values so far
            for (int b = B - 1; b >= 0; b--) {            //    (x itself is there: x ^ x = 0 is a floor)
                int want = ((x >> b) & 1) ^ 1;            // the opposite bit sets bit b of the XOR
                if (child[u][want] != 0) {
                    cur |= 1 << b;
                    u = child[u][want];
                } else {
                    u = child[u][want ^ 1];
                }
            }
            best = max(best, cur);
        }
        return best;
    }
};
// [/snippet]

// Brute force for the stress test: every pair, O(n^2).
int brute(const vector<int>& nums) {
    int best = 0;
    for (size_t i = 0; i < nums.size(); i++)
        for (size_t j = i; j < nums.size(); j++) best = max(best, nums[i] ^ nums[j]);
    return best;
}

int main() {
    Solution sol;
    vector<int> ex1{3, 10, 5, 25, 2, 8};
    vector<int> ex2{14, 70, 53, 83, 49, 91, 36, 80, 92, 51, 66, 70};
    CHECK_EQ(sol.findMaximumXOR(ex1), 28);                // 5 ^ 25
    CHECK_EQ(sol.findMaximumXOR(ex2), 127);

    vector<int> one{7}, same{9, 9, 9}, extremes{0, INT_MAX}, top{1 << 30, (1 << 30) - 1};
    CHECK_EQ(sol.findMaximumXOR(one), 0);                 // a single number pairs with itself
    CHECK_EQ(sol.findMaximumXOR(same), 0);
    CHECK_EQ(sol.findMaximumXOR(extremes), INT_MAX);
    CHECK_EQ(sol.findMaximumXOR(top), INT_MAX);           // bit 30 alone vs bits 29..0

    // The largest input: n = 2e5 random values in the full range must stay fast.
    vector<int> big = t::rand_vec(200000, 0, INT_MAX);
    int big_best = sol.findMaximumXOR(big);
    CHECK(big_best >= (1 << 30));                         // values straddle bit 30, so the best XOR has it

    // Stress against the O(n^2) brute force: small values (shared prefixes) and full-range values.
    for (int iter = 0; iter < 300; iter++) {
        int hi = (iter % 2) ? INT_MAX : 31;
        vector<int> v = t::rand_vec((int)t::rand_int(1, 30), 0, hi);
        CHECK_EQ(sol.findMaximumXOR(v), brute(v));
    }
    return t::summary("0421-maximum-xor-of-two-numbers-in-an-array");
}
