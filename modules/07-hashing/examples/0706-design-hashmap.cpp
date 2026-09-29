// 706. Design HashMap: https://leetcode.com/problems/design-hashmap/
// Pattern: build a hash map yourself, with separate chaining. Module 07 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
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
// [/snippet]

int main() {
    MyHashMap m;                                    // the official example, call by call
    m.put(1, 1);
    m.put(2, 2);
    CHECK_EQ(m.get(1), 1);
    CHECK_EQ(m.get(3), -1);
    m.put(2, 1);
    CHECK_EQ(m.get(2), 1);
    m.remove(2);
    CHECK_EQ(m.get(2), -1);

    MyHashMap edge;                                 // keys that share bucket 0, and the key range ends
    edge.put(0, 5);
    edge.put(10007, 6);
    edge.put(20014, 7);
    edge.put(1000000, 1000000);
    CHECK_EQ(edge.get(0), 5);
    CHECK_EQ(edge.get(10007), 6);
    edge.remove(0);                                 // removing the first of a chain keeps the rest
    CHECK_EQ(edge.get(0), -1);
    CHECK_EQ(edge.get(10007), 6);
    CHECK_EQ(edge.get(20014), 7);
    edge.remove(0);                                 // removing an absent key is a no-op
    CHECK_EQ(edge.get(1000000), 1000000);

    // Stress test vs std::unordered_map. Keys are r*10007 + small, so chains get long.
    MyHashMap mine;
    unordered_map<int, int> oracle;
    for (int iter = 0; iter < 5000; iter++) {
        int key = (int)t::rand_int(0, 99) * 10007 + (int)t::rand_int(0, 2);
        int op = (int)t::rand_int(0, 2), value = (int)t::rand_int(0, 1000000);
        if (op == 0) {
            mine.put(key, value);
            oracle[key] = value;
        } else if (op == 1) {
            mine.remove(key);
            oracle.erase(key);
        } else {
            auto it = oracle.find(key);
            CHECK_EQ(mine.get(key), it == oracle.end() ? -1 : it->second);
        }
    }
    return t::summary("0706-design-hashmap");
}
