// Disjoint set union (union-find): merge sets and ask "same set?" in near-constant amortized time.
// Module 14 section 4 (connectivity, cycle detection, grouping) and module 15 section 3 (Kruskal's MST).
//
// Interview-style header (see binary_search.hpp): unqualified std names, snippet-ready.
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:dsu]
struct DSU {
    vector<int> parent, sz;   // sz[r] is only meaningful while r is a root
    int components;           // number of disjoint sets right now

    explicit DSU(int n) : parent(n), sz(n, 1), components(n) {
        iota(parent.begin(), parent.end(), 0);   // every element starts as its own root
    }

    // Root of x's set. Path compression: every node on the walk is re-pointed straight at the root.
    // Union by size keeps trees O(log n) deep, so this recursion is always shallow.
    int find(int x) {
        if (parent[x] == x) return x;
        return parent[x] = find(parent[x]);
    }

    // Merge the sets containing a and b. Returns false if they were already one set
    // (for an edge a-b, that means the edge closes a cycle).
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) swap(a, b);   // hang the smaller tree under the larger root
        parent[b] = a;
        sz[a] += sz[b];
        components--;
        return true;
    }

    bool same(int a, int b) { return find(a) == find(b); }
    int size_of(int x) { return sz[find(x)]; }
};
// [/snippet]
