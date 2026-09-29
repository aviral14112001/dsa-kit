// Stdin/stdout template for OAs (HackerRank, HackerEarth, Codeforces, CSES...).
//     make run F=practice/cp/weird.cpp IN=practice/cp/weird.in
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

#ifdef LOCAL   // `make run` defines LOCAL, so dbg() prints to stderr locally and vanishes on the judge
#define dbg(x) cerr << #x << " = " << (x) << '\n'
#else
#define dbg(x)
#endif

void solve() {
    int n;
    cin >> n;
    vector<ll> a(n);
    for (auto& x : a) cin >> x;
    cout << accumulate(a.begin(), a.end(), 0LL) << '\n';   // 0LL, not 0: the sum is 64-bit
}

int main() {
    ios::sync_with_stdio(false);   // stop syncing with C stdio: several times faster input
    cin.tie(nullptr);              // don't flush cout before every cin
    int tests = 1;
    // cin >> tests;               // uncomment when the input starts with the number of test cases
    while (tests--) solve();
}
