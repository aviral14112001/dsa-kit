# 18 · Tries at Scale + Bit Manipulation + Number Theory + String Algorithms
> The specialist toolkit that turns a hard problem into a short one.

**Time:** ~8 h of Core work ([problems](problems.md)) · **Prereqs:** modules 04 (strings), 07 (hashing), 11 (binary search on the answer), 13 (tries), 16 (bitmask DP uses section 2's masks); module 17's Fenwick tree reuses `x & -x` · **You're done when:** (1) from a blank file you can write the XOR trie, the sieve, `mod_pow` and the prefix function, and state the invariant each one relies on; (2) you can name four C++ bit traps (precedence, `1 << 31`, shifting by ≥ the width, `__builtin_popcount` on a 64-bit value) and their fixes without looking; (3) every list problem is ticked, and 421, 137, 204, 28 and 1044 re-solve from blank files.

Every `cpp` block below is pulled from a file that `make test M=modules/18-` (examples) or `make test M=templates/tests/<name>_test` (e.g. `xor_trie_test`) compiles and runs against a brute force.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Tries at scale | Each int is a 31-bit root-to-leaf path; for max XOR, walk down taking the opposite bit whenever some stored value has it | [xor_trie.hpp](../../templates/xor_trie.hpp) | 421 📖, 1707 |
| 2 | Bit manipulation | A mask is a set; `x & (x − 1)` and `x & −x` isolate the lowest bit; XOR cancels pairs; bits can be solved one position at a time | [bit_tricks.cpp](examples/bit_tricks.cpp) | 136, 191, 645, 137 📖, 260 |
| 3 | Number theory | Sieve from i·i; Euclid halves every two steps; reduce mod p after every operation and divide by multiplying with an inverse | [number_theory.hpp](../../templates/number_theory.hpp) | 204 📖, 1922 |
| 4 | String algorithms | Never redo matched work: borders (KMP), Z-boxes, rolling hashes, mirrored palindromes, sorted suffixes | [strings.hpp](../../templates/strings.hpp) | 28 📖, 1392, 686, 647, 1044 📖, 214 |

## 1. Tries at scale — bitwise tries, maximum XOR pair, prefix aggregation

### Concept

Module 13's trie branches on letters. A **bitwise trie** branches on bits: every non-negative `int` becomes a path of 31 bits, most significant first (bit 30 down to bit 0), with two children per node.

**Max XOR against a query x, greedily from the top bit.** Bit b is worth 2ᵇ, more than all lower bits together (2ᵇ − 1). So if some stored value makes bit b of x ^ y equal to 1, taking it beats every alternative, whatever the lower bits do. At each node, look for the child with the *opposite* of x's bit. If it holds any stored value, go there and set bit b of the result; otherwise take the other child, which must exist because every stored value continues somewhere. That's 31 steps per query, and "max XOR over all pairs" is n inserts + n queries: O(31·n) instead of O(n²).

**Prefix aggregation.** Store a count on every node: `pass[v]` = how many stored values run through v. Counts give you:
- **deletion**: decrement along the path, and treat a count of 0 as "no child";
- "how many stored values start with this bit prefix";
- **counting pairs with XOR < k.** Walk down keeping x ^ y equal to k on the bits seen so far. At a bit where k has a 1, every y whose bit matches x's makes x ^ y have a 0 where k has a 1, so it's already smaller than k, whatever follows. Add that whole subtree's count, then follow the other child to keep the tie. Where k has a 0, x ^ y must have a 0 too. A tie all the way down means x ^ y = k, which isn't counted. For "XOR in [lo, hi]", count below hi + 1 and below lo, and subtract.

**Pool allocation and memory estimates.**
- Nodes live in one `vector<array<int, 2>>` indexed by int: no `new` per node, and neighbouring nodes sit close in memory.
- Node 0 is the root. Since nobody points back to the root, child id 0 can mean "no child".
- The simple bound is 1 + 31·n nodes: 6.2·10⁶ for n = 2·10⁵, about 50 MB of child links (plus 25 MB if you keep counts). A tighter one: depth d holds at most min(2ᵈ, n) nodes, so the top ~log₂ n levels are shared, giving at most about 2n + n(31 − log₂ n) ≈ 3·10⁶ nodes, 25 MB, for any input.
- A pointer-based node costs 16 bytes of pointers plus allocator overhead and one allocation each: several times the memory, and much slower to build.
- `reserve` the pool up front so it never reallocates during inserts.

**Offline query ordering.** Sometimes each query only allows some of the elements, "only elements ≤ m", with a different m per query. Rebuilding the structure per query is too slow. Instead:
1. Read all the queries first and remember each one's original index.
2. Sort the queries by threshold and the elements by value.
3. Sweep: insert elements while they're within the current query's threshold, then answer that query from the structure.
4. Write each answer back to its original index.

The structure only grows, so no deletion is needed. Decide up front what to answer when nothing has been inserted yet. This is the pattern 1707 names.

### Template

<!-- snippet: templates/xor_trie.hpp#xor_trie -->
```cpp
// Each value is a root-to-leaf path of B bits, most significant first. Nodes live in one pool
// (vectors indexed by int): no pointers, no `new` per node, at most 1 + B * (number of inserts) nodes.
struct XorTrie {
    static constexpr int B = 31;              // bits 30..0: every non-negative int
    vector<array<int, 2>> child{{0, 0}};      // node 0 is the root; child id 0 means "no child"
    vector<int> pass{0};                      // pass[v] = how many stored values run through node v

    explicit XorTrie(int expected_inserts = 0) {
        child.reserve(1 + (size_t)expected_inserts * B);
        pass.reserve(1 + (size_t)expected_inserts * B);
    }

    void insert(int x) { walk(x, +1); }
    void erase(int x) { walk(x, -1); }        // x must be stored; its nodes stay in the pool with lower counts
    bool empty() const { return pass[0] == 0; }

    // max over stored y of (x ^ y). The trie must not be empty.
    // Greedy from the top bit: a 1 at bit b is worth more than all lower bits together (2^b > 2^b - 1),
    // so take the child with the opposite bit whenever some stored value lives there.
    int max_xor(int x) const {
        int v = 0, result = 0;
        for (int b = B - 1; b >= 0; b--) {
            int want = ((x >> b) & 1) ^ 1;
            int next = child[v][want];
            if (next != 0 && pass[next] > 0) {
                result |= 1 << b;
                v = next;
            } else {
                v = child[v][want ^ 1];
            }
        }
        return result;
    }

private:
    void walk(int x, int delta) {
        int v = 0;
        pass[v] += delta;
        for (int b = B - 1; b >= 0; b--) {
            int bit = (x >> b) & 1;
            if (child[v][bit] == 0) {
                child[v][bit] = (int)child.size();    // assign first, then grow: push_back may move the pool
                child.push_back({0, 0});
                pass.push_back(0);
            }
            v = child[v][bit];
            pass[v] += delta;
        }
    }
};
```
<!-- /snippet -->

<!-- snippet: templates/xor_trie.hpp#count_less -->
```cpp
// How many stored y have (x ^ y) < k. Walk down keeping x ^ y equal to k on the bits seen so far.
// Where k has a 1, every y whose bit matches x's makes x ^ y smaller than k right here, whatever the
// lower bits are: count that whole subtree, then continue on the side that keeps the tie. O(B).
inline int count_xor_less(const XorTrie& trie, int x, int k) {
    int v = 0, count = 0;
    for (int b = XorTrie::B - 1; b >= 0; b--) {
        int xb = (x >> b) & 1, kb = (k >> b) & 1;
        if (kb == 1) {
            int smaller = trie.child[v][xb];          // x ^ y has a 0 here while k has a 1
            if (smaller != 0) count += trie.pass[smaller];
            v = trie.child[v][xb ^ 1];                // x ^ y has a 1 here, like k: still tied
        } else {
            v = trie.child[v][xb];                    // x ^ y must have a 0 here to stay <= k
        }
        if (v == 0) break;                            // no stored value continues the tie
    }
    return count;                                     // a tie all the way down means x ^ y == k: not counted
}
```
<!-- /snippet -->

Insert, erase, `max_xor`, `count_xor_less`: O(B) = O(31) each. Memory: at most 1 + 31·(inserts) nodes.

### Pitfalls

- **`1 << 31` is not 2³¹** (Section 2). B = 31 covers non-negative `int`s; for full 32-bit values use `uint32_t`, B = 32 and `1u << b`.
- **Querying an empty trie**: `max_xor` keeps stepping to child 0, which is the root, and returns a meaningless 0. Check `empty()`, or insert before you query, as 421 does.
- **Erasing a value that isn't there** drives counts negative, and later queries go wrong silently.
- **Holding a reference across `push_back`.** `auto& node = child[v];` followed by a `push_back` leaves `node` dangling when the vector reallocates. The template assigns the new index first, then grows the pool.
- **Ignoring the counts after erasing.** A child id can be non-zero while its count is 0, so check `pass[next] > 0`, not just `next != 0`.

### Recognize it when…

- "Maximum XOR of two numbers / of x with an element" (421, 1707), "count pairs whose XOR is below k / in a range".
- The maximum XOR of a *subarray*: XOR of a[l..r] is px[r+1] ^ px[l] with prefix XORs, so it's a max-XOR pair over the prefix values.
- Queries each restrict which elements may be used, via a threshold → sort the queries, sweep, insert (offline).
- n up to ~10⁵–2·10⁵ with values up to 10⁹: 31 steps per operation fits easily.

### Worked example: 421. Maximum XOR of Two Numbers in an Array
[LeetCode 421](https://leetcode.com/problems/maximum-xor-of-two-numbers-in-an-array/) · Medium

**Problem (paraphrased):** Given up to 2·10⁵ non-negative integers below 2³¹, return the largest value of nums[i] XOR nums[j] (i may equal j, which gives 0).

**Signals:** "maximum XOR" over pairs; n = 2·10⁵ forbids checking all pairs; values fit in 31 bits.

**Brute force, and why it fails:** all pairs, O(n²) = 2·10¹⁰ XORs.

**Key insight:** for each x, the best partner is found greedily from the top bit in a trie of the values seen so far. Every pair is examined when its later element is processed.

**Dry run:** [3, 10, 5, 25, 2, 8] in 5 bits: 3 = 00011, 10 = 01010, 5 = 00101, 25 = 11001. After inserting 3, 10, 5 and 25, query x = 25:

| bit | bit of 25 | want | stored values in the wanted child | move | XOR so far |
|---|---|---|---|---|---|
| 4 | 1 | 0 | 3, 10, 5 | take it | 10000 = 16 |
| 3 | 1 | 0 | 3, 5 (10 has a 1) | take it | 11000 = 24 |
| 2 | 0 | 1 | 5 (3 has a 0) | take it | 11100 = 28 |
| 1 | 0 | 1 | none (5 has a 0) | forced to 0 | 28 |
| 0 | 1 | 0 | none (5 has a 1) | forced to 1 | 28 |

25 ^ 5 = 28, and no later value beats it.

<!-- snippet: modules/18-specialist-toolkit/examples/0421-maximum-xor-of-two-numbers-in-an-array.cpp#solution -->
```cpp
class Solution {
public:
    int findMaximumXOR(vector<int>& nums) {
        const int B = 31;                                 // 0 <= nums[i] < 2^31: bits 30..0
        vector<array<int, 2>> child(1, {0, 0});           // node 0 = root; child id 0 = "none"
        child.reserve(1 + nums.size() * B);               // pool: at most 1 + 31n nodes, no reallocation
        int best = 0;
        for (int x : nums) {
            int v = 0;                                    // 1) insert x
            for (int b = B - 1; b >= 0; b--) {
                int bit = (x >> b) & 1;
                if (child[v][bit] == 0) {
                    child[v][bit] = (int)child.size();
                    child.push_back({0, 0});
                }
                v = child[v][bit];
            }
            int u = 0, cur = 0;                           // 2) best partner for x among values so far
            for (int b = B - 1; b >= 0; b--) {            //    (x itself is there: x ^ x = 0 is a floor)
                int want = ((x >> b) & 1) ^ 1;            // the opposite bit sets bit b of the XOR
                if (child[u][want] != 0) {
                    cur |= 1 << b;
                    u = child[u][want];
                } else {
                    u = child[u][want ^ 1];
                }
            }
            best = max(best, cur);
        }
        return best;
    }
};
```
<!-- /snippet -->

**Complexity:** O(31·n) time; O(31·n) memory. At n = 2·10⁵ the `reserve` asks for the 1 + 31n bound (~50 MB), of which at most ~3·10⁶ nodes (~25 MB) get used.

**Edge cases:**
- One element: the answer is 0.
- All values equal: 0.
- 0 and 2³¹ − 1 together: all 31 bits set.
- Values straddling bit 30.

**Follow-ups:**
- *Without a trie?* Build the answer bit by bit. Keep the set of prefixes (values with the lower bits masked off) and test whether best | (1 << b) is achievable: is there a prefix p with p ^ candidate also in the set? That relies on a ^ b = c ⇔ a ^ c = b. O(31·n) with hashing.
- *With deletions or a sliding window?* Keep counts and erase (the template).
- *Queries that each allow only some elements?* Offline ordering, above.

## 2. Bit manipulation — masks, set-bit counting, subset enumeration, XOR tricks

### Concept

**Masks are sets.** Bit i of a mask says whether element i is in the set: union `a | b`, intersection `a & b`, difference `a & ~b`, symmetric difference `a ^ b`, complement within n bits `a ^ ((1 << n) - 1)`, membership `(a >> i) & 1`.

**Operator precedence traps.** `&`, `^` and `|` bind *looser* than `==`, `!=` and `<`, and `<<` binds looser than `+`:

```c++
if (x & 1 == 0)     // parses as x & (1 == 0), i.e. x & 0: always false
if ((x & 1) == 0)   // what you meant: x is even
int m = 1 << n - 1; // 1 << (n - 1), not (1 << n) - 1
```

This kit's warnings flag the first and third lines (`-Wparentheses`, `-Wshift-op-parentheses`; `make test` turns them into errors). A judge usually compiles them silently. Parenthesize every bit expression inside a comparison or shift.

**lowbit and "drop the lowbit".** Take x = 12 = 01100₂:

| expression | bits | value | why |
|---|---|---|---|
| x − 1 | 01011 | 11 | the lowest 1 becomes 0 and the zeros below it become 1s |
| x & (x − 1) | 01000 | 8 | the lowest set bit is cleared |
| −x = ~x + 1 | …10100 | −12 | the +1 carries up to exactly x's lowest set bit |
| x & −x | 00100 | 4 | only the lowest set bit survives |

`x & (x − 1)` removes one set bit per step. `x & −x` is Fenwick's lowbit (module 17) and splits a set by one differing bit. x is a power of two iff x > 0 and `x & (x − 1)` is 0.

**Counting set bits.** The builtins are fast: a single instruction when the target CPU has one, a short bit-trick sequence otherwise:

| you want | GCC/Clang builtin | C++20 `<bit>` (unsigned types only) |
|---|---|---|
| number of set bits | `__builtin_popcount(x)`, `__builtin_popcountll(x)` for 64-bit | `std::popcount(x)` |
| index of the lowest set bit | `__builtin_ctz(x)`, undefined for 0 | `std::countr_zero(x)`, 32 for 0 |
| leading zeros | `__builtin_clz(x)`, undefined for 0 | `std::countl_zero(x)` |
| ⌊log₂ x⌋ (index of the highest set bit) | `31 - __builtin_clz(x)` | `std::bit_width(x) - 1` |
| is a power of two | `x && !(x & (x - 1))` | `std::has_single_bit(x)` |

`bit_tricks.cpp` checks every row against `std::bitset`. Two traps: `__builtin_popcount` takes a 32-bit unsigned, so a `long long` passed to it silently loses its upper half; and `std::popcount(-5)` doesn't compile, so cast: `std::popcount((unsigned)x)`. In an interview, expect "and without the builtin?" as the follow-up.

**Shifts: signed values and the width.**

```c++
int a = 1 << 31;          // doesn't fit in int: INT_MIN in C++20 (not 2147483648)
long long b = 1 << 40;    // UB: the shift happens in int, before the conversion to long long
long long c = 1LL << 40;  // right: shift a 64-bit one
unsigned d = 1u << 31;    // right: 2147483648
int e = -1 << 1;          // UB before C++20; -2 since
int f = -7 >> 1;          // -4: arithmetic shift, rounds toward -infinity (-7 / 2 is -3)
```

Shifting by the width or more (`1 << 32`, `1LL << 64`) is undefined in every standard. This kit's UBSan build catches it. On the machine it often gives 1, because the CPU uses only the low bits of the shift count. Rules: shift a `1LL` or `1u`/`1ULL` literal, never a negative value, and never by ≥ the width. And `-x` overflows for x = INT_MIN, so lowbit needs x > INT_MIN (or unsigned).

**Subset enumeration.** For n ≤ ~20 items, the masks 0 … 2ⁿ − 1 are all the subsets:

```c++
for (int mask = 0; mask < (1 << n); mask++)       // 2^n subsets
    for (int i = 0; i < n; i++)
        if ((mask >> i) & 1) { /* item i is in this subset */ }
```

That's O(2ⁿ · n): 2²⁰ ≈ 10⁶ subsets is fine, n = 30 is not. Module 10's 78 asks you to redo its subsets with exactly this loop.

**Submask enumeration, and why all of them cost 3ⁿ.** `s = (s − 1) & mask` steps through the submasks of mask in decreasing order (snippet below). Enumerating the submasks of *every* mask of n bits costs 3ⁿ in total, not 4ⁿ: each bit is either outside mask, in mask but not s, or in both. That's three choices per bit (equivalently Σ C(n, k)·2ᵏ = 3ⁿ). n = 15 gives 1.4·10⁷, fine; n = 20 gives 3.5·10⁹, too slow. It's the inner loop of "dp over subsets of subsets" in bitmask DP (module 16).

**XOR properties.**
- x ^ x = 0, x ^ 0 = x, and XOR is commutative and associative: XOR-ing a list cancels every value that appears an even number of times.
- XOR is its own inverse: a ^ b = c ⇔ a ^ c = b. "Find y with x ^ y = t" is a lookup of x ^ t, and prefix XORs work like prefix sums: XOR(a[l..r]) = px[r+1] ^ px[l].
- XOR is addition without carries: a + b = (a ^ b) + 2·(a & b).
- **Bits are independent** under XOR, AND and OR. When a problem is stuck on whole numbers, solve it for each bit position separately and reassemble. 137 below does exactly that.

**Gray code.** g(i) = i ^ (i >> 1) lists all n-bit values so that neighbours differ in exactly one bit, which is useful for visiting all subsets while changing one element at a time.

### Template

<!-- snippet: modules/18-specialist-toolkit/examples/bit_tricks.cpp#basics -->
```cpp
// Bit k of x, k = 0 being the least significant. & | ^ bind LOOSER than == and <, so parenthesize.
bool test_bit(int x, int k)  { return (x >> k) & 1; }
int set_bit(int x, int k)    { return x | (1 << k); }       // 1 << k is an int: fine for k <= 30
int clear_bit(int x, int k)  { return x & ~(1 << k); }
int toggle_bit(int x, int k) { return x ^ (1 << k); }
int lowbit(int x)            { return x & -x; }             // the lowest set bit, as a value (x != INT_MIN)
int drop_lowbit(int x)       { return x & (x - 1); }        // x with its lowest set bit cleared
bool is_power_of_two(long long x) { return x > 0 && (x & (x - 1)) == 0; }   // exactly one bit set
```
<!-- /snippet -->

<!-- snippet: modules/18-specialist-toolkit/examples/bit_tricks.cpp#submasks -->
```cpp
// Every submask of mask, from mask itself down to 0. s - 1 clears s's lowest set bit and sets all
// bits below it; & mask keeps only bits that belong to mask: the result is the next smaller submask.
vector<int> submasks(int mask) {
    vector<int> result;
    for (int s = mask; ; s = (s - 1) & mask) {
        result.push_back(s);
        if (s == 0) break;                    // 0 is a submask too, and (0 - 1) & mask would wrap to mask
    }
    return result;
}
```
<!-- /snippet -->

O(2^popcount(mask)) for one mask; 3ⁿ summed over all masks of n bits.

<!-- snippet: modules/18-specialist-toolkit/examples/bit_tricks.cpp#gray -->
```cpp
// Gray code: g(i) = i ^ (i >> 1). Consecutive codes differ in exactly one bit: going from i to i + 1
// flips a block of trailing bits of i, and after XOR-ing with the shifted copy only the top bit of
// that block survives.
int gray(int i) { return i ^ (i >> 1); }

// Inverse: bit k of i is the XOR of all bits of g at positions >= k, a prefix XOR from the top.
int gray_inverse(int g) {
    int i = 0;
    for (; g != 0; g >>= 1) i ^= g;
    return i;
}
```
<!-- /snippet -->

### Pitfalls

- **Precedence:** `x & 1 == 0`, `a ^ b > 0`, `1 << n - 1`. Parenthesize.
- **`int` shifts:** `1 << k` for k ≥ 31 (use `1LL << k`), shifting negatives, shift counts ≥ the width.
- **64-bit popcount:** `__builtin_popcount(long long)` truncates; use `__builtin_popcountll`.
- **`__builtin_ctz(0)` / `__builtin_clz(0)` are undefined.** Guard, or use the `<bit>` versions.
- **Right-shifting a negative number** rounds toward −∞; `x / 2` rounds toward 0. They differ for odd negatives.
- **The last submask:** stopping the loop at `s > 0` skips the empty set; `(0 − 1) & mask` wraps back to mask and loops forever if you don't break.
- **n too large for masks:** 2ⁿ with n = 40 doesn't fit in memory or time. Consider meet-in-the-middle (two halves of 2²⁰).

### Recognize it when…

- "Every element appears twice/three times except …", "without extra space": XOR, or per-bit counting (136, 137, 260). A missing plus a repeated value (645): two equations, or split by a set bit of the XOR.
- n ≤ 20 and "choose a subset", "assign each item to …": masks. n ≤ 15 with "subsets of subsets": submask enumeration, 3ⁿ.
- "Power of two", "set bits" (191), "Hamming distance", "flip bits", permissions or flags.
- "XOR of a subarray / range": prefix XOR. "Maximum XOR": the trie of section 1.
- A statement that's hard for whole numbers but easy for a single bit: solve per bit.

### Worked example: 137. Single Number II
[LeetCode 137](https://leetcode.com/problems/single-number-ii/) · Medium

**Problem (paraphrased):** Every value in the array appears exactly three times, except one that appears once; return it, in linear time and constant extra space. Up to 3·10⁴ values anywhere in the `int` range.

**Signals:** "three times except one", "constant extra space": XOR alone fails (x ^ x ^ x = x), so look at individual bits.

**Brute force, and why it fails:** a frequency map is O(n) time but O(n) space. Sorting is O(n log n) and modifies the input.

**Key insight:** for each bit position, the tripled values contribute a multiple of 3 to that bit's count. The count mod 3 is therefore the single number's bit. Counting 32 positions is O(32n). The same mod-3 counter can run on all 32 bits at once with two variables: per bit, (twos, ones) holds the count mod 3 in binary, cycling 00 → 01 → 10 → 00.

**Dry run:** [2, 2, 3, 2] (2 = 10₂, 3 = 11₂), showing ones and twos in binary:

| x | ones = (ones ^ x) & ~twos | twos = (twos ^ x) & ~ones | per bit (bit1, bit0) count mod 3 |
|---|---|---|---|
| 2 (10) | 10 | 00 | (1, 0) |
| 2 (10) | 00 | 10 | (2, 0) |
| 3 (11) | 01 | 00 | (0, 1) |
| 2 (10) | 11 | 00 | (1, 1) |

ones = 11₂ = 3. The bit-count version sees the same thing: bit 1 is set in all four values (4 mod 3 = 1) and bit 0 only in 3 (1 mod 3 = 1).

The per-bit count first. It's the version to explain first in an interview:

<!-- snippet: modules/18-specialist-toolkit/examples/0137-single-number-ii.cpp#bit_count -->
```cpp
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        unsigned result = 0;
        for (int b = 0; b < 32; b++) {
            int ones = 0;
            for (int x : nums) ones += ((unsigned)x >> b) & 1;   // unsigned: well-defined for negatives
            if (ones % 3 != 0) result |= 1u << b;               // triples add a multiple of 3 to every bit
        }
        return (int)result;
    }
};
```
<!-- /snippet -->

Then the state machine:

<!-- snippet: modules/18-specialist-toolkit/examples/0137-single-number-ii.cpp#solution -->
```cpp
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        // Per bit, (twos, ones) holds that bit's count mod 3 in binary: 00 -> 01 -> 10 -> 00.
        // These two lines are that 3-state counter, run on all 32 bits at once.
        int ones = 0, twos = 0;
        for (int x : nums) {
            ones = (ones ^ x) & ~twos;         // flip "ones" where x has a 1, unless the count is at 2
            twos = (twos ^ x) & ~ones;         // uses the NEW ones: 01 + 1 -> 10, and 10 + 1 -> 00
        }
        return ones;                           // bits whose count is 1 (mod 3): the single number
    }
};
```
<!-- /snippet -->

**Complexity:** O(32n) and O(n) time respectively; O(1) extra space for both.

**Edge cases:**
- Negative numbers and INT_MIN (the sign bit is just bit 31; the counting version shifts an `unsigned` to stay well-defined).
- A single-element array.

**Follow-ups:**
- *Every value appears k times except one?* Count each bit mod k. The state-machine version needs ⌈log₂ k⌉ variables.
- *Why is `twos` computed from the new `ones`?* The pair is updated as one transition: going 01 → 10 needs ones cleared first, so twos can pick the bit up.
- *Two singles, and everything else twice?* That's 260 on the list; its pattern note gives the split.

## 3. Number theory — sieve, GCD / LCM, modular arithmetic, fast exponentiation

### Concept

**Sieve of Eratosthenes.** Mark everything as prime, then for each i still marked, cross out its multiples. Two optimizations make it fast:
- Start crossing out at i·i: a smaller multiple i·j (j < i) has a prime factor below i and is already crossed out.
- For the same reason the outer loop stops once i·i > n.

The total work is Σ n/p over primes p ≤ √n ≈ n · ln ln √n, which is O(n log log n): about 2n crossings for n = 5·10⁶, effectively linear. `vector<bool>` stores one bit per number (5·10⁶ → 625 KB instead of 20 MB of `int`s): slower per access, but far more cache-friendly.

**Linear (smallest-prime-factor) sieve.** Record spf[x], the smallest prime dividing x. Each composite c is written exactly once, as spf(c) · (c / spf(c)): for each i, cross out p·i only for primes p ≤ spf(i). That's O(n), and factorizing any x ≤ n then takes O(log x) (divide by spf[x] repeatedly; each division at least halves x). Use it when you need many factorizations up to ~10⁷ (4n bytes of memory). To factorize a single big number (up to 10¹²), trial division by d up to √x is enough: 10⁶ steps.

**Euclid's gcd and its O(log) bound.** gcd(a, b) = gcd(b, a mod b), because a mod b = a − q·b has exactly the same common divisors as a and b. It's fast because a mod b < a/2 whenever b ≤ a (if b ≤ a/2 the remainder is below b; otherwise it's a − b < a/2). So the numbers halve at least every two steps: O(log min(a, b)). The worst case is consecutive Fibonacci numbers. `std::gcd` and `std::lcm` (`<numeric>`, C++17) exist; gcd(0, x) = x, and the gcd of an array is a fold.

**Overflow-safe lcm.** lcm(a, b) = a / gcd(a, b) · b: divide first. a · b / gcd overflows even when the answer fits: lcm(4·10⁹, 6·10⁹) = 1.2·10¹⁰ fits easily, but 4·10⁹ · 6·10⁹ = 2.4·10¹⁹ does not. The result itself must still fit (and `std::lcm` has the same limit).

**Modular arithmetic rules.** "Answer modulo 10⁹ + 7" means the true answer is astronomically large, so compute every intermediate result mod m:
- (a + b) mod m and (a · b) mod m can be reduced after every step. Products of two residues below 10⁹ + 7 stay below ~10¹⁸ and fit in `long long`; a product of three doesn't, so reduce between multiplications.
- (a − b) mod m can go negative; add m before the final `%`.
- **Division is not** (a / b) mod m: multiply by b's modular inverse instead.
- **The negative-modulo fix.** C++ `%` truncates toward zero, so the sign follows the dividend: `-7 % 3 == -1` (C# does the same; Python gives 2). Normalize with `(a % m + m) % m` before using a value as an index or comparing residues.
- `1e9 + 7` is a `double`: `x % (1e9 + 7)` doesn't compile. Write `const long long MOD = 1'000'000'007;`.

**Fast exponentiation.** Write the exponent in binary: 3¹³ = 3⁸ · 3⁴ · 3¹ because 13 = 1101₂. Square the base at every step and multiply it into the result when the current bit is 1. That's O(log e) multiplications instead of e. Don't use `pow` from `<cmath>` for integers: it computes in `double` and loses exactness past 2⁵³.

| exponent bits left | bit | result | base (squared each step) |
|---|---|---|---|
| 1101 | 1 | 3 | 3² = 9 |
| 110 | 0 | 3 | 9² = 81 |
| 11 | 1 | 3·81 = 243 | 81² = 6561 |
| 1 | 1 | 243·6561 = 1594323 = 3¹³ | – |

**Modular inverse.** b⁻¹ mod m (the x with b·x ≡ 1) exists iff gcd(b, m) = 1.
- For a **prime** p, Fermat's little theorem gives b^(p−1) ≡ 1, so b^(p−2) is the inverse: one `mod_pow`, O(log p).
- For **any** modulus, the extended Euclidean algorithm finds x, y with b·x + m·y = 1; then x mod m is the inverse.

**Bézout, in one paragraph.** For any integers a and b, some integers x, y satisfy a·x + b·y = gcd(a, b), and gcd(a, b) is the smallest positive number of that form. The extended Euclidean algorithm finds the x and y: each step's equation is rewritten in terms of the previous pair. Consequences you'll use: a·x + b·y = c has integer solutions iff gcd(a, b) divides c; a has an inverse mod m iff gcd(a, m) = 1; and if the gcd of a whole set of numbers is 1, some integer combination of them equals 1.

**nCr mod p.** C(n, r) = n! / (r! (n − r)!). Precompute fact[i] = i! mod p. Then inv_fact[maxn] with one Fermat inverse, then walk down with 1/(i−1)! = (1/i!) · i. Each query is then three table lookups and two multiplications: O(maxn + log p) setup, O(1) per query. It requires n < p (otherwise n! contains the factor p and has no inverse; Lucas' theorem handles that case, rarely needed). For n up to a few thousand and *any* modulus, Pascal's triangle C(n, r) = C(n−1, r−1) + C(n−1, r) is O(n²) and needs no inverses.

### Template

<!-- snippet: templates/number_theory.hpp#sieve -->
```cpp
// Sieve of Eratosthenes: is_prime[i] for 0 <= i <= n (n up to ~1e8), O(n log log n).
// Crossing out starts at i * i: a smaller multiple i * j with j < i has a prime factor below i, so it
// is already crossed out. For the same reason the outer loop can stop once i * i > n.
inline vector<bool> prime_sieve(int n) {
    vector<bool> is_prime(n + 1, true);
    is_prime[0] = false;
    if (n >= 1) is_prime[1] = false;
    for (int i = 2; (long long)i * i <= n; i++)
        if (is_prime[i])
            for (int j = i * i; j <= n; j += i) is_prime[j] = false;
    return is_prime;
}
```
<!-- /snippet -->

<!-- snippet: templates/number_theory.hpp#spf_sieve -->
```cpp
// Linear sieve: the smallest prime factor of every x <= n, plus the list of primes, in O(n).
// Every composite c is written exactly once, as c = p * i with p = spf(c), which forces p <= spf(i).
struct Sieve {
    vector<int> spf;                              // spf[x] = smallest prime dividing x, for 2 <= x <= n
    vector<int> primes;

    explicit Sieve(int n) : spf(max(n + 1, 2), 0) {
        for (int i = 2; i <= n; i++) {
            if (spf[i] == 0) {                    // nothing smaller divides i: prime
                spf[i] = i;
                primes.push_back(i);
            }
            for (int p : primes) {                // mark p * i for each prime p <= spf(i)
                if (p > spf[i] || (long long)p * i > n) break;
                spf[p * i] = p;
            }
        }
    }

    bool is_prime(int x) const { return x >= 2 && spf[x] == x; }

    // (prime, exponent) pairs, smallest prime first, for 1 <= x <= n.
    // O(log x): every division at least halves x.
    vector<pair<int, int>> factorize(int x) const {
        vector<pair<int, int>> factors;
        while (x > 1) {
            int p = spf[x], e = 0;
            while (x % p == 0) {
                x /= p;
                e++;
            }
            factors.push_back({p, e});
        }
        return factors;
    }
};
```
<!-- /snippet -->

<!-- snippet: templates/number_theory.hpp#gcd -->
```cpp
// Euclid: gcd(a, b) = gcd(b, a % b), since a % b = a - q*b keeps exactly the same common divisors.
// a % b < a / 2 whenever b <= a, so the numbers halve every two steps: O(log min(a, b)).
// std::gcd and std::lcm (<numeric>) do this for you; inputs here are >= 0.
inline long long gcd_euclid(long long a, long long b) {
    while (b != 0) {
        long long r = a % b;
        a = b;
        b = r;
    }
    return a;
}

// Divide before multiplying: a * b can overflow even when the lcm itself fits in 64 bits.
inline long long lcm_safe(long long a, long long b) {
    if (a == 0 || b == 0) return 0;
    return a / gcd_euclid(a, b) * b;
}
```
<!-- /snippet -->

<!-- snippet: templates/number_theory.hpp#mod_pow -->
```cpp
// C++'s % keeps the sign of the dividend: -7 % 3 == -1, not 2. Normalize before using a residue.
inline long long mod_norm(long long a, long long m) { return (a % m + m) % m; }

// base^exp mod m by repeated squaring. Write exp in binary: base^exp is the product of base^(2^k)
// over exp's set bits, and each base^(2^k) is the square of the previous one. O(log exp).
// Products of two residues must fit in long long, so m must stay below ~3e9 (1e9 + 7 is fine).
inline long long mod_pow(long long base, long long exp, long long m) {
    long long result = 1 % m;                     // 1 % m makes m == 1 return 0
    base = mod_norm(base, m);
    while (exp > 0) {
        if (exp & 1) result = result * base % m;  // this bit is set: multiply its square in
        base = base * base % m;                   // base^(2^k) -> base^(2^(k+1))
        exp >>= 1;
    }
    return result;
}
```
<!-- /snippet -->

<!-- snippet: templates/number_theory.hpp#mod_inv -->
```cpp
// Fermat: for prime p and a not divisible by p, a^(p-1) = 1 (mod p), so a * a^(p-2) = 1 (mod p).
inline long long mod_inv_prime(long long a, long long p) { return mod_pow(a, p - 2, p); }

// Extended Euclid: returns g = gcd(a, b) and sets x, y with a*x + b*y = g (Bezout), for a, b >= 0.
inline long long ext_gcd(long long a, long long b, long long& x, long long& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    long long x1, y1;
    long long g = ext_gcd(b, a % b, x1, y1);      // b*x1 + (a % b)*y1 = g
    x = y1;                                       // put a % b = a - (a / b)*b in, regroup by a and b
    y = x1 - (a / b) * y1;
    return g;
}

// Inverse of a modulo any m >= 1, prime or not. It exists iff gcd(a, m) == 1; returns -1 otherwise.
inline long long mod_inv(long long a, long long m) {
    long long x, y;
    if (ext_gcd(mod_norm(a, m), m, x, y) != 1) return -1;
    return mod_norm(x, m);                        // x can be negative
}
```
<!-- /snippet -->

<!-- snippet: templates/number_theory.hpp#binomial -->
```cpp
// nCr mod a prime p, for n <= maxn < p: O(maxn + log p) setup, then O(1) per query.
//   C(n, r) = n! / (r! (n-r)!) = fact[n] * inv_fact[r] * inv_fact[n-r]   (mod p)
// One Fermat inverse for maxn!, then walk down with 1/(i-1)! = (1/i!) * i.
struct Binomial {
    long long p;
    vector<long long> fact, inv_fact;

    Binomial(int maxn, long long prime) : p(prime), fact(maxn + 1), inv_fact(maxn + 1) {
        fact[0] = 1;
        for (int i = 1; i <= maxn; i++) fact[i] = fact[i - 1] * i % p;
        inv_fact[maxn] = mod_inv_prime(fact[maxn], p);
        for (int i = maxn; i >= 1; i--) inv_fact[i - 1] = inv_fact[i] * i % p;
    }

    long long C(int n, int r) const {
        if (r < 0 || r > n) return 0;
        return fact[n] * inv_fact[r] % p * inv_fact[n - r] % p;
    }
};
```
<!-- /snippet -->

Sieve O(n log log n); linear sieve O(n) and factorize O(log x); gcd O(log min); `mod_pow` O(log e); inverses O(log m); `Binomial` O(maxn + log p) setup and O(1) per query.

### Pitfalls

- **`i * i` in `int`** overflows once i passes 46340: loop with `long long`, or compare `i <= n / i`.
- **"Primes less than n" vs "up to n":** size the sieve and loop bounds to match the statement (204 is strictly less than n).
- **A missing reduction:** `a * b * c % MOD` overflows before the `%`. Reduce after every multiplication.
- **A negative result from `%`** after a subtraction: `(a - b) % MOD` can be negative. Use `(a - b + MOD) % MOD`, or `mod_norm`.
- **Fermat with a non-prime modulus** gives garbage. Use `mod_inv` (extended Euclid), and check that it returned −1 when gcd ≠ 1.
- **`Binomial` with n ≥ p:** the factorial is 0 mod p. Guard the constructor's maxn.
- **`pow()` from `<cmath>` for integers:** it works in doubles and loses exactness past 2⁵³. Use `mod_pow`, or a loop.
- **The `1e9 + 7` literal is a double** (see above).

### Recognize it when…

- "Return it modulo 10⁹ + 7": reduce after every operation; divide by multiplying with an inverse; fast power for big exponents (1922).
- "Count the primes up to n" (204), or many factorizations of numbers up to 10⁶–10⁷: sieve / SPF sieve.
- "How many ways / arrangements / paths" with large n and a prime modulus: nCr with factorial tables.
- "Divisible by a or b", "the n-th number divisible by …": inclusion–exclusion with lcm (divide first!).
- "gcd of all elements", "can these be combined into 1": gcd folds and Bézout.
- An exponent like 10¹⁸, or a power tower: `mod_pow`, reducing the exponent mod p − 1 when the base is coprime to a prime p.

### Worked example: 204. Count Primes
[LeetCode 204](https://leetcode.com/problems/count-primes/) · Medium

**Problem (paraphrased):** Return how many primes are strictly less than n, for 0 ≤ n ≤ 5·10⁶.

**Signals:** "count primes below n" with n in the millions: one sieve, not a primality test per number.

**Brute force, and why it fails:** trial-divide every x < n up to √x: about Σ√x ≈ (2/3)·n^1.5 ≈ 7·10⁹ steps at n = 5·10⁶ (fewer in practice, since composites fail early, but primes pay the full √x).

**Key insight:** cross out multiples instead of testing divisors. Starting at i·i and stopping when i·i ≥ n makes it O(n log log n).

**Dry run:** n = 10, numbers 2–9.

| i | i·i < 10? | crossed out |
|---|---|---|
| 2 | 4 yes | 4, 6, 8 |
| 3 | 9 yes | 9 |
| 4 | 16 no: stop | – |

Left: 2, 3, 5, 7 → 4.

<!-- snippet: modules/18-specialist-toolkit/examples/0204-count-primes.cpp#solution -->
```cpp
class Solution {
public:
    int countPrimes(int n) {                          // primes strictly less than n
        if (n < 3) return 0;
        vector<bool> composite(n, false);             // index = number; n bits, not n ints
        for (long long i = 2; i * i < n; i++) {       // long long: i * i must not overflow
            if (composite[i]) continue;
            for (long long j = i * i; j < n; j += i)  // smaller multiples of i are already crossed out
                composite[j] = true;
        }
        int count = 0;
        for (int i = 2; i < n; i++) count += !composite[i];
        return count;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n log log n) time; n bits of memory (625 KB at n = 5·10⁶).

**Edge cases:**
- n = 0, 1, 2 → 0 (strictly less than 2 leaves nothing).
- n = 3 → 1.
- n = 5·10⁶ → 348513 (checked in the test).
- `i * i` computed in `long long`.

**Follow-ups:**
- *List the primes, or factorize many numbers?* The linear sieve records every prime and each number's smallest prime factor in O(n).
- *n up to 10¹⁰?* Too much memory for a full sieve. A segmented sieve processes blocks of √n numbers using the primes up to √n.
- *Is a single number up to 10¹² prime?* Trial division up to 10⁶. For 10¹⁸, a deterministic Miller–Rabin (beyond interviews).

## 4. String algorithms — KMP, Z-function, Rabin-Karp, Manacher, suffix arrays

### Concept

The naive search tries every alignment and compares up to m characters, O(n·m). On "aaaa…ab" against a text of a's, it re-reads the same characters m times. Every algorithm here avoids redoing work it has already matched.

**The prefix function (KMP).** π[i] = the length of the longest proper prefix of s[0..i] that is also a suffix of it, called a **border**. For s = "aabaaab":

| i | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|---|
| s[i] | a | a | b | a | a | a | b |
| π[i] | 0 | 1 | 0 | 1 | 2 | 2 | 3 |

Computing it: if s[0..i] has a border of length k, then removing the last character leaves a border of s[0..i−1] of length k − 1. So the candidates for π[i] are "the previous border, extended by one", tried from the longest: π[i−1], then π[π[i−1] − 1], and so on. Each fallback is the next shorter border. It's O(n) in total: k grows by at most 1 per character, and every fallback shrinks it, so there are at most n fallbacks overall.

**Matching in O(n + m).** Run the same loop over the text, where k = how many characters of the pattern match the text ending at position i. On a mismatch, k falls back through the *pattern's* borders, and the text index never moves backwards. When k reaches m you have a match; set k = π[m−1] to keep finding overlapping ones. The same bound applies: at most n + m increments and fallbacks. (Equivalently, compute π of pattern + '#' + text, with '#' in neither string, and look for values equal to m.) Borders also give periods: the smallest period of s is n − π[n−1].

**The Z-function.** z[i] = the length of the longest common prefix of s and s[i..]. "aaabaab" → [7, 2, 1, 0, 2, 1, 0] (z[0] = n by convention here). Keep the window [l, r) that matched a prefix and reaches furthest right. For i inside it, s[i..r) equals s[i−l..r−l), so z[i] starts at min(z[i−l], r − i) for free. Only characters past r are compared, and each successful comparison moves r right: O(n). Matching: the Z-function of pattern + '$' + text has value m exactly where the pattern starts. Z and π answer the same questions; Z is often easier to reason about for "the prefix occurs at position i".

**Rabin–Karp and the rolling hash.** Treat a string as a number in base B modulo a prime M: hash(t) = t₀·B^(L−1) + … + t_{L−1}. With prefix hashes h[i] (of s[0..i)), any substring's hash is h[r] − h[l]·B^(r−l), O(1) after O(n) setup. Rabin–Karp compares the pattern's hash with every window's hash: O(n + m). Equal strings always have equal hashes; the danger is a **collision**, where different strings get equal hashes.
- **Collision probability.** For a random base, two different strings of equal length L collide with probability at most L/M: their difference is a nonzero polynomial in B of degree < L, which has at most L − 1 roots mod a prime.
- **Why a big modulus.** Checking 10⁵ windows against each other is ~5·10⁹ pairs. Treating each pair as colliding with probability 1/M, M ≈ 10⁹ gives about 5 expected collisions; M = 2⁶¹ − 1 gives about 2·10⁻⁹. So use M = 2⁶¹ − 1 (with a 128-bit multiply, as the template does), or **double hashing**: two independent moduli near 10⁹, compared as a pair, for an effective modulus near 10¹⁸.
- **Why a random base.** With a fixed base and modulus, someone can build colliding inputs in advance. Hashing mod 2⁶⁴ (letting `unsigned long long` overflow do the mod) is broken for *every* base by a known family of strings (Thue–Morse).
- **Monte Carlo vs verified.** Trusting a hash match can, with tiny probability, report a false match. Verify with `compare` when a wrong answer is unacceptable. 1044 below verifies every hit, so a returned answer is always a real repeat. Verifying doesn't fix the other direction: a collision can still hide a real repeat (1044's map keeps one start per hash), which is why the modulus must be big as well.

`rabin_karp_search(text, pattern)` in the header wraps this into an all-occurrences search, tested against KMP.

**Manacher.** It finds the longest palindrome around every center in O(n); it's the Z-function's window trick applied to palindromes.
- *One kind of center.* Picture s with a separator in every gap, "#a#b#a#": all 2n + 1 centers (n characters, n + 1 gaps) become centers of odd-length palindromes. The radius len[c] at center c equals the palindrome's length in s, and it starts at index (c − len[c]) / 2 of s.
- *Reuse the mirror.* Keep the palindrome that reaches furthest right, [center − len, right]. For c inside it, the mirror position 2·center − c is already computed, and inside the big palindrome the neighbourhood of c is a mirror image of the mirror's. So len[c] starts at min(len[mirror], right − c) for free.
- *Why O(n).* Comparisons only happen beyond `right`, and each successful one moves `right` forward.

The O(n²) alternative, expanding around each center, is fine up to n ≈ 5000.

**Suffix array + LCP.** sa lists the start indices of the suffixes in sorted order. For "banana" that's [5, 3, 1, 0, 4, 2]: a, ana, anana, banana, na, nana. lcp[i] is the longest common prefix of the neighbours sa[i] and sa[i+1]: [1, 3, 0, 0, 2].
- **Building by prefix doubling.** Rank suffixes by their first k characters. The first 2k characters of suffix i are then the pair (rank[i], rank[i + k]), so one sort by pairs doubles k. That's log n rounds, O(n log² n) with `std::sort`, or O(n log n) with radix sort.
- **Kasai's LCP in O(n).** Visit suffixes in text order: if suffix i shares h characters with its sorted neighbour, suffix i + 1 shares at least h − 1 with its own.
- **What they answer:**
  - The number of **distinct substrings** is n(n+1)/2 − Σ lcp. Each suffix contributes its prefixes, minus the ones already counted by its sorted predecessor.
  - The **longest repeated substring** is max lcp. For i < j in sorted order, LCP(sa[i], sa[j]) = min(lcp[i..j−1]), so the best pair is always adjacent.
  - **LCP of any two suffixes** is a range-min over lcp: a sparse table (module 17 section 1) makes it O(1).
  - **Pattern search** is a binary search over sa, O(m log n).

### Which algorithm for which question

| Question | Tool | Cost |
|---|---|---|
| All occurrences of a pattern in a text | KMP, or Z on pattern + '$' + text | O(n + m) |
| Borders and periods of one string | prefix function | O(n) |
| At each position, how long does the string's prefix match there? | Z-function | O(n) |
| Compare arbitrary substrings for equality, many times | rolling hash | O(n) build, O(1) per compare |
| The longest substring with a property that's monotone in length (repeats, shared by two strings) | binary search on the length + rolling hash | O(n log n) |
| All palindromic substrings, the longest palindrome | Manacher (expand around centers for small n) | O(n) |
| Distinct substrings, longest repeated substring, LCP of two suffixes | suffix array + LCP (+ sparse table) | O(n log² n) build |

### Template

Everything above is in [`templates/strings.hpp`](../../templates/strings.hpp), each piece tested against a brute force. Two of its pieces appear in full in the worked examples below, so they aren't repeated here:
- **the prefix function and KMP scan** are the 28 solution; the header's `kmp_search` is the same loop returning every (overlapping) match;
- **the rolling hash mod 2⁶¹ − 1** is inside the 1044 solution; the header's `RollingHash` wraps it with `get(pos, len)` = hash of `s.substr(pos, len)`, and `rabin_karp_search` uses it.

The suffix array (prefix doubling) and Kasai's LCP are in the header too. You're unlikely to type them in an interview; know what they answer (the Concept above). The two you might write from memory:

<!-- snippet: templates/strings.hpp#z_function -->
```cpp
// z[i] = length of the longest common prefix of s and s[i..]; z[0] = n by convention.
// [l, r) is the match window s[l..r) == s[0..r-l) reaching furthest right. For i inside it,
// s[i..r) == s[i-l..r-l), so z[i] starts at min(z[i-l], r - i) for free; only chars past r are
// compared, and each successful comparison pushes r right: O(n).
inline vector<int> z_function(const string& s) {
    int n = (int)s.size();
    vector<int> z(n, 0);
    if (n > 0) z[0] = n;
    for (int i = 1, l = 0, r = 0; i < n; i++) {
        if (i < r) z[i] = min(z[i - l], r - i);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) z[i]++;
        if (i + z[i] > r) {
            l = i;
            r = i + z[i];
        }
    }
    return z;
}
```
<!-- /snippet -->

<!-- snippet: templates/strings.hpp#manacher -->
```cpp
// Manacher: the longest palindrome around every center, O(n). There are 2n + 1 centers: odd c is the
// character s[c / 2] (odd lengths), even c is the gap before s[c / 2] (even lengths). Picture s with a
// separator in every gap, "#a#b#a#": the radius len[c] there is exactly the palindrome's length in s,
// and the palindrome starts at index (c - len[c]) / 2.
inline vector<int> manacher(const string& s) {
    int m = 2 * (int)s.size() + 1;
    // c - k and c + k have the same parity: two gaps always match, two characters must be equal.
    auto match = [&](int a, int b) { return a % 2 == 0 || s[a / 2] == s[b / 2]; };
    vector<int> len(m, 0);
    for (int c = 0, center = 0, right = 0; c < m; c++) {   // the palindrome at `center` reaches furthest right
        int k = c < right ? min(len[2 * center - c], right - c) : 0;   // its mirror's radius, clipped to it
        while (c - k - 1 >= 0 && c + k + 1 < m && match(c - k - 1, c + k + 1)) k++;
        len[c] = k;
        if (c + k > right) {
            center = c;
            right = c + k;
        }
    }
    return len;
}
```
<!-- /snippet -->

Prefix function, KMP, Z, Manacher and Kasai: O(n) (KMP O(n + m)). Rolling hash O(n) build, O(1) per substring. Suffix array O(n log² n).

### Pitfalls

- **A separator that occurs in the strings** (pattern + '#' + text with '#' in the text): matches cross the boundary. Pick a character outside the alphabet, or use `kmp_search`, which needs no separator.
- **Resetting k to 0 after a KMP match** instead of π[m−1] misses overlapping matches ("aa" in "aaaa" occurs 3 times).
- **Comparing hashes of different lengths:** the polynomial hash doesn't encode the length. Compare equal lengths only.
- **Hashing with a small or fixed modulus** (1e9+7 alone, or unsigned overflow mod 2⁶⁴): collisions or crafted anti-hash tests. Use 2⁶¹ − 1 or two moduli, with a random base.
- **`(unsigned char)` for character codes:** a plain `char` can be negative for non-ASCII bytes.
- **`__uint128_t`** exists in GCC and Clang on 64-bit targets, but not in MSVC or 32-bit builds. Double hashing with two 32-bit moduli avoids it.
- **Manacher's index mapping:** center c in [0, 2n]; odd c is character c/2; the start is (c − len[c]) / 2. Test with "abba" and "aba".
- **Suffix array tie-breaking:** a suffix that runs out of characters must sort first (the −1 in the pair).

### Recognize it when…

- "Find / count occurrences of a pattern", "the first index where needle appears": KMP or Z (28; 686 wraps a search in a repeat count).
- "Longest prefix that is also a suffix", "period", "repeated pattern": the prefix function (1392; 214 after building the right combined string).
- "Longest duplicate / repeated substring", "longest common substring of two strings": binary search + hashing, or a suffix array.
- "Palindromic substrings" (647), "longest palindrome", and n up to 10⁵–10⁶: Manacher. For n ≤ ~5000, expand around centers.
- "Number of distinct substrings": a suffix array + LCP (or hashing per length for small n).
- Many substring-equality checks: a rolling hash.

### Worked example: 28. Find the Index of the First Occurrence in a String
[LeetCode 28](https://leetcode.com/problems/find-the-index-of-the-first-occurrence-in-a-string/) · Easy

**Problem (paraphrased):** Return the index of the first occurrence of needle in haystack, or −1 if it doesn't occur. Both strings are 1 to 10⁴ lowercase letters.

**Signals:** substring search: the naive scan passes at these limits, and the follow-up is always "can you do it in O(n + m)?", which is KMP.

**Brute force, and why it fails:** try every alignment and compare. O((n − m + 1)·m): about 2.5·10⁷ steps at n = 10⁴, m = 5000 on "aaa…ab"-style inputs. Fine here, hopeless at n = 10⁶.

**Key insight:** after matching k characters and failing, you already know the last k text characters: they're needle[0..k). So resume with the longest border of needle[0..k) instead of starting over. The text index never moves back.

**Dry run:** haystack "abababca", needle "ababca" with π = [0, 0, 1, 2, 0, 1]:

| i | text[i] | k before | what happens | k after |
|---|---|---|---|---|
| 0–3 | a b a b | 0 → 3 | match | 4 |
| 4 | a | 4 | needle[4] = 'c' ≠ 'a' → k = π[3] = 2; needle[2] = 'a' matches | 3 |
| 5 | b | 3 | match | 4 |
| 6 | c | 4 | match | 5 |
| 7 | a | 5 | match: k = 6 = m | return 7 − 6 + 1 = **2** |

The naive scan would restart at index 1 and re-read characters it had already matched.

<!-- snippet: modules/18-specialist-toolkit/examples/0028-find-the-index-of-the-first-occurrence-in-a-string.cpp#brute -->
```cpp
class Solution {
public:
    int strStr(string haystack, string needle) {
        int n = (int)haystack.size(), m = (int)needle.size();
        for (int i = 0; i + m <= n; i++) {            // try every alignment
            int k = 0;
            while (k < m && haystack[i + k] == needle[k]) k++;
            if (k == m) return i;                     // on a mismatch, all k matched chars are thrown away
        }
        return -1;
    }
};
```
<!-- /snippet -->

<!-- snippet: modules/18-specialist-toolkit/examples/0028-find-the-index-of-the-first-occurrence-in-a-string.cpp#solution -->
```cpp
class Solution {
public:
    int strStr(string haystack, string needle) {
        int n = (int)haystack.size(), m = (int)needle.size();
        if (m == 0) return 0;
        vector<int> pi(m, 0);                         // pi[i] = longest proper border of needle[0..i]
        for (int i = 1; i < m; i++) {
            int k = pi[i - 1];
            while (k > 0 && needle[i] != needle[k]) k = pi[k - 1];
            if (needle[i] == needle[k]) k++;
            pi[i] = k;
        }
        for (int i = 0, k = 0; i < n; i++) {          // k = chars of needle matched, ending at haystack[i]
            while (k > 0 && haystack[i] != needle[k]) k = pi[k - 1];   // keep the longest border
            if (haystack[i] == needle[k]) k++;
            if (k == m) return i - m + 1;
        }
        return -1;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n + m) time, O(m) extra space for π. (The naive scan is O(n·m) time, O(1) space.)

**Edge cases:**
- Needle longer than haystack → −1.
- Needle equal to haystack → 0.
- A match at the very end.
- Partial matches that overlap the real one ("aaab" / "aab").

**Follow-ups:**
- *All occurrences, overlapping included?* Don't return at k == m; record it and set k = π[m−1] (`kmp_search`).
- *Rabin–Karp instead?* Also O(n + m) expected: hash the needle, roll over the windows, verify hits.
- *Many needles in one text?* Hash each needle length, or Aho–Corasick (a trie of needles with KMP-style failure links), which is beyond interview scope.

### Worked example: 1044. Longest Duplicate Substring
[LeetCode 1044](https://leetcode.com/problems/longest-duplicate-substring/) · Hard

**Problem (paraphrased):** Return any longest substring that occurs at least twice in s (occurrences may overlap), or "" if none does. 2 ≤ n ≤ 3·10⁴, lowercase letters.

**Signals:** "longest substring that repeats": a length that's monotone, and substring equality checked for many windows.

**Brute force, and why it fails:** check every pair of start positions and extend while equal: O(n²) pairs × O(n) comparison, or insert all O(n²) substrings into a set. Around 10¹³ character operations at n = 3·10⁴.

**Key insight:** if some substring of length L occurs twice, its prefix of length L − 1 does too, so the answer is the largest L where "some length-L substring repeats" is true: binary search on L (module 11). For a fixed L, a rolling hash turns each window into one number in O(1), and a hash set finds a repeat in O(n) expected. Verify each hash hit with `compare`, so a false positive can't slip through.

**Dry run:** "banana", L in [1, 5].

| lo, hi | L = mid | windows | repeat? | next |
|---|---|---|---|---|
| 1, 5 | 3 | ban, ana, nan, ana | yes: "ana" at 1 and 3 | best = 3, lo = 4 |
| 4, 5 | 4 | bana, anan, nana | no | hi = 3 |

lo > hi: the answer is the length-3 window found, "ana".

<!-- snippet: modules/18-specialist-toolkit/examples/1044-longest-duplicate-substring.cpp#solution -->
```cpp
class Solution {
    static constexpr uint64_t MOD = (1ULL << 61) - 1;        // prime; hashes collide with prob ~ L / 2^61
    static uint64_t mul(uint64_t a, uint64_t b) {            // a * b mod 2^61 - 1
        __uint128_t c = (__uint128_t)a * b;
        uint64_t r = (uint64_t)(c & MOD) + (uint64_t)(c >> 61);
        return r >= MOD ? r - MOD : r;
    }

public:
    string longestDupSubstring(string s) {
        int n = (int)s.size();
        uint64_t base = 256 + mt19937_64(random_device{}())() % (MOD - 512);   // random: no input targets it
        vector<uint64_t> h(n + 1, 0), pw(n + 1, 1);          // h[i] = hash of s[0, i), pw[i] = base^i
        for (int i = 0; i < n; i++) {
            uint64_t x = mul(h[i], base) + (unsigned char)s[i];
            h[i + 1] = x >= MOD ? x - MOD : x;
            pw[i + 1] = mul(pw[i], base);
        }
        auto window = [&](int pos, int len) {                // hash of s.substr(pos, len), O(1)
            uint64_t x = h[pos + len] + MOD - mul(h[pos], pw[len]);
            return x >= MOD ? x - MOD : x;
        };
        // Start of a length-len substring that occurs at least twice, or -1. O(n) expected.
        auto find_repeat = [&](int len) {
            unordered_map<uint64_t, int> first_start;
            first_start.reserve(n);
            for (int i = 0; i + len <= n; i++) {
                auto [it, inserted] = first_start.try_emplace(window(i, len), i);
                if (!inserted && s.compare(it->second, len, s, i, len) == 0) return i;   // verify the hit
            }
            return -1;
        };
        // "Some substring of length L occurs twice" is monotone: if it holds for L, the prefixes of that
        // substring show it for every shorter length. So binary search the largest L that holds.
        int lo = 1, hi = n - 1, best_len = 0, best_start = 0;
        while (lo <= hi) {
            int mid = lo + (hi - lo) / 2;
            int start = find_repeat(mid);
            if (start != -1) {
                best_len = mid;
                best_start = start;
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
        return s.substr(best_start, best_len);
    }
};
```
<!-- /snippet -->

**Complexity:** O(n log n) expected: log n lengths × O(n) windows, hashing in O(1) each; O(n) memory.

**Edge cases:**
- No repeat at all ("abcd" → "").
- All equal letters: overlapping occurrences, answer length n − 1.
- n = 2.
- Several valid answers: the stress test checks the length and that the returned string really occurs twice.

**Follow-ups:**
- *Deterministic?* A suffix array + LCP: the answer is the max lcp between sorted neighbours, O(n log² n) with the template, and no randomness.
- *Why a random base and the 2⁶¹ − 1 modulus?* 3·10⁴ windows per length means ~4.5·10⁸ pairs: about 0.45 expected collisions per length with a 10⁹-size modulus, several over the ~15 lengths tried. And a fixed base can be targeted.
- *Longest common substring of two strings?* The same binary search, hashing the windows of one string and probing with the other's.

## Common mistakes

- **Bit precedence:** `x & 1 == 0`, `1 << n - 1`. Parenthesize every bit expression in a comparison.
- **`1 << k` with k ≥ 31,** or `1 << 40` meant as a 64-bit value: write `1LL << k`. Never shift negatives or by ≥ the width.
- **`__builtin_popcount` on a `long long`:** use `__builtin_popcountll`, or `std::popcount` on an unsigned type.
- **Querying an empty XOR trie,** or erasing a value that isn't there.
- **Sieve loops in `int`** with `i * i`; "less than n" vs "up to n".
- **Forgetting a `% MOD`** between multiplications; a negative `%` after a subtraction; the `1e9 + 7` double literal.
- **Fermat's inverse with a non-prime modulus;** nCr with n ≥ p.
- **A KMP match that resets k to 0,** missing overlapping matches.
- **A small or fixed hash modulus,** hashes of different lengths compared, or plain `char` codes that can be negative.
- **Manacher indexing:** mixing up the transformed index and the index in s.

## Say it out loud

A talk track for 421 (bit problems follow the same arc):
1. "Restating: the maximum XOR of any two numbers in the array, n up to 2·10⁵, values below 2³¹."
2. "Brute force: all pairs, O(n²) ≈ 2·10¹⁰. Too slow."
3. "Insight: a higher bit outweighs all lower bits together, so for each number I greedily want the opposite bit from the top down. A binary trie of the numbers answers 'does a number with this prefix exist?' at each step."
4. "Insert each number, then query it: 31 steps each, so O(31·n) time. The trie has at most 31·n nodes; I allocate them from a vector pool."
5. "Edge cases: one element gives 0; values are non-negative, so 31 bits suffice and `1 << 30` is the top bit."
6. "Testing: an O(n²) brute force on random small arrays."

Follow-ups interviewers commonly ask:
- *Without the trie?* The prefix-set method: decide bits from the top, checking candidates with a hash set, using a ^ b = c ⇔ a ^ c = b.
- *Memory?* Nodes of two ints: the simple 31·n bound is ~50 MB for n = 2·10⁵, but the shared top levels cap it near 3·10⁶ nodes, ~25 MB.
- *Queries that each allow only elements ≤ m?* Sort the queries by m and insert elements as m grows (offline).
- *Without the builtin?* (for set-bit questions) Expect it every time; the `x & (x − 1)` row of the section 2 table is where to start.
- *Why that modulus?* (for hashing and counting questions) A prime near 10⁹ keeps products in 64 bits and has inverses; 2⁶¹ − 1 makes hash collisions negligible.

## Self-check

1. Why is the greedy "take the opposite bit from the top" correct for max XOR?
   <details><summary>Answer</summary>Bit b is worth 2ᵇ, and all lower bits together are worth at most 2ᵇ − 1. Any choice that makes bit b of the XOR equal to 1 beats every choice that makes it 0, whatever happens below. So the decision at each bit, from the top, never needs revisiting.</details>
2. How does counting stored y with x ^ y &lt; k work in the trie, in one sentence per case?
   <details><summary>Answer</summary>Walk down keeping x ^ y equal to k on the bits so far. Where k's bit is 1, all y in the child that matches x's bit give a 0 there, so they're already smaller: add that child's count, then continue on the other child. Where k's bit is 0, continue on the child matching x's bit. Reaching the end means x ^ y = k, which isn't counted.</details>
3. What do `x & (x - 1)` and `x & -x` compute for x = 40 (101000₂)?
   <details><summary>Answer</summary>x & (x − 1) = 100000₂ = 32 (the lowest set bit is cleared). x & −x = 001000₂ = 8 (only the lowest set bit is kept).</details>
4. Why does iterating over all submasks of every mask of n bits cost 3ⁿ?
   <details><summary>Answer</summary>Each (mask, submask) pair assigns every bit one of three states: not in mask, in mask but not in the submask, or in both. So there are 3ⁿ pairs, and the submask loop spends O(1) per pair. Equivalently, Σₖ C(n, k)·2ᵏ = (1 + 2)ⁿ.</details>
5. What's wrong with `long long big = 1 << 40;`, and with `if (x & 1 == 0)`?
   <details><summary>Answer</summary>`1 << 40` is computed in `int` before it's converted: shifting by ≥ 32 is undefined behaviour. Write `1LL << 40`. And `x & 1 == 0` parses as `x & (1 == 0)` = `x & 0` = 0, so it's always false. Write `(x & 1) == 0`.</details>
6. Why may the sieve start crossing out at i·i, and why is it O(n log log n)?
   <details><summary>Answer</summary>A multiple i·j with j &lt; i has a prime factor below i (from j), so an earlier prime already crossed it out. The work is Σ n/p over primes p ≤ √n, and the sum of 1/p over primes up to x grows like ln ln x. So the total is about n ln ln n.</details>
7. Why does `-7 % 3` give −1 in C++, and how do you get 2?
   <details><summary>Answer</summary>C++ integer division truncates toward zero (−7 / 3 = −2), and the remainder satisfies (a / b)·b + a % b = a, so −7 % 3 = −7 − (−6) = −1. The sign follows the dividend. Normalize with `((a % m) + m) % m` = 2.</details>
8. When does a modular inverse exist, and how do you compute it for a prime modulus and for a general one?
   <details><summary>Answer</summary>It exists iff gcd(a, m) = 1 (Bézout: a·x + m·y = 1 has a solution exactly then). For a prime p: Fermat, a^(p−2) mod p. For any m: extended Euclid gives x with a·x + m·y = 1, and x mod m (normalized) is the inverse.</details>
9. Why is KMP matching O(n + m) even though the inner `while` loop can run many times for one character?
   <details><summary>Answer</summary>k (the matched length) increases by at most 1 per text character, so by at most n in total, and every iteration of the fallback loop decreases it. It never goes below 0, so the total number of fallbacks over the whole scan is at most n. Computing π for the pattern is O(m) by the same argument.</details>
10. You're hashing all 10⁵ windows of a string into a hash set to find duplicates. Why is a modulus near 10⁹ not enough, and what do you use instead?
    <details><summary>Answer</summary>Duplicates are found by comparing every pair of windows: about 5·10⁹ pairs, against a 10⁹-size modulus, so several collisions are expected (the birthday bound). Use 2⁶¹ − 1 with a random base, or double hashing (two ~10⁹ moduli compared as a pair), and verify a hit with a real string comparison if a false positive would be wrong.</details>
