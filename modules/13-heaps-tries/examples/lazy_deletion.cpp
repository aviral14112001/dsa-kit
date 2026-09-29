// Lazy deletion: how to erase from, or change priorities in, a std::priority_queue, which has
// neither erase nor decrease-key. Module 13 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:by_value]
// A min-heap with erase(x). Erased values stay inside the heap until they reach the top, where
// they're thrown away. Each pushed copy is thrown away at most once, so the cleanup is amortized
// into the pushes: every operation is still O(log n) amortized.
struct ErasableMinHeap {
    priority_queue<int, vector<int>, greater<int>> heap;
    unordered_map<int, int> doomed;     // value -> copies erased but still inside `heap`
    int live = 0;                       // heap.size() counts the doomed copies too; this doesn't

    void push(int x) { heap.push(x); live++; }
    void erase(int x) { doomed[x]++; live--; }        // precondition: a live copy of x is stored
    int top() { discard_doomed(); return heap.top(); }
    void pop() { discard_doomed(); heap.pop(); live--; }
    int size() const { return live; }

    void discard_doomed() {             // invariant afterwards: heap.top() (if any) is live
        while (!heap.empty()) {
            auto it = doomed.find(heap.top());
            if (it == doomed.end()) return;
            heap.pop();
            if (--it->second == 0) doomed.erase(it);
        }
    }
};
// [/snippet]

// [snippet:by_version]
// "Change the priority of item id" without decrease-key: push a fresh entry and remember the
// current priority. An entry that disagrees with `current` is stale and is skipped when it
// surfaces. (Dijkstra does exactly this with its dist[] array; see module 15.)
struct UpdatableMaxHeap {
    priority_queue<pair<int, int>> heap;    // (priority, id): highest priority on top
    unordered_map<int, int> current;        // id -> its live priority; absent = not stored

    void set(int id, int priority) {        // insert or update: O(log n)
        current[id] = priority;
        heap.push({priority, id});
    }
    void remove(int id) { current.erase(id); }
    bool empty() { discard_stale(); return heap.empty(); }
    pair<int, int> top() { discard_stale(); return heap.top(); }    // (priority, id)
    void pop() {
        discard_stale();
        current.erase(heap.top().second);
        heap.pop();
    }

    void discard_stale() {
        while (!heap.empty()) {
            auto [priority, id] = heap.top();
            auto it = current.find(id);
            if (it != current.end() && it->second == priority) return;   // live entry
            heap.pop();                     // stale: updated or removed since it was pushed
        }
    }
};
// [/snippet]

int main() {
    ErasableMinHeap h;
    for (int x : {5, 1, 3, 1}) h.push(x);
    h.erase(1);                             // one of the two 1s
    CHECK_EQ(h.top(), 1);
    h.erase(1);
    CHECK_EQ(h.top(), 3);
    CHECK_EQ(h.size(), 2);
    h.erase(5);
    h.pop();                                // pops 3; 5 is doomed
    CHECK_EQ(h.size(), 0);
    h.push(5);                              // a new live 5 next to the doomed one
    CHECK_EQ(h.top(), 5);
    CHECK_EQ(h.size(), 1);

    UpdatableMaxHeap u;
    u.set(1, 10);
    u.set(2, 20);
    u.set(3, 15);
    CHECK_EQ(u.top(), make_pair(20, 2));
    u.set(2, 5);                            // lower item 2's priority: its old entry goes stale
    CHECK_EQ(u.top(), make_pair(15, 3));
    u.remove(3);
    CHECK_EQ(u.top(), make_pair(10, 1));
    u.pop();
    CHECK_EQ(u.top(), make_pair(5, 2));
    u.pop();
    CHECK(u.empty());

    // stress: ErasableMinHeap against a multiset
    for (int iter = 0; iter < 100; iter++) {
        ErasableMinHeap mine;
        multiset<int> brute;
        for (int op = 0; op < 100; op++) {
            int kind = (int)t::rand_int(0, 3);
            if (brute.empty() || kind == 0) {
                int x = (int)t::rand_int(0, 10);
                mine.push(x);
                brute.insert(x);
            } else if (kind == 1) {         // erase a random stored value
                auto it = next(brute.begin(), t::rand_int(0, (long long)brute.size() - 1));
                mine.erase(*it);
                brute.erase(it);            // erase ONE copy: by iterator, not by value
            } else if (kind == 2) {
                mine.pop();
                brute.erase(brute.begin());
            }
            CHECK_EQ(mine.size(), brute.size());
            if (!brute.empty()) CHECK_EQ(mine.top(), *brute.begin());
        }
    }
    // stress: UpdatableMaxHeap against a map scanned for its best (priority, id)
    for (int iter = 0; iter < 100; iter++) {
        UpdatableMaxHeap mine;
        map<int, int> brute;                // id -> priority
        for (int op = 0; op < 100; op++) {
            int kind = (int)t::rand_int(0, 3);
            int id = (int)t::rand_int(0, 6);
            if (kind <= 1) {
                int priority = (int)t::rand_int(0, 8);
                mine.set(id, priority);
                brute[id] = priority;
            } else if (kind == 2) {
                mine.remove(id);
                brute.erase(id);
            } else if (!brute.empty()) {
                mine.pop();
                pair<int, int> best{-1, -1};
                for (auto [bid, bp] : brute) best = max(best, make_pair(bp, bid));
                brute.erase(best.second);
            }
            CHECK_EQ(mine.empty(), brute.empty());
            if (!brute.empty()) {
                pair<int, int> best{-1, -1};
                for (auto [bid, bp] : brute) best = max(best, make_pair(bp, bid));
                CHECK_EQ(mine.top(), best);
            }
        }
    }
    return t::summary("lazy_deletion");
}
