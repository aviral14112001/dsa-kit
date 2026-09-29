// Tests for templates/trie.hpp: fixed cases, then random operation sequences checked against a
// brute force that keeps the words in a plain vector<string> and scans it for every query.
#include <bits/stdc++.h>
#include "test.hpp"
#include "trie.hpp"
using namespace std;

// Obviously correct, O(n * L) per query: a multiset of words kept as a vector.
struct BruteWords {
    vector<string> words;
    void insert(const string& w) { words.push_back(w); }
    bool contains(const string& w) const { return find(words.begin(), words.end(), w) != words.end(); }
    int count_prefix(const string& p) const {
        return (int)count_if(words.begin(), words.end(), [&](const string& w) { return w.starts_with(p); });
    }
    bool starts_with(const string& p) const { return count_prefix(p) > 0; }
    bool erase(const string& w) {
        auto it = find(words.begin(), words.end(), w);
        if (it == words.end()) return false;
        words.erase(it);
        return true;
    }
};

// Random operations over a small alphabet (so words share prefixes and repeat), checked after each.
void stress(int ops, int max_len, char hi) {
    Trie trie;
    BruteWords brute;
    long long inserted_chars = 0;
    for (int op = 0; op < ops; op++) {
        string s = t::rand_string((int)t::rand_int(0, max_len), 'a', hi);
        int kind = (int)t::rand_int(0, 9);
        if (kind < 4) {
            trie.insert(s);
            brute.insert(s);
            inserted_chars += (long long)s.size();
        } else if (kind < 6) {
            CHECK_EQ(trie.erase(s), brute.erase(s));
        } else if (kind < 8) {
            CHECK_EQ(trie.contains(s), brute.contains(s));
        } else {
            CHECK_EQ(trie.count_prefix(s), brute.count_prefix(s));
            CHECK_EQ(trie.starts_with(s), brute.starts_with(s));
        }
    }
    CHECK_EQ(trie.count_prefix(""), (int)brute.words.size());   // the root is passed by every stored word
    CHECK(trie.nodes.size() <= 1 + (size_t)inserted_chars);      // at most one new node per inserted char
}

int main() {
    // the header's example
    Trie tr;
    tr.insert("car");
    tr.insert("cart");
    tr.insert("car");
    CHECK(tr.contains("car"));
    CHECK(tr.contains("cart"));
    CHECK(!tr.contains("ca"));                  // a prefix of a word is not a word
    CHECK(!tr.contains("carts"));               // the path runs out
    CHECK(tr.starts_with("ca"));
    CHECK(!tr.starts_with("cb"));
    CHECK_EQ(tr.count_prefix("car"), 3);
    CHECK_EQ(tr.count_prefix("cart"), 1);
    CHECK_EQ(tr.count_prefix(""), 3);
    CHECK_EQ(tr.count_prefix("x"), 0);

    // erase: one copy at a time; missing words and bare prefixes are refused without side effects
    CHECK(tr.erase("car"));
    CHECK(tr.contains("car"));                  // one copy left
    CHECK_EQ(tr.count_prefix("car"), 2);
    CHECK(!tr.erase("ca"));
    CHECK(!tr.erase("dog"));
    CHECK_EQ(tr.count_prefix(""), 2);
    CHECK(tr.erase("car"));
    CHECK(!tr.contains("car"));
    CHECK(tr.contains("cart"));
    CHECK(tr.erase("cart"));
    CHECK(!tr.starts_with("c"));                // dead nodes (pass == 0) don't count as prefixes
    CHECK_EQ(tr.count_prefix(""), 0);
    size_t nodes_before = tr.nodes.size();
    tr.insert("cart");                          // re-inserting reuses the dead nodes
    CHECK_EQ(tr.nodes.size(), nodes_before);
    CHECK(tr.contains("cart"));
    CHECK(!tr.contains("car"));

    // LeetCode 208's example
    Trie lc;
    lc.insert("apple");
    CHECK(lc.contains("apple"));
    CHECK(!lc.contains("app"));
    CHECK(lc.starts_with("app"));
    lc.insert("app");
    CHECK(lc.contains("app"));

    // the empty string is a legal word (it ends at the root)
    Trie empty_word;
    CHECK(!empty_word.contains(""));
    CHECK(!empty_word.starts_with(""));
    empty_word.insert("");
    CHECK(empty_word.contains(""));
    CHECK(empty_word.starts_with(""));
    CHECK(empty_word.erase(""));
    CHECK(!empty_word.contains(""));

    // every letter of the alphabet gets its own slot
    Trie abc;
    for (char c = 'a'; c <= 'z'; c++) abc.insert(string(1, c) + "x");
    for (char c = 'a'; c <= 'z'; c++) CHECK_EQ(abc.count_prefix(string(1, c)), 1);
    CHECK_EQ(abc.nodes.size(), 1 + 26 + 26);

    // stress against the brute force
    for (int trial = 0; trial < 5; trial++) stress(2000, 4, 'c');   // tiny alphabet: heavy sharing, many repeats
    for (int trial = 0; trial < 3; trial++) stress(2000, 8, 'z');   // full alphabet: mostly misses
    return t::summary("trie");
}
