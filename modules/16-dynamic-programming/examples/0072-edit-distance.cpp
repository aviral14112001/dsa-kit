// 72. Edit Distance: https://leetcode.com/problems/edit-distance/
// Pattern: two-sequence table; insert / delete / replace are the three neighbour cells. Module 16 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int minDistance(string word1, string word2) {
        int n = word1.size(), m = word2.size();
        // dist[i][j] = fewest edits turning word1[0..i) into word2[0..j)
        vector<vector<int>> dist(n + 1, vector<int>(m + 1));
        for (int i = 0; i <= n; i++) dist[i][0] = i;       // delete all i chars
        for (int j = 0; j <= m; j++) dist[0][j] = j;       // insert all j chars
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= m; j++) {
                if (word1[i - 1] == word2[j - 1])
                    dist[i][j] = dist[i - 1][j - 1];       // the ends already agree: no edit
                else
                    dist[i][j] = 1 + min({dist[i - 1][j - 1],   // replace word1[i-1] by word2[j-1]
                                          dist[i - 1][j],       // delete word1[i-1]
                                          dist[i][j - 1]});     // insert word2[j-1] at the end
            }
        return dist[n][m];
    }
};
// [/snippet]

// Brute force 1, straight from the definition: BFS over strings, one edit per edge.
// An optimal script can do its deletions first, then replacements, then insertions, so the search
// never needs strings longer than max(|a|, |b|) or letters outside a and b.
int bruteBfs(const string& a, const string& b) {
    string letters;
    for (char c : a + b)
        if (letters.find(c) == string::npos) letters += c;
    size_t maxLen = max(a.size(), b.size());
    map<string, int> dist{{a, 0}};
    queue<string> q;
    q.push(a);
    while (!q.empty()) {
        string s = q.front();
        q.pop();
        int d = dist[s];
        if (s == b) return d;
        vector<string> next;
        for (size_t i = 0; i < s.size(); i++) {
            next.push_back(s.substr(0, i) + s.substr(i + 1));                   // delete s[i]
            for (char c : letters)
                if (c != s[i]) next.push_back(s.substr(0, i) + c + s.substr(i + 1));   // replace s[i]
        }
        if (s.size() < maxLen)
            for (size_t i = 0; i <= s.size(); i++)
                for (char c : letters) next.push_back(s.substr(0, i) + c + s.substr(i));   // insert
        for (const string& nx : next)
            if (dist.emplace(nx, d + 1).second) q.push(nx);
    }
    return -1;   // not reached: b itself is inside the search space
}

// Brute force 2: the recurrence with no memo (exponential), for slightly longer strings.
int bruteRec(const string& a, const string& b, int i, int j) {
    if (i == 0) return j;
    if (j == 0) return i;
    if (a[i - 1] == b[j - 1]) return bruteRec(a, b, i - 1, j - 1);
    return 1 + min({bruteRec(a, b, i - 1, j - 1), bruteRec(a, b, i - 1, j), bruteRec(a, b, i, j - 1)});
}

int main() {
    Solution sol;
    CHECK_EQ(sol.minDistance("horse", "ros"), 3);            // official examples
    CHECK_EQ(sol.minDistance("intention", "execution"), 5);

    CHECK_EQ(sol.minDistance("", ""), 0);                    // edge cases: empty strings are allowed here
    CHECK_EQ(sol.minDistance("", "abc"), 3);
    CHECK_EQ(sol.minDistance("abc", ""), 3);
    CHECK_EQ(sol.minDistance("abc", "abc"), 0);
    CHECK_EQ(sol.minDistance("a", "b"), 1);
    CHECK_EQ(sol.minDistance("ab", "ba"), 2);                // no "swap" operation
    CHECK_EQ(sol.minDistance(string(500, 'a'), string(500, 'b')), 500);   // largest input

    for (int iter = 0; iter < 300; iter++) {                 // stress test vs BFS over the edit graph
        string a = t::rand_string((int)t::rand_int(0, 4), 'a', 'c');
        string b = t::rand_string((int)t::rand_int(0, 4), 'a', 'c');
        CHECK_EQ(sol.minDistance(a, b), bruteBfs(a, b));
    }
    for (int iter = 0; iter < 200; iter++) {                 // longer strings vs the plain recursion
        string a = t::rand_string((int)t::rand_int(0, 6), 'a', 'd');
        string b = t::rand_string((int)t::rand_int(0, 6), 'a', 'd');
        CHECK_EQ(sol.minDistance(a, b), bruteRec(a, b, (int)a.size(), (int)b.size()));
    }
    return t::summary("0072-edit-distance");
}
