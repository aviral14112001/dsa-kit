#include <bits/stdc++.h>
#include "test.hpp"
#include "dsu.hpp"
using namespace std;

// Depth of x, walking parent pointers directly so nothing gets compressed.
int depth_of(const DSU& d, int x) {
    int depth = 0;
    while (d.parent[x] != x) {
        x = d.parent[x];
        depth++;
    }
    return depth;
}

int max_depth(const DSU& d) {
    int best = 0;
    for (int x = 0; x < (int)d.parent.size(); x++) best = max(best, depth_of(d, x));
    return best;
}

// Brute force: label[i] = set id, and a union relabels a whole set. O(n) per union, obviously correct.
struct BruteDSU {
    vector<int> label;
    explicit BruteDSU(int n) : label(n) { iota(label.begin(), label.end(), 0); }
    bool unite(int a, int b) {
        int la = label[a], lb = label[b];
        if (la == lb) return false;
        for (int& l : label)
            if (l == lb) l = la;
        return true;
    }
    bool same(int a, int b) const { return label[a] == label[b]; }
    int size_of(int x) const { return (int)count(label.begin(), label.end(), label[x]); }
    int components() const { return (int)set<int>(label.begin(), label.end()).size(); }
};

int main() {
    // ---- deterministic ----
    DSU d(6);
    CHECK_EQ(d.components, 6);
    CHECK(d.unite(0, 1));
    CHECK(d.unite(2, 3));
    CHECK(!d.unite(1, 0));  // already one set
    CHECK(d.same(0, 1));
    CHECK(!d.same(1, 2));
    CHECK_EQ(d.size_of(0), 2);
    CHECK_EQ(d.components, 4);
    CHECK(d.unite(1, 3));
    CHECK(d.same(0, 2));
    CHECK_EQ(d.size_of(3), 4);
    CHECK_EQ(d.components, 3);  // {0,1,2,3} {4} {5}
    CHECK(!d.unite(0, 3));      // an edge inside a component closes a cycle
    CHECK_EQ(d.size_of(5), 1);
    CHECK_EQ(d.components, 3);

    DSU one(1);
    CHECK(!one.unite(0, 0));  // self-union is a no-op
    CHECK_EQ(one.components, 1);
    CHECK_EQ(one.size_of(0), 1);

    DSU empty(0);
    CHECK_EQ(empty.components, 0);

    // ---- union by size: depth <= log2(n), and the bound is tight ----
    // Merging equal-sized trees pairwise (1+1, 2+2, 4+4, ...) is the worst case: depth grows by one per level.
    {
        const int n = 1 << 10;
        DSU pairs(n);
        for (int step = 1; step < n; step *= 2)
            for (int i = 0; i + step < n; i += 2 * step) pairs.unite(i, i + step);
        CHECK_EQ(pairs.components, 1);
        CHECK_EQ(max_depth(pairs), 10);  // exactly log2(1024): union by size alone
        int deepest = n - 1;
        CHECK_EQ(depth_of(pairs, deepest), 10);
        pairs.find(deepest);  // path compression flattens the whole walk
        CHECK_EQ(depth_of(pairs, deepest), 1);
    }
    // The chain order that ruins a naive DSU (unite(i, i-1) for every i) stays flat here.
    {
        const int n = 5000;
        DSU chain(n);
        for (int i = 1; i < n; i++) chain.unite(i, i - 1);
        CHECK_EQ(chain.components, 1);
        CHECK(max_depth(chain) <= 1);
        CHECK_EQ(chain.size_of(n - 1), n);
    }

    // ---- stress: random operations vs the brute force, plus the log2(n) depth bound ----
    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(1, 30);
        DSU fast(n);
        BruteDSU slow(n);
        int ops = (int)t::rand_int(0, 80);
        for (int op = 0; op < ops; op++) {
            int a = (int)t::rand_int(0, n - 1), b = (int)t::rand_int(0, n - 1);
            if (t::rand_int(0, 1)) {
                CHECK_EQ(fast.unite(a, b), slow.unite(a, b));
            } else {
                CHECK_EQ(fast.same(a, b), slow.same(a, b));
                CHECK_EQ(fast.size_of(a), slow.size_of(a));
            }
            CHECK_EQ(fast.components, slow.components());
        }
        CHECK(max_depth(fast) <= (int)floor(log2(n)));
    }

    // ---- large: 2*10^5 elements merged in random order ----
    {
        const int n = 200000;
        DSU big(n);
        vector<int> perm(n);
        iota(perm.begin(), perm.end(), 0);
        shuffle(perm.begin(), perm.end(), t::rng());
        for (int i = 1; i < n; i++) big.unite(perm[i], perm[(int)t::rand_int(0, i - 1)]);
        CHECK_EQ(big.components, 1);
        CHECK_EQ(big.size_of(0), n);
        CHECK(max_depth(big) <= 17);  // log2(200000) < 18
    }
    return t::summary("dsu");
}
