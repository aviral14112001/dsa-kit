// 1044. Longest Duplicate Substring: https://leetcode.com/problems/longest-duplicate-substring/
// Pattern: binary search on the length + Rabin–Karp rolling hash. Module 18 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
    static constexpr uint64_t MOD = (1ULL << 61) - 1;        // prime; hashes collide with prob ~ L / 2^61
    static uint64_t mul(uint64_t a, uint64_t b) {            // a * b mod 2^61 - 1
        __uint128_t c = (__uint128_t)a * b;
        uint64_t r = (uint64_t)(c & MOD) + (uint64_t)(c >> 61);
        return r >= MOD ? r - MOD : r;
    }

public:
    string longestDupSubstring(string s) {
        int n = (int)s.size();
        uint64_t base = 256 + mt19937_64(random_device{}())() % (MOD - 512);   // random: no input targets it
        vector<uint64_t> h(n + 1, 0), pw(n + 1, 1);          // h[i] = hash of s[0, i), pw[i] = base^i
        for (int i = 0; i < n; i++) {
            uint64_t x = mul(h[i], base) + (unsigned char)s[i];
            h[i + 1] = x >= MOD ? x - MOD : x;
            pw[i + 1] = mul(pw[i], base);
        }
        auto window = [&](int pos, int len) {                // hash of s.substr(pos, len), O(1)
            uint64_t x = h[pos + len] + MOD - mul(h[pos], pw[len]);
            return x >= MOD ? x - MOD : x;
        };
        // Start of a length-len substring that occurs at least twice, or -1. O(n) expected.
        auto find_repeat = [&](int len) {
            unordered_map<uint64_t, int> first_start;
            first_start.reserve(n);
            for (int i = 0; i + len <= n; i++) {
                auto [it, inserted] = first_start.try_emplace(window(i, len), i);
                if (!inserted && s.compare(it->second, len, s, i, len) == 0) return i;   // verify the hit
            }
            return -1;
        };
        // "Some substring of length L occurs twice" is monotone: if it holds for L, the prefixes of that
        // substring show it for every shorter length. So binary search the largest L that holds.
        int lo = 1, hi = n - 1, best_len = 0, best_start = 0;
        while (lo <= hi) {
            int mid = lo + (hi - lo) / 2;
            int start = find_repeat(mid);
            if (start != -1) {
                best_len = mid;
                best_start = start;
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
        return s.substr(best_start, best_len);
    }
};
// [/snippet]

// Brute force: the longest length at which some substring appears twice (set of all substrings).
int brute_len(const string& s) {
    int n = (int)s.size();
    for (int len = n - 1; len >= 1; len--) {
        set<string> seen;
        for (int i = 0; i + len <= n; i++)
            if (!seen.insert(s.substr(i, len)).second) return len;
    }
    return 0;
}

int occurrences(const string& s, const string& sub) {   // overlapping occurrences count
    int count = 0;
    for (int i = 0; i + (int)sub.size() <= (int)s.size(); i++) count += s.compare(i, sub.size(), sub) == 0;
    return count;
}

int main() {
    Solution sol;
    CHECK_EQ(sol.longestDupSubstring("banana"), "ana");     // "ana" at 1 and 3 overlap
    CHECK_EQ(sol.longestDupSubstring("abcd"), "");
    CHECK_EQ(sol.longestDupSubstring("aa"), "a");
    CHECK_EQ(sol.longestDupSubstring("aaaaa"), "aaaa");

    // The maximum size: all equal letters (answer n - 1) and random letters (short answer).
    CHECK_EQ(sol.longestDupSubstring(string(30000, 'z')).size(), (size_t)29999);
    string rnd = t::rand_string(30000, 'a', 'z');
    string ans = sol.longestDupSubstring(rnd);
    CHECK(!ans.empty() && occurrences(rnd, ans) >= 2);

    // Stress: several answers can be valid, so check the length against brute force and that the
    // returned string really occurs twice.
    for (int iter = 0; iter < 300; iter++) {
        string s = t::rand_string((int)t::rand_int(2, 25), 'a', (char)('a' + t::rand_int(1, 3)));
        string got = sol.longestDupSubstring(s);
        CHECK_EQ((int)got.size(), brute_len(s));
        if (!got.empty()) CHECK(occurrences(s, got) >= 2);
    }
    return t::summary("1044-longest-duplicate-substring");
}
