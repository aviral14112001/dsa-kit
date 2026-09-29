// Module 04 section 3: std::string mechanics. Every claim in the notes' API table is a CHECK here.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:api]
void string_api_tour() {
    string s = "hello world";
    CHECK_EQ(s.size(), 11);
    CHECK_EQ(s.substr(6), "world");                  // from index 6 to the end
    CHECK_EQ(s.substr(0, 4), "hell");                // (start, LENGTH), like C#'s Substring
    CHECK_EQ(s.find('o'), 4);                        // first match
    CHECK_EQ(s.rfind('o'), 7);                       // last match
    CHECK(s.find("xyz") == string::npos);            // not found: npos (a huge size_t), not -1
    s[0] = 'H';                                      // strings are mutable: no new object
    s += '!';                                        // append in place, amortized O(1)
    CHECK_EQ(s, "Hello world!");
    s.insert(5, ",");                                // O(n): shifts the tail right
    CHECK_EQ(s, "Hello, world!");
    s.erase(5, 1);                                   // (start, COUNT), O(n)
    CHECK_EQ(s, "Hello world!");
    s.pop_back();
    CHECK_EQ(s.back(), 'd');

    string copy = s;                                 // a real copy (value semantics), not a reference
    copy[0] = 'J';
    CHECK_EQ(s[0], 'H');
    reverse(copy.begin(), copy.end());               // <algorithm> works on strings
    CHECK_EQ(copy, "dlrow olleJ");
    string letters = "banana";
    sort(letters.begin(), letters.end());
    CHECK_EQ(letters, "aaabnn");                     // sorted letters: an anagram signature

    CHECK_EQ(string(3, 'z'), "zzz");
    CHECK_EQ(to_string(-42), "-42");
    CHECK_EQ(stoi("123") + 1, 124);
    CHECK_EQ(stoll("9000000000"), 9'000'000'000LL);  // stoi would throw: out of int range
    CHECK_EQ('7' - '0', 7);                          // digit character -> its value
    CHECK_EQ(char('a' + 2), 'c');                    // letter arithmetic
    CHECK(string("apple") < string("banana"));       // <, ==, != compare contents lexicographically
    CHECK(isalnum((unsigned char)'x') && !isalnum((unsigned char)','));   // cast first: see Pitfalls

    istringstream in("  split   on\twhitespace ");
    vector<string> words;
    for (string w; in >> w;) words.push_back(w);     // >> skips any run of spaces, tabs, newlines
    CHECK_EQ(words, vector<string>{"split", "on", "whitespace"});

    istringstream input("3\nhello world\n");         // an OA-style input: a number, then a line
    int n;
    string line;
    input >> n;
    getline(input, line);                            // reads the rest of the FIRST line: ""
    CHECK_EQ(line, "");
    getline(input, line);
    CHECK_EQ(line, "hello world");
}
// [/snippet]

// [snippet:builder]
// Both build "abcabc...". Same output, very different cost.
string build_slow(int n) {
    string s;
    for (int i = 0; i < n; i++) s = s + char('a' + i % 3);   // s + c builds a NEW string: O(length) per step
    return s;
}

string build_fast(int n) {
    string s;
    s.reserve(n);                                           // optional: one allocation up front
    for (int i = 0; i < n; i++) s += char('a' + i % 3);      // appends in place: amortized O(1) per step
    return s;
}
// [/snippet]

// [snippet:counts]
// Can the letters of s (lowercase a-z) be rearranged into a palindrome?
// A palindrome mirrors every letter, except possibly one in the middle: at most one odd count.
bool can_rearrange_to_palindrome(const string& s) {
    int cnt[26] = {};                        // = {} zero-fills; a bare `int cnt[26];` holds garbage
    for (char c : s) cnt[c - 'a']++;         // 'a' -> 0, ..., 'z' -> 25
    int odd = 0;
    for (int x : cnt) odd += x % 2;
    return odd <= 1;
}
// [/snippet]

// [snippet:palindrome]
// Is s[l..r] a palindrome? Compare the two ends, step inward. O(r - l) time, O(1) space.
bool is_palindrome(const string& s, int l, int r) {
    while (l < r)
        if (s[l++] != s[r--]) return false;
    return true;
}
// [/snippet]

int main() {
    string_api_tour();

    CHECK_EQ(build_slow(7), "abcabca");
    CHECK_EQ(build_fast(7), "abcabca");
    CHECK_EQ(build_slow(2000), build_fast(2000));

    CHECK(can_rearrange_to_palindrome("carerac"));    // -> "racecar"
    CHECK(can_rearrange_to_palindrome("aabb"));       // -> "abba"
    CHECK(!can_rearrange_to_palindrome("abc"));
    CHECK(can_rearrange_to_palindrome(""));

    CHECK(is_palindrome("xabbay", 1, 4));
    CHECK(!is_palindrome("abca", 0, 3));
    CHECK(is_palindrome("z", 0, 0));

    for (int iter = 0; iter < 300; iter++) {
        string s = t::rand_string((int)t::rand_int(0, 10), 'a', 'c');
        // palindrome-permutation brute force: does ANY permutation read the same backwards?
        string p = s;
        sort(p.begin(), p.end());
        bool any = false;
        do any = any || equal(p.begin(), p.end(), p.rbegin());
        while (next_permutation(p.begin(), p.end()));
        CHECK_EQ(can_rearrange_to_palindrome(s), any);
        if (!s.empty()) {
            int l = (int)t::rand_int(0, (int)s.size() - 1), r = (int)t::rand_int(l, (int)s.size() - 1);
            string sub = s.substr(l, r - l + 1);
            CHECK_EQ(is_palindrome(s, l, r), sub == string(sub.rbegin(), sub.rend()));
        }
    }
    return t::summary("04-string-mechanics");
}
