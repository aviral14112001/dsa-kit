# 07 · Hashing
> O(1) lookup — and the mistakes that silently make it O(n).

**Time:** ~9 h of Core work ([problems](problems.md)) · **Prereqs:** modules 01 (STL containers), 03 (amortized analysis), 06 section 3 (prefix sums) · **You're done when:** you can explain why `unordered_map` is O(1) on average but O(n) in the worst case, and how to defend against the worst case · you can write a chaining hash map from scratch and explain why open addressing needs tombstones · you can hash a pair, a vector and a struct, and write the prefix-sum + hash-map count (with its `{0: 1}` seed) without looking.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Hash maps & sets | trade memory for O(1) average lookups: count, group, look up a complement | [hash_basics.cpp](examples/hash_basics.cpp) | 1, 169, 229, 49, 128 |
| 2 | Hash functions & collisions | chaining vs probing, a bounded load factor, mixed bits against bad keys | [hashmap_chaining.cpp](examples/hashmap_chaining.cpp), `SplitMixHash` in [custom_hash.cpp](examples/custom_hash.cpp) | 706 |
| 3 | Key design | the key must encode exactly "the same"; pairs, vectors and structs need a hash or an encoding | [custom_hash.cpp](examples/custom_hash.cpp) | 36 |
| 4 | Prefix sum + hash map | sum of a[i..j) = P[j] − P[i]: look up P[j] − k among earlier prefixes | 560 below, `mod` in [negative_modulo.cpp](examples/negative_modulo.cpp) | 560, 325, 525, 974 |

## 1. Hash maps & sets — frequency counts, grouping, de-duplication

**Concept.** A hash table is an array of *buckets*; it keeps each key in bucket `hash(key) % bucket_count`. With a hash that spreads keys evenly and a bounded *load factor* (elements ÷ buckets), each bucket holds O(1) keys on average, so insert, find and erase are O(1) on average. The worst case is O(n): every key in one bucket (Section 2 shows how that happens, by accident or on purpose). The price is memory: every element lives in its own heap-allocated node, plus the bucket array.

| C++ | C# | notes |
|---|---|---|
| `unordered_map<K, V>` / `unordered_set<T>` | `Dictionary<K, V>` / `HashSet<T>` | hash table, O(1) average |
| `map<K, V>` / `set<T>` | `SortedDictionary<K, V>` / `SortedSet<T>` | red-black tree, O(log n), sorted by key |
| `m[key]` read on a missing key | the indexer's get throws `KeyNotFoundException` | C++ **inserts** a default value and returns it |
| `m.at(key)` | the indexer | throws `std::out_of_range` when missing |
| `m.find(key)`, `m.count(key)`, C++20 `m.contains(key)` | `TryGetValue`, `ContainsKey` | |

<!-- snippet: modules/07-hashing/examples/hash_basics.cpp#api -->
```cpp
unordered_map<string, int> age;
age["ana"] = 31;                            // insert, or overwrite if present
age.insert({"bo", 25});                     // insert only if absent; returns {iterator, inserted?}
bool inserted = age.try_emplace("bo", 99).second;   // "bo" exists: nothing changes -> false
int a = age.at("ana");                      // a read that throws std::out_of_range when absent
bool has_cy = age.count("cy") > 0;          // 0 or 1 for a map; C++20: age.contains("cy")
if (auto it = age.find("bo"); it != age.end()) it->second++;   // ONE lookup to test and update
age.erase("bo");                            // by key; returns how many were erased (0 or 1)
int z = age["zed"];                         // TRAP: [] on a missing key INSERTS {"zed", 0}
```
<!-- /snippet -->

**`unordered_map` or `map`?**

| | `unordered_map` | `map` |
|---|---|---|
| find / insert / erase | O(1) average, O(n) worst | O(log n) worst |
| iteration order | unspecified; can change after a rehash | sorted by key |
| smallest key, predecessor, range queries | no | `begin()`, `lower_bound`, `prev` |
| pair / vector / tuple keys | need a custom hash (Section 3) | work as is (they have `operator<`) |
| what invalidates | a rehash invalidates iterators (references stay valid) | only erasing, and only the erased element |

Default to `unordered_map` for plain lookups. Switch to `map` when you need order (smallest key, ranges, predecessor or successor), when the key is awkward to hash, or when the tests may be adversarial and you'd rather not write a custom hash (Section 2).

**Iteration order is unspecified.** It differs between compilers and can change after a rehash. Sort before printing or comparing, and never let an answer depend on it.

**The three patterns: count, group, de-duplicate.** `operator[]` value-initializes a missing entry (0 for an int, an empty vector for a vector), which is exactly what counting and grouping want:

<!-- snippet: modules/07-hashing/examples/hash_basics.cpp#patterns -->
```cpp
// Frequency count: [] value-initializes a missing int to 0, so ++ just works.
unordered_map<int, int> freq;
for (int x : nums) freq[x]++;
// Grouping: [] default-constructs an empty vector the first time a key shows up.
unordered_map<int, vector<int>> positions;
for (int i = 0; i < (int)nums.size(); i++) positions[nums[i]].push_back(i);
// De-duplication in first-seen order: insert(x).second is true only the first time.
unordered_set<int> seen;
vector<int> first_seen;
for (int x : nums)
    if (seen.insert(x).second) first_seen.push_back(x);
```
<!-- /snippet -->

<!-- snippet: modules/07-hashing/examples/hash_basics.cpp#erase_loop -->
```cpp
// Erasing while iterating: erase(it) returns the iterator after it. Never ++ an erased one.
for (auto it = freq.begin(); it != freq.end();) {
    if (it->second < 2) it = freq.erase(it);
    else ++it;
}
// C++20 says the same in one line: erase_if(freq, [](const auto& kv) { return kv.second < 2; });
```
<!-- /snippet -->

<!-- snippet: modules/07-hashing/examples/hash_basics.cpp#ordered -->
```cpp
// std::map: keys come out sorted, pair keys work with no extra code, and you get lower_bound,
// prev and next. Every operation is O(log n), worst case included.
map<pair<int, int>, string> cell{{{2, 1}, "c"}, {{0, 5}, "a"}, {{0, 7}, "b"}};
string in_order;
for (const auto& [pos, label] : cell) in_order += label;   // "abc": sorted by (row, col)
auto below_row_0 = cell.lower_bound({1, INT_MIN});         // first key with row >= 1: (2, 1)
// unordered_map<pair<int, int>, string> would not compile: std::hash has no pair version (Section 3).
```
<!-- /snippet -->

**The complement lookup** is the move behind Two Sum and much of this module. While scanning, remember what you've seen; for each new element, ask the map for the partner it needs. A nested "search for the partner" loop (O(n²)) becomes one O(1) lookup per element.

**When keys are small integers, use an array.** `int count[26]` for letters, `vector<int>(maxValue + 1)` for bounded values: no hashing, no nodes, contiguous memory. Far faster than any hash map, and it has no bad worst case.

**Complexity:** O(1) average per operation, O(n) worst case; O(n) memory, with a sizeable constant per element (a node each).

### Pitfalls

- **`m[key]` in a read inserts.** The size grows, iteration shows phantom zeros, and it doesn't compile on a `const` map. Read with `find`, `count` or `at`.
- **Two lookups where one will do:** `if (m.count(k)) m[k]++;` hashes twice. Call `find` once and reuse the iterator.
- **`for (auto [k, v] : m)` copies each pair**, so changing `v` changes the copy. Write `auto&` to modify, `const auto&` to read.
- **Inserting while iterating:** a rehash invalidates every iterator. Collect first, insert afterwards.
- **Erasing while iterating:** `it = m.erase(it)`, never `m.erase(it); ++it;`.
- **No `reserve(n)` when n is known:** each rehash is O(n). Amortized that's still O(1), but it adds up.

### Recognize it when…

- "Have we seen X before?", "first repeated / first unique", "contains duplicate": a set.
- "Count occurrences", "most frequent", "appears more than n/k times": a count map (169, 229), with the O(1)-space voting trick as the usual follow-up.
- "Two elements with sum or difference equal to target": the complement lookup (1).
- "Group items that are equal up to rearrangement or some transformation": a map from a canonical key to a list (49; section 3 is about designing the key).
- "Longest run of consecutive values in an unsorted array, in O(n)": a set, and only start counting at values that begin a run (128).
- Any nested loop whose inner loop searches for a partner: replace the inner loop with a lookup.

### Worked example: 1. Two Sum
[LeetCode 1](https://leetcode.com/problems/two-sum/) · Easy

**Problem (paraphrased):** Return the indices of the two positions whose values add up to target; exactly one such pair exists. n ≤ 10⁴, values and target in [−10⁹, 10⁹].

**Signals:** "two numbers that add up to", unsorted input, and the answer is indices, so sorting would lose them unless you carry them along.

**Brute force, and why it fails:** check every pair: about 5·10⁷ steps at n = 10⁴. That passes here, but the follow-up explicitly asks for better than O(n²), and at n = 10⁵ it becomes 5·10⁹.

**Key insight:** For each x, the partner is exactly target − x. Scan left to right with a map value → index of everything seen so far; one O(1) lookup replaces the inner loop.

**Dry run:** `nums = [3, 2, 4]`, target 6

| i | nums[i] | need | map before | result |
|---|---|---|---|---|
| 0 | 3 | 3 | {} | not found; add 3 → 0 |
| 1 | 2 | 4 | {3: 0} | not found; add 2 → 1 |
| 2 | 4 | 2 | {3: 0, 2: 1} | found: [1, 2] |

Looking up before inserting is what stops i = 0 from pairing 3 with itself (3 + 3 = 6).

<!-- snippet: modules/07-hashing/examples/0001-two-sum.cpp#solution -->
```cpp
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> index_of;   // value -> its index, for every value left of i
        for (int i = 0; i < (int)nums.size(); i++) {
            int need = target - nums[i];    // |need| <= 2*10^9 < 2^31 - 1: fits in int, barely
            auto it = index_of.find(need);  // find, not []: [] would insert `need` with index 0
            if (it != index_of.end()) return {it->second, i};
            index_of[nums[i]] = i;          // AFTER the lookup, so i is never paired with itself
        }
        return {};                          // unreachable: exactly one answer is guaranteed
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) average time (one find and one insert per element), O(n) memory.

**Edge cases:**
- Duplicates, `[3, 3]` with target 6: the first 3 is already in the map when the second arrives → `[0, 1]`.
- Negatives and zeros need nothing special.
- Overflow: |target − x| ≤ 2·10⁹ < 2³¹ − 1 ≈ 2.147·10⁹, so `int` fits, barely. With wider bounds, use `long long`.

**Follow-ups:**
- *Without a hash map?* Sort (value, index) pairs and walk two pointers inward: O(n log n) time, still O(n) memory for the pairs (module 06 section 1).
- *The input is already sorted?* Two pointers directly: O(n) time, O(1) extra memory.
- *Count all pairs instead?* A count map: each element adds `count[target - x]` before inserting itself.

### Worked example: 49. Group Anagrams
[LeetCode 49](https://leetcode.com/problems/group-anagrams/) · Medium

**Problem (paraphrased):** Group the words that are anagrams of each other; any order of groups, and of words within a group, is accepted. Up to 10⁴ lowercase words of length ≤ 100.

**Signals:** "group", and "anagram" means the same letters in a different order, so you want a canonical form that forgets the order.

**Brute force, and why it fails:** compare each word with a representative of every group so far: up to n²/2 = 5·10⁷ comparisons of O(k) each, about 5·10⁹ steps when no two words match.

**Key insight:** Two words are anagrams iff their sorted letters are equal. The sorted word is therefore a key that means exactly "same group": map it to the list of its words.

**Dry run:** `["eat", "tea", "tan", "ate", "nat", "bat"]`

| word | key | groups after |
|---|---|---|
| eat | aet | aet: [eat] |
| tea | aet | aet: [eat, tea] |
| tan | ant | aet: [eat, tea] · ant: [tan] |
| ate | aet | aet: [eat, tea, ate] · ant: [tan] |
| nat | ant | aet: [eat, tea, ate] · ant: [tan, nat] |
| bat | abt | … · abt: [bat] |

<!-- snippet: modules/07-hashing/examples/0049-group-anagrams.cpp#solution -->
```cpp
class Solution {
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        unordered_map<string, vector<string>> groups;   // signature -> the words that have it
        for (const string& s : strs) {
            string key = s;
            sort(key.begin(), key.end());               // anagrams, and only anagrams, share it
            groups[key].push_back(s);                   // [] creates the empty group on first sight
        }
        vector<vector<string>> result;
        result.reserve(groups.size());
        for (auto& [key, words] : groups) result.push_back(std::move(words));   // move, don't copy
        return result;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n · k log k) time for sorting each word's letters, O(n · k) memory for the keys and groups.

**Edge cases:**
- The empty string: key `""`, its own group `[""]`.
- Duplicate words land in the same group, once each time they appear.
- A single word gives a single group.

**Follow-ups:**
- *Faster keys?* The 26 letter counts: O(k) per word instead of O(k log k) (`count_signature` in section 3). Mind the separators.
- *Arbitrary Unicode?* The sorted key still works; a count array would have to become a map.
- *Words arriving as a stream?* The same map, updated online; output any group whenever it's asked for.

## 2. Hash functions & collisions — chaining, open addressing, load factor

**Concept.** A hash function turns a key into a `size_t`; the table reduces it to a bucket index, `h % bucket_count`. Requirements: equal keys must get equal hashes; different keys should spread evenly; it must be fast. Collisions are unavoidable (far more possible keys than buckets) and come early: with m buckets, the first collision is expected after about √(πm/2) insertions (the birthday paradox), roughly 1,250 for m = 10⁶.

Two ways to handle them. Keys 8, 16, 1, 3 in a table of size 8 (8 and 16 both hash to 0):

```
separate chaining                      open addressing, linear probing
bucket 0: (8) -> (16)                  slot:  0    1    2    3    4 ...
bucket 1: (1)                                 8    16   1    3    .
bucket 2:                              16's home 0 is taken: next free slot is 1
bucket 3: (3)                          1's home 1 is now taken by 16: it goes to 2
```

| | separate chaining | open addressing (linear probing) |
|---|---|---|
| storage | an array of buckets, each a list | one flat array of slots |
| collision | append to the bucket's list | try the next slot until a free one |
| erase | unlink from the list | leave a tombstone |
| load factor α = n / m | may exceed 1; expected chain length α | must stay below 1: probe runs explode as α → 1, so keep α ≤ ~0.5–0.7 |
| memory access | pointer chasing | contiguous: fast while α is low |
| who uses it | `std::unordered_map` (its bucket interface effectively requires it), C#'s `Dictionary` | Abseil `flat_hash_map`, Rust's `HashMap`, Python's `dict` (each with a smarter probe sequence than "next slot") |

**Load factor and rehashing.** When an insert would push α above `max_load_factor()` (1.0 by default for `std::unordered_map`), the table grows (roughly doubling its bucket count) and moves every element to its new bucket: O(n) for that one insert. Doubling makes it O(1) amortized, the same argument as `vector` growth (module 03): a rehash after n inserts moves n elements, paid for by those n inserts. `reserve(n)` sizes the table up front, so n inserts cause no rehash (faster, and iterators stay valid).

**Tombstones.** Deleting is the delicate part of open addressing. A lookup stops at the first EMPTY slot, so emptying a slot in the middle of a probe run hides every key stored after it:

```
capacity 8; keys 0, 8, 16 all have home slot 0
slot:                 0      1      2      3
                      0      8      16     EMPTY
erase(0) -> EMPTY:    EMPTY  8      16     EMPTY    find(8) stops at slot 0: "absent"  WRONG
erase(0) -> DELETED:  DEL    8      16     EMPTY    find(8) probes past slot 0 to slot 1  OK
```

A tombstone still occupies its slot, so it counts toward the load; otherwise a table full of tombstones has no EMPTY slot left and a probe loop never ends. A rebuild drops them.

**Adversarial keys (anti-hash tests).** In GCC's and Clang's standard libraries, `std::hash` of an integer is the integer itself, and the bucket is `key % bucket_count` (the bucket counts are primes).

- **The attack:** anyone who knows (or guesses) the bucket count sends keys that are all multiples of it. They all land in one bucket, and n operations cost O(n²). `custom_hash.cpp` does exactly that below, producing one chain of length 2000.
- **When it matters:** when tests can be chosen against your code (contests with a hacking phase); fixed test sets rarely do it.
- **The fix** is a few lines: a mixing hash (splitmix64) with a per-run random offset.
- **Identity hashing hurts even without an attacker:** in a table whose size is a power of two, keys that are all multiples of 1024 share a handful of slots (`hashmap_chaining.cpp` tests exactly this case).

### Template

A generic chaining map that grows. The [706 worked example](#worked-example-706-design-hashmap) below is the fixed-size version LeetCode asks for; this one adds the size counter and the rehash:

<!-- snippet: modules/07-hashing/examples/hashmap_chaining.cpp#chaining -->
```cpp
// Separate chaining that grows: 706's map plus a size counter and a rehash. Doubling the bucket
// count whenever size would pass it keeps the load factor (average chain length) <= 1.
template <class K, class V, class Hash = hash<K>>
class ChainedHashMap {
public:
    V* find(const K& key) {                          // nullptr when absent
        for (auto& [k, v] : buckets_[index(key)])
            if (k == key) return &v;
        return nullptr;
    }

    bool erase(const K& key) {
        auto& chain = buckets_[index(key)];
        for (size_t i = 0; i < chain.size(); i++)
            if (chain[i].first == key) {
                swap(chain[i], chain.back());        // order in a chain doesn't matter
                chain.pop_back();
                size_--;
                return true;
            }
        return false;
    }

    // Insert or overwrite. When an insert would push the load factor past 1, double the bucket
    // count first. A rehash moves all n entries, O(n), but the next one is n inserts away
    // (2n buckets now), so each insert pays O(1) amortized.
    void put(const K& key, const V& value) {
        if (V* v = find(key)) { *v = value; return; }
        if (size_ + 1 > buckets_.size()) rehash(2 * buckets_.size());
        buckets_[index(key)].push_back({key, value});
        size_++;
    }

    size_t size() const { return size_; }
    size_t bucket_count() const { return buckets_.size(); }

private:
    vector<vector<pair<K, V>>> buckets_ = vector<vector<pair<K, V>>>(8);
    size_t size_ = 0;

    size_t index(const K& key) const { return Hash{}(key) % buckets_.size(); }

    void rehash(size_t bucket_count) {
        vector<vector<pair<K, V>>> old(bucket_count);
        swap(old, buckets_);                         // buckets_: empty chains; old: every entry
        for (auto& chain : old)
            for (auto& entry : chain)                // index() depends on the bucket count,
                buckets_[index(entry.first)].push_back(std::move(entry));   // so every entry moves
    }
};
```
<!-- /snippet -->

Open addressing with linear probing and tombstones:

<!-- snippet: modules/07-hashing/examples/hashmap_chaining.cpp#open_addressing -->
```cpp
// Open addressing, linear probing: one flat array of slots. A key sits in its home slot
// (hash % capacity) or in the first free slot after it, wrapping around at the end.
// erase() can't just mark the slot EMPTY: lookups stop at the first EMPTY slot, so a key stored
// further along the same run would vanish. It leaves a tombstone (DELETED) that lookups probe
// past. Rebuilding the table drops the tombstones.
template <class K, class V, class Hash = hash<K>>
class ProbingHashMap {
public:
    void put(const K& key, const V& value) {
        if (2 * (full_ + deleted_ + 1) > slots_.size()) rebuild();   // keep used slots <= half
        size_t i = probe(key);
        if (slots_[i].state == FULL) { slots_[i].value = value; return; }
        slots_[i] = {key, value, FULL};
        full_++;
    }

    V* find(const K& key) {
        size_t i = probe(key);
        return slots_[i].state == FULL ? &slots_[i].value : nullptr;
    }

    bool erase(const K& key) {
        size_t i = probe(key);
        if (slots_[i].state != FULL) return false;
        slots_[i].state = DELETED;                   // a tombstone, NOT EMPTY
        full_--;
        deleted_++;
        return true;
    }

    size_t size() const { return full_; }
    size_t capacity() const { return slots_.size(); }

private:
    enum State : char { EMPTY, FULL, DELETED };
    struct Slot {
        K key{};
        V value{};
        State state = EMPTY;
    };
    vector<Slot> slots_ = vector<Slot>(8);
    size_t full_ = 0, deleted_ = 0;                  // tombstones lengthen probes, so they count as used

    // key's slot if present, else the EMPTY slot that ends its probe run. Always terminates:
    // at most half the slots are FULL or DELETED.
    size_t probe(const K& key) const {
        size_t i = Hash{}(key) % slots_.size();
        while (slots_[i].state != EMPTY && !(slots_[i].state == FULL && slots_[i].key == key))
            i = (i + 1) % slots_.size();
        return i;
    }

    // Re-insert the live entries only. Double the capacity if they fill more than a quarter of
    // it; otherwise keep the size: the rebuild was needed only to clear tombstones.
    void rebuild() {
        size_t capacity = slots_.size();
        if (4 * (full_ + 1) > capacity) capacity *= 2;
        vector<Slot> old(capacity);
        swap(old, slots_);
        full_ = deleted_ = 0;
        for (auto& s : old)
            if (s.state == FULL) {
                slots_[probe(s.key)] = {std::move(s.key), std::move(s.value), FULL};
                full_++;
            }
    }
};
```
<!-- /snippet -->

The standard defense against anti-hash inputs, and the attack it defends against:

<!-- snippet: modules/07-hashing/examples/custom_hash.cpp#splitmix -->
```cpp
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
```
<!-- /snippet -->

<!-- snippet: modules/07-hashing/examples/custom_hash.cpp#anti_hash -->
```cpp
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
```
<!-- /snippet -->

With `SplitMixHash` the same 2000 keys spread out: the test checks that no bucket holds more than 16.

**Complexity:** O(1) expected per operation for both tables (α ≤ 1 for chaining, α ≤ ½ including tombstones for probing), O(n) worst case; a rehash is O(n), O(1) amortized.

### Pitfalls

- **Weak combines for composite keys.** `h = a + b` or `a ^ b` make (1, 2) and (2, 1) collide, and `a ^ a = 0` sends every (x, x) to one bucket. Pack, then mix (Section 3).
- **A custom key without `operator==`** (or with one that disagrees with the hash): a compile error, or equal keys with different hashes ending up as duplicate entries.
- **Open addressing:** emptying a slot on erase (breaks probe runs); not counting tombstones as used (an endless probe loop once no EMPTY slot remains).
- **A signed hash in your own table:** C++ hashes are `size_t`, but a hand-rolled `int` hash can go negative, and `negative % m` is a negative index.
- **`reserve` vs `rehash`:** `reserve(n)` takes an element count, `rehash(m)` a bucket count.
- **Assuming `unordered_map` is always the fast choice:** 10⁶ int keys still means 10⁶ node allocations. Call `reserve` first, or use a `vector` when the keys are dense.

### Recognize it when…

- "Design a HashMap / HashSet without built-in hash tables": chaining with a fixed prime bucket count, then talk about growth (706).
- The interviewer asks "what's the worst case?" after you used `unordered_map`: O(n) per operation with colliding keys; mention splitmix64 or falling back to `map`.
- A hash-map solution times out on an adversarial judge, or on keys like multiples of a large prime: anti-hash input. Switch to `SplitMixHash`.
- Composite keys (pairs, tuples, structs): Section 3.

### Worked example: 706. Design HashMap
[LeetCode 706](https://leetcode.com/problems/design-hashmap/) · Easy

**Problem (paraphrased):** Implement `put(key, value)`, `get(key)` (−1 if absent) and `remove(key)` without any built-in hash table. Keys and values are in [0, 10⁶]; there are at most 10⁴ calls.

**Signals:** "design", "without built-in hash table libraries", bounded keys, bounded number of calls.

**Brute force, and why it fails:** a vector of (key, value) pairs scanned linearly is O(n) per call, about 5·10⁷ steps at the limits. That passes, but it isn't a hash map. A direct-address array `int val[1000001]` filled with −1 is O(1) and legal here (keys ≤ 10⁶), but it spends 4 MB however few keys you store and doesn't generalize to large or non-integer keys. Say so, then build the real thing.

**Key insight:** Separate chaining: bucket = key % m for a prime m about the size of the key set (10007), each bucket a small vector of (key, value) pairs. With at most 10⁴ keys the load factor stays around 1 or below, so each call scans about one entry.

**Dry run:** m = 10007

| call | bucket | bucket contents after |
|---|---|---|
| put(1, 1) | 1 | [(1, 1)] |
| put(10008, 5) | 10008 % 10007 = 1 | [(1, 1), (10008, 5)] ← a collision, chained |
| get(10008) | 1 | scan: key 1 ≠ 10008, then 10008 → return 5 |
| remove(1) | 1 | move (10008, 5) into slot 0, pop → [(10008, 5)] |
| get(1) | 1 | not found → −1 |

<!-- snippet: modules/07-hashing/examples/0706-design-hashmap.cpp#solution -->
```cpp
class MyHashMap {
public:
    MyHashMap() : buckets(kBuckets) {}

    void put(int key, int value) {
        auto& bucket = buckets[key % kBuckets];      // keys are >= 0, so % is a valid index
        for (auto& [k, v] : bucket)
            if (k == key) { v = value; return; }     // already present: overwrite
        bucket.push_back({key, value});
    }

    int get(int key) {
        for (auto& [k, v] : buckets[key % kBuckets])
            if (k == key) return v;
        return -1;
    }

    void remove(int key) {
        auto& bucket = buckets[key % kBuckets];
        for (size_t i = 0; i < bucket.size(); i++)
            if (bucket[i].first == key) {
                bucket[i] = bucket.back();           // order inside a bucket doesn't matter:
                bucket.pop_back();                   // overwrite with the last entry, pop, O(1)
                return;
            }
    }

private:
    // A prime near the maximum number of keys (at most 10^4 calls): load factor <= ~1,
    // so each chain holds about one entry on average.
    static constexpr int kBuckets = 10007;
    vector<vector<pair<int, int>>> buckets;          // bucket i: every (key, value) with key % kBuckets == i
};
```
<!-- /snippet -->

**Complexity:** O(1 + α) expected per call with α = keys / 10007 ≤ ~1; O(m + n) memory. In general chaining is O(n) per call when all keys share a bucket; here the key range caps that, since at most 100 keys in [0, 10⁶] share a remainder mod 10007 (multiples of 10007, say).

**Edge cases:**
- `put` on an existing key overwrites; it must not add a second entry.
- `remove` of an absent key does nothing.
- Keys 0 and 10⁶ are both valid: bucket 0 and bucket 10⁶ % 10007.

**Follow-ups:**
- *The number of keys isn't known?* Track the size; when it passes the bucket count, double the buckets and rehash (`ChainedHashMap` above).
- *Open addressing instead?* One flat array with linear probing; erase leaves a tombstone (`ProbingHashMap` above).
- *Negative or huge keys?* Hash to an unsigned value first (splitmix64), then reduce; in C++, `key % m` of a negative `int` is negative.

## 3. Key design — tuples, sorted signatures, custom hashable objects

**Concept.** A hash map groups by key equality, so design the key so that `key(a) == key(b)` exactly when a and b are "the same" for your problem. Everything after that is making the key hashable.

| "the same" means | canonical key |
|---|---|
| anagrams | the sorted letters, or the 26 letter counts |
| the same cell or point | `(r, c)` as a pair, or one integer `r * cols + c` |
| the same line direction | `(dy, dx)` divided by their gcd, with the sign fixed |
| the same sequence of numbers | the vector itself, or a string with separators |
| the same multiset of values | the sorted vector |

**Pair keys.** `map<pair<int, int>, V>` works as is. `unordered_map<pair<int, int>, V>` doesn't compile, because `std::hash` has no pair version. Give it a hash, or avoid hashing pairs by packing each pair into one integer:

<!-- snippet: modules/07-hashing/examples/custom_hash.cpp#pair_hash -->
```cpp
// std::hash has no pair overload, so unordered_map<pair<int, int>, V> doesn't compile without
// one. Pack both halves into 64 bits (see pair_key below), then mix.
struct PairHash {
    size_t operator()(const pair<int, int>& p) const {
        uint64_t packed = ((uint64_t)(uint32_t)p.first << 32) | (uint32_t)p.second;
        return SplitMixHash{}(packed);
    }
};
// Usage: unordered_map<pair<int, int>, int, PairHash> seen;
```
<!-- /snippet -->

<!-- snippet: modules/07-hashing/examples/custom_hash.cpp#encode -->
```cpp
// Or skip the custom hash: turn the pair into ONE integer, and the default hash just works.
// Grid cells (0 <= c < cols): the row-major index, like flattening a 2D array.
long long cell_key(int r, int c, int cols) { return (long long)r * cols + c; }
// Any two ints: 32 bits each. The (uint32_t) casts stop a negative number's sign bits from
// spilling into the other half.
long long pair_key(int a, int b) { return (long long)(((uint64_t)(uint32_t)a << 32) | (uint32_t)b); }
```
<!-- /snippet -->

**Vector keys.** `map<vector<int>, V>` needs nothing extra; `unordered_map` needs a hash that folds the elements in order; a string key is the quickest to write, but only with separators: without them, {1, 23} and {12, 3} both become "123".

<!-- snippet: modules/07-hashing/examples/custom_hash.cpp#vector_key -->
```cpp
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
```
<!-- /snippet -->

**Sorted signature vs count signature.** For anagrams (49), sorting a word costs O(k log k) while counting letters costs O(k + 26): better for long words, about the same for short ones. The count key must be unambiguous, so separate the counts:

<!-- snippet: modules/07-hashing/examples/0049-group-anagrams.cpp#count_signature -->
```cpp
// O(k) signature instead of O(k log k): the 26 letter counts, written out with separators.
// Without them, counts {1, 11} and {11, 1} would both read "111".
string count_signature(const string& s) {
    int count[26] = {};
    for (char c : s) count[c - 'a']++;
    string key;
    for (int c : count) key += to_string(c) + '#';
    return key;
}
```
<!-- /snippet -->

**Custom struct keys.** Either pass a hash functor as the container's third template argument, or specialize `std::hash` once. Both need `operator==`, which C++20 can generate with `= default`. C# bridge: this is overriding `GetHashCode` and `Equals`; the functor is like passing an `IEqualityComparer<T>` to the `Dictionary` constructor.

<!-- snippet: modules/07-hashing/examples/custom_hash.cpp#struct_hash -->
```cpp
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
```
<!-- /snippet -->

**Normalized keys.** Anything that can be written several ways needs one canonical form before it becomes a key. Directions and slopes are the classic case: 1/3 isn't exact as a double, two computations of "the same" slope can differ in the last bit, and dx = 0 divides by zero. Reducing by the gcd gives an exact fraction; fixing the sign makes (1, 2) and (−1, −2) identical.

<!-- snippet: modules/07-hashing/examples/custom_hash.cpp#slope_key -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** building and hashing the key is part of the cost. A length-k vector or string key costs O(k) per hash and per equality check; a sorted signature costs O(k log k) to build.

### Pitfalls

- **XOR-combining hashes** is symmetric (h(a, b) = h(b, a)) and sends every (x, x) to 0. Pack-then-mix, or fold polynomially.
- **Packing without the `(uint32_t)` casts:** a negative value's sign bits overwrite the other half, so `pair_key(1, -1)` would equal `pair_key(0, -1)` (the test checks this).
- **`r * C + c` with C smaller than the real column count** collides; with big grids, `r * C` overflows `int`. Use `long long` and the true width.
- **String keys without separators**, and **floating-point keys** at all.
- **A key that isn't canonical:** gcd without the sign fix splits one line into two keys; forgetting to lowercase or trim splits one word into several.
- **A struct key without `operator<` (map) or hash + `==` (unordered_map)** gives a long template error; read its first line.

### Recognize it when…

- "Group…", "count identical…", "are these the same up to…": design a canonical key.
- Grid coordinates or (x, y) points in a set or map: `PairHash`, or an encoded integer.
- One unit among several (row, column, 3×3 box…) plus a value: a composite key that names the unit, e.g. the box index (r/3)·3 + c/3 (36).
- BFS or DP states that are tuples (position, mask, steps…): pack them into one integer when the ranges allow, otherwise a struct plus a hash.
- Points on the same line, equal ratios: a gcd-reduced fraction with a fixed sign.

## 4. Prefix sum + hash map — subarray-sum-equals-K and its whole family

**Concept.** Module 06's prefix sums: P[0] = 0 and P[j] = a[0] + … + a[j−1], so the sum of a[i..j−1] is P[j] − P[i]. Therefore:

> a subarray ending at index j − 1 sums to k ⟺ some earlier prefix P[i] (i < j) equals P[j] − k.

Scan left to right, keeping a hash map of the prefixes seen so far; one lookup per step answers "how many subarrays ending here sum to k?". That is O(n) instead of O(n²) pairs.

```
index:       0    1    2    3             k = 2
a:           2   -1    1    2
P:      0    2    1    2    4
             ^         ^    ^
P[4] - k = 2 matches P[1] and P[3]: the subarrays a[1..3] = [-1, 1, 2] and a[3..3] = [2]
```

The `{0: 1}` seed is P[0], the empty prefix. Without it, a subarray that starts at index 0 (P[j] − P[0] = k) is never counted.

**Why not a sliding window?** A window needs "growing the window can only increase the sum", which holds only when every value is non-negative (module 06 section 2). With negatives, a sum that is too big can come back down, so neither pointer's move is safe. The prefix map doesn't care about signs.

**The family.** The same scan with a different map:

| question | map: key → value | why it works |
|---|---|---|
| how many subarrays sum to k (560) | prefix → how many times seen | each earlier prefix equal to P − k starts one such subarray |
| longest subarray with sum k (325) | prefix → first index seen | for a fixed end, the earliest start is the longest; a later equal prefix never beats it |
| sum divisible by k (974) | prefix mod k → count | P[j] − P[i] is a multiple of k ⟺ P[j] and P[i] leave the same remainder |
| as many 0s as 1s (525) | read 0 as −1, then prefix → first index | "balanced" means the ±1 sum is 0 |
| XOR instead of sum | prefix XOR → count | XOR of a[i..j) = P[j] ^ P[i], and P[i] = P[j] ^ k |

Each variant needs its own seed for the empty prefix (what does "P[0], ending before index 0" become?) and its own answer to "look up first, or insert first?". Work both out before you code.

**The negative-modulo trap.** C++'s `%` keeps the dividend's sign: `-7 % 3 == -1`. Used as a key, −1 and 2 are the same remainder class but different keys, so the remainder of a negative prefix never meets its partners.

<!-- snippet: modules/07-hashing/examples/negative_modulo.cpp#mod -->
```cpp
// C++'s % truncates toward zero, so -7 % 3 == -1 (Python says 2). As map keys, -1 and 2 are the
// same remainder class but different keys: normalize every remainder into [0, m).
long long mod(long long a, long long m) { return (a % m + m) % m; }   // needs m > 0
```
<!-- /snippet -->

**Template:** the 560 solution below is the counting template; the other rows change what the map stores.

**Complexity:** O(n) average time, O(n) memory (at most n + 1 distinct prefixes; at most k distinct remainders in the mod variant).

### Pitfalls

- **Forgetting the seed** misses every subarray that starts at index 0.
- **Updating the map before the lookup:** with k = 0, P − k = P finds the prefix you just added and counts an empty subarray.
- **Overwriting the first index** in a "longest" variant keeps the latest start, which gives the shortest span, not the longest.
- **`int` prefix sums overflow:** 10⁵ values of 10⁹ reach 10¹⁴. Use `long long` (560's constraints happen to fit in `int`; don't count on that elsewhere).
- **Raw `%` as a key** with negative numbers: −1 and k − 1 are one class but two keys.
- **k = 0 in a mod problem** divides by zero; check the constraints.
- **Reaching for a sliding window** when values can be negative.

### Recognize it when…

- "Number of (contiguous) subarrays with sum / XOR equal to k": a count map (560).
- "Longest (or shortest) subarray with sum k": a first-index (or last-index) map (325).
- "Subarray sum divisible by k", "a multiple of k": remainder keys, normalized (974).
- "Equal number of 0s and 1s", "balanced": transform to ±1, then look for sum 0 (525).
- Values can be negative and n is around 10⁵: O(n²) is too slow and a window is invalid.
- "Submatrices with sum k": fix a pair of rows, collapse the columns into a 1D array, run the 1D version: O(R² · C).

### Worked example: 560. Subarray Sum Equals K
[LeetCode 560](https://leetcode.com/problems/subarray-sum-equals-k/) · Medium

**Problem (paraphrased):** Count the contiguous, non-empty subarrays whose sum is exactly k. n ≤ 2·10⁴, values in [−1000, 1000], k in [−10⁷, 10⁷].

**Signals:** "number of subarrays", "sum equals k", negative values allowed (so no sliding window), n = 2·10⁴.

**Brute force, and why it fails:** every start, with a running sum over every end: n²/2 = 2·10⁸ additions. That is borderline at best, and it is O(n²) when O(n) exists. (Recomputing each sum from scratch makes it O(n³).)

**Key insight:** A subarray ending at the current element sums to k iff some earlier prefix equals prefix − k. Keep the counts of earlier prefixes in a hash map, seeded with `{0: 1}` for the empty prefix, and add `count[prefix - k]` at every step.

**Dry run:** `nums = [2, -1, 1, 2]`, k = 2

| x | prefix | prefix − k | seen before the lookup | adds | count | seen after |
|---|---|---|---|---|---|---|
| 2 | 2 | 0 | {0:1} | 1 | 1 | {0:1, 2:1} |
| −1 | 1 | −1 | {0:1, 2:1} | 0 | 1 | {0:1, 2:1, 1:1} |
| 1 | 2 | 0 | {0:1, 2:1, 1:1} | 1 | 2 | {0:1, 2:2, 1:1} |
| 2 | 4 | 2 | {0:1, 2:2, 1:1} | 2 | 4 | {0:1, 2:2, 1:1, 4:1} |

The last row adds 2 because two earlier prefixes equal 2, giving the subarrays [−1, 1, 2] and [2]. That is why the map stores counts, not just presence. The answer, 4, counts [2] (index 0), [2, −1, 1], [−1, 1, 2] and [2] (index 3).

<!-- snippet: modules/07-hashing/examples/0560-subarray-sum-equals-k.cpp#solution -->
```cpp
class Solution {
public:
    int subarraySum(vector<int>& nums, int k) {
        // |prefix| <= 2*10^4 * 1000 = 2*10^7 here, so int is safe; in general use long long.
        unordered_map<int, int> seen;   // prefix value -> how many earlier prefixes have it
        seen[0] = 1;                    // the empty prefix: lets a subarray start at index 0
        int prefix = 0, count = 0;
        for (int x : nums) {
            prefix += x;                // prefix = sum of everything up to and including x
            // A subarray ending here sums to k  <=>  some earlier prefix equals prefix - k.
            auto it = seen.find(prefix - k);
            if (it != seen.end()) count += it->second;
            seen[prefix]++;             // AFTER the lookup: a subarray needs at least one element
        }
        return count;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) average time (one find and one increment per element), O(n) memory (at most n + 1 distinct prefixes).

**Edge cases:**
- k = 0: `[0, 0, 0]` has 6 zero-sum subarrays. Looking up before inserting keeps the empty subarray out (`[1]` with k = 0 gives 0).
- A subarray that starts at index 0 needs the `{0: 1}` seed.
- Negative k and negative values change nothing.
- Overflow: here |prefix| ≤ 2·10⁷ fits in `int`; with bigger values, use `long long` for the prefix and the keys.

**Follow-ups:**
- *The longest such subarray instead?* Store first indices instead of counts (325).
- *Sums divisible by k?* Key by the normalized remainder (974).
- *All values positive?* A sliding window also works, in O(1) extra memory (module 06 section 2).
- *A 2D matrix?* Fix two rows, collapse the columns, and run this on the 1D sums.

## Common mistakes

- Reading with `m[key]`: it inserts. Use `find`, `count` or `at`.
- Letting output order depend on `unordered_map` iteration. Sort first.
- Using `unordered_map<pair<int,int>, …>` without a hash (compile error), or hashing a pair with `a ^ b` (collisions).
- Claiming `unordered_map` is O(1) with no qualifier: it's O(1) *average*, O(n) worst case. Say both, and know the fix (splitmix64, or `map`).
- Open addressing that empties slots on erase, or doesn't count tombstones toward the load.
- Prefix-map bugs: a missing seed, inserting before the lookup, overwriting first indices, `int` overflow, and raw `%` on negative sums.
- A hash map where an array would do (letters, small bounded values): slower, for nothing.
- Keys that aren't canonical: slopes as doubles, gcd without the sign fix, strings without separators.

## Say it out loud

A talk track for 560:

1. "Restating: count the contiguous non-empty subarrays that sum to exactly k; values can be negative."
2. "Brute force fixes each start and extends a running sum over each end: O(n²), about 2·10⁸ steps at n = 2·10⁴."
3. "A sliding window doesn't work because negatives break monotonicity."
4. "With prefix sums, sum(i..j) = P[j+1] − P[i], so I need, for each end, the number of earlier prefixes equal to the current prefix minus k. A hash map of prefix counts gives that in O(1)."
5. "I seed it with {0: 1} for the empty prefix, look up before inserting, and add the count each step: O(n) time and O(n) space on average."
6. "Edge cases: k = 0, subarrays starting at index 0, all negative values, and overflow (I'd use long long if the bounds were bigger)."

Follow-ups interviewers commonly ask in this module:
- "What's the worst case of `unordered_map`? How would someone trigger it, and how do you defend?" (colliding keys; splitmix64 with a random offset, or `map`)
- "How is `unordered_map` implemented? What happens on a rehash?" (chaining; the buckets grow and every element is re-bucketed, O(1) amortized)
- "`map` or `unordered_map` here, and why?"
- "How would you hash a pair or a custom object?"
- "Why the `{0: 1}`? What changes for the longest subarray instead of the count?"
- "Can you do it with less memory?" (Two Sum when only a yes/no is needed: sort in place, then two pointers; 560 when all values are non-negative: a sliding window)
- "What if the data doesn't fit in memory?" (partition by hash into files that do, process each partition separately)

## Self-check

1. What are the average and worst-case costs of `unordered_map::find`, and what makes the worst case happen?
<details><summary>Answer</summary>

O(1) average, O(n) worst case. The worst case happens when many keys share a bucket: a poor hash for the key distribution, or adversarial keys chosen against the identity hash (multiples of the bucket count).
</details>

2. What does `m[k]` do when k is absent? Why is that a bug in a membership check?
<details><summary>Answer</summary>

It inserts `{k, V{}}` (0 for ints) and returns a reference to it. A check like `if (m[k] > 0)` therefore grows the map, shows up in iteration and in `size()`, and doesn't compile on a const map. Use `find`, `count` or `contains`.
</details>

3. How do separate chaining and linear probing each handle a collision, and a deletion?
<details><summary>Answer</summary>

Chaining appends the colliding key to its bucket's list and deletes by unlinking it. Linear probing stores the key in the next free slot after its home slot, and deletes by leaving a tombstone, so that lookups keep probing past it to the keys stored later in the same run.
</details>

4. Why must tombstones count toward the load factor in open addressing?
<details><summary>Answer</summary>

They occupy slots and lengthen probe runs just like live keys. If only live keys counted, repeated insert/erase could fill every slot with tombstones; a lookup for an absent key would then find no EMPTY slot to stop at and loop forever. Counting them triggers a rebuild that clears them.
</details>

5. Why is rehashing O(1) amortized, even though one rehash costs O(n)?
<details><summary>Answer</summary>

The table doubles its bucket count, so after a rehash at size n the next one comes only after about n more inserts. The O(n) move is spread over those n inserts: O(1) each, the same argument as `vector`'s doubling.
</details>

6. Why doesn't `unordered_map<pair<int, int>, int>` compile? Give two fixes.
<details><summary>Answer</summary>

The standard library has no `std::hash<pair<int, int>>`. Fix 1: pass a hash functor (`PairHash`: pack both 32-bit halves into 64 bits and mix) as the third template argument. Fix 2: encode the pair into one `long long` key (`r * cols + c`, or 32/32-bit packing). A third option is `map<pair<int, int>, int>`, which needs only `operator<`.
</details>

7. Why is `hash(a) ^ hash(b)` a poor hash for a pair?
<details><summary>Answer</summary>

XOR is symmetric, so (a, b) and (b, a) always collide, and x ^ x = 0 sends every (x, x) to the same bucket. With the identity hash for ints, it doesn't mix bits either: the 10⁶ pairs with both values below 1000 share just 1024 possible hash values (every XOR is below 1024).
</details>

8. In 560, what goes wrong without the `{0: 1}` seed? And if you insert the current prefix before the lookup?
<details><summary>Answer</summary>

Without the seed, subarrays that start at index 0 (current prefix == k) are never counted. Inserting first goes wrong exactly when k = 0: the lookup for prefix − 0 finds the prefix just inserted and counts an empty subarray (for k ≠ 0, prefix − k can't be the prefix itself).
</details>

9. Why can't a sliding window solve 560 in general?
<details><summary>Answer</summary>

A window relies on monotonicity: extending it never decreases the sum, so when the sum is too big you can safely move the left end. With negative values, extending can decrease the sum, so neither move can be justified. Prefix sums with a hash map don't depend on signs.
</details>

10. What is `-7 % 3` in C++, and how do you get the mathematical remainder?
<details><summary>Answer</summary>

−1: C++ truncates toward zero, so the result takes the sign of the dividend. `((a % m) + m) % m` gives a value in [0, m) for m > 0; here, 2.
</details>
