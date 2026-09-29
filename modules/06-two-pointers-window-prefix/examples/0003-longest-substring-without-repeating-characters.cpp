// 3. Longest Substring Without Repeating Characters: https://leetcode.com/problems/longest-substring-without-repeating-characters/
// Pattern: variable sliding window (longest). Module 06 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        int count[256] = {};                   // occurrences of each byte inside the window s[l..r]
        int best = 0;
        for (int l = 0, r = 0; r < (int)s.size(); r++) {
            unsigned char c = s[r];            // unsigned: a negative char would index out of bounds
            count[c]++;
            while (count[c] > 1)               // only s[r] can be duplicated: shrink until it isn't
                count[(unsigned char)s[l++]]--;
            best = max(best, r - l + 1);       // s[l..r] is the longest valid window ending at r
        }
        return best;
    }
};
// [/snippet]

// [snippet:jump]
// Variant: remember where each character was last seen and jump l past it in one step.
int longest_by_jumping(const string& s) {
    vector<int> last(256, -1);                 // last index of each byte, -1 = not seen yet
    int best = 0;
    for (int l = 0, r = 0; r < (int)s.size(); r++) {
        unsigned char c = s[r];
        l = max(l, last[c] + 1);               // max: never move l backwards to an old occurrence
        last[c] = r;
        best = max(best, r - l + 1);
    }
    return best;
}
// [/snippet]

// Brute force: every substring, checked with a set. O(n^3); fine for the stress test only.
int brute(const string& s) {
    int best = 0, n = (int)s.size();
    for (int i = 0; i < n; i++)
        for (int j = i; j < n; j++) {
            set<char> seen(s.begin() + i, s.begin() + j + 1);
            if ((int)seen.size() == j - i + 1) best = max(best, j - i + 1);
        }
    return best;
}

int main() {
    Solution sol;
    CHECK_EQ(sol.lengthOfLongestSubstring("abcabcbb"), 3);
    CHECK_EQ(sol.lengthOfLongestSubstring("bbbbb"), 1);
    CHECK_EQ(sol.lengthOfLongestSubstring("pwwkew"), 3);

    CHECK_EQ(sol.lengthOfLongestSubstring(""), 0);
    CHECK_EQ(sol.lengthOfLongestSubstring(" "), 1);           // a space is a character too
    CHECK_EQ(sol.lengthOfLongestSubstring("abba"), 2);        // the jump variant's trap: l must not go back
    CHECK_EQ(longest_by_jumping("abba"), 2);
    CHECK_EQ(sol.lengthOfLongestSubstring("dvdf"), 3);

    for (int iter = 0; iter < 300; iter++) {
        string s = t::rand_string((int)t::rand_int(0, 12), 'a', 'd');
        int expected = brute(s);
        CHECK_EQ(sol.lengthOfLongestSubstring(s), expected);
        CHECK_EQ(longest_by_jumping(s), expected);
    }
    return t::summary("0003-longest-substring");
}
