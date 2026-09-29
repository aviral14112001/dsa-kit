// DSU from naive to fast, measured; union by rank; a DSU that carries extra data per set. Module 14 section 4.
// The DSU to actually use is templates/dsu.hpp (union by size + path compression).
#include <bits/stdc++.h>
#include "test.hpp"
#include "dsu.hpp"
using namespace std;

// [snippet:naive]
// Naive union-find: link one root under the other, whichever way round. A bad sequence of unions
// builds a single long path, and then every find walks all of it: O(n) per operation.
struct NaiveDSU {
    vector<int> parent;
    explicit NaiveDSU(int n) : parent(n) { iota(parent.begin(), parent.end(), 0); }
    int find(int x) {
        while (parent[x] != x) x = parent[x];
        return x;
    }
    void unite(int a, int b) { parent[find(a)] = find(b); }
};
// [/snippet]

struct DSUByRank {
    vector<int> parent, rnk;
    explicit DSUByRank(int n) : parent(n), rnk(n, 0) { iota(parent.begin(), parent.end(), 0); }
    int find(int x) { return parent[x] == x ? x : parent[x] = find(parent[x]); }
    // [snippet:union_by_rank]
    // Union by rank: rank[r] is an upper bound on the height of r's tree (path compression can only
    // lower heights, so ranks are never updated by find).
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (rnk[a] < rnk[b]) swap(a, b);          // the shorter tree goes under the taller one
        parent[b] = a;
        if (rnk[a] == rnk[b]) rnk[a]++;           // only equal ranks make the result taller
        return true;
    }
    // [/snippet]
};

struct DSUWithMin {
    vector<int> parent, sz, min_of;
    explicit DSUWithMin(int n) : parent(n), sz(n, 1), min_of(n) {
        iota(parent.begin(), parent.end(), 0);
        iota(min_of.begin(), min_of.end(), 0);   // each singleton's minimum is itself
    }
    int find(int x) { return parent[x] == x ? x : parent[x] = find(parent[x]); }
    // [snippet:dsu_min]
    // Extra data per set lives at the root and is combined in unite. Here: the smallest element of
    // each set. Sums, counts, edge totals and max values work exactly the same way.
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) swap(a, b);
        parent[b] = a;
        sz[a] += sz[b];
        min_of[a] = min(min_of[a], min_of[b]);   // the surviving root now summarizes both sets
        return true;
    }
    int smallest_in_set(int x) { return min_of[find(x)]; }
    // [/snippet]
};

// Depth of x, walking parent pointers without compressing anything.
template <class D>
int depth_of(const D& d, int x) {
    int depth = 0;
    while (d.parent[x] != x) {
        x = d.parent[x];
        depth++;
    }
    return depth;
}

template <class D>
int max_depth(const D& d) {
    int best = 0;
    for (int x = 0; x < (int)d.parent.size(); x++) best = max(best, depth_of(d, x));
    return best;
}

int main() {
    // ---- the chain that kills the naive version ----
    const int n = 2000;
    NaiveDSU naive(n);
    DSU fast(n);
    DSUByRank by_rank(n);
    for (int i = 0; i + 1 < n; i++) {             // unite(0,1), unite(1,2), ... each old root goes under the new one
        naive.unite(i, i + 1);
        fast.unite(i, i + 1);
        by_rank.unite(i, i + 1);
    }
    CHECK_EQ(max_depth(naive), n - 1);            // a path: find(0) walks 1999 pointers
    CHECK(max_depth(fast) <= 1);                  // union by size: the big tree keeps its root
    CHECK(max_depth(by_rank) <= 1);
    CHECK_EQ(naive.find(0), n - 1);
    CHECK(fast.same(0, n - 1));

    // ---- rank bounds height: log2(n) even with no path compression happening ----
    DSUByRank pairs(1024);
    for (int step = 1; step < 1024; step *= 2)
        for (int i = 0; i + step < 1024; i += 2 * step) pairs.unite(i, i + step);
    CHECK_EQ(max_depth(pairs), 10);
    CHECK_EQ(pairs.rnk[pairs.find(0)], 10);

    // ---- min element per set ----
    DSUWithMin m(6);
    m.unite(4, 5);
    CHECK_EQ(m.smallest_in_set(5), 4);
    m.unite(5, 2);
    CHECK_EQ(m.smallest_in_set(4), 2);
    CHECK_EQ(m.smallest_in_set(0), 0);
    m.unite(0, 3);
    m.unite(3, 5);
    CHECK_EQ(m.smallest_in_set(2), 0);
    CHECK_EQ(m.smallest_in_set(1), 1);

    // ---- stress: all variants agree with a brute-force labelling ----
    for (int iter = 0; iter < 300; iter++) {
        int k = (int)t::rand_int(1, 20);
        vector<int> label(k);
        iota(label.begin(), label.end(), 0);
        NaiveDSU a(k);
        DSUByRank b(k);
        DSUWithMin c(k);
        int ops = (int)t::rand_int(0, 40);
        for (int op = 0; op < ops; op++) {
            int x = (int)t::rand_int(0, k - 1), y = (int)t::rand_int(0, k - 1);
            bool was_same = label[x] == label[y];
            int old = label[y];
            for (int& l : label)
                if (l == old) l = label[x];
            a.unite(x, y);
            CHECK_EQ(b.unite(x, y), !was_same);
            CHECK_EQ(c.unite(x, y), !was_same);
            for (int i = 0; i < k; i++) {
                CHECK_EQ(a.find(i) == a.find(x), label[i] == label[x]);
                CHECK_EQ(b.find(i) == b.find(x), label[i] == label[x]);
                int smallest = i;
                for (int j = 0; j < k; j++)
                    if (label[j] == label[i]) smallest = min(smallest, j);
                CHECK_EQ(c.smallest_in_set(i), smallest);
            }
        }
        CHECK(max_depth(b) <= (int)floor(log2(k)));
    }
    return t::summary("14 dsu_variants");
}
