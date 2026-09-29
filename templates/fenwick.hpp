// Fenwick tree (binary indexed tree): point add + prefix sums in O(log n), order statistics by
// binary lifting, and a range-add / range-sum variant built from two of them. Module 17 section 2.
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:fenwick]
// Public indices are 0-based. Inside, tree[] is 1-based so the lowbit arithmetic works:
// tree[j] = sum of a over the 1-based positions (j - lowbit(j), j], where lowbit(j) = j & -j.
template <class T = long long>
struct Fenwick {
    int n;
    vector<T> tree;

    explicit Fenwick(int n) : n(n), tree(n + 1, T{}) {}

    // O(n) build: put each value in its own slot, then push each finished block into the next
    // block that contains it (j + lowbit(j)), instead of n separate O(log n) add() calls.
    explicit Fenwick(const vector<T>& a) : Fenwick((int)a.size()) {
        for (int j = 1; j <= n; j++) {
            tree[j] += a[j - 1];
            int parent = j + (j & -j);
            if (parent <= n) tree[parent] += tree[j];
        }
    }

    void add(int i, T delta) {                       // a[i] += delta
        for (int j = i + 1; j <= n; j += j & -j)     // climb to every block that contains position i
            tree[j] += delta;
    }

    T prefix(int k) const {                          // a[0] + ... + a[k-1]: the first k elements
        T sum{};
        for (int j = k; j > 0; j -= j & -j)          // peel off disjoint blocks, right to left
            sum += tree[j];
        return sum;
    }

    T range_sum(int l, int r) const {                // a[l] + ... + a[r], inclusive
        return prefix(r + 1) - prefix(l);
    }

    // Smallest i with a[0] + ... + a[i] >= target, or n if the total is smaller. Needs every a[i] >= 0
    // (prefix sums never decrease). Binary lifting: pos only grows by decreasing powers of two, so
    // tree[pos + step] is exactly the block (pos, pos + step]; take it while the sum stays below target.
    // With a[v] = count of value v, lower_bound(k) is the k-th smallest value (k is 1-based). O(log n).
    int lower_bound(T target) const {
        int pos = 0;                                 // invariant: a[0] + ... + a[pos-1] < original target
        for (int step = (int)bit_floor((unsigned)n); step > 0; step >>= 1) {
            if (pos + step <= n && tree[pos + step] < target) {
                pos += step;
                target -= tree[pos];                 // what is still missing after taking that block
            }
        }
        return pos;
    }
};
// [/snippet]

// [snippet:range_fenwick]
// Range add + range sum. Store the difference array D (a[i] = D[0] + ... + D[i]); a range add changes
// only D[l] and D[r+1]. Summing a[0..k-1] counts each D[j] (k - j) times, so
//     prefix(k) = k * sum(D[j]) - sum(D[j] * j)      over j < k,
// which two Fenwicks maintain: b1 over D[j], b2 over D[j] * j.
template <class T = long long>
struct RangeFenwick {
    int n;
    Fenwick<T> b1, b2;

    explicit RangeFenwick(int n) : n(n), b1(n), b2(n) {}

    void range_add(int l, int r, T delta) {          // a[l..r] += delta, inclusive
        add_to_diff(l, delta);
        if (r + 1 < n) add_to_diff(r + 1, -delta);
    }
    T prefix(int k) const { return b1.prefix(k) * k - b2.prefix(k); }   // a[0] + ... + a[k-1]
    T range_sum(int l, int r) const { return prefix(r + 1) - prefix(l); }
    T point(int i) const { return b1.prefix(i + 1); }   // a[i]: b1 alone is "range add, point query"

private:
    void add_to_diff(int j, T delta) {
        b1.add(j, delta);
        b2.add(j, delta * j);
    }
};
// [/snippet]
