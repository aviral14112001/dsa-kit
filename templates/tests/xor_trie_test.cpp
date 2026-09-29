#include <bits/stdc++.h>
#include "test.hpp"
#include "xor_trie.hpp"
using namespace std;

int main() {
    // Fixed cases: the LeetCode 421 examples, answered by querying each value against the rest.
    auto best_pair = [](const vector<int>& nums) {
        XorTrie trie((int)nums.size());
        int best = 0;
        for (int x : nums) {
            trie.insert(x);
            best = max(best, trie.max_xor(x));
        }
        return best;
    };
    CHECK_EQ(best_pair({3, 10, 5, 25, 2, 8}), 28);
    CHECK_EQ(best_pair({14, 70, 53, 83, 49, 91, 36, 80, 92, 51, 66, 70}), 127);
    CHECK_EQ(best_pair({0}), 0);
    CHECK_EQ(best_pair({0, INT_MAX}), INT_MAX);

    // Erase via counts: after removing 25, the best partner for 5 changes.
    XorTrie trie;
    CHECK(trie.empty());
    for (int x : {3, 10, 5, 25}) trie.insert(x);
    CHECK_EQ(trie.max_xor(5), 28);                    // 5 ^ 25
    trie.erase(25);
    CHECK_EQ(trie.max_xor(5), 15);                    // 5 ^ 10
    trie.insert(10);                                  // duplicates are counted
    trie.erase(10);
    CHECK_EQ(trie.max_xor(5), 15);
    trie.erase(10);
    CHECK_EQ(trie.max_xor(5), 6);                     // only 3 and 5 left: 5 ^ 3
    trie.erase(3);
    trie.erase(5);
    CHECK(trie.empty());

    // count_xor_less on {1, 2, 3, 4} against x = 1: XORs are 0, 3, 2, 5.
    XorTrie small;
    for (int x : {1, 2, 3, 4}) small.insert(x);
    CHECK_EQ(count_xor_less(small, 1, 0), 0);
    CHECK_EQ(count_xor_less(small, 1, 1), 1);         // only 0
    CHECK_EQ(count_xor_less(small, 1, 3), 2);         // 0 and 2
    CHECK_EQ(count_xor_less(small, 1, 4), 3);
    CHECK_EQ(count_xor_less(small, 1, 6), 4);
    CHECK_EQ(count_xor_less(small, 1, INT_MAX), 4);
    CHECK_EQ(count_xor_less(XorTrie(), 5, 100), 0);   // empty trie

    // Stress: a random multiset under inserts and erases; max_xor and count_xor_less against brute
    // force. Half the rounds use small values (many shared paths), half use the full int range.
    for (int iter = 0; iter < 200; iter++) {
        int hi = (iter % 2) ? INT_MAX : 63;
        XorTrie tr;
        multiset<int> stored;
        for (int op = 0; op < 60; op++) {
            if (stored.empty() || t::rand_int(0, 2) > 0) {
                int x = (int)t::rand_int(0, hi);
                tr.insert(x);
                stored.insert(x);
            } else {
                auto it = next(stored.begin(), t::rand_int(0, (int)stored.size() - 1));
                tr.erase(*it);
                stored.erase(it);
            }
            CHECK_EQ(tr.empty(), stored.empty());
            if (stored.empty()) continue;
            int x = (int)t::rand_int(0, hi);
            int k = (int)t::rand_int(0, hi);
            int best = 0, below = 0;
            for (int y : stored) {
                best = max(best, x ^ y);
                below += (x ^ y) < k;
            }
            CHECK_EQ(tr.max_xor(x), best);
            CHECK_EQ(count_xor_less(tr, x, k), below);
        }
    }
    return t::summary("xor_trie");
}
