// 76. Minimum Window Substring: https://leetcode.com/problems/minimum-window-substring/
// Pattern: shortest window with counts ("formed" counter + shrink loop). Module 06 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    string minWindow(string s, string t) {         // constraints: s and t are non-empty
        int need[256] = {}, have[256] = {};
        int required = 0;                          // distinct characters in t
        for (unsigned char c : t)
            if (need[c]++ == 0) required++;
        int formed = 0;                            // distinct characters whose need is met by s[l..r]
        int bestStart = 0, bestLen = INT_MAX;
        for (int l = 0, r = 0; r < (int)s.size(); r++) {
            unsigned char c = s[r];
            if (++have[c] == need[c]) formed++;    // c just reached its required count
            while (formed == required) {           // valid: record it, then try a shorter one
                if (r - l + 1 < bestLen) {
                    bestStart = l;
                    bestLen = r - l + 1;
                }
                unsigned char d = s[l++];
                if (have[d]-- == need[d]) formed--; // d just dropped below its required count
            }
        }
        return bestLen == INT_MAX ? "" : s.substr(bestStart, bestLen);
    }
};
// [/snippet]

// Brute force: try every length from short to long, every start from left to right, and check the
// counts. Returns the leftmost shortest window, which is also what the window solution returns.
string brute(const string& s, const string& t) {
    int n = (int)s.size();
    for (int len = 1; len <= n; len++)
        for (int start = 0; start + len <= n; start++) {
            int cnt[256] = {};
            for (int k = start; k < start + len; k++) cnt[(unsigned char)s[k]]++;
            bool ok = true;
            for (unsigned char c : t) ok = ok && cnt[c]-- > 0;
            if (ok) return s.substr(start, len);
        }
    return "";
}

int main() {
    Solution sol;
    CHECK_EQ(sol.minWindow("ADOBECODEBANC", "ABC"), "BANC");
    CHECK_EQ(sol.minWindow("a", "a"), "a");
    CHECK_EQ(sol.minWindow("a", "aa"), "");                  // t needs two a's

    CHECK_EQ(sol.minWindow("ab", "b"), "b");
    CHECK_EQ(sol.minWindow("abc", "cba"), "abc");            // the whole string
    CHECK_EQ(sol.minWindow("aaflslflsldkalskaaa", "aaa"), "aaa");
    CHECK_EQ(sol.minWindow("xyz", "Z"), "");                 // case matters

    for (int iter = 0; iter < 300; iter++) {
        string s = t::rand_string((int)t::rand_int(1, 12), 'a', 'd');
        string pattern = t::rand_string((int)t::rand_int(1, 4), 'a', 'd');
        CHECK_EQ(sol.minWindow(s, pattern), brute(s, pattern));
    }
    return t::summary("0076-minimum-window-substring");
}
