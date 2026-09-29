// Custom hashing (module 07 section 2 and section 3): splitmix64 against anti-hash inputs, then key design:
// pair keys, packing a pair into one integer, vector keys, struct keys, and normalized slopes.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:splitmix]
// splitmix64 scrambles all 64 bits: flip one input bit and about half the output bits flip.
// The per-run random offset means nobody can precompute a set of colliding keys.
struct SplitMixHash {
    static uint64_t splitmix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }
    size_t operator()(uint64_t x) const {
        static const uint64_t offset = chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + offset);
    }
};
// Usage: unordered_map<long long, int, SplitMixHash> count;
// [/snippet]

// [snippet:pair_hash]
// std::hash has no pair overload, so unordered_map<pair<int, int>, V> doesn't compile without
// one. Pack both halves into 64 bits (see pair_key below), then mix.
struct PairHash {
    size_t operator()(const pair<int, int>& p) const {
        uint64_t packed = ((uint64_t)(uint32_t)p.first << 32) | (uint32_t)p.second;
        return SplitMixHash{}(packed);
    }
};
// Usage: unordered_map<pair<int, int>, int, PairHash> seen;
// [/snippet]

// [snippet:encode]
// Or skip the custom hash: turn the pair into ONE integer, and the default hash just works.
// Grid cells (0 <= c < cols): the row-major index, like flattening a 2D array.
long long cell_key(int r, int c, int cols) { return (long long)r * cols + c; }
// Any two ints: 32 bits each. The (uint32_t) casts stop a negative number's sign bits from
// spilling into the other half.
long long pair_key(int a, int b) { return (long long)(((uint64_t)(uint32_t)a << 32) | (uint32_t)b); }
// [/snippet]
pair<int, int> unpack(long long key) { return {(int)(uint32_t)((uint64_t)key >> 32), (int)(uint32_t)key}; }

// [snippet:vector_key]
// Whole vectors as keys. std::map works as is (vector has operator<), at O(k log n) per
// operation for length-k keys. unordered_map needs a hash: fold the elements in, in order.
struct VectorHash {
    size_t operator()(const vector<int>& v) const {
        uint64_t h = v.size();
        for (int x : v) h = h * 1'000'003 + (uint32_t)x;   // polynomial, like Java's 31 * h + x
        return SplitMixHash{}(h);
    }
};
// Usage: map<vector<int>, int> a;  unordered_map<vector<int>, int, VectorHash> b;
// Or a string key with separators: "3,1,4," (to_string of each element, then ',').
// [/snippet]

// [snippet:struct_hash]
struct Point {
    int x, y;
    bool operator==(const Point&) const = default;   // unordered_* needs == as well as a hash
};
// Option 1: a hash functor, passed as the container's third template argument.
struct PointHash {
    size_t operator()(const Point& p) const { return PairHash{}({p.x, p.y}); }
};
// Option 2: specialize std::hash once; afterwards unordered_set<Point> needs no extra argument.
namespace std {
template <>
struct hash<Point> {
    size_t operator()(const Point& p) const { return PairHash{}({p.x, p.y}); }
};
}  // namespace std
// [/snippet]

// [snippet:slope_key]
// The direction from (x1, y1) to (x2, y2) as an exact, canonical key. Divide out the gcd, then
// fix the sign so (2, 4), (1, 2) and (-1, -2) (all the same line) give one key. Doubles
// would round (1.0/3 is not exact) and dx == 0 would divide by zero.
pair<int, int> slope_key(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1, dy = y2 - y1;
    int g = gcd(dx, dy);                 // std::gcd takes negatives and returns >= 0
    if (g == 0) return {0, 0};           // the same point: no direction
    dx /= g;
    dy /= g;
    if (dx < 0 || (dx == 0 && dy < 0)) { // canonical sign: dx > 0, or a vertical line pointing up
        dx = -dx;
        dy = -dy;
    }
    return {dy, dx};
}
// [/snippet]

int main() {
    // ---- anti-hash input: every key in one bucket ----
    // [snippet:anti_hash]
    // std::hash<long long> is the identity on GCC and Clang, and a key's bucket is
    // hash % bucket_count. So keys that are all multiples of bucket_count share one bucket.
    const int n = 2000;
    unordered_map<long long, int> plain;
    plain.reserve(n);                           // fixes bucket_count: no rehash in the loop below
    long long b = plain.bucket_count();         // 2003 here; the exact prime depends on the library
    for (long long i = 0; i < n; i++) plain[i * b] = 1;     // insert i walks a chain of length i
    size_t longest_chain = 0;
    for (size_t i = 0; i < plain.bucket_count(); i++) longest_chain = max(longest_chain, plain.bucket_size(i));
    // longest_chain == 2000: the "hash table" is one linked list, and the n inserts cost O(n^2)
    // [/snippet]
    if (hash<long long>{}(123456789) == 123456789) CHECK_EQ(longest_chain, (size_t)n);

    unordered_map<long long, int, SplitMixHash> mixed;
    mixed.reserve(n);
    long long mb = mixed.bucket_count();
    for (long long i = 0; i < n; i++) mixed[i * mb] = 1;
    size_t longest_mixed = 0;
    for (size_t i = 0; i < mixed.bucket_count(); i++) longest_mixed = max(longest_mixed, mixed.bucket_size(i));
    CHECK(longest_mixed <= 16);                  // ~n/buckets = 1 on average; a handful at worst
    CHECK_EQ(mixed.size(), (size_t)n);

    // ---- pair keys: custom hash vs encoding ----
    unordered_map<pair<int, int>, int, PairHash> by_pair;
    unordered_map<long long, int> by_key;
    map<pair<int, int>, int> by_tree;
    for (int iter = 0; iter < 2000; iter++) {
        int x = (int)t::rand_int(-5, 5), y = (int)t::rand_int(-5, 5);
        by_pair[{x, y}]++;
        by_key[pair_key(x, y)]++;
        by_tree[{x, y}]++;
    }
    CHECK_EQ(by_pair.size(), by_tree.size());
    CHECK_EQ(by_key.size(), by_tree.size());
    for (auto& [p, c] : by_tree) {
        CHECK_EQ(by_pair[p], c);
        CHECK_EQ(by_key[pair_key(p.first, p.second)], c);
    }
    for (int iter = 0; iter < 300; iter++) {    // pair_key is one-to-one: it unpacks exactly
        int x = (int)t::rand_int(INT_MIN, INT_MAX), y = (int)t::rand_int(INT_MIN, INT_MAX);
        CHECK_EQ(unpack(pair_key(x, y)), make_pair(x, y));
    }
    CHECK(pair_key(-1, 0) != pair_key(0, -1));
    CHECK(pair_key(1, -1) != pair_key(0, -1));   // without the casts, -1's sign bits would clobber the 1
    CHECK_EQ(cell_key(2, 3, 10), 23);

    // ---- vector keys: tree map, hashed, and string-encoded give the same counts ----
    vector<vector<int>> rows{{1, 2}, {2, 1}, {1, 2}, {3}, {}, {1, 2}, {}};
    map<vector<int>, int> tree_count;
    unordered_map<vector<int>, int, VectorHash> hash_count;
    unordered_map<string, int> string_count;
    for (const auto& r : rows) {
        tree_count[r]++;
        hash_count[r]++;
        string key;
        for (int x : r) key += to_string(x) + ',';
        string_count[key]++;
    }
    vector<int> one_two{1, 2};                  // a named key: a braced {1, 2} inside CHECK_EQ(...) would split the macro
    CHECK_EQ(tree_count[one_two], 3);
    CHECK_EQ(hash_count[one_two], 3);
    CHECK_EQ(string_count["1,2,"], 3);
    CHECK_EQ(hash_count[{}], 2);
    CHECK_EQ(hash_count.size(), tree_count.size());
    CHECK_EQ(string_count.size(), tree_count.size());
    CHECK(VectorHash{}({1, 2}) != VectorHash{}({2, 1}));   // order matters, unlike a XOR combine

    // ---- struct keys, both ways ----
    unordered_set<Point, PointHash> with_functor{{1, 2}, {3, 4}, {1, 2}};
    unordered_set<Point> with_specialization{{1, 2}, {3, 4}, {1, 2}};
    CHECK_EQ(with_functor.size(), 2);
    CHECK_EQ(with_specialization.size(), 2);
    CHECK_EQ(with_specialization.count(Point{3, 4}), 1);
    CHECK_EQ(with_functor.count(Point{4, 3}), 0);

    // ---- normalized slopes ----
    CHECK_EQ(slope_key(0, 0, 2, 4), slope_key(0, 0, 1, 2));
    CHECK_EQ(slope_key(0, 0, 2, 4), slope_key(0, 0, -1, -2));   // opposite direction, same line
    CHECK_EQ(slope_key(3, 3, 1, 1), make_pair(1, 1));
    CHECK_EQ(slope_key(0, 0, 0, -5), make_pair(1, 0));          // vertical
    CHECK_EQ(slope_key(0, 0, -7, 0), make_pair(0, 1));          // horizontal
    CHECK_EQ(slope_key(1, 1, 1, 1), make_pair(0, 0));
    CHECK(slope_key(0, 0, 1, 2) != slope_key(0, 0, 2, 1));
    for (int iter = 0; iter < 500; iter++) {    // same key <=> collinear with the origin (cross product 0)
        int x1 = (int)t::rand_int(-6, 6), y1 = (int)t::rand_int(-6, 6);
        int x2 = (int)t::rand_int(-6, 6), y2 = (int)t::rand_int(-6, 6);
        if ((x1 == 0 && y1 == 0) || (x2 == 0 && y2 == 0)) continue;
        bool collinear = x1 * y2 - y1 * x2 == 0;
        CHECK_EQ(slope_key(0, 0, x1, y1) == slope_key(0, 0, x2, y2), collinear);
    }
    return t::summary("custom_hash");
}
