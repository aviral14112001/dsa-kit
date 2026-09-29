// 49. Group Anagrams: https://leetcode.com/problems/group-anagrams/
// Pattern: group by a canonical signature (sorted letters, or letter counts). Module 07 section 1, section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        unordered_map<string, vector<string>> groups;   // signature -> the words that have it
        for (const string& s : strs) {
            string key = s;
            sort(key.begin(), key.end());               // anagrams, and only anagrams, share it
            groups[key].push_back(s);                   // [] creates the empty group on first sight
        }
        vector<vector<string>> result;
        result.reserve(groups.size());
        for (auto& [key, words] : groups) result.push_back(std::move(words));   // move, don't copy
        return result;
    }
};
// [/snippet]

// [snippet:count_signature]
// O(k) signature instead of O(k log k): the 26 letter counts, written out with separators.
// Without them, counts {1, 11} and {11, 1} would both read "111".
string count_signature(const string& s) {
    int count[26] = {};
    for (char c : s) count[c - 'a']++;
    string key;
    for (int c : count) key += to_string(c) + '#';
    return key;
}
// [/snippet]

class SolutionCounts {                           // the same grouping, keyed by letter counts
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        unordered_map<string, vector<string>> groups;
        for (const string& s : strs) groups[count_signature(s)].push_back(s);
        vector<vector<string>> result;
        for (auto& [key, words] : groups) result.push_back(std::move(words));
        return result;
    }
};

// Brute force for the stress test: compare each word with one member of every existing group.
vector<vector<string>> brute(const vector<string>& strs) {
    auto is_anagram = [](const string& a, const string& b) {
        if (a.size() != b.size()) return false;
        int diff[26] = {};
        for (size_t i = 0; i < a.size(); i++) diff[a[i] - 'a']++, diff[b[i] - 'a']--;
        return all_of(begin(diff), end(diff), [](int d) { return d == 0; });
    };
    vector<vector<string>> groups;
    for (const string& s : strs) {
        bool placed = false;
        for (auto& g : groups)
            if (is_anagram(g[0], s)) { g.push_back(s); placed = true; break; }
        if (!placed) groups.push_back({s});
    }
    return groups;
}

// LeetCode accepts the groups in any order: sort inside each group, then sort the groups.
vector<vector<string>> normalized(vector<vector<string>> groups) {
    for (auto& g : groups) sort(g.begin(), g.end());
    sort(groups.begin(), groups.end());
    return groups;
}

int main() {
    Solution sol;
    SolutionCounts counts;
    vector<string> ex1{"eat", "tea", "tan", "ate", "nat", "bat"}, ex2{""}, ex3{"a"};
    vector<vector<string>> want1{{"ate", "eat", "tea"}, {"bat"}, {"nat", "tan"}};
    CHECK_EQ(normalized(sol.groupAnagrams(ex1)), want1);
    CHECK_EQ(normalized(sol.groupAnagrams(ex2)), vector<vector<string>>{{""}});
    CHECK_EQ(normalized(sol.groupAnagrams(ex3)), vector<vector<string>>{{"a"}});
    CHECK_EQ(normalized(counts.groupAnagrams(ex1)), want1);

    vector<string> dups{"ab", "ba", "ab", "", ""};              // duplicates stay, empty strings group
    CHECK_EQ(normalized(sol.groupAnagrams(dups)), vector<vector<string>>{{"", ""}, {"ab", "ab", "ba"}});
    CHECK(count_signature("abbbbbbbbbbb") != count_signature("aaaaaaaaaaab"));   // {1, 11} vs {11, 1}
    CHECK_EQ(count_signature("bca"), count_signature("abc"));

    for (int iter = 0; iter < 300; iter++) {       // stress test vs the pairwise brute force
        int n = (int)t::rand_int(0, 12);
        vector<string> strs(n);
        for (auto& s : strs) s = t::rand_string((int)t::rand_int(0, 4), 'a', 'c');
        vector<vector<string>> expected = normalized(brute(strs));
        vector<string> copy1 = strs, copy2 = strs;
        CHECK_EQ(normalized(sol.groupAnagrams(copy1)), expected);
        CHECK_EQ(normalized(counts.groupAnagrams(copy2)), expected);
    }
    return t::summary("0049-group-anagrams");
}
