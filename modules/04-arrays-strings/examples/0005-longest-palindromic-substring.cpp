// 5. Longest Palindromic Substring: https://leetcode.com/problems/longest-palindromic-substring/
// Pattern: expand around each of the 2n - 1 centers. Module 04 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    string longestPalindrome(string s) {
        int n = (int)s.size(), bestStart = 0, bestLen = 0;
        // Grow outward while both ends match; return the length of the palindrome found.
        auto expand = [&](int l, int r) {
            while (l >= 0 && r < n && s[l] == s[r]) {
                l--;
                r++;
            }
            return r - l - 1;                  // the loop overshot by one on each side
        };
        for (int c = 0; c < n; c++) {
            int len = max(expand(c, c),        // odd length: centered on s[c]        "aba"
                          expand(c, c + 1));   // even length: centered between c, c+1 "abba"
            if (len > bestLen) {
                bestLen = len;
                bestStart = c - (len - 1) / 2; // works for both parities
            }
        }
        return s.substr(bestStart, bestLen);
    }
};
// [/snippet]

// Brute force: try every substring, longest first. O(n^3).
string brute(const string& s) {
    int n = (int)s.size();
    for (int len = n; len >= 1; len--)
        for (int start = 0; start + len <= n; start++) {
            string sub = s.substr(start, len);
            if (equal(sub.begin(), sub.end(), sub.rbegin())) return sub;
        }
    return "";
}

int main() {
    Solution sol;
    string ex1 = sol.longestPalindrome("babad");
    CHECK(ex1 == "bab" || ex1 == "aba");                  // either is accepted
    CHECK_EQ(sol.longestPalindrome("cbbd"), "bb");

    CHECK_EQ(sol.longestPalindrome("a"), "a");
    CHECK_EQ(sol.longestPalindrome("ac"), "a");             // no length-2 palindrome
    CHECK_EQ(sol.longestPalindrome("aaaa"), "aaaa");        // even, whole string
    CHECK_EQ(sol.longestPalindrome("racecar"), "racecar");  // odd, whole string
    CHECK_EQ(sol.longestPalindrome("abacdfgdcaba"), "aba"); // the reversed string shares "abacd": not a palindrome

    for (int iter = 0; iter < 300; iter++) {
        string s = t::rand_string((int)t::rand_int(1, 14), 'a', 'c');
        CHECK_EQ(sol.longestPalindrome(s), brute(s));       // both pick the leftmost longest
    }
    return t::summary("0005-longest-palindromic-substring");
}
