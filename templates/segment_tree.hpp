// Segment trees. Module 17 section 3.
//   SegTree       iterative bottom-up tree over any monoid: point set, range query
//   SumSegTree    the classic recursive 4n tree: point update, range sum
//   LazySegTree   range add + range assign, range sum / min / max, first index with value >= x
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:iterative]
// Bottom-up segment tree over a monoid: an associative op plus its identity element
// (+ with 0, min with INT_MAX, max with INT_MIN, gcd with 0, ...). Leaves live in t[n .. 2n-1] and
// node p = op(t[2p], t[2p+1]). Any n works; no padding to a power of two.
//     SegTree st(a, INT_MAX, [](int x, int y) { return min(x, y); });     // range min over vector<int> a
template <class T, class Op>
struct SegTree {
    int n;
    T identity;
    Op op;
    vector<T> t;

    SegTree(const vector<T>& a, T identity, Op op)
        : n((int)a.size()), identity(identity), op(op), t(2 * n, identity) {
        copy(a.begin(), a.end(), t.begin() + n);
        for (int p = n - 1; p >= 1; p--) t[p] = op(t[2 * p], t[2 * p + 1]);
    }

    void set(int i, T value) {                        // a[i] = value, then recompute every ancestor
        int p = i + n;
        t[p] = value;
        for (p /= 2; p >= 1; p /= 2) t[p] = op(t[2 * p], t[2 * p + 1]);
    }

    T query(int l, int r) const {                     // op over a[l..r], inclusive; identity if l > r
        T left = identity, right = identity;          // two accumulators keep non-commutative ops in order
        for (l += n, r += n + 1; l < r; l /= 2, r /= 2) {   // nodes [l, r) at this level are still owed
            if (l & 1) left = op(left, t[l++]);       // l is a right child: its parent would stick out left
            if (r & 1) right = op(t[--r], right);     // r-1 is a left child whose sibling r is outside
        }
        return op(left, right);
    }
};
// [/snippet]

// [snippet:recursive]
// The classic recursive tree: node 1 is the root and covers [0, n-1]; node p covering [lo, hi] has
// children 2p = [lo, mid] and 2p+1 = [mid+1, hi]. 4n slots always suffice: the depth d is
// ceil(log2 n), so every index is below 2^(d+1) < 4n.
struct SumSegTree {
    int n;
    vector<long long> sum;

    explicit SumSegTree(const vector<long long>& a) : n((int)a.size()), sum(4 * max(n, 1)) {
        if (n > 0) build(1, 0, n - 1, a);
    }
    void update(int i, long long value) { update(1, 0, n - 1, i, value); }       // a[i] = value
    long long query(int l, int r) const { return query(1, 0, n - 1, l, r); }     // a[l..r], inclusive

private:
    void build(int p, int lo, int hi, const vector<long long>& a) {
        if (lo == hi) { sum[p] = a[lo]; return; }
        int mid = (lo + hi) / 2;
        build(2 * p, lo, mid, a);
        build(2 * p + 1, mid + 1, hi, a);
        sum[p] = sum[2 * p] + sum[2 * p + 1];
    }
    void update(int p, int lo, int hi, int i, long long value) {
        if (lo == hi) { sum[p] = value; return; }
        int mid = (lo + hi) / 2;
        if (i <= mid) update(2 * p, lo, mid, i, value);
        else update(2 * p + 1, mid + 1, hi, i, value);
        sum[p] = sum[2 * p] + sum[2 * p + 1];         // recompute on the way back up
    }
    long long query(int p, int lo, int hi, int l, int r) const {
        if (r < lo || hi < l) return 0;               // disjoint: contributes the identity
        if (l <= lo && hi <= r) return sum[p];        // fully inside: use the stored answer
        int mid = (lo + hi) / 2;                      // partial overlap: ask both children
        return query(2 * p, lo, mid, l, r) + query(2 * p + 1, mid + 1, hi, l, r);
    }
};
// [/snippet]

// Lazy propagation: range add and range assign, with range sum, min and max, all O(log n).
struct LazySegTree {
// [snippet:lazy_types]
    // What a node knows about its whole range. It already includes the node's own pending tag.
    struct Node { long long sum = 0, mn = 0, mx = 0; };
    // A promise to the node's children, not yet delivered: every element x below this node
    // becomes (has_set ? set_to : x) + add.
    struct Tag { bool has_set = false; long long set_to = 0, add = 0; };
// [/snippet]

    int n;
    vector<Node> node;
    vector<Tag> tag;

    explicit LazySegTree(const vector<long long>& a)
        : n((int)a.size()), node(4 * max(n, 1)), tag(4 * max(n, 1)) {
        if (n > 0) build(1, 0, n - 1, a);
    }

    // All ranges are inclusive, 0 <= l <= r < n.
    void range_add(int l, int r, long long delta) { update(1, 0, n - 1, l, r, Tag{false, 0, delta}); }
    void range_assign(int l, int r, long long value) { update(1, 0, n - 1, l, r, Tag{true, value, 0}); }
    long long range_sum(int l, int r) { return query(1, 0, n - 1, l, r).sum; }
    long long range_min(int l, int r) { return query(1, 0, n - 1, l, r).mn; }
    long long range_max(int l, int r) { return query(1, 0, n - 1, l, r).mx; }
    int first_at_least(int from, long long x) { return descend(1, 0, n - 1, from, x); }   // or -1

private:
    static Node merge(const Node& a, const Node& b) {
        return {a.sum + b.sum, min(a.mn, b.mn), max(a.mx, b.mx)};
    }

    void build(int p, int lo, int hi, const vector<long long>& a) {
        if (lo == hi) { node[p] = {a[lo], a[lo], a[lo]}; return; }
        int mid = (lo + hi) / 2;
        build(2 * p, lo, mid, a);
        build(2 * p + 1, mid + 1, hi, a);
        node[p] = merge(node[2 * p], node[2 * p + 1]);
    }

// [snippet:lazy_apply]
    // Apply f to all len elements under node p: fix p's aggregates now, and compose f after p's
    // pending tag for the children to receive later. f happens AFTER the old tag, so an assign
    // replaces the old tag completely (pending adds die with it), while an add stacks on top.
    void apply(int p, int len, Tag f) {               // f by value: push() passes tag[p] itself
        if (f.has_set) {
            node[p] = {f.set_to * len, f.set_to, f.set_to};
            tag[p] = {true, f.set_to, 0};
        }
        if (f.add != 0) {
            node[p].sum += f.add * len;
            node[p].mn += f.add;
            node[p].mx += f.add;
            tag[p].add += f.add;
        }
    }

    // Deliver p's pending tag to its children. Call it before recursing below p.
    void push(int p, int lo, int hi) {
        int mid = (lo + hi) / 2;
        apply(2 * p, mid - lo + 1, tag[p]);
        apply(2 * p + 1, hi - mid, tag[p]);
        tag[p] = Tag{};
    }

    void update(int p, int lo, int hi, int l, int r, const Tag& f) {
        if (r < lo || hi < l) return;
        if (l <= lo && hi <= r) { apply(p, hi - lo + 1, f); return; }   // covered: stop, stay lazy
        push(p, lo, hi);
        int mid = (lo + hi) / 2;
        update(2 * p, lo, mid, l, r, f);
        update(2 * p + 1, mid + 1, hi, l, r, f);
        node[p] = merge(node[2 * p], node[2 * p + 1]);
    }
// [/snippet]

    Node query(int p, int lo, int hi, int l, int r) {     // needs [l, r] to overlap [lo, hi]
        if (l <= lo && hi <= r) return node[p];
        push(p, lo, hi);
        int mid = (lo + hi) / 2;
        if (r <= mid) return query(2 * p, lo, mid, l, r);
        if (l > mid) return query(2 * p + 1, mid + 1, hi, l, r);
        return merge(query(2 * p, lo, mid, l, r), query(2 * p + 1, mid + 1, hi, l, r));
    }

// [snippet:descent]
    // first_at_least(from, x) = descend(1, 0, n - 1, from, x): the first index i >= from with
    // a[i] >= x, or -1. A subtree whose max is below x cannot hold the answer, so it is rejected
    // without looking inside. The search follows the path to `from`, rejects at most one sibling
    // per level, then walks down one path inside the first subtree that passes: O(log n).
    int descend(int p, int lo, int hi, int from, long long x) {
        if (hi < from || node[p].mx < x) return -1;
        if (lo == hi) return lo;
        push(p, lo, hi);
        int mid = (lo + hi) / 2;
        int found = descend(2 * p, lo, mid, from, x);            // leftmost first
        if (found == -1) found = descend(2 * p + 1, mid + 1, hi, from, x);
        return found;
    }
// [/snippet]
};
