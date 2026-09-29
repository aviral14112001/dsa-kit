// 208. Implement Trie (Prefix Tree): https://leetcode.com/problems/implement-trie-prefix-tree/
// Pattern: implement a trie: children[26] + end flag; pointer vs pool allocation. Module 13 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Trie {
public:
    Trie() {}

    void insert(string word) {
        Node* cur = &root;
        for (char ch : word) {
            unique_ptr<Node>& next = cur->child[ch - 'a'];
            if (!next) next = make_unique<Node>();      // create the edge the first time it's needed
            cur = next.get();
        }
        cur->is_word = true;
    }

    bool search(string word) {
        const Node* node = walk(word);
        return node && node->is_word;       // the path must exist AND a word must end there
    }

    bool startsWith(string prefix) {
        return walk(prefix) != nullptr;     // no deletions, so every node lies on some word's path
    }

private:
    struct Node {
        array<unique_ptr<Node>, 26> child;  // owning pointers: destroying the root frees the whole trie
        bool is_word = false;               // a stored word ends here (not just passes through)
    };
    Node root;                              // the empty prefix

    // The node reached by spelling s from the root, or nullptr if the path breaks off.
    const Node* walk(const string& s) const {
        const Node* cur = &root;
        for (char ch : s) {
            cur = cur->child[ch - 'a'].get();
            if (!cur) return nullptr;
        }
        return cur;
    }
};
// [/snippet]

int main() {
    // official example
    Trie trie;
    trie.insert("apple");
    CHECK(trie.search("apple"));
    CHECK(!trie.search("app"));             // only a prefix so far
    CHECK(trie.startsWith("app"));
    trie.insert("app");
    CHECK(trie.search("app"));
    // edge cases: longer than any word, a word that is a prefix of nothing else, all 26 letters
    CHECK(!trie.search("apples"));
    CHECK(!trie.startsWith("applesauce"));
    CHECK(!trie.startsWith("b"));
    trie.insert("zyxwvutsrqponmlkjihgfedcba");
    CHECK(trie.startsWith("zyx"));
    CHECK(trie.search("zyxwvutsrqponmlkjihgfedcba"));
    string long_word(2000, 'q');            // the maximum length: destruction recurses 2000 deep, fine
    trie.insert(long_word);
    CHECK(trie.search(long_word));
    CHECK(!trie.search(long_word.substr(1)));

    // stress against a brute force: a set of words, scanned for prefix queries
    for (int iter = 0; iter < 30; iter++) {
        Trie mine;
        set<string> words;
        for (int op = 0; op < 200; op++) {
            string s = t::rand_string((int)t::rand_int(1, 5), 'a', 'c');
            int kind = (int)t::rand_int(0, 2);
            if (kind == 0) {
                mine.insert(s);
                words.insert(s);
            } else if (kind == 1) {
                CHECK_EQ(mine.search(s), words.count(s) > 0);
            } else {
                bool any = any_of(words.begin(), words.end(), [&](const string& w) { return w.starts_with(s); });
                CHECK_EQ(mine.startsWith(s), any);
            }
        }
    }
    return t::summary("0208-implement-trie-prefix-tree");
}
