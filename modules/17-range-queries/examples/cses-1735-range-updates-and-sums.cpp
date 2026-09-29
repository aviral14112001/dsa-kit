// CSES 1735. Range Updates and Sums: https://cses.fi/problemset/task/1735
// Pattern: lazy segment tree with two kinds of update; an assign overrides pending adds. Module 17 section 3.
// Tested by feeding cses-1735-range-updates-and-sums*.in and diffing against the .out files.
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// [snippet:solution]
// Pending update on a node: every element x below it becomes (has_set ? set_to : x) + add.
struct Tag {
    bool has_set = false;
    ll set_to = 0, add = 0;
};

struct LazySum {
    int n;
    vector<ll> sum;                                // sum[p] already includes p's own pending tag
    vector<Tag> tag;

    explicit LazySum(const vector<ll>& a) : n((int)a.size()), sum(4 * n), tag(4 * n) { build(1, 0, n - 1, a); }

    void build(int p, int lo, int hi, const vector<ll>& a) {
        if (lo == hi) { sum[p] = a[lo]; return; }
        int mid = (lo + hi) / 2;
        build(2 * p, lo, mid, a);
        build(2 * p + 1, mid + 1, hi, a);
        sum[p] = sum[2 * p] + sum[2 * p + 1];
    }

    // f happens after whatever p has pending.
    void apply(int p, int len, Tag f) {
        if (f.has_set) {                           // assign: the old tag, pending adds included, is void
            sum[p] = f.set_to * len;
            tag[p] = {true, f.set_to, 0};
        }
        if (f.add != 0) {                          // add: stacks on top of whatever is pending
            sum[p] += f.add * len;
            tag[p].add += f.add;
        }
    }

    void push(int p, int lo, int hi) {             // hand p's pending tag to its children
        int mid = (lo + hi) / 2;
        apply(2 * p, mid - lo + 1, tag[p]);
        apply(2 * p + 1, hi - mid, tag[p]);
        tag[p] = Tag{};
    }

    void update(int p, int lo, int hi, int l, int r, const Tag& f) {
        if (r < lo || hi < l) return;
        if (l <= lo && hi <= r) { apply(p, hi - lo + 1, f); return; }
        push(p, lo, hi);
        int mid = (lo + hi) / 2;
        update(2 * p, lo, mid, l, r, f);
        update(2 * p + 1, mid + 1, hi, l, r, f);
        sum[p] = sum[2 * p] + sum[2 * p + 1];
    }

    ll query(int p, int lo, int hi, int l, int r) {
        if (r < lo || hi < l) return 0;
        if (l <= lo && hi <= r) return sum[p];
        push(p, lo, hi);
        int mid = (lo + hi) / 2;
        return query(2 * p, lo, mid, l, r) + query(2 * p + 1, mid + 1, hi, l, r);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, q;
    cin >> n >> q;
    vector<ll> a(n);
    for (ll& x : a) cin >> x;
    LazySum tree(a);
    while (q--) {
        int type, l, r;
        cin >> type >> l >> r;
        l--;                                       // 1-based input
        r--;
        if (type == 3) {
            cout << tree.query(1, 0, n - 1, l, r) << '\n';
            continue;
        }
        ll x;
        cin >> x;
        if (type == 1) tree.update(1, 0, n - 1, l, r, Tag{false, 0, x});   // add x
        else tree.update(1, 0, n - 1, l, r, Tag{true, x, 0});              // set to x
    }
}
// [/snippet]
