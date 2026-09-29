// Binary trie over the bits of non-negative ints: max XOR against a query value, how many stored
// values have XOR below a bound, and deletions via counts. Module 18 section 1.
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:xor_trie]
// Each value is a root-to-leaf path of B bits, most significant first. Nodes live in one pool
// (vectors indexed by int): no pointers, no `new` per node, at most 1 + B * (number of inserts) nodes.
struct XorTrie {
    static constexpr int B = 31;              // bits 30..0: every non-negative int
    vector<array<int, 2>> child{{0, 0}};      // node 0 is the root; child id 0 means "no child"
    vector<int> pass{0};                      // pass[v] = how many stored values run through node v

    explicit XorTrie(int expected_inserts = 0) {
        child.reserve(1 + (size_t)expected_inserts * B);
        pass.reserve(1 + (size_t)expected_inserts * B);
    }

    void insert(int x) { walk(x, +1); }
    void erase(int x) { walk(x, -1); }        // x must be stored; its nodes stay in the pool with lower counts
    bool empty() const { return pass[0] == 0; }

    // max over stored y of (x ^ y). The trie must not be empty.
    // Greedy from the top bit: a 1 at bit b is worth more than all lower bits together (2^b > 2^b - 1),
    // so take the child with the opposite bit whenever some stored value lives there.
    int max_xor(int x) const {
        int v = 0, result = 0;
        for (int b = B - 1; b >= 0; b--) {
            int want = ((x >> b) & 1) ^ 1;
            int next = child[v][want];
            if (next != 0 && pass[next] > 0) {
                result |= 1 << b;
                v = next;
            } else {
                v = child[v][want ^ 1];
            }
        }
        return result;
    }

private:
    void walk(int x, int delta) {
        int v = 0;
        pass[v] += delta;
        for (int b = B - 1; b >= 0; b--) {
            int bit = (x >> b) & 1;
            if (child[v][bit] == 0) {
                child[v][bit] = (int)child.size();    // assign first, then grow: push_back may move the pool
                child.push_back({0, 0});
                pass.push_back(0);
            }
            v = child[v][bit];
            pass[v] += delta;
        }
    }
};
// [/snippet]

// [snippet:count_less]
// How many stored y have (x ^ y) < k. Walk down keeping x ^ y equal to k on the bits seen so far.
// Where k has a 1, every y whose bit matches x's makes x ^ y smaller than k right here, whatever the
// lower bits are: count that whole subtree, then continue on the side that keeps the tie. O(B).
inline int count_xor_less(const XorTrie& trie, int x, int k) {
    int v = 0, count = 0;
    for (int b = XorTrie::B - 1; b >= 0; b--) {
        int xb = (x >> b) & 1, kb = (k >> b) & 1;
        if (kb == 1) {
            int smaller = trie.child[v][xb];          // x ^ y has a 0 here while k has a 1
            if (smaller != 0) count += trie.pass[smaller];
            v = trie.child[v][xb ^ 1];                // x ^ y has a 1 here, like k: still tied
        } else {
            v = trie.child[v][xb];                    // x ^ y must have a 0 here to stay <= k
        }
        if (v == 0) break;                            // no stored value continues the tie
    }
    return count;                                     // a tie all the way down means x ^ y == k: not counted
}
// [/snippet]
