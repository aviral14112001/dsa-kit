// CSES 1068. Weird Algorithm: https://cses.fi/problemset/task/1068
// Pattern: direct simulation + fast I/O; long long because the values climb past 2^31. Module 01 section 1.
//     make run F=modules/01-language/examples/cses-1068-weird-algorithm.cpp IN=modules/01-language/examples/cses-1068-weird-algorithm.in
// Sample files: .in is the official sample; .2.in starts at 704511, whose sequence peaks at
// 56,991,483,520 (the highest peak of any start up to 10^6); .3.in is n = 1.
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// [snippet:solution]
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ll n;                     // NOT int: starting from 113383 (or higher), 3n + 1 passes 2^31 - 1
    cin >> n;
    cout << n;
    while (n != 1) {
        n = (n % 2 == 0) ? n / 2 : 3 * n + 1;
        cout << ' ' << n;     // one line, space-separated; '\n' only once at the end
    }
    cout << '\n';
}
// [/snippet]
