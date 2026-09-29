// CSES 1647. Static Range Minimum Queries: https://cses.fi/problemset/task/1647
// Pattern: sparse table (min is idempotent, so two overlapping blocks answer a query). Module 17 section 1.
// Tested by feeding cses-1647-static-range-minimum-queries*.in and diffing against the .out files.
#include <bits/stdc++.h>
using namespace std;

// [snippet:solution]
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, q;
    cin >> n >> q;

    vector<int> lg(n + 1, 0);                      // lg[len] = floor(log2(len))
    for (int len = 2; len <= n; len++) lg[len] = lg[len / 2] + 1;

    vector<vector<int>> mn(lg[n] + 1, vector<int>(n));   // mn[k][i] = min of the 2^k values from i
    for (int& x : mn[0]) cin >> x;
    for (int k = 1; k <= lg[n]; k++)
        for (int i = 0; i + (1 << k) <= n; i++)
            mn[k][i] = min(mn[k - 1][i], mn[k - 1][i + (1 << (k - 1))]);

    while (q--) {
        int a, b;
        cin >> a >> b;
        a--;                                       // CSES positions are 1-based
        b--;
        int k = lg[b - a + 1];                     // two blocks of 2^k cover [a, b]
        cout << min(mn[k][a], mn[k][b - (1 << k) + 1]) << '\n';
    }
}
// [/snippet]
