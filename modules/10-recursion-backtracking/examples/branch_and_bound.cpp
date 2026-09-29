// Branch and bound on the assignment problem: n workers, n jobs, cost[w][j] = the cost of worker w
// doing job j. Give every worker a different job, minimising the total cost. Module 10 section 4.
// The search is the permutations template (Section 2); the bound is what makes it fast.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:branch_and_bound]
class AssignmentSolver {
public:
    int solve(const vector<vector<int>>& costs) {
        cost = costs;
        int n = cost.size();
        restMin.assign(n + 1, 0);           // restMin[w] = sum of the cheapest job of workers w..n-1
        for (int w = n - 1; w >= 0; w--)
            restMin[w] = restMin[w + 1] + *min_element(cost[w].begin(), cost[w].end());
        taken.assign(n, false);
        best = INT_MAX;
        nodes = 0;
        search(0, 0);
        return best;
    }

    long long nodes = 0;                    // search nodes visited, to measure the pruning

private:
    vector<vector<int>> cost;
    vector<int> restMin;
    vector<bool> taken;                     // taken[j]: job j already has a worker
    int best = INT_MAX;                     // cheapest complete assignment found so far

    void search(int w, int costSoFar) {     // workers 0..w-1 have jobs, costing costSoFar
        nodes++;
        int n = cost.size();
        if (w == n) {
            best = min(best, costSoFar);
            return;
        }
        // Bound: even if every remaining worker got their cheapest job (clashes ignored), this branch
        // costs at least costSoFar + restMin[w]. If that can't beat best, nothing below can: prune.
        if (costSoFar + restMin[w] >= best) return;
        for (int j = 0; j < n; j++) {       // branch: worker w tries every free job
            if (taken[j]) continue;
            taken[j] = true;
            search(w + 1, costSoFar + cost[w][j]);
            taken[j] = false;
        }
    }
};
// [/snippet]

// Brute force: every permutation of jobs. O(n! * n).
int brute(const vector<vector<int>>& cost) {
    int n = cost.size();
    vector<int> job(n);
    iota(job.begin(), job.end(), 0);
    int best = INT_MAX;
    do {
        int total = 0;
        for (int w = 0; w < n; w++) total += cost[w][job[w]];
        best = min(best, total);
    } while (next_permutation(job.begin(), job.end()));
    return best;
}

int main() {
    AssignmentSolver solver;
    CHECK_EQ(solver.solve({{4, 1, 3}, {2, 0, 5}, {3, 2, 2}}), 5);   // 1 + 2 + 2: w0->j1, w1->j0, w2->j2
    CHECK_EQ(solver.solve({{7}}), 7);
    CHECK_EQ(solver.solve({{1, 2}, {1, 2}}), 3);                     // both prefer job 0: one can't have it
    CHECK_EQ(solver.solve({{5, 5, 5}, {5, 5, 5}, {5, 5, 5}}), 15);  // all ties

    for (int iter = 0; iter < 300; iter++) {                         // stress test vs brute force
        int n = (int)t::rand_int(1, 6);
        vector<vector<int>> cost(n);
        for (auto& row : cost) row = t::rand_vec(n, 1, 20);
        CHECK_EQ(solver.solve(cost), brute(cost));
    }

    // How much the bound saves on an 8 x 8 instance. Without it, the search visits every partial
    // assignment: sum over d = 0..8 of 8!/(8-d)! = 109601 nodes.
    int n = 8;
    vector<vector<int>> cost(n, vector<int>(n));
    for (int w = 0; w < n; w++)
        for (int j = 0; j < n; j++) cost[w][j] = (w * 37 + j * 91 + w * j * 13) % 50 + 1;
    long long fullTree = 0, perms = 1;
    for (int d = 0; d <= n; d++) {
        fullTree += perms;                                           // n!/(n-d)! nodes at depth d
        perms *= n - d;
    }
    CHECK_EQ(fullTree, 109601LL);
    CHECK_EQ(solver.solve(cost), brute(cost));
    CHECK_EQ(solver.nodes, 1700LL);                                  // 64x fewer nodes
    return t::summary("branch_and_bound");
}
