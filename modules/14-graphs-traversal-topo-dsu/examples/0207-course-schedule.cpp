// 207. Course Schedule: https://leetcode.com/problems/course-schedule/
// Pattern: cycle detection in a directed graph via topological sort (Kahn's algorithm). Module 14 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> adj(numCourses);
        vector<int> indegree(numCourses, 0);
        for (auto& p : prerequisites) {        // [a, b] = "take b before a": edge b -> a
            adj[p[1]].push_back(p[0]);
            indegree[p[0]]++;
        }
        queue<int> ready;                      // courses with every prerequisite already taken
        for (int c = 0; c < numCourses; c++)
            if (indegree[c] == 0) ready.push(c);
        int taken = 0;
        while (!ready.empty()) {
            int c = ready.front();
            ready.pop();
            taken++;
            for (int next : adj[c])
                if (--indegree[next] == 0) ready.push(next);
        }
        return taken == numCourses;            // courses on or behind a cycle never become ready
    }
};
// [/snippet]

// [snippet:dfs]
// The same answer with a three-colour DFS: 0 = unvisited, 1 = on the current path, 2 = finished.
class SolutionDFS {
    vector<vector<int>> adj;
    vector<int> state;
    bool has_cycle(int u) {
        state[u] = 1;
        for (int v : adj[u]) {
            if (state[v] == 1) return true;                 // back edge into the current path
            if (state[v] == 0 && has_cycle(v)) return true;
        }
        state[u] = 2;
        return false;
    }
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        adj.assign(numCourses, {});
        state.assign(numCourses, 0);
        for (auto& p : prerequisites) adj[p[1]].push_back(p[0]);
        for (int c = 0; c < numCourses; c++)
            if (state[c] == 0 && has_cycle(c)) return false;
        return true;
    }
};
// [/snippet]

// Brute force: some ordering of the courses respects every prerequisite.
bool brute(int n, const vector<vector<int>>& prereq) {
    vector<int> perm(n), pos(n);
    iota(perm.begin(), perm.end(), 0);
    do {
        for (int i = 0; i < n; i++) pos[perm[i]] = i;
        bool ok = true;
        for (auto& p : prereq) ok &= pos[p[1]] < pos[p[0]];
        if (ok) return true;
    } while (next_permutation(perm.begin(), perm.end()));
    return false;
}

int main() {
    Solution sol;
    SolutionDFS dfs;
    vector<vector<int>> ex1{{1, 0}};
    CHECK_EQ(sol.canFinish(2, ex1), true);
    vector<vector<int>> ex2{{1, 0}, {0, 1}};
    CHECK_EQ(sol.canFinish(2, ex2), false);
    vector<vector<int>> none;
    CHECK_EQ(sol.canFinish(3, none), true);              // no prerequisites at all
    vector<vector<int>> self{{0, 0}};
    CHECK_EQ(sol.canFinish(1, self), false);             // a course that requires itself
    vector<vector<int>> diamond{{1, 0}, {2, 0}, {3, 1}, {3, 2}};
    CHECK_EQ(sol.canFinish(4, diamond), true);           // 3 has two prerequisites, still no cycle
    CHECK_EQ(dfs.canFinish(4, diamond), true);
    vector<vector<int>> tail{{1, 0}, {2, 1}, {1, 2}, {3, 2}};
    CHECK_EQ(sol.canFinish(4, tail), false);             // 1 <-> 2 cycle also blocks 3
    CHECK_EQ(dfs.canFinish(4, tail), false);

    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 6);
        vector<vector<int>> prereq;
        int m = (int)t::rand_int(0, 8);
        for (int i = 0; i < m; i++) prereq.push_back({(int)t::rand_int(0, n - 1), (int)t::rand_int(0, n - 1)});
        bool expect = brute(n, prereq);
        CHECK_EQ(sol.canFinish(n, prereq), expect);
        CHECK_EQ(dfs.canFinish(n, prereq), expect);
    }
    // a 10^5-course chain: Kahn's queue has no recursion to overflow
    int n = 100000;
    vector<vector<int>> chain;
    for (int i = 1; i < n; i++) chain.push_back({i, i - 1});
    CHECK_EQ(sol.canFinish(n, chain), true);
    chain.push_back({0, n - 1});                          // close the loop
    CHECK_EQ(sol.canFinish(n, chain), false);
    return t::summary("0207-course-schedule");
}
