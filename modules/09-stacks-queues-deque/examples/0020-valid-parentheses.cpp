// 20. Valid Parentheses: https://leetcode.com/problems/valid-parentheses/
// Pattern: matching stack (push openers, pop on closers). Module 09 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    bool isValid(string s) {
        stack<char> open;                 // unmatched openers, innermost on top
        for (char c : s) {
            if (c == '(' || c == '[' || c == '{') {
                open.push(c);
                continue;
            }
            char want = c == ')' ? '(' : c == ']' ? '[' : '{';
            if (open.empty() || open.top() != want) return false;  // nothing to close, or crossed
            open.pop();                   // std::stack::pop() returns void: read top() first
        }
        return open.empty();              // leftover openers were never closed
    }
};
// [/snippet]

// Brute force for the stress test: erase adjacent matching pairs until nothing changes.
// Valid strings shrink to "". O(n^2), obviously correct.
bool brute(string s) {
    for (bool changed = true; changed;) {
        changed = false;
        for (const char* pair : {"()", "[]", "{}"}) {
            size_t at = s.find(pair);
            if (at != string::npos) { s.erase(at, 2); changed = true; }
        }
    }
    return s.empty();
}

// Random bracket strings are almost never valid, so build valid ones on purpose, then
// sometimes break one character.
string random_valid(int pairs) {
    const string opens = "([{", closes = ")]}";
    string s, pending;  // pending: closers still owed, innermost last
    int opened = 0;
    while (opened < pairs || !pending.empty()) {
        if (opened < pairs && (pending.empty() || t::rand_int(0, 1))) {
            int k = (int)t::rand_int(0, 2);
            s += opens[k];
            pending += closes[k];
            opened++;
        } else {
            s += pending.back();
            pending.pop_back();
        }
    }
    return s;
}

int main() {
    Solution sol;
    // official examples
    CHECK_EQ(sol.isValid("()"), true);
    CHECK_EQ(sol.isValid("()[]{}"), true);
    CHECK_EQ(sol.isValid("(]"), false);
    CHECK_EQ(sol.isValid("([])"), true);
    // edge cases
    CHECK_EQ(sol.isValid("([)]"), false);    // right counts, crossed nesting
    CHECK_EQ(sol.isValid("{[]}"), true);
    CHECK_EQ(sol.isValid("("), false);       // opener never closed
    CHECK_EQ(sol.isValid(")"), false);       // closer with an empty stack
    CHECK_EQ(sol.isValid("(()"), false);
    CHECK_EQ(sol.isValid("())"), false);
    CHECK_EQ(sol.isValid(""), true);         // outside LeetCode's constraints, but well defined

    // stress: valid strings, broken valid strings, and plain random strings vs the brute force
    const string alphabet = "()[]{}";
    for (int iter = 0; iter < 600; iter++) {
        string s;
        if (iter % 3 == 2) {
            int n = (int)t::rand_int(0, 12);
            for (int i = 0; i < n; i++) s += alphabet[t::rand_int(0, 5)];
        } else {
            s = random_valid((int)t::rand_int(0, 6));
            if (iter % 3 == 1 && !s.empty()) s[t::rand_int(0, (long long)s.size() - 1)] = alphabet[t::rand_int(0, 5)];
        }
        CHECK_EQ(sol.isValid(s), brute(s));
    }
    return t::summary("0020-valid-parentheses");
}
