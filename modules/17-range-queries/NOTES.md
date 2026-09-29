# 17 · Advanced Data Structures + Range Queries + Project
> For when queries outnumber updates — or the other way round.

**Time:** ~7 h of Core work ([problems](problems.md)), plus ~2 h re-solving the worked examples from blank files · **Prereqs:** modules 03 (log-factor costs), 05 (sorting, `lower_bound`), 06 (prefix sums, difference arrays), 10 (recursion), 12 (tree recursion) · **You're done when:** (1) from a blank file you can write a Fenwick tree and a recursive segment tree in about 10 minutes each, and both survive a stress test against a brute force; (2) without notes, you can explain why a sparse table answers range min in O(1) but not range sum, and why an assign must wipe a node's pending add; (3) your own blank-file CSES 1735 matches both `.in`/`.out` pairs in [`examples/`](examples/), and the project's Milestone 2 differential tester passes.

Every `cpp` block below is pulled from a file that `make test M=modules/17-` (examples) or `make test M=templates/tests/<name>_test` (e.g. `fenwick_test`) compiles and runs against a brute force.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Sparse tables | Precompute the min of every power-of-two block; any range is two overlapping blocks, harmless for min/max because counting an element twice changes nothing | [sparse_table.hpp](../../templates/sparse_table.hpp) | none on the list · worked example: CSES 1647 |
| 2 | Fenwick tree (BIT) | `tree[j]` holds the sum of the `lowbit(j)` elements ending at j; updates climb, prefix queries descend, both O(log n) | [fenwick.hpp](../../templates/fenwick.hpp), [inversions.cpp](examples/inversions.cpp) | 307 📖, 315 📖 |
| 3 | Segment trees | Each node stores the answer for its range; a query stitches O(log n) nodes; lazy tags postpone range updates until someone looks below | [segment_tree.hpp](../../templates/segment_tree.hpp) | 732, 699 · worked example: CSES 1735 |
| 4 | Project | One range-query interface, several backends, a differential tester and a benchmark | [projects/17-range-query-engine](../../projects/17-range-query-engine/README.md) | Milestones 1–3 |

The one-screen summary of which structure to pick is the [table at the end of section 3](#which-structure-when).

## 1. Sparse tables — immutable range min / max in O(1) after preprocessing

### Concept

The array never changes, and you get up to 10⁵–10⁶ queries "min of a[l..r]". Scanning each range is O(n) per query: 2·10⁵ × 2·10⁵ = 4·10¹⁰ steps, minutes of work at roughly 10⁸ simple steps per second.

Prefix sums (module 06) answer range *sums* in O(1) because subtraction undoes addition: sum(l, r) = P[r+1] − P[l]. Min has no inverse. Knowing min(a[0..r]) and min(a[0..l−1]) tells you nothing about min(a[l..r]). So precompute something else: the answer for every block whose length is a power of two.

- `table[k][i]` = min(a[i], …, a[i + 2ᵏ − 1]), the block of length 2ᵏ starting at i.
- `table[0]` is the array itself, and a block of length 2ᵏ is two halves of length 2ᵏ⁻¹: `table[k][i] = min(table[k−1][i], table[k−1][i + 2ᵏ⁻¹])`.
- There are ⌊log₂ n⌋ + 1 levels of at most n entries each: O(n log n) time and memory to build.

**The O(1) query.** For [l, r] let len = r − l + 1 and k = ⌊log₂ len⌋, so 2ᵏ ≤ len < 2ᵏ⁺¹. Take the block of length 2ᵏ that starts at l and the one that ends at r. Neither sticks out (each is no longer than len), and together they are longer than len, so they cover all of [l, r], overlapping in the middle.

```text
index:            0  1  2  3  4  5  6  7
query [1, 6]:     len = 6, k = 2, blocks of 4
block from l:        [1  2  3  4]
block ending at r:         [3  4  5  6]
3 and 4 are counted twice: fine for min, wrong for a sum
```

That overlap is the whole trick, and it's why the operation must be **idempotent**: op(x, x) = x. Min, max, gcd, bitwise AND and OR qualify. Sum, product, XOR (x ^ x = 0) and count don't.

**Why sums use prefix sums instead, or disjoint blocks.** For sums you don't need a sparse table at all: prefix sums give O(1) queries from O(n) memory ([`templates/prefix_sum.hpp`](../../templates/prefix_sum.hpp), module 06). For an associative operation (one where grouping doesn't matter: op(op(a, b), c) = op(a, op(b, c))) that is neither idempotent nor invertible (matrix products, the "max subarray" info of section 3), the table still works if the blocks don't overlap: peel off the largest power of two that fits, then the next, one block per set bit of len. That's O(log n) per query, the `fold` function below. A segment tree also gives O(log n) and supports updates too, so `fold` is mostly a way to see why the overlap matters.

**The log table.** `lg[len] = lg[len / 2] + 1` fills ⌊log₂ len⌋ for every len in O(n), with integers only. Alternatives: `std::bit_width((unsigned)len) - 1` (C++20 `<bit>`) or `31 - __builtin_clz(len)`. You'll see `__lg(len)` in competitive code; it's a GCC library extension and doesn't compile with Apple clang.

**Updates break it.** Changing a[i] invalidates every block containing i: up to 2ᵏ blocks on level k, O(n) entries in total. If values change, use a segment tree (Section 3).

### Template

<!-- snippet: templates/sparse_table.hpp#ops -->
```cpp
// Operations as tiny function objects, so the table's type says what it computes:
//   SparseTable<int> st(a);                  range min
//   SparseTable<int, Max<int>> st(a);        range max
template <class T> struct Min { T operator()(const T& a, const T& b) const { return min(a, b); } };
template <class T> struct Max { T operator()(const T& a, const T& b) const { return max(a, b); } };
```
<!-- /snippet -->

<!-- snippet: templates/sparse_table.hpp#sparse_table -->
```cpp
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
```
<!-- /snippet -->

Build O(n log n) time and memory; `query` O(1).

<!-- snippet: templates/sparse_table.hpp#fold -->
```cpp
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
```
<!-- /snippet -->

O(log n) per query. The test file checks `fold` with a sum and with string concatenation (associative but not commutative), and shows `query` double-counting a sum on purpose.

### Pitfalls

- **A non-idempotent op in `query`** (sum, XOR, count): silently wrong, no crash. The overlap is only safe for min, max, gcd, AND, OR.
- **The second block starts at `r - (1 << k) + 1`,** not `r - (1 << k)`. Check it on a length-1 query: both blocks must be the single cell.
- **Floating-point logs.** `(int)(log(x) / log(base))` can land just below an exact integer and truncate one too low: with this kit's toolchain, `log(1000) / log(10)` gives 2. Base 2 happens to come out exact here, but that's the math library's behaviour, not a guarantee. Use the integer table or `bit_width`.
- **Memory.** n = 2·10⁵ ints: 18 levels × 2·10⁵ × 4 bytes ≈ 14 MB, fine. n = 10⁶ `long long`s: 20 × 10⁶ × 8 bytes = 160 MB, possibly over a judge's limit. Use `int` when the values fit.
- **Level-major layout.** `table[k][i]` keeps each level contiguous and the build reads the previous level in order. `table[i][k]` also works but jumps around memory on large inputs.
- **1-based input** (CSES and many stdin/stdout problems): convert once when you read the query, not inside the structure.

### Recognize it when…

- The array is **static** and the queries are range min / max / gcd, many of them: n, q around 10⁵–10⁶.
- O(log n) per query is too slow or too much code, e.g. q = 10⁶ queries inside another loop.
- It's a building block inside something else: LCA through an Euler tour plus range-min, or LCP of two suffixes as a range-min over the LCP array (module 18 section 4).
- **Not** when values change (→ segment tree) or the query is a sum or XOR (→ prefix sums).

### Worked example: CSES 1647. Static Range Minimum Queries
[CSES 1647](https://cses.fi/problemset/task/1647) · stdin/stdout · teaching example (Section 1 has no list problem, so re-solving this is your practice)

**Problem (paraphrased):** Read n integers, then answer q queries (a, b), each asking for the minimum of positions a..b (1-based). n, q ≤ 2·10⁵, values up to 10⁹.

**Signals:** no updates; many range-min queries; 2·10⁵ of each, so the target is about O((n + q) log n) or better.

**Brute force, and why it fails:** scan each range: O(nq) = 4·10¹⁰ in the worst case.

**Key insight:** min is idempotent, so a sparse table answers each query with two overlapping blocks in O(1) after an O(n log n) build.

**Dry run:** the sample, a = [3, 2, 4, 5, 1, 1, 5, 3] after reading (0-based).

| k (block length) | i=0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---|---|---|---|---|---|---|---|---|
| 0 (1) | 3 | 2 | 4 | 5 | 1 | 1 | 5 | 3 |
| 1 (2) | 2 | 2 | 4 | 1 | 1 | 1 | 3 | – |
| 2 (4) | 2 | 1 | 1 | 1 | 1 | – | – | – |
| 3 (8) | 1 | – | – | – | – | – | – | – |

- Query (2, 4) → [1, 3], len 3, k = 1: min(table[1][1], table[1][2]) = min(2, 4) = **2**.
- Query (5, 6) → [4, 5], len 2, k = 1: both blocks are table[1][4] = **1**.
- Query (1, 8) → [0, 7], len 8, k = 3: table[3][0] = **1**.
- Query (3, 3) → [2, 2], k = 0: table[0][2] = **4**.

<!-- snippet: modules/17-range-queries/examples/cses-1647-static-range-minimum-queries.cpp#solution -->
```cpp
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, q;
    cin >> n >> q;

    vector<int> lg(n + 1, 0);                      // lg[len] = floor(log2(len))
    for (int len = 2; len <= n; len++) lg[len] = lg[len / 2] + 1;

    vector<vector<int>> mn(lg[n] + 1, vector<int>(n));   // mn[k][i] = min of the 2^k values from i
    for (int& x : mn[0]) cin >> x;
    for (int k = 1; k <= lg[n]; k++)
        for (int i = 0; i + (1 << k) <= n; i++)
            mn[k][i] = min(mn[k - 1][i], mn[k - 1][i + (1 << (k - 1))]);

    while (q--) {
        int a, b;
        cin >> a >> b;
        a--;                                       // CSES positions are 1-based
        b--;
        int k = lg[b - a + 1];                     // two blocks of 2^k cover [a, b]
        cout << min(mn[k][a], mn[k][b - (1 << k) + 1]) << '\n';
    }
}
```
<!-- /snippet -->

**Complexity:** O(n log n) build, O(1) per query; O(n log n) memory (about 14 MB here).

**Edge cases:**
- n = 1: `lg[1] = 0`, a single level (the `.2.in` case).
- Length-1 queries: k = 0 and both blocks are the same cell.
- The whole array: k = lg[n].
- 1-based input, converted once.

**Follow-ups:**
- *What if values can change?* A segment tree: O(log n) per update and per query (Section 3).
- *Range sums instead?* Prefix sums: O(n) build, O(1) query, no log factor anywhere.
- *Less memory?* A segment tree uses O(n) memory for O(log n) queries. O(n)-memory, O(1)-query structures exist, but interviews don't expect them.

## 2. Fenwick tree (BIT) — point update, prefix query, inversion counting

### Concept

Now the array changes. With a plain array, an update is O(1) and a prefix sum O(n). With a prefix-sum array it's the reverse. A Fenwick tree (binary indexed tree) makes both O(log n) with n + 1 numbers and about ten lines of code. Neither .NET nor the STL has one; you write it.

**lowbit.** `j & -j` is the value of j's lowest set bit: 12 = 1100₂ → 4. In two's complement, −j = ~j + 1. The +1 carries through ~j's trailing ones (j's trailing zeros) and stops at j's lowest set bit. So j and −j are both 0 below that bit, both 1 on it, and complements above it: `&` keeps only that bit.

**Responsibility.** Index the tree from 1 (this section's positions are 1-based, as inside the tree; the template's public API converts from 0-based). `tree[j]` holds the sum of the `lowbit(j)` elements that end at position j, i.e. positions (j − lowbit(j), j]:

```text
position j:   1     2     3     4     5     6     7     8
lowbit(j):    1     2     1     4     1     2     1     8
tree[j]:     [1]  [1-2]  [3]  [1-4]  [5]  [5-6]  [7]  [1-8]
```

**Prefix query: walk down.** prefix(k) = a₁ + … + a_k. Start at j = k, add `tree[j]`, then drop j's lowest set bit (`j -= j & -j`). The blocks tile [1, k] from right to left, one per set bit of k: at most ⌊log₂ n⌋ + 1 steps. k = 7 = 111₂ visits tree[7] (7), tree[6] (5–6), tree[4] (1–4).

**Point update: walk up.** add(p, d) must fix every block that contains p: `tree[p]`, then `j += j & -j` repeatedly. Adding the lowest set bit jumps to the next larger block that still contains p. p = 3 visits tree[3], tree[4], tree[8]. Also O(log n).

**Range sum** of 0-based a[l..r] = prefix(r + 1) − prefix(l): two walks. This needs an inverse, so a BIT does sums, XOR and counts, but not range min under updates (there is no way to "subtract" the part before l). That's a segment tree job.

**O(n) build.** Put each value in its own slot, then push each finished block into the next block that contains it (`j + lowbit(j)`). One pass, and it yields exactly the tree that n `add()` calls would; the test checks that.

**Range add + point query: a BIT over the difference array.** Store D with a[i] = D[0] + … + D[i] (module 06's difference array). Adding x to a[l..r] changes only D[l] (+x) and D[r+1] (−x): two point updates. The value a[i] is prefix(i + 1) of D. Module 06 rebuilds the array once after all updates; the BIT keeps it queryable between updates.

**Range add + range sum: two BITs.** Summing a[0..k−1] counts each D[j] once for every i ≥ j below k, i.e. (k − j) times:

  a[0] + … + a[k−1] = Σ_{j<k} D[j]·(k − j) = k · Σ_{j<k} D[j] − Σ_{j<k} D[j]·j

So keep B1 over D[j] and B2 over D[j]·j; a range add touches two entries of each. That's `RangeFenwick`.

**Order statistics by binary lifting.** Let the BIT store counts: cnt[v] = how many times value v is present. The k-th smallest value is the smallest v with cnt[0] + … + cnt[v] ≥ k.
- Binary searching over `prefix()` costs O(log n) probes × O(log n) each = O(log² n).
- Binary lifting walks the tree itself in O(log n). Keep pos = the number of slots skipped so far, and try steps of 2^⌊log₂ n⌋, then half that, and so on. Because pos only ever grows by decreasing powers of two, `tree[pos + step]` is exactly the block (pos, pos + step]. Take the block whenever its sum is still below what's missing.

Counts [0, 2, 0, 1, 3] (the multiset {1, 1, 3, 4, 4, 4}; value v sits in slot v + 1), k = 3:

| step | pos + step | block sum | still missing | take? | pos after |
|---|---|---|---|---|---|
| 4 | 4 | tree[4] = 3 | 3 | no (3 ≥ 3) | 0 |
| 2 | 2 | tree[2] = 2 | 3 | yes | 2 (missing 1) |
| 1 | 3 | tree[3] = 0 | 1 | yes | 3 (missing 1) |

The loop ends at pos = 3: slots 1–3 (values 0–2) hold only 2 elements, so the 3rd smallest is in the next slot, value 3. This needs every count ≥ 0, so the prefix sums never decrease.

**Coordinate compression.** A BIT indexed by value needs one slot per possible value: impossible for values up to 10⁹, and negative values can't be indices at all. If only the *order* of values matters ("how many are smaller"), replace each value by its rank among the distinct values: sort, `unique`, then `lower_bound`. m ≤ n slots, same answers.

**Inversion counting.** An inversion is a pair i < j with a[i] > a[j]. Scan left to right with a count-BIT over compressed values. Before recording a[j], the number of earlier values greater than it is j − (earlier values ≤ a[j]). The total reaches n(n−1)/2 ≈ 5·10⁹ for n = 10⁵: `long long`. Merge sort counts inversions in O(n log n) too (module 05); the BIT version extends more easily to "count smaller after self" style questions.

### Template

<!-- snippet: templates/fenwick.hpp#fenwick -->
```cpp
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
```
<!-- /snippet -->

`add`, `prefix`, `range_sum`, `lower_bound`: O(log n). Build O(n). Memory n + 1 values.

<!-- snippet: templates/fenwick.hpp#range_fenwick -->
```cpp
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
```
<!-- /snippet -->

Coordinate compression and inversion counting, from [`examples/inversions.cpp`](examples/inversions.cpp):

<!-- snippet: modules/17-range-queries/examples/inversions.cpp#compress -->
```cpp
// Coordinate compression: replace each value by its rank among the distinct values (0 .. m-1).
// Order is kept, so "smaller than" questions have the same answers, but a Fenwick tree over values
// now needs m <= n slots instead of one per possible value (1e9, or negative values).
vector<int> compress(const vector<int>& a) {
    vector<int> sorted_vals(a);
    sort(sorted_vals.begin(), sorted_vals.end());
    sorted_vals.erase(unique(sorted_vals.begin(), sorted_vals.end()), sorted_vals.end());
    vector<int> ranks(a.size());
    for (size_t i = 0; i < a.size(); i++)
        ranks[i] = (int)(lower_bound(sorted_vals.begin(), sorted_vals.end(), a[i]) - sorted_vals.begin());
    return ranks;
}
```
<!-- /snippet -->

<!-- snippet: modules/17-range-queries/examples/inversions.cpp#inversions -->
```cpp
// Inversions: pairs i < j with a[i] > a[j]. Scan left to right; before recording a[j], count how
// many earlier values are greater: j values seen so far, minus those <= a[j]. O(n log n).
long long count_inversions(const vector<int>& a) {
    vector<int> r = compress(a);
    int n = (int)r.size();
    Fenwick<int> seen(n);                         // ranks are < n
    long long inversions = 0;                     // up to n(n-1)/2: about 5e9 for n = 1e5
    for (int j = 0; j < n; j++) {
        inversions += j - seen.prefix(r[j] + 1);  // prefix(r + 1) = earlier values with rank <= r[j]
        seen.add(r[j], 1);
    }
    return inversions;
}
```
<!-- /snippet -->

O(n log n) each.

### Pitfalls

- **Index 0.** With a 0-based `tree[]`, `j += j & -j` at j = 0 adds 0 forever: an infinite loop. Keep the tree 1-based inside and convert at the API boundary, as the template does.
- **"Set" is not "add".** The BIT stores sums of deltas. To set a[i] = v, add v − a[i], which means keeping the current values in a separate array (307).
- **Overflow.** 2·10⁵ values of 10⁹ sum past 2³¹; inversion counts reach 5·10⁹. Use `long long` (the template's default T).
- **`lower_bound` with negative values** is wrong: the prefix sums must never decrease.
- **Range min/max under updates:** not a BIT. Use a segment tree.
- **Size of a value-indexed BIT** = number of distinct values after compression, not n and not the maximum value.
- **Compression and duplicates:** equal values must get equal ranks, which `lower_bound` on the sorted distinct values guarantees. A rank taken from the position in the sorted (non-deduplicated) array gives equal values different ranks. `upper_bound` shifts every rank up by one (1..m): fine only if you size and index for that.

### Recognize it when…

- Point updates interleaved with prefix or range **sums** (or XOR, or counts), n and q up to ~10⁶.
- "For each element, how many before/after it are smaller/larger", "number of inversions", "pairs i < j with a[i] > a[j]" (or a transformed condition like a[i] > 2·a[j]).
- "k-th smallest in a changing multiset" when values are bounded or can be compressed in advance.
- "Add x to a range" plus "what is a[i] now", online.
- A segment tree would work but only an invertible operation is needed: the BIT is shorter and faster.

### Worked example: 307. Range Sum Query - Mutable
[LeetCode 307](https://leetcode.com/problems/range-sum-query-mutable/) · Medium

**Problem (paraphrased):** Implement a class over an integer array with two operations: set one element to a new value, and return the sum of an inclusive index range. Up to 3·10⁴ elements and 3·10⁴ calls.

**Signals:** "update" and "sum of range", interleaved, in a design-a-class format. The calls arrive one at a time, so you can't batch them.

**Brute force, and why it fails:** keep the plain array: update O(1), sum O(n), so up to 3·10⁴ × 3·10⁴ = 9·10⁸ steps. A prefix-sum array flips the costs, with the same worst case.

**Key insight:** a Fenwick tree makes both operations O(log n). The one twist is that it stores sums of *changes*, so "set a[i] = val" becomes add(i, val − a[i]), which needs the current values kept alongside.

**Dry run:** nums = [1, 3, 5] → tree[1] = 1 (position 1), tree[2] = 1 + 3 = 4 (positions 1–2), tree[3] = 5 (position 3).

| call | what happens | result |
|---|---|---|
| sumRange(0, 2) | prefix(3) − prefix(0): j = 3 → +tree[3] = 5, j = 2 → +tree[2] = 4, j = 0 stop | 9 |
| update(1, 2) | delta = 2 − 3 = −1 at position 2: tree[2] = 3; next j = 4 > n, stop | – |
| sumRange(0, 2) | tree[3] + tree[2] = 5 + 3 | 8 |

<!-- snippet: modules/17-range-queries/examples/0307-range-sum-query-mutable.cpp#solution -->
```cpp
class NumArray {
    int n;
    vector<int> value;                   // current a[i]: turns "set a[i] = val" into "add val - a[i]"
    vector<int> tree;                    // 1-based Fenwick: tree[j] = sum of a over (j - lowbit(j), j]

    void add(int i, int delta) {
        for (int j = i + 1; j <= n; j += j & -j) tree[j] += delta;
    }
    int prefix(int k) const {            // a[0] + ... + a[k-1]
        int sum = 0;
        for (int j = k; j > 0; j -= j & -j) sum += tree[j];
        return sum;
    }

public:
    NumArray(vector<int>& nums) : n((int)nums.size()), value(nums), tree(n + 1, 0) {
        for (int i = 0; i < n; i++) add(i, nums[i]);
    }

    void update(int index, int val) {
        add(index, val - value[index]);
        value[index] = val;
    }

    int sumRange(int left, int right) {
        return prefix(right + 1) - prefix(left);
    }
};
```
<!-- /snippet -->

**Complexity:** constructor O(n log n) (n calls to `add`; the template's O(n) build works too), `update` and `sumRange` O(log n), memory O(n).

**Edge cases:**
- A single element.
- Updating to the same value (delta 0).
- Negative values.
- left == right.
- `int` suffices here: |sum| ≤ 3·10⁴ × 100.

**Follow-ups:**
- *Segment tree instead?* Also O(log n), with more code. It also handles min and max, which a BIT can't under updates (`SumSegTree` in the template is the sum version).
- *What if the updates add to whole ranges?* `RangeFenwick` (two BITs) or a lazy segment tree.
- *A matrix with point updates and rectangle sums?* A 2D BIT: nested lowbit loops over rows and columns, O(log n · log m) per operation.

### Worked example: 315. Count of Smaller Numbers After Self
[LeetCode 315](https://leetcode.com/problems/count-of-smaller-numbers-after-self/) · Hard

**Problem (paraphrased):** For every position i, count the elements after i that are strictly smaller than nums[i]. n ≤ 10⁵, values in [−10⁴, 10⁴].

**Signals:** "for each element, count the smaller elements to its right" is the inversion family. n = 10⁵ rules out looking at all ~5·10⁹ pairs.

**Brute force, and why it fails:** a double loop, O(n²) ≈ 5·10⁹ comparisons.

**Key insight:** scan from the right, keeping a count-BIT of the values already passed, which are exactly the elements to the right of i. The answer for i is "how many passed values are smaller than nums[i]": a prefix sum over value ranks. Compress the values to ranks first (negatives can't index an array, and it bounds the BIT by n).

**Dry run:** nums = [5, 2, 6, 1] → sorted distinct [1, 2, 5, 6] → ranks [2, 1, 3, 0].

| i | nums[i] | rank r | counts by rank before | smaller = prefix(r) | counts after |
|---|---|---|---|---|---|
| 3 | 1 | 0 | [0, 0, 0, 0] | 0 | [1, 0, 0, 0] |
| 2 | 6 | 3 | [1, 0, 0, 0] | 1 | [1, 0, 0, 1] |
| 1 | 2 | 1 | [1, 0, 0, 1] | 1 | [1, 1, 0, 1] |
| 0 | 5 | 2 | [1, 1, 0, 1] | 2 | [1, 1, 1, 1] |

Result [2, 1, 1, 0].

<!-- snippet: modules/17-range-queries/examples/0315-count-of-smaller-numbers-after-self.cpp#solution -->
```cpp
class Solution {
public:
    vector<int> countSmaller(vector<int>& nums) {
        int n = (int)nums.size();
        // Coordinate compression: each value -> its rank among the distinct values (0 .. m-1).
        vector<int> sorted_vals(nums);
        sort(sorted_vals.begin(), sorted_vals.end());
        sorted_vals.erase(unique(sorted_vals.begin(), sorted_vals.end()), sorted_vals.end());
        int m = (int)sorted_vals.size();

        vector<int> tree(m + 1, 0);          // Fenwick over ranks: how many of each rank lie right of i
        vector<int> result(n);
        for (int i = n - 1; i >= 0; i--) {
            int r = (int)(lower_bound(sorted_vals.begin(), sorted_vals.end(), nums[i]) - sorted_vals.begin());
            int smaller = 0;
            for (int j = r; j > 0; j -= j & -j) smaller += tree[j];    // count of ranks 0 .. r-1 seen
            result[i] = smaller;
            for (int j = r + 1; j <= m; j += j & -j) tree[j]++;       // now nums[i] is "to the right"
        }
        return result;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n log n): the sort, then per element one binary search and two BIT walks. O(n) memory.

**Edge cases:**
- Duplicates: equal is not smaller, and prefix(r) counts only ranks below r.
- Negative values.
- A single element.
- Strictly increasing input: all zeros.

**Follow-ups:**
- *Can you skip compression?* Here, yes: the value range is only 2·10⁴ + 1 wide, so shift by 10⁴ and index directly. O(n log V).
- *Without a BIT?* Merge sort with original indices: while merging, each left-half element gains the number of right-half elements already placed before it. Also O(n log n).
- *The total number of inversions?* The sum of all answers, in `long long` (up to ~5·10⁹); see `count_inversions` above.

## 3. Segment trees — range sum / min, lazy propagation, range assignment

### Concept

A segment tree is a binary tree over the array. The root covers [0, n−1]. A node covering [lo, hi] has children covering [lo, mid] and [mid+1, hi]. Leaves are single elements. Every node stores the answer (sum, min, …) for its range, computed from its two children. The height is ⌈log₂ n⌉.

```text
                    [0,7]
            [0,3]               [4,7]
       [0,1]     [2,3]     [4,5]     [6,7]
      [0] [1]   [2] [3]   [4] [5]   [6] [7]
query [1, 6] uses [1], [2,3], [4,5], [6]: at most 2 nodes per level
```

- **Query [l, r]:** recurse from the root. A node entirely inside [l, r] returns its stored answer. A node entirely outside returns the identity (0 for sum, +∞ for min). A node that partly overlaps asks both children. On each level at most two nodes partly overlap (the ones containing the boundaries l and r), and only those recurse, so at most four nodes per level are visited: O(log n).
- **Point update:** walk from the root to the leaf, then recompute each ancestor from its children on the way back: O(log n).
- **Any associative operation works**, no inverse needed: sum, min, max, gcd, matrix product, or a small struct of facts about the range (below). That is what a segment tree adds over a BIT.

**Two layouts.**
- The *recursive* tree numbers the root 1 and the children of p as 2p and 2p+1. It needs up to 4n slots and is the easiest to extend (lazy tags, descents, custom merges).
- The *iterative* bottom-up tree stores the leaves at t[n..2n−1] and node p = op(t[2p], t[2p+1]). It needs exactly 2n slots and no recursion.

The iterative query walks up from both ends of the half-open range [l, r) at the current level. While l < r:
- If l is a right child (odd), its parent would also cover l − 1, which is outside. So take t[l] alone and step right.
- If r is odd, r − 1 is a left child whose sibling r is outside. So take t[r − 1] alone.

Then both move up a level. For a non-commutative operation, keep a left and a right accumulator so the pieces are combined in order. Any n works: for n not a power of two, a few internal nodes mix unrelated leaves, but the walk never uses them. The test checks this with string concatenation for every n up to 33.

**Why 4n.** The depth d is ⌈log₂ n⌉, so indices stay below 2^(d+1) < 4n. 2n is not enough for the recursive layout: n = 6 uses index 13.

**Lazy propagation.** A range update touches up to n leaves. Instead, stop at the O(log n) nodes that the range fully covers (the same nodes a query would use). Update their stored answer directly, and leave a **tag**: "everything below me still has this update pending". When a later operation needs to go below a tagged node, it first **pushes** the tag to the two children. The invariant: a node's stored answer is correct for its range, given that none of its ancestors holds an undelivered tag. Every operation pushes on its way down, so every node it reads satisfies that.

**Two kinds of update: tag composition.** For range add *and* range assign, make the tag a function applied to every element below the node:

  f(x) = (has_set ? set_to : x) + add

A new update g arriving at a node whose tag is f must become g ∘ f (g happens after f):
- g = assign v: g(f(x)) = v, whatever f was. The tag becomes {set v, add 0}. **The assign wipes pending adds** (and any older assign).
- g = add d: g(f(x)) = f(x) + d. The tag's add grows by d; has_set and set_to stay.

When pushing, each child receives the parent's tag composed after its own, by the same two rules: the set part first, then the add. Order matters: "add 3, then set 5" leaves 5, "set 5, then add 3" leaves 8. A tag that stored the two updates separately, without an order, couldn't tell these apart.

A four-element trace, all zeros at the start (sums shown):
1. add 5 on [0, 3]: the root is covered. root.sum = 20, root.tag = {add 5}; nothing below is touched.
2. assign 7 on [0, 1]: the root is only partly covered, so push first: [0,1] and [2,3] get {add 5} (sum 10 each). [0,1] is covered: sum = 14, tag = {set 7, add 0}, and the +5 is gone. root.sum = 14 + 10 = 24.
3. add 1 on [0, 0]: push [0,1]'s {set 7} to both leaves (7 each); leaf 0 is covered: 8. Recompute [0,1] = 15, root = 25.
4. sum [0, 3] = root.sum = 25 = 8 + 7 + 5 + 5.

**Descent: the first index with value ≥ x.** Store the max per node. A subtree whose max is below x can't contain the answer, so skip it without looking inside. Otherwise go left first. Searching from a start index `from`, the walk follows the path to `from`, rejects at most one subtree per level, then goes down one path inside the first subtree whose max passes: O(log n), not O(log² n). The same shape answers "k-th one" (store counts, go left if the left count ≥ k, else subtract and go right) and "first prefix sum ≥ x".

**Richer node info: max subarray sum.** To merge two halves' best-subarray answers you need more than the two answers. The best subarray of a range is in the left half, in the right half, or crosses the middle. A crossing one is the best suffix of the left plus the best prefix of the right. So each node carries four numbers: sum, best prefix, best suffix, best (`Info` below). The prefix and suffix can themselves cross the middle. Designing the node is always this question: what must I know about each half to answer for the whole?

**When the index range is huge.** Coordinates up to 10⁹ can't be array indices. If all positions are known in advance, compress them (Section 2). If they arrive online, use a **dynamic** segment tree: allocate nodes only along the paths an operation touches, children stored as indices into a pool (like the trie in module 18 section 1). That's O(q log C) nodes for q operations over a coordinate range of size C.

### Template

<!-- snippet: templates/segment_tree.hpp#recursive -->
```cpp
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
```
<!-- /snippet -->

<!-- snippet: templates/segment_tree.hpp#iterative -->
```cpp
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
```
<!-- /snippet -->

Both O(n) build, O(log n) per update and query. For range min over a `vector<int> a`: `SegTree st(a, INT_MAX, [](int x, int y) { return min(x, y); });`.

The lazy tree (`LazySegTree`: range add, range assign, range sum / min / max, descent). Its node and tag types:

<!-- snippet: templates/segment_tree.hpp#lazy_types -->
```cpp
// What a node knows about its whole range. It already includes the node's own pending tag.
struct Node { long long sum = 0, mn = 0, mx = 0; };
// A promise to the node's children, not yet delivered: every element x below this node
// becomes (has_set ? set_to : x) + add.
struct Tag { bool has_set = false; long long set_to = 0, add = 0; };
```
<!-- /snippet -->

Its `apply` / `push` / `update` follow the CSES 1735 worked example below exactly, except that a node holds sum, min and max: an assign sets all three, an add shifts all three. The descent:

<!-- snippet: templates/segment_tree.hpp#descent -->
```cpp
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
```
<!-- /snippet -->

All O(log n) per operation; memory 4n nodes + 4n tags. The max-subarray node, plugged into `SegTree` in the test file:

<!-- snippet: templates/tests/segment_tree_test.cpp#max_subarray_node -->
```cpp
// Richer node info: the best subarray of a range lies in the left half, in the right half, or crosses
// the middle, and a crossing one = (best suffix of the left) + (best prefix of the right). So every
// node carries four numbers, and the prefix/suffix can themselves extend across the middle.
struct Info { long long sum, pref, suf, best; };

Info leaf(long long v) { return {v, v, v, v}; }

Info combine(const Info& L, const Info& R) {
    return {L.sum + R.sum,
            max(L.pref, L.sum + R.pref),               // stays in L, or takes all of L plus a prefix of R
            max(R.suf, R.sum + L.suf),                 // stays in R, or takes all of R plus a suffix of L
            max({L.best, R.best, L.suf + R.pref})};    // left, right, or across the middle
}
```
<!-- /snippet -->

Its identity element is {0, −∞, −∞, −∞} with −∞ = −10¹⁸, small enough that adding two of them doesn't overflow.

### Pitfalls

- **Recursive tree sized 2n:** out of bounds for n = 6 and many other n. ASan catches it here; a judge may not. Use 4n.
- **Forgetting `push` in the query.** Queries go below tagged nodes too; without the push they read stale children.
- **Forgetting to recompute the parent** after updating the children.
- **Tag order.** Deliver the set, then the add. An assign must reset the pending add to 0.
- **Wrong identity.** Min needs +∞ (INT_MAX), not 0. Sum needs 0. An iterative tree returns the identity for an empty range, so pick one that can't be mistaken for an answer.
- **Overflow in `v * len`** for sums: `long long` for values, tags and sums.
- **One accumulator in the iterative query** with a non-commutative operation: pieces come out in the wrong order.
- **`(lo + hi) / 2` in a dynamic tree over coordinates near 2³¹:** it overflows. Write `lo + (hi - lo) / 2`.
- Recursion depth is only ⌈log₂ n⌉ + 1, so stack overflow is not a risk here, unlike tree problems in module 12.

### Recognize it when…

- Updates and queries interleave, and the query is **not invertible** (min, max, gcd) or needs **richer facts** (max subarray, count plus position).
- **Range updates** (add, assign, flip) with range queries → lazy propagation.
- "The first position from i on with value ≥ x", "the k-th one", "the leftmost index whose prefix sum reaches x" → descent.
- n and q around 10⁵–10⁶.
- Coordinates up to 10⁹ → compress offline, or allocate nodes dynamically.

The list problems here: [732. My Calendar III](https://leetcode.com/problems/my-calendar-iii/), whose pattern note offers two routes (a sweep over a sorted map, or a dynamic segment tree; check the constraints before choosing), and [699. Falling Squares](https://leetcode.com/problems/falling-squares/), coordinate compression plus a range-assign / range-max tree, as its note says.

### Worked example: CSES 1735. Range Updates and Sums
[CSES 1735](https://cses.fi/problemset/task/1735) · stdin/stdout · teaching example (not on the list)

**Problem (paraphrased):** An array of n values receives q operations: add x to every element in [a, b]; set every element in [a, b] to x; or print the sum of [a, b]. n, q ≤ 2·10⁵; initial values and x up to 10⁶.

**Signals:** two kinds of range update plus range sums, 2·10⁵ of each: O(log n) per operation.

**Brute force, and why it fails:** loop over the range each time: O(nq) = 4·10¹⁰.

**Key insight:** a lazy segment tree whose tag is the function x ↦ (has_set ? set_to : x) + add. A new assign wipes whatever is pending; a new add stacks on top; a push delivers the set first, then the add. The four-element trace in the Concept is exactly this code running.

**Dry run:** the sample in array terms (0-based): a = [2, 3, 1, 1, 5, 3].

| operation | array afterwards | printed |
|---|---|---|
| sum [2, 4] | unchanged | 1 + 1 + 5 = 7 |
| add 2 on [1, 3] | [2, 5, 3, 3, 5, 3] | – |
| sum [2, 4] | unchanged | 3 + 3 + 5 = 11 |
| set 5 on [1, 3] | [2, 5, 5, 5, 5, 3] | – |
| sum [2, 4] | unchanged | 5 + 5 + 5 = 15 |

The hand-made `.2.in` adds what the sample doesn't test: adds pending on nodes that an overlapping assign then covers, adds arriving on top of pending assigns, single-element queries that force pushes to the leaves, and sums past 2³¹.

<!-- snippet: modules/17-range-queries/examples/cses-1735-range-updates-and-sums.cpp#solution -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** O((n + q) log n) time. O(n) memory: 4n sums and 4n tags.

**Edge cases:**
- An assign, then an add on an overlapping range (`.2.in`).
- Sums up to ~4·10¹⁶: `long long` everywhere, including `set_to * len`.
- n = 1.
- Queries of length 1.
- The whole array.

**Follow-ups:**
- *Also answer range min and max?* Keep them per node: an assign sets both to v, an add shifts both by d (`LazySegTree` in the template).
- *Why not two independent lazy arrays, one for adds and one for sets?* Without an order they can't distinguish "add then set" from "set then add". The function form records the order.
- *Could a BIT do this?* Range add + range sum, yes (`RangeFenwick`). Range assign, no: an assign isn't a sum of deltas you can precompute without reading the current values.

### Which structure when

| Structure | Update | Query | Build · memory | Pick it when |
|---|---|---|---|---|
| Prefix sums (module 06) | rebuild, O(n) | O(1) sum / XOR / count | O(n) · n + 1 | static array, invertible query |
| Difference array (module 06) | range add O(1), offline | read the values after all updates, O(n) | O(n) · n + 1 | all updates come before any query |
| Sparse table (Section 1) | none (rebuild) | O(1) min / max / gcd; O(log n) any associative op | O(n log n) · n log n | static array, idempotent query, very many queries |
| Fenwick tree (Section 2) | point add O(log n); range add via the difference trick | O(log n) prefix-based: sum / XOR / count; k-th by lifting | O(n) · n + 1 | updates + sums or counts; shortest code |
| Segment tree (Section 3) | point O(log n); range O(log n) with lazy tags (add, assign, flip) | O(log n) any associative op; descents | O(n) · 2n–4n (+ tags) | min / max under updates, range assign, richer node info, "first index where…" |
| Sqrt decomposition | O(1)–O(√n) | O(√n) | O(n) · n + √n | per-block summaries are easy but a tree merge is awkward; the offline query-reordering tricks built on it |

Sqrt decomposition, in one paragraph: cut the array into blocks of about √n and keep a summary per block. A query scans the partial blocks at its two ends (≤ 2√n elements) plus the whole blocks' summaries (≤ √n). A point update fixes one element and its block's summary. With n = 10⁵ that's ~300 steps per operation: slower than a tree, but it handles operations that don't merge cleanly.

## 4. Project — a range-query engine over a live data stream, benchmarked against brute force

The templates above are only yours once you've debugged them. The project makes you do that, deliberately. You build a small **range-query engine**: one interface (point or range updates, range queries) with several interchangeable backends: a brute-force array, a Fenwick tree, a sparse table and a lazy segment tree. A **differential tester** feeds the same random operation stream to a backend and to the brute force and stops at the first disagreement. A **benchmark** then measures where each backend wins from n = 10³ to 10⁶.

Why it's worth the time:
- **Differential testing** is the stress-testing habit from every test file in this kit, turned into a tool. It's also the fastest way to find a missing `push`.
- **The benchmark turns the table above into numbers you've measured.** "The BIT beat the segment tree by about k× at n = 10⁶" is a better interview answer than a memorized claim.
- It's a concrete, tests-first piece of engineering to talk about in the "tell me about something you built" part of an interview.

Two practical notes:
- Write the brute-force backend and the tester *first*. Every later milestone is then "make the tester pass".
- Time an optimized build. The sanitizer build (`make run`) is several times slower, and that would distort the comparison.

The spec, interface and milestones are in [projects/17-range-query-engine/README.md](../../projects/17-range-query-engine/README.md). The project code is yours to write; the templates here are the reference to test against.

## Common mistakes

- **Sparse table on a sum or XOR:** the overlapping blocks double-count. Prefix sums, or `fold`.
- **A 0-based Fenwick walk:** `j & -j` is 0 at j = 0, so the update loop never ends. Keep it 1-based inside.
- **Treating a BIT update as "set":** it adds. Store current values and add the difference.
- **`int` sums and counts:** 2·10⁵ × 10⁹ and n(n−1)/2 both overflow. `long long` by default in this module.
- **A recursive segment tree with 2n slots:** use 4n.
- **Lazy bugs,** in order of frequency: no push in the query path; parent not recomputed after the children change; add delivered before set; an assign that keeps the old pending add.
- **Identity mistakes:** 0 as the identity of min, or −∞ values that overflow when added.
- **A BIT for range min with updates:** it can't subtract a prefix min. Segment tree.
- **Mixing 1-based input with 0-based structures:** convert exactly once, at input.
- **Benchmarking a debug/sanitizer build** and concluding the wrong structure is faster.

## Say it out loud

A talk track for 307 (the same shape works for any range-query design question):
1. "Restating: an array with interleaved point updates and inclusive range-sum queries, up to 3·10⁴ of each."
2. "Brute force: a plain array gives O(1) updates and O(n) sums; a prefix-sum array is the reverse. Either way the worst case is about 9·10⁸."
3. "Both operations need to be sublinear. Sums are invertible, so a Fenwick tree works: range sum = prefix(r+1) − prefix(l), O(log n) each."
4. "The BIT stores deltas, so update(i, val) adds val − current[i]; I keep the current values in an array."
5. "O(log n) per call, O(n) memory, and the constructor is n adds, O(n log n); there's an O(n) build if it matters."
6. "Edge cases: a single element, an update to the same value, negative numbers. The sums fit in int here, but I'd use long long in general."

Follow-ups interviewers commonly ask:
- *Why not a segment tree?* It works too, with more code. It becomes necessary for min/max under updates or for range assignments.
- *What if updates cover ranges?* Two BITs for range add + range sum, or a lazy segment tree once assignments or min/max appear.
- *What if the array never changes?* Prefix sums for sums; a sparse table for min/max: O(1) per query.
- *How would you test it?* A brute-force twin and a randomized differential test, which is exactly what the test files and the project do.
- *Memory?* BIT n + 1; segment tree 2n (iterative) to 4n (recursive), doubled by lazy tags; sparse table n log n.
- *Can you do the segment tree without recursion?* Yes, the bottom-up version: leaves at n..2n−1, walk up from both ends.

## Self-check

1. Why does a sparse table answer range min in O(1) but not range sum, and what can it still do for sums?
   <details><summary>Answer</summary>The O(1) query covers [l, r] with two power-of-two blocks that may overlap. Counting an element twice doesn't change a min (op(x, x) = x) but does change a sum. For sums, either use prefix sums (O(1), no table needed), or answer with disjoint blocks, one per set bit of the length (`fold`, O(log n)).</details>
2. In a 1-based Fenwick tree, which positions does `tree[12]` cover, and which entries does prefix(13) add up?
   <details><summary>Answer</summary>lowbit(12) = 4, so tree[12] covers positions 9–12. prefix(13) visits j = 13 → 12 → 8 → 0: tree[13] (13), tree[12] (9–12), tree[8] (1–8). One entry per set bit of 13 = 1101₂.</details>
3. How do you support "add x to a[l..r]" plus "what is a[i]?" with one BIT? And range add plus range *sum*?
   <details><summary>Answer</summary>One BIT over the difference array: add(l, x), add(r + 1, −x); a[i] = prefix(i + 1). For range sums, use two BITs, over D[j] and D[j]·j: prefix(k) = k·ΣD[j] − ΣD[j]·j over j &lt; k, because each D[j] is counted (k − j) times.</details>
4. Why does the Fenwick `lower_bound` need non-negative values, and why is it O(log n) rather than O(log² n)?
   <details><summary>Answer</summary>It finds the first index where the running sum reaches the target, which is only well-defined as a single boundary if the prefix sums never decrease. It's O(log n) because it walks the tree's own blocks: pos grows by decreasing powers of two, so tree[pos + step] is exactly the block (pos, pos + step]. Each of the log n steps is O(1), instead of an O(log n) prefix() per binary-search probe.</details>
5. Why is 4n enough for the recursive segment tree, and why isn't 2n?
   <details><summary>Answer</summary>The depth is d = ⌈log₂ n⌉, so every index is below 2^(d+1), and 2^(d−1) &lt; n gives 2^(d+1) &lt; 4n. 2n fails for n = 6: node 6 covers [3, 4], so its children are 12 and 13, and 13 ≥ 12 = 2n.</details>
6. In the lazy tree, a node has tag {add 3}. What is its tag after an assign of 5 arrives? After that, an add of 2?
   <details><summary>Answer</summary>After the assign: {set 5, add 0}. The assign overrides everything pending, since every element becomes 5 whatever the pending add was. After the add: {set 5, add 2}: elements are 5 + 2 = 7. On a push, a child first gets "set 5", then "add 2".</details>
7. Why does the iterative segment tree's query keep two accumulators, and when would one be enough?
   <details><summary>Answer</summary>Pieces taken from the left end are further left than everything still to come; pieces from the right end are further right. With two accumulators, left = op(left, piece) and right = op(piece, right), the final op(left, right) keeps left-to-right order. One accumulator is enough only for a commutative operation (sum, min, max, gcd). Concatenation or the max-subarray node need two.</details>
8. Why is the descent "first index ≥ from with a[i] ≥ x" O(log n) and not O(log² n)?
   <details><summary>Answer</summary>It walks the single path toward `from`. At each level at most one fully-inside subtree is checked, and a subtree with max &lt; x is rejected in O(1) without entering it. The first subtree that passes certainly contains the answer, and the walk goes straight down inside it. That's O(log n) nodes in total.</details>
9. Pick a structure: (a) a static array with 10⁶ range-gcd queries; (b) point updates with range sums; (c) range assign with range min; (d) 10⁵ range adds, then one final read of every value.
   <details><summary>Answer</summary>(a) Sparse table: gcd is idempotent, so the query is O(1) table lookups plus one gcd. (b) Fenwick tree. (c) Lazy segment tree. (d) A difference array plus one prefix pass (module 06): all updates come before the read, so no tree is needed.</details>
10. For n = 10⁵, why must an inversion count be a `long long`, and what goes wrong in C++ if it isn't?
    <details><summary>Answer</summary>The maximum is n(n−1)/2 = 4,999,950,000 > 2³¹ − 1 ≈ 2.1·10⁹. Signed overflow is undefined behaviour: in practice a wrapped, often negative, count, with no error. UBSan flags it in this kit's builds; a judge just prints the wrong number.</details>
