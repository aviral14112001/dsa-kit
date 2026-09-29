// 1268. Search Suggestions System: https://leetcode.com/problems/search-suggestions-system/
// Pattern: autocomplete: a trie with top-3 lists per node, vs sort + lower_bound. Module 13 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace sorted_scan {
// [snippet:sorted]
class Solution {
public:
    vector<vector<string>> suggestedProducts(vector<string>& products, string searchWord) {
        sort(products.begin(), products.end());
        vector<vector<string>> result;
        string prefix;
        auto start = products.begin();      // first product >= prefix; it only ever moves right
        for (char ch : searchWord) {
            prefix += ch;
            // The words starting with prefix form one contiguous run in sorted order, beginning
            // at the first word >= prefix. A longer prefix is larger, so search from `start` on.
            start = lower_bound(start, products.end(), prefix);
            vector<string> suggestions;
            for (auto it = start; it != products.end() && suggestions.size() < 3; ++it) {
                if (it->compare(0, prefix.size(), prefix) != 0) break;   // left the run (C++20: starts_with)
                suggestions.push_back(*it);
            }
            result.push_back(suggestions);
        }
        return result;
    }
};
// [/snippet]
}  // namespace sorted_scan

namespace trie_top3 {
// [snippet:trie]
class Solution {
public:
    vector<vector<string>> suggestedProducts(vector<string>& products, string searchWord) {
        // Insert in sorted order: the first 3 words to pass through a node are its 3 smallest.
        sort(products.begin(), products.end());
        vector<Node> nodes(1);                               // pool: nodes[0] is the root
        for (int i = 0; i < (int)products.size(); i++) {
            int cur = 0;
            for (char ch : products[i]) {
                int c = ch - 'a';
                if (nodes[cur].child[c] == -1) {
                    nodes[cur].child[c] = (int)nodes.size(); // index first, then grow the pool
                    nodes.emplace_back();
                }
                cur = nodes[cur].child[c];
                if (nodes[cur].top3.size() < 3) nodes[cur].top3.push_back(i);
            }
        }
        vector<vector<string>> result;
        int cur = 0;                                         // -1 once the prefix has left the trie
        for (char ch : searchWord) {
            if (cur != -1) cur = nodes[cur].child[ch - 'a'];
            vector<string> suggestions;
            if (cur != -1)
                for (int i : nodes[cur].top3) suggestions.push_back(products[i]);
            result.push_back(suggestions);
        }
        return result;
    }

private:
    struct Node {
        array<int, 26> child;
        vector<int> top3;        // indices into the sorted products: 4 bytes each, not a string copy
        Node() { child.fill(-1); }
    };
};
// [/snippet]
}  // namespace trie_top3

// Brute force: for every prefix, filter, sort, keep the first 3.
vector<vector<string>> brute(vector<string> products, const string& search) {
    vector<vector<string>> result;
    for (size_t len = 1; len <= search.size(); len++) {
        string prefix = search.substr(0, len);
        vector<string> matches;
        for (const string& p : products)
            if (p.starts_with(prefix)) matches.push_back(p);
        sort(matches.begin(), matches.end());
        if (matches.size() > 3) matches.resize(3);
        result.push_back(matches);
    }
    return result;
}

int main() {
    sorted_scan::Solution a;
    trie_top3::Solution b;
    auto both = [&](vector<string> products, const string& search, const vector<vector<string>>& expected) {
        vector<string> copy = products;     // both solutions sort their input in place
        CHECK_EQ(a.suggestedProducts(products, search), expected);
        CHECK_EQ(b.suggestedProducts(copy, search), expected);
    };
    // official examples
    both({"mobile", "mouse", "moneypot", "monitor", "mousepad"}, "mouse",
         {{"mobile", "moneypot", "monitor"}, {"mobile", "moneypot", "monitor"},
          {"mouse", "mousepad"}, {"mouse", "mousepad"}, {"mouse", "mousepad"}});
    both({"havana"}, "havana",
         {{"havana"}, {"havana"}, {"havana"}, {"havana"}, {"havana"}, {"havana"}});
    // edge cases: the prefix leaves every word early; a word that is a prefix of the others
    both({"havana"}, "tatiana", {{}, {}, {}, {}, {}, {}, {}});
    both({"bags", "baggage", "banner", "box", "cloths"}, "bags",
         {{"baggage", "bags", "banner"}, {"baggage", "bags", "banner"}, {"baggage", "bags"}, {"bags"}});
    both({"ab", "abc", "abcd", "abcde"}, "abcdef",
         {{"ab", "abc", "abcd"}, {"ab", "abc", "abcd"}, {"abc", "abcd", "abcde"}, {"abcd", "abcde"}, {"abcde"}, {}});

    // stress against the brute force (distinct products, as the problem guarantees)
    for (int iter = 0; iter < 300; iter++) {
        set<string> distinct;
        int n = (int)t::rand_int(1, 15);
        while ((int)distinct.size() < n) distinct.insert(t::rand_string((int)t::rand_int(1, 5), 'a', 'c'));
        vector<string> products(distinct.begin(), distinct.end());
        shuffle(products.begin(), products.end(), t::rng());
        string search = t::rand_string((int)t::rand_int(1, 6), 'a', 'c');
        both(products, search, brute(products, search));
    }
    return t::summary("1268-search-suggestions-system");
}
