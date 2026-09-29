// Sparse table: O(n log n) build, then O(1) range min / max (any idempotent operation) on an array
// that never changes. Module 17 section 1.
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:ops]
// Operations as tiny function objects, so the table's type says what it computes:
//   SparseTable<int> st(a);                  range min
//   SparseTable<int, Max<int>> st(a);        range max
template <class T> struct Min { T operator()(const T& a, const T& b) const { return min(a, b); } };
template <class T> struct Max { T operator()(const T& a, const T& b) const { return max(a, b); } };
// [/snippet]

// [snippet:sparse_table]
// table[k][i] = op(a[i], ..., a[i + 2^k - 1]): the block of length 2^k that starts at i.
// query(l, r) covers [l, r] with two blocks of length 2^k that may overlap in the middle. That is only
// correct when op(x, x) == x (idempotent: min, max, gcd, &, |). A sum would count the overlap twice.
template <class T, class Op = Min<T>>
struct SparseTable {
    vector<int> lg;               // lg[len] = floor(log2(len)) for len >= 1
    vector<vector<T>> table;
    Op op;

    explicit SparseTable(const vector<T>& a) {
        int n = (int)a.size();
        lg.assign(n + 1, 0);
        for (int len = 2; len <= n; len++) lg[len] = lg[len / 2] + 1;
        table.assign(lg[n] + 1, vector<T>(n));
        table[0] = a;
        for (int k = 1; k <= lg[n]; k++)
            for (int i = 0; i + (1 << k) <= n; i++)          // two halves of length 2^(k-1)
                table[k][i] = op(table[k - 1][i], table[k - 1][i + (1 << (k - 1))]);
    }

    T query(int l, int r) const {         // inclusive, 0 <= l <= r < n
        int k = lg[r - l + 1];            // the largest power of two that fits in the range
        return op(table[k][l], table[k][r - (1 << k) + 1]);   // [l, l + 2^k) and (r - 2^k, r]
    }
};
// [/snippet]

// [snippet:fold]
// Any associative op, idempotent or not (sum, product, matrix multiply): peel off disjoint
// power-of-two blocks from the left, largest first. One block per set bit of the length, so
// O(log n) per query, and the blocks stay in left-to-right order for non-commutative ops.
template <class T, class Op>
T fold(const SparseTable<T, Op>& st, int l, int r) {   // inclusive, 0 <= l <= r < n
    int k = st.lg[r - l + 1];
    T result = st.table[k][l];
    for (l += 1 << k; l <= r; l += 1 << k) {
        k = st.lg[r - l + 1];
        result = st.op(result, st.table[k][l]);
    }
    return result;
}
// [/snippet]
