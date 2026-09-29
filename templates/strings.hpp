// String algorithms: KMP prefix function, Z-function, Rabin–Karp rolling hash, Manacher, suffix array
// + LCP. Module 18 section 4. (Free functions are `inline` because they live in a header.)
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:prefix_function]
// pi[i] = length of the longest proper prefix of s[0..i] that is also a suffix of s[0..i] (a "border").
// A border of s[0..i] minus its last char is a border of s[0..i-1], so the candidates for pi[i] are
// pi[i-1] + 1, then pi[pi[i-1] - 1] + 1, ... (each fallback is the next shorter border). O(n) total:
// k rises by at most 1 per step, and every fallback lowers it.
inline vector<int> prefix_function(const string& s) {
    int n = (int)s.size();
    vector<int> pi(n, 0);
    for (int i = 1; i < n; i++) {
        int k = pi[i - 1];
        while (k > 0 && s[i] != s[k]) k = pi[k - 1];   // fall back to the next shorter border
        if (s[i] == s[k]) k++;
        pi[i] = k;
    }
    return pi;
}
// [/snippet]

// [snippet:kmp_search]
// Every start index of pattern in text, O(n + m). k = how many chars of pattern match the text ending
// at position i. On a mismatch, k falls back through pattern's borders; i never moves backwards.
inline vector<int> kmp_search(const string& text, const string& pattern) {
    int n = (int)text.size(), m = (int)pattern.size();
    vector<int> matches;
    if (m == 0) {                                      // the empty pattern occurs at every index 0..n
        for (int i = 0; i <= n; i++) matches.push_back(i);
        return matches;
    }
    vector<int> pi = prefix_function(pattern);
    for (int i = 0, k = 0; i < n; i++) {
        while (k > 0 && text[i] != pattern[k]) k = pi[k - 1];
        if (text[i] == pattern[k]) k++;
        if (k == m) {                                  // full match ending at i
            matches.push_back(i - m + 1);
            k = pi[k - 1];                             // keep the longest border: overlapping matches count
        }
    }
    return matches;
}
// [/snippet]

// [snippet:z_function]
// z[i] = length of the longest common prefix of s and s[i..]; z[0] = n by convention.
// [l, r) is the match window s[l..r) == s[0..r-l) reaching furthest right. For i inside it,
// s[i..r) == s[i-l..r-l), so z[i] starts at min(z[i-l], r - i) for free; only chars past r are
// compared, and each successful comparison pushes r right: O(n).
inline vector<int> z_function(const string& s) {
    int n = (int)s.size();
    vector<int> z(n, 0);
    if (n > 0) z[0] = n;
    for (int i = 1, l = 0, r = 0; i < n; i++) {
        if (i < r) z[i] = min(z[i - l], r - i);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) z[i]++;
        if (i + z[i] > r) {
            l = i;
            r = i + z[i];
        }
    }
    return z;
}
// [/snippet]

// [snippet:rolling_hash]
// Polynomial hash modulo the prime M = 2^61 - 1 with a random base B:
//     hash(t) = t[0]*B^(len-1) + t[1]*B^(len-2) + ... + t[len-1]   (mod M)
// Prefix hashes give any substring's hash in O(1). Two different strings of the same length L collide
// with probability <= L / M (about L * 4e-19) over the random choice of B. Compare equal lengths only.
struct RollingHash {
    static constexpr uint64_t M = (1ULL << 61) - 1;

    static uint64_t mul(uint64_t a, uint64_t b) {        // a * b mod M, using 2^61 = 1 (mod M)
        __uint128_t c = (__uint128_t)a * b;
        uint64_t r = (uint64_t)(c & M) + (uint64_t)(c >> 61);
        return r >= M ? r - M : r;
    }
    static uint64_t base() {                             // random per run, shared by every RollingHash
        static const uint64_t b = 256 + mt19937_64(random_device{}())() % (M - 512);
        return b;
    }

    vector<uint64_t> h, pw;                              // h[i] = hash of s[0, i), pw[i] = B^i

    explicit RollingHash(const string& s) : h(s.size() + 1, 0), pw(s.size() + 1, 1) {
        for (size_t i = 0; i < s.size(); i++) {
            uint64_t x = mul(h[i], base()) + (unsigned char)s[i];   // shift left one "digit", append s[i]
            h[i + 1] = x >= M ? x - M : x;
            pw[i + 1] = mul(pw[i], base());
        }
    }

    uint64_t get(int pos, int len) const {               // hash of s.substr(pos, len)
        uint64_t x = h[pos + len] + M - mul(h[pos], pw[len]);   // h[pos+len] - h[pos]*B^len, kept >= 0
        return x >= M ? x - M : x;
    }
};
// [/snippet]

// [snippet:rabin_karp]
// Rabin–Karp: every start index of pattern in text by comparing window hashes, O(n + m). A hash match
// is trusted (Monte Carlo); add a text.compare(i, m, pattern) check if a wrong answer is unacceptable.
inline vector<int> rabin_karp_search(const string& text, const string& pattern) {
    int n = (int)text.size(), m = (int)pattern.size();
    vector<int> matches;
    if (m > n) return matches;
    RollingHash ht(text), hp(pattern);                   // same base B, so hashes are comparable
    uint64_t target = hp.get(0, m);
    for (int i = 0; i + m <= n; i++)
        if (ht.get(i, m) == target) matches.push_back(i);
    return matches;
}
// [/snippet]

// [snippet:manacher]
// Manacher: the longest palindrome around every center, O(n). There are 2n + 1 centers: odd c is the
// character s[c / 2] (odd lengths), even c is the gap before s[c / 2] (even lengths). Picture s with a
// separator in every gap, "#a#b#a#": the radius len[c] there is exactly the palindrome's length in s,
// and the palindrome starts at index (c - len[c]) / 2.
inline vector<int> manacher(const string& s) {
    int m = 2 * (int)s.size() + 1;
    // c - k and c + k have the same parity: two gaps always match, two characters must be equal.
    auto match = [&](int a, int b) { return a % 2 == 0 || s[a / 2] == s[b / 2]; };
    vector<int> len(m, 0);
    for (int c = 0, center = 0, right = 0; c < m; c++) {   // the palindrome at `center` reaches furthest right
        int k = c < right ? min(len[2 * center - c], right - c) : 0;   // its mirror's radius, clipped to it
        while (c - k - 1 >= 0 && c + k + 1 < m && match(c - k - 1, c + k + 1)) k++;
        len[c] = k;
        if (c + k > right) {
            center = c;
            right = c + k;
        }
    }
    return len;
}
// [/snippet]

// [snippet:suffix_array]
// Suffix array: the start indices of s's suffixes in sorted order. Prefix doubling: once suffixes are
// ranked by their first k chars, the first 2k chars of suffix i are the pair (rank[i], rank[i + k]),
// so one sort by pairs doubles k. At most log n rounds of an O(n log n) sort: O(n log^2 n).
inline vector<int> suffix_array(const string& s) {
    int n = (int)s.size();
    vector<int> sa(n), rnk(n), next_rnk(n);
    iota(sa.begin(), sa.end(), 0);
    for (int i = 0; i < n; i++) rnk[i] = (unsigned char)s[i];
    for (int k = 1; n > 1; k *= 2) {
        // -1 when suffix i has fewer than k + 1 chars: the suffix that runs out first sorts first
        auto key = [&](int i) { return pair(rnk[i], i + k < n ? rnk[i + k] : -1); };
        sort(sa.begin(), sa.end(), [&](int a, int b) { return key(a) < key(b); });
        next_rnk[sa[0]] = 0;
        for (int i = 1; i < n; i++) next_rnk[sa[i]] = next_rnk[sa[i - 1]] + (key(sa[i - 1]) < key(sa[i]));
        rnk = next_rnk;
        if (rnk[sa[n - 1]] == n - 1) break;              // all ranks distinct: fully sorted
    }
    return sa;
}
// [/snippet]

// [snippet:lcp]
// Kasai: lcp[i] = length of the longest common prefix of suffixes sa[i] and sa[i + 1], in O(n).
// Visit suffixes in text order: if suffix i shares h chars with the suffix after it in sorted order,
// suffix i + 1 shares at least h - 1 with its own successor (drop the first char of both). So h falls
// by at most 1 per step and never exceeds n: the inner loop runs O(n) times in total.
inline vector<int> lcp_array(const string& s, const vector<int>& sa) {
    int n = (int)s.size();
    vector<int> rnk(n), lcp(max(n - 1, 0));
    for (int i = 0; i < n; i++) rnk[sa[i]] = i;
    for (int i = 0, h = 0; i < n; i++) {
        if (rnk[i] == n - 1) {                           // the last suffix in sorted order has no successor
            h = 0;
            continue;
        }
        int j = sa[rnk[i] + 1];
        while (i + h < n && j + h < n && s[i + h] == s[j + h]) h++;
        lcp[rnk[i]] = h;
        if (h > 0) h--;
    }
    return lcp;
}
// [/snippet]
