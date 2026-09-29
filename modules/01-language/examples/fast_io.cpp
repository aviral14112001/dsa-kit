// Fast I/O and the three input shapes OAs use: T test cases, a line containing spaces, and
// "numbers until the input ends". Module 01 section 4.
//     make run F=modules/01-language/examples/fast_io.cpp IN=modules/01-language/examples/fast_io.in
// Input: T; then T blocks of "n" and n integers; then one line of text; then integers until EOF.
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// [snippet:dbg]
#ifdef LOCAL   // `make run` passes -DLOCAL: dbg() prints to stderr on your machine, vanishes on the judge
#define dbg(x) cerr << #x << " = " << (x) << '\n'
#else
#define dbg(x)
#endif
// [/snippet]

// [snippet:solve]
void solve(int test_case) {
    int n;
    cin >> n;
    vector<ll> a(n);
    for (auto& x : a) cin >> x;
    ll sum = accumulate(a.begin(), a.end(), 0LL);
    dbg(sum);
    ll biggest = n > 0 ? *max_element(a.begin(), a.end()) : 0;
    cout << "Case #" << test_case << ": sum=" << sum << " max=" << biggest << '\n';   // '\n', not endl
}
// [/snippet]

int main() {
    // [snippet:main]
    ios::sync_with_stdio(false);   // stop syncing with C's stdio: cin/cout get much faster
    cin.tie(nullptr);              // stop flushing cout before every cin read
    int tests;
    cin >> tests;
    for (int tc = 1; tc <= tests; tc++) solve(tc);
    // [/snippet]

    // [snippet:getline]
    string line;
    getline(cin >> ws, line);      // >> left the last line's '\n' unread; ws skips it (and blank lines)
    stringstream words(line);
    int count = 0;
    string longest;
    for (string w; words >> w;) {  // >> splits on any run of spaces
        count++;
        if (w.size() > longest.size()) longest = w;   // strict >: the first longest word wins
    }
    cout << count << " words, longest: " << longest << '\n';
    // [/snippet]

    // [snippet:until_eof]
    ll value, numbers = 0, total = 0;
    while (cin >> value) {         // false at end of input (or at the first token that isn't a number)
        numbers++;
        total += value;
    }
    cout << "tail: " << numbers << " numbers, sum " << total << '\n';
    // [/snippet]
}
