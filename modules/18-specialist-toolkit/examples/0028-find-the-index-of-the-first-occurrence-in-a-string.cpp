// 28. Find the Index of the First Occurrence in a String: https://leetcode.com/problems/find-the-index-of-the-first-occurrence-in-a-string/
// Pattern: naive O(n*m) scan, then KMP with the prefix function, O(n + m). Module 18 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace naive {
// [snippet:brute]
class Solution {
public:
    int strStr(string haystack, string needle) {
        int n = (int)haystack.size(), m = (int)needle.size();
        for (int i = 0; i + m <= n; i++) {            // try every alignment
            int k = 0;
            while (k < m && haystack[i + k] == needle[k]) k++;
            if (k == m) return i;                     // on a mismatch, all k matched chars are thrown away
        }
        return -1;
    }
};
// [/snippet]
}  // namespace naive

// [snippet:solution]
class Solution {
public:
    int strStr(string haystack, string needle) {
        int n = (int)haystack.size(), m = (int)needle.size();
        if (m == 0) return 0;
        vector<int> pi(m, 0);                         // pi[i] = longest proper border of needle[0..i]
        for (int i = 1; i < m; i++) {
            int k = pi[i - 1];
            while (k > 0 && needle[i] != needle[k]) k = pi[k - 1];
            if (needle[i] == needle[k]) k++;
            pi[i] = k;
        }
        for (int i = 0, k = 0; i < n; i++) {          // k = chars of needle matched, ending at haystack[i]
            while (k > 0 && haystack[i] != needle[k]) k = pi[k - 1];   // keep the longest border
            if (haystack[i] == needle[k]) k++;
            if (k == m) return i - m + 1;
        }
        return -1;
    }
};
// [/snippet]

int main() {
    Solution kmp;
    naive::Solution brute;
    CHECK_EQ(kmp.strStr("sadbutsad", "sad"), 0);
    CHECK_EQ(kmp.strStr("leetcode", "leeto"), -1);
    CHECK_EQ(brute.strStr("sadbutsad", "sad"), 0);
    CHECK_EQ(brute.strStr("leetcode", "leeto"), -1);

    // Edge cases: needle longer than haystack, whole-string match, match at the very end,
    // and the fallback that naive restarts get wrong if they skip ahead too far.
    CHECK_EQ(kmp.strStr("a", "aa"), -1);
    CHECK_EQ(kmp.strStr("abc", "abc"), 0);
    CHECK_EQ(kmp.strStr("xxab", "ab"), 2);
    CHECK_EQ(kmp.strStr("aaab", "aab"), 1);
    CHECK_EQ(kmp.strStr("abababca", "ababca"), 2);

    // The input that makes the naive scan slow (1e4 'a's vs "a...ab": ~(n - m) * m comparisons);
    // KMP stays linear on it.
    string hay(10000, 'a'), needle(5000, 'a');
    needle.back() = 'b';
    CHECK_EQ(kmp.strStr(hay, needle), -1);
    CHECK_EQ(kmp.strStr(hay + "b", needle), 5001);

    // Stress: both against std::string::find on tiny alphabets (lots of partial matches).
    for (int iter = 0; iter < 500; iter++) {
        string h = t::rand_string((int)t::rand_int(1, 30), 'a', 'b');
        string nd = t::rand_string((int)t::rand_int(1, 5), 'a', 'b');
        size_t pos = h.find(nd);
        int expect = pos == string::npos ? -1 : (int)pos;
        CHECK_EQ(kmp.strStr(h, nd), expect);
        CHECK_EQ(brute.strStr(h, nd), expect);
    }
    return t::summary("0028-find-the-index-of-the-first-occurrence-in-a-string");
}
