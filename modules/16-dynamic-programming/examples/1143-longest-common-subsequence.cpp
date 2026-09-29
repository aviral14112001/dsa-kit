// 1143. Longest Common Subsequence: https://leetcode.com/problems/longest-common-subsequence/
// Pattern: two-sequence table over prefixes; one-row version; reconstruction by walking back. Module 16 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int longestCommonSubsequence(string text1, string text2) {
        int n = text1.size(), m = text2.size();
        // lcs[i][j] = LCS length of the prefixes text1[0..i) and text2[0..j); row 0 / column 0: empty prefix
        vector<vector<int>> lcs(n + 1, vector<int>(m + 1, 0));
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= m; j++)
                if (text1[i - 1] == text2[j - 1])
                    lcs[i][j] = lcs[i - 1][j - 1] + 1;                 // last chars match: pair them up
                else
                    lcs[i][j] = max(lcs[i - 1][j], lcs[i][j - 1]);     // one of the two last chars is unused
        return lcs[n][m];
    }
};
// [/snippet]

namespace rolling {
// [snippet:rolling]
class Solution {
public:
    int longestCommonSubsequence(string text1, string text2) {
        if (text1.size() < text2.size()) swap(text1, text2);   // the row spans the shorter string
        int m = text2.size();
        vector<int> row(m + 1, 0);               // row[j] = lcs[i][j] for the current i
        for (char c : text1) {
            int diag = 0;                        // lcs[i-1][j-1]: saved before row[j-1] was overwritten
            for (int j = 1; j <= m; j++) {
                int up = row[j];                 // lcs[i-1][j], about to be overwritten
                row[j] = (c == text2[j - 1]) ? diag + 1 : max(up, row[j - 1]);
                diag = up;
            }
        }
        return row[m];
    }
};
// [/snippet]
}  // namespace rolling

// [snippet:reconstruct]
// One LCS as a string: fill the same table, then walk back from (n, m).
string lcsString(const string& a, const string& b) {
    int n = a.size(), m = b.size();
    vector<vector<int>> lcs(n + 1, vector<int>(m + 1, 0));
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= m; j++)
            lcs[i][j] = a[i - 1] == b[j - 1] ? lcs[i - 1][j - 1] + 1 : max(lcs[i - 1][j], lcs[i][j - 1]);
    string out;
    for (int i = n, j = m; i > 0 && j > 0;) {
        if (a[i - 1] == b[j - 1]) { out += a[i - 1]; i--; j--; }    // this pair is in the LCS
        else if (lcs[i - 1][j] >= lcs[i][j - 1]) i--;               // go where the value came from
        else j--;
    }
    reverse(out.begin(), out.end());
    return out;
}
// [/snippet]

bool isSubsequence(const string& small, const string& big) {
    size_t k = 0;
    for (char c : big)
        if (k < small.size() && small[k] == c) k++;
    return k == small.size();
}

// Brute force straight from the definition: every subsequence of a (2^n), keep the longest one inside b.
int brute(const string& a, const string& b) {
    int n = a.size(), best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        string sub;
        for (int i = 0; i < n; i++)
            if (mask >> i & 1) sub += a[i];
        if ((int)sub.size() > best && isSubsequence(sub, b)) best = sub.size();
    }
    return best;
}

int main() {
    Solution sol;
    rolling::Solution roll;
    CHECK_EQ(sol.longestCommonSubsequence("abcde", "ace"), 3);   // official examples
    CHECK_EQ(sol.longestCommonSubsequence("abc", "abc"), 3);
    CHECK_EQ(sol.longestCommonSubsequence("abc", "def"), 0);
    CHECK_EQ(roll.longestCommonSubsequence("abcde", "ace"), 3);
    CHECK_EQ(roll.longestCommonSubsequence("ace", "abcde"), 3);  // swapped: the row takes the shorter one
    CHECK_EQ(lcsString("abcde", "ace"), "ace");

    CHECK_EQ(sol.longestCommonSubsequence("", "abc"), 0);        // empty prefix (outside the constraints)
    CHECK_EQ(roll.longestCommonSubsequence("abc", ""), 0);
    CHECK_EQ(lcsString("", "xyz"), "");
    CHECK_EQ(sol.longestCommonSubsequence("bl", "yby"), 1);
    string longA(1000, 'a'), longB(1000, 'a');
    CHECK_EQ(sol.longestCommonSubsequence(longA, longB), 1000);  // largest input

    for (int iter = 0; iter < 300; iter++) {   // stress test vs every subsequence of a
        string a = t::rand_string((int)t::rand_int(0, 10), 'a', 'c');
        string b = t::rand_string((int)t::rand_int(0, 10), 'a', 'c');
        int expected = brute(a, b);
        CHECK_EQ(sol.longestCommonSubsequence(a, b), expected);
        CHECK_EQ(roll.longestCommonSubsequence(a, b), expected);
        string common = lcsString(a, b);
        CHECK_EQ((int)common.size(), expected);
        CHECK(isSubsequence(common, a) && isSubsequence(common, b));
    }
    return t::summary("1143-longest-common-subsequence");
}
