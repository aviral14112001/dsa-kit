// A hash map built two ways (module 07 section 2): separate chaining that rehashes as it grows, and
// open addressing with linear probing and tombstones. Both are stress-tested against
// std::unordered_map. The 706 worked example is the fixed-size version of the first one; this
// file adds what it skips: growing the table, and the other collision strategy.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:chaining]
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
// [/snippet]

// [snippet:open_addressing]
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
// [/snippet]

// Runs the same random operations on `mine` and on std::unordered_map, comparing every answer.
template <class Map>
void stress(Map& mine, int ops, long long key_lo, long long key_hi, long long key_step) {
    unordered_map<long long, long long> oracle;
    for (int iter = 0; iter < ops; iter++) {
        long long key = t::rand_int(key_lo, key_hi) * key_step;
        long long value = t::rand_int(0, 1'000'000);
        int op = (int)t::rand_int(0, 3);
        if (op <= 1) {
            mine.put(key, value);
            oracle[key] = value;
        } else if (op == 2) {
            CHECK_EQ(mine.erase(key), oracle.erase(key) == 1);
        } else {
            long long* got = mine.find(key);
            auto it = oracle.find(key);
            CHECK_EQ(got != nullptr, it != oracle.end());
            if (got && it != oracle.end()) CHECK_EQ(*got, it->second);
        }
    }
    CHECK_EQ(mine.size(), oracle.size());
}

int main() {
    // ---- chaining ----
    ChainedHashMap<string, int> words;
    words.put("apple", 1);
    words.put("pear", 2);
    words.put("apple", 3);                           // overwrite
    CHECK_EQ(*words.find("apple"), 3);
    CHECK(words.find("fig") == nullptr);
    CHECK(words.erase("pear"));
    CHECK(!words.erase("pear"));
    CHECK_EQ(words.size(), 1);

    ChainedHashMap<long long, long long> grow;
    for (long long i = 0; i < 1000; i++) {
        grow.put(i * 7919, i);
        CHECK(grow.size() <= grow.bucket_count());   // the load factor never passes 1
    }
    CHECK_EQ(grow.bucket_count(), 1024);             // 8 -> 16 -> ... -> 1024: seven doublings
    CHECK_EQ(*grow.find(999 * 7919), 999);

    ChainedHashMap<long long, long long> chained;
    stress(chained, 20000, -300, 300, 1);            // dense keys
    ChainedHashMap<long long, long long> chained_patterned;
    stress(chained_patterned, 5000, 0, 200, 1024);   // multiples of 1024: identity hash + % 2^k collide

    // ---- open addressing ----
    ProbingHashMap<string, int> probing_words;
    probing_words.put("a", 1);
    probing_words.put("b", 2);
    CHECK(probing_words.erase("a"));
    CHECK(probing_words.find("a") == nullptr);
    CHECK_EQ(*probing_words.find("b"), 2);           // still reachable past the tombstone

    // A key stored after a tombstone must stay reachable: 0, 8 and 16 share home slot 0 when the
    // capacity is 8. Erase the first; the others sit further along the same run.
    ProbingHashMap<long long, long long> run;
    run.put(0, 10);
    run.put(8, 18);
    run.put(16, 26);
    CHECK(run.erase(0));
    CHECK_EQ(*run.find(8), 18);
    CHECK_EQ(*run.find(16), 26);
    run.put(0, 11);                                  // re-insert after the tombstone
    CHECK_EQ(*run.find(0), 11);

    // Insert-erase churn on a few keys: rebuilds clear tombstones instead of growing forever.
    ProbingHashMap<long long, long long> churn;
    for (int iter = 0; iter < 20000; iter++) {
        long long key = iter % 10 + 100LL * (iter / 10);   // a fresh key every 10 steps
        churn.put(key, iter);
        churn.erase(key);
    }
    CHECK_EQ(churn.size(), 0);
    CHECK(churn.capacity() <= 16);

    ProbingHashMap<long long, long long> probing;
    stress(probing, 20000, -300, 300, 1);
    ProbingHashMap<long long, long long> probing_patterned;
    stress(probing_patterned, 5000, 0, 200, 1024);
    return t::summary("hashmap_chaining");
}
