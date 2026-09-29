#include <bits/stdc++.h>
#include "test.hpp"
#include "strings.hpp"
using namespace std;

// Brute forces straight from the definitions.
vector<int> pi_brute(const string& s) {
    int n = (int)s.size();
    vector<int> pi(n, 0);
    for (int i = 0; i < n; i++)
        for (int k = i; k >= 1; k--)                              // proper: k <= i
            if (s.compare(0, k, s, i - k + 1, k) == 0) { pi[i] = k; break; }
    return pi;
}
vector<int> find_all_brute(const string& text, const string& pattern) {
    vector<int> res;
    for (int i = 0; i + (int)pattern.size() <= (int)text.size(); i++)
        if (text.compare(i, pattern.size(), pattern) == 0) res.push_back(i);
    return res;
}
vector<int> z_brute(const string& s) {
    int n = (int)s.size();
    vector<int> z(n, 0);
    for (int i = 0; i < n; i++)
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) z[i]++;
    return z;
}
vector<int> manacher_brute(const string& s) {                     // expand around each of the 2n + 1 centers
    int n = (int)s.size();
    vector<int> len(2 * n + 1, 0);
    for (int c = 0; c <= 2 * n; c++) {
        int lo, hi;                                               // palindrome s[lo..hi] grows outwards
        if (c % 2 == 1) { lo = hi = c / 2; }
        else { lo = c / 2 - 1; hi = c / 2; }
        while (lo >= 0 && hi < n && s[lo] == s[hi]) { lo--; hi++; }
        len[c] = hi - lo - 1;
    }
    return len;
}

int main() {
    // Fixed cases.
    CHECK_EQ(prefix_function("aabaaab"), vector<int>{0, 1, 0, 1, 2, 2, 3});
    CHECK_EQ(prefix_function("abcd"), vector<int>{0, 0, 0, 0});
    CHECK_EQ(prefix_function(""), vector<int>{});
    CHECK_EQ(kmp_search("sadbutsad", "sad"), vector<int>{0, 6});
    CHECK_EQ(kmp_search("leetcode", "leeto"), vector<int>{});
    CHECK_EQ(kmp_search("aaaa", "aa"), vector<int>{0, 1, 2});    // overlapping matches
    CHECK_EQ(kmp_search("ab", ""), vector<int>{0, 1, 2});        // empty pattern: every index
    CHECK_EQ(kmp_search("", "a"), vector<int>{});
    CHECK_EQ(z_function("aaabaab"), vector<int>{7, 2, 1, 0, 2, 1, 0});
    CHECK_EQ(z_function(""), vector<int>{});
    CHECK_EQ(rabin_karp_search("abababa", "aba"), vector<int>{0, 2, 4});
    CHECK_EQ(rabin_karp_search("ab", ""), vector<int>{0, 1, 2});
    CHECK_EQ(rabin_karp_search("ab", "abc"), vector<int>{});
    CHECK_EQ(manacher("aba"), vector<int>{0, 1, 0, 3, 0, 1, 0}); // centers: gap a gap b gap a gap
    CHECK_EQ(manacher("abba"), vector<int>{0, 1, 0, 1, 4, 1, 0, 1, 0});
    CHECK_EQ(manacher(""), vector<int>{0});
    CHECK_EQ(suffix_array("banana"), vector<int>{5, 3, 1, 0, 4, 2});   // a, ana, anana, banana, na, nana
    CHECK_EQ(lcp_array("banana", suffix_array("banana")), vector<int>{1, 3, 0, 0, 2});
    CHECK_EQ(suffix_array(""), vector<int>{});
    CHECK_EQ(suffix_array("z"), vector<int>{0});
    CHECK_EQ(lcp_array("z", {0}), vector<int>{});

    // Longest palindrome via manacher: the max len and its start index.
    {
        string s = "forgeeksskeegfor";
        auto len = manacher(s);
        int c = (int)(max_element(len.begin(), len.end()) - len.begin());
        CHECK_EQ(s.substr((c - len[c]) / 2, len[c]), "geeksskeeg");
    }

    // RollingHash: substrings compare equal exactly when the strings are equal.
    {
        RollingHash rh("abcabcabc");
        CHECK_EQ(rh.get(0, 3), rh.get(3, 3));
        CHECK_EQ(rh.get(1, 5), rh.get(4, 5));
        CHECK(rh.get(0, 3) != rh.get(1, 3));
        CHECK_EQ(rh.get(2, 0), 0ULL);                             // the empty substring
        CHECK_EQ(RollingHash::mul(RollingHash::M - 1, RollingHash::M - 1), 1ULL);   // (-1)^2 = 1
    }

    // Stress: every algorithm against its brute force on random strings over tiny alphabets
    // (many repeats, borders and palindromes).
    for (int iter = 0; iter < 300; iter++) {
        char hi = (char)('a' + t::rand_int(0, 2));                // alphabet {a}, {a,b} or {a,b,c}
        string s = t::rand_string((int)t::rand_int(0, 25), 'a', hi);
        string p = t::rand_string((int)t::rand_int(1, 4), 'a', hi);
        int n = (int)s.size();

        CHECK_EQ(prefix_function(s), pi_brute(s));
        CHECK_EQ(z_function(s), z_brute(s));
        CHECK_EQ(kmp_search(s, p), find_all_brute(s, p));
        CHECK_EQ(rabin_karp_search(s, p), find_all_brute(s, p));
        CHECK_EQ(manacher(s), manacher_brute(s));

        RollingHash rh(s);
        for (int q = 0; q < 20 && n > 0; q++) {
            int len = (int)t::rand_int(1, n);
            int i = (int)t::rand_int(0, n - len), j = (int)t::rand_int(0, n - len);
            CHECK_EQ(rh.get(i, len) == rh.get(j, len), s.compare(i, len, s, j, len) == 0);
        }

        vector<int> sa = suffix_array(s);
        vector<int> brute_sa(n);
        iota(brute_sa.begin(), brute_sa.end(), 0);
        sort(brute_sa.begin(), brute_sa.end(), [&](int a, int b) { return s.substr(a) < s.substr(b); });
        CHECK_EQ(sa, brute_sa);

        vector<int> lcp = lcp_array(s, sa);
        for (int i = 0; i + 1 < n; i++) {
            int h = 0;
            while (sa[i] + h < n && sa[i + 1] + h < n && s[sa[i] + h] == s[sa[i + 1] + h]) h++;
            CHECK_EQ(lcp[i], h);
        }
        // What SA + LCP answer: the number of distinct substrings, and the longest repeated one.
        set<string> distinct;
        int longest_repeat = 0;
        for (int i = 0; i < n; i++)
            for (int len = 1; i + len <= n; len++) {
                string sub = s.substr(i, len);
                if (s.find(sub, i + 1) != string::npos) longest_repeat = max(longest_repeat, len);
                distinct.insert(sub);
            }
        long long total = (long long)n * (n + 1) / 2 - accumulate(lcp.begin(), lcp.end(), 0LL);
        CHECK_EQ(total, (long long)distinct.size());
        CHECK_EQ(lcp.empty() ? 0 : *max_element(lcp.begin(), lcp.end()), longest_repeat);
    }

    // A long run: prefix doubling must still terminate, and the Z/KMP answers stay linear.
    string run(2000, 'a');
    CHECK_EQ(suffix_array(run)[0], 1999);
    CHECK_EQ(kmp_search(run, string(1000, 'a')).size(), (size_t)1001);
    CHECK_EQ(z_function(run)[1], 1999);
    return t::summary("strings");
}
