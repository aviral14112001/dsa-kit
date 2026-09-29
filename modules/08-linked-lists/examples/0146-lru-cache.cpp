// 146. LRU Cache: https://leetcode.com/problems/lru-cache/
// Pattern: hash map (find the entry in O(1)) + doubly linked list (reorder and evict in O(1)).
// Module 08 section 4. Two versions: std::list + splice, and a hand-rolled list with sentinels.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace stl_version {
// [snippet:solution]
class LRUCache {
public:
    LRUCache(int capacity) : cap(capacity) {}

    int get(int key) {
        auto it = where.find(key);                     // find, not where[key]: [] would insert
        if (it == where.end()) return -1;
        items.splice(items.begin(), items, it->second);  // move to the front; iterator stays valid
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = where.find(key);
        if (it != where.end()) {                       // existing key: update it, then refresh it
            it->second->second = value;
            items.splice(items.begin(), items, it->second);
            return;
        }
        if ((int)items.size() == cap) {                // full: evict the least recently used
            where.erase(items.back().first);           // the list stores the key for this line
            items.pop_back();
        }
        items.emplace_front(key, value);
        where[key] = items.begin();
    }

private:
    int cap;
    list<pair<int, int>> items;                              // (key, value), most recent first
    unordered_map<int, list<pair<int, int>>::iterator> where;  // key -> its node in items
};
// [/snippet]
}  // namespace stl_version

namespace handrolled_version {
// [snippet:handrolled]
class LRUCache {
    struct Node {
        int key, val;
        Node* prev = nullptr;
        Node* next = nullptr;
    };
    int cap;
    unordered_map<int, Node*> where;
    Node head{}, tail{};  // sentinels: head.next is the most recent, tail.prev the least recent

    void unlink(Node* x) {
        x->prev->next = x->next;
        x->next->prev = x->prev;
    }
    void push_front(Node* x) {
        x->prev = &head;
        x->next = head.next;
        head.next->prev = x;
        head.next = x;
    }

public:
    LRUCache(int capacity) : cap(capacity) {
        head.next = &tail;
        tail.prev = &head;
    }
    ~LRUCache() {
        for (auto& [key, node] : where) delete node;
    }
    LRUCache(const LRUCache&) = delete;             // owns raw pointers: forbid copies
    LRUCache& operator=(const LRUCache&) = delete;

    int get(int key) {
        auto it = where.find(key);
        if (it == where.end()) return -1;
        Node* x = it->second;
        unlink(x);
        push_front(x);
        return x->val;
    }

    void put(int key, int value) {
        if (auto it = where.find(key); it != where.end()) {
            Node* x = it->second;
            x->val = value;
            unlink(x);
            push_front(x);
            return;
        }
        if ((int)where.size() == cap) {
            Node* lru = tail.prev;
            unlink(lru);
            where.erase(lru->key);   // read the key BEFORE freeing the node
            delete lru;
        }
        Node* x = new Node{key, value};
        push_front(x);
        where[key] = x;
    }
};
// [/snippet]
}  // namespace handrolled_version

// Brute force: a vector ordered from least to most recent. O(capacity) per operation.
class BruteLRU {
    int cap;
    vector<pair<int, int>> order;
    int find(int key) {
        for (int i = 0; i < (int)order.size(); i++)
            if (order[i].first == key) return i;
        return -1;
    }
public:
    explicit BruteLRU(int capacity) : cap(capacity) {}
    int get(int key) {
        int i = find(key);
        if (i < 0) return -1;
        auto entry = order[i];
        order.erase(order.begin() + i);
        order.push_back(entry);
        return entry.second;
    }
    void put(int key, int value) {
        int i = find(key);
        if (i >= 0) order.erase(order.begin() + i);
        else if ((int)order.size() == cap) order.erase(order.begin());
        order.push_back({key, value});
    }
};

// Runs the same scripted and random operations against any LRUCache type.
template <class Cache>
void test_cache() {
    {   // the official example
        Cache c(2);
        c.put(1, 1);
        c.put(2, 2);
        CHECK_EQ(c.get(1), 1);     // 1 becomes the most recent
        c.put(3, 3);               // evicts key 2
        CHECK_EQ(c.get(2), -1);
        c.put(4, 4);               // evicts key 1
        CHECK_EQ(c.get(1), -1);
        CHECK_EQ(c.get(3), 3);
        CHECK_EQ(c.get(4), 4);
    }
    {   // capacity 1
        Cache c(1);
        c.put(1, 10);
        c.put(2, 20);
        CHECK_EQ(c.get(1), -1);
        CHECK_EQ(c.get(2), 20);
    }
    {   // updating an existing key while full must not evict anything, and must refresh it
        Cache c(2);
        c.put(1, 1);
        c.put(2, 2);
        c.put(1, 100);             // update: 2 is now the least recent
        CHECK_EQ(c.get(2), 2);     // still present; now 1 is the least recent
        c.put(3, 3);               // evicts 1
        CHECK_EQ(c.get(1), -1);
        CHECK_EQ(c.get(2), 2);
        CHECK_EQ(c.get(3), 3);
    }
    // stress vs the vector version
    for (int iter = 0; iter < 300; iter++) {
        int cap = (int)t::rand_int(1, 4);
        Cache c(cap);
        BruteLRU ref(cap);
        for (int step = 0; step < 40; step++) {
            int key = (int)t::rand_int(0, 5);
            if (t::rand_int(0, 1)) {
                CHECK_EQ(c.get(key), ref.get(key));
            } else {
                int value = (int)t::rand_int(0, 999);
                c.put(key, value);
                ref.put(key, value);
            }
        }
    }
}

int main() {
    test_cache<stl_version::LRUCache>();
    test_cache<handrolled_version::LRUCache>();
    return t::summary("0146-lru-cache");
}
