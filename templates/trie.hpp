// A trie (prefix tree) over lowercase words, with every node in one vector and int child indices
// (module 13 section 3).
//
//   Trie tr;
//   tr.insert("car"); tr.insert("cart"); tr.insert("car");     duplicates are counted
//   tr.contains("car")      -> true        tr.starts_with("ca") -> true
//   tr.count_prefix("car")  -> 3           (car, car, cart)
//   tr.erase("car")         -> true        removes one copy; false if the word isn't stored
//
// Why a pool instead of `new Node` per node: one buffer that grows geometrically instead of one
// allocation per node, 4-byte indices instead of 8-byte pointers (a node is ~half the size),
// indices stay valid when the vector reallocates, and resetting the whole trie is one assign.
// Words must be lowercase a-z: `ch - 'a'` is the child slot.
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:trie]
struct Trie {
    struct Node {
        array<int, 26> child;   // child[c] = index in `nodes` of the node for (this prefix + c), or -1
        int pass = 0;           // stored words whose path runs through here = words with this prefix
        int end = 0;            // stored words that end exactly here (a count, so duplicates work)
        Node() { child.fill(-1); }
    };
    vector<Node> nodes = {Node()};   // nodes[0] is the root: the empty prefix, passed by every word

    void insert(const string& word) {                 // O(L)
        int cur = 0;
        nodes[cur].pass++;
        for (char ch : word) {
            int c = ch - 'a';
            if (nodes[cur].child[c] == -1) {
                nodes[cur].child[c] = (int)nodes.size();   // the index the new node is about to get
                nodes.emplace_back();                      // may reallocate: never hold a Node& across this
            }
            cur = nodes[cur].child[c];
            nodes[cur].pass++;
        }
        nodes[cur].end++;
    }

    // Index of the node spelling s, or -1 if the path leaves the trie. After erases the node may
    // be dead (pass == 0), so the queries below look at the counts, not just at the index.
    int find(const string& s) const {                 // O(L)
        int cur = 0;
        for (char ch : s) {
            cur = nodes[cur].child[ch - 'a'];
            if (cur == -1) return -1;
        }
        return cur;
    }

    bool contains(const string& word) const {
        int v = find(word);
        return v != -1 && nodes[v].end > 0;           // the path exists AND a word ends there
    }

    int count_prefix(const string& prefix) const {    // stored words (with repeats) starting with prefix
        int v = find(prefix);
        return v == -1 ? 0 : nodes[v].pass;
    }

    bool starts_with(const string& prefix) const { return count_prefix(prefix) > 0; }
// [/snippet]

// [snippet:erase]
    // Removes one copy of word: undo exactly the increments its insert made. Nodes are not freed:
    // a node whose pass drops to 0 is dead, and a later insert through it brings it back.
    bool erase(const string& word) {                  // O(L)
        if (!contains(word)) return false;            // a missing word must not touch any count
        int cur = 0;
        nodes[cur].pass--;
        for (char ch : word) {
            cur = nodes[cur].child[ch - 'a'];
            nodes[cur].pass--;
        }
        nodes[cur].end--;
        return true;
    }
};
// [/snippet]
