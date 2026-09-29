// 51. N-Queens: https://leetcode.com/problems/n-queens/
// Pattern: constraint backtracking, one queen per row; columns and both diagonals as O(1) lookups.
// Module 10 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<vector<string>> solveNQueens(int n) {
        board.assign(n, string(n, '.'));
        colUsed.assign(n, false);
        diagUsed.assign(2 * n - 1, false);      // "\" diagonals: r - c is constant (shifted by n - 1)
        antiUsed.assign(2 * n - 1, false);      // "/" diagonals: r + c is constant
        solutions.clear();
        placeRow(0);
        return solutions;
    }

private:
    vector<string> board;
    vector<bool> colUsed, diagUsed, antiUsed;
    vector<vector<string>> solutions;

    // Rows 0..r-1 hold one queen each, none attacking another. Place a queen in row r.
    void placeRow(int r) {
        int n = board.size();
        if (r == n) {                            // a queen in every row: a full solution
            solutions.push_back(board);
            return;
        }
        for (int c = 0; c < n; c++) {
            int d = r - c + n - 1, a = r + c;
            if (colUsed[c] || diagUsed[d] || antiUsed[a]) continue;   // attacked: O(1) check
            board[r][c] = 'Q';                                        // choose
            colUsed[c] = diagUsed[d] = antiUsed[a] = true;
            placeRow(r + 1);                                          // explore
            colUsed[c] = diagUsed[d] = antiUsed[a] = false;           // un-choose: undo every
            board[r][c] = '.';                                        // change made above
        }
    }
};
// [/snippet]

// Independent validator: n queens, one per row and column, no two on a shared diagonal.
bool validBoard(const vector<string>& b) {
    int n = b.size();
    vector<pair<int, int>> queens;
    for (int r = 0; r < n; r++) {
        if ((int)b[r].size() != n) return false;
        for (int c = 0; c < n; c++)
            if (b[r][c] == 'Q') queens.push_back({r, c});
            else if (b[r][c] != '.') return false;
    }
    if ((int)queens.size() != n) return false;
    for (size_t i = 0; i < queens.size(); i++)
        for (size_t j = i + 1; j < queens.size(); j++) {
            auto [r1, c1] = queens[i];
            auto [r2, c2] = queens[j];
            if (r1 == r2 || c1 == c2 || abs(r1 - r2) == abs(c1 - c2)) return false;
        }
    return true;
}

// The same search, counting nodes (calls) instead of building boards. Quoted in Notes section 3.
long long countNodes(int n, int r, vector<bool>& col, vector<bool>& diag, vector<bool>& anti) {
    long long nodes = 1;
    if (r == n) return nodes;
    for (int c = 0; c < n; c++) {
        int d = r - c + n - 1, a = r + c;
        if (col[c] || diag[d] || anti[a]) continue;
        col[c] = diag[d] = anti[a] = true;
        nodes += countNodes(n, r + 1, col, diag, anti);
        col[c] = diag[d] = anti[a] = false;
    }
    return nodes;
}

int main() {
    Solution sol;
    vector<bool> col(8), diag(15), anti(15);
    CHECK_EQ(countNodes(8, 0, col, diag, anti), 2057LL);   // vs 8! = 40320 column orders

    CHECK_EQ(sol.solveNQueens(4), vector<vector<string>>{{".Q..", "...Q", "Q...", "..Q."},
                                                          {"..Q.", "Q...", "...Q", ".Q.."}});
    CHECK_EQ(sol.solveNQueens(1), vector<vector<string>>{{"Q"}});
    CHECK(sol.solveNQueens(2).empty());
    CHECK(sol.solveNQueens(3).empty());

    // Known solution counts for n = 1..9; every board valid; no board twice.
    const int expectedCount[10] = {0, 1, 0, 0, 2, 10, 4, 40, 92, 352};
    for (int n = 1; n <= 9; n++) {
        auto sols = sol.solveNQueens(n);                 // also checks that state resets between calls
        CHECK_EQ(sols.size(), expectedCount[n]);
        bool allValid = all_of(sols.begin(), sols.end(), validBoard);
        CHECK(allValid);
        set<vector<string>> distinct(sols.begin(), sols.end());
        CHECK_EQ(distinct.size(), sols.size());
    }
    return t::summary("0051-n-queens");
}
