// Module 08 section 1: a doubly linked list with head and tail sentinels. The same two link operations
// (insert_before, unlink) are the heart of the hand-rolled LRU cache in section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:dlist]
struct DNode {
    int val = 0;
    DNode* prev = nullptr;
    DNode* next = nullptr;
};

// head.next is the first real node, tail.prev the last; an empty list is head <-> tail.
// Every real node always has a real prev and next, so linking and unlinking never test for
// null and never special-case the first or last node.
class DList {
public:
    DList() { head.next = &tail; tail.prev = &head; }
    ~DList() { while (!empty()) pop_front(); }
    DList(const DList&) = delete;             // it owns raw pointers: a copy would free them twice
    DList& operator=(const DList&) = delete;

    bool empty() const { return head.next == &tail; }
    DNode* first() { return head.next; }      // == end() when empty
    DNode* last() { return tail.prev; }       // == rend() when empty
    DNode* end() { return &tail; }            // one past the last node
    DNode* rend() { return &head; }           // one before the first node

    // Link x in just before pos (pos may be end()). O(1).
    void insert_before(DNode* pos, DNode* x) {
        x->prev = pos->prev;
        x->next = pos;
        pos->prev->next = x;
        pos->prev = x;
    }
    // Unlink a real node without freeing it. O(1): x already knows both neighbours.
    void unlink(DNode* x) {
        x->prev->next = x->next;
        x->next->prev = x->prev;
    }

    void push_front(int v) { insert_before(head.next, new DNode{v}); }
    void push_back(int v) { insert_before(&tail, new DNode{v}); }
    int pop_front() { return erase(head.next); }  // precondition: !empty()
    int pop_back() { return erase(tail.prev); }   // precondition: !empty()
    void move_to_front(DNode* x) {                // the LRU cache's core move (Section 4)
        unlink(x);                                // two statements on purpose (see Pitfalls)
        insert_before(head.next, x);
    }

private:
    DNode head, tail;  // sentinels: they never hold data and are never unlinked
    int erase(DNode* x) {
        unlink(x);
        int v = x->val;
        delete x;
        return v;
    }
};
// [/snippet]

vector<int> forward(DList& l) {
    vector<int> out;
    for (DNode* x = l.first(); x != l.end(); x = x->next) out.push_back(x->val);
    return out;
}

vector<int> backward_reversed(DList& l) {  // walk the prev pointers, then flip: must equal forward()
    vector<int> out;
    for (DNode* x = l.last(); x != l.rend(); x = x->prev) out.push_back(x->val);
    reverse(out.begin(), out.end());
    return out;
}

int main() {
    DList l;
    CHECK(l.empty());
    CHECK(l.first() == l.end());
    l.push_back(2);
    l.push_back(3);
    l.push_front(1);
    CHECK_EQ(forward(l), vector<int>{1, 2, 3});
    CHECK_EQ(backward_reversed(l), vector<int>{1, 2, 3});
    l.move_to_front(l.last());                 // 3 to the front
    CHECK_EQ(forward(l), vector<int>{3, 1, 2});
    l.move_to_front(l.first());                // already first: must be a no-op
    CHECK_EQ(forward(l), vector<int>{3, 1, 2});
    CHECK_EQ(backward_reversed(l), vector<int>{3, 1, 2});
    CHECK_EQ(l.pop_back(), 2);
    CHECK_EQ(l.pop_front(), 3);
    CHECK_EQ(l.pop_front(), 1);
    CHECK(l.empty());
    CHECK(l.last() == l.rend());

    // stress: random operations vs std::deque; both link directions checked after every step
    for (int iter = 0; iter < 200; iter++) {
        DList list;
        deque<int> ref;
        for (int step = 0; step < 40; step++) {
            int op = (int)t::rand_int(0, 4);
            int v = (int)t::rand_int(0, 99);
            if (op == 0) { list.push_front(v); ref.push_front(v); }
            else if (op == 1) { list.push_back(v); ref.push_back(v); }
            else if (ref.empty()) continue;
            else if (op == 2) { CHECK_EQ(list.pop_front(), ref.front()); ref.pop_front(); }
            else if (op == 3) { CHECK_EQ(list.pop_back(), ref.back()); ref.pop_back(); }
            else {
                int k = (int)t::rand_int(0, (long long)ref.size() - 1);  // move the k-th node to the front
                DNode* x = list.first();
                for (int s = 0; s < k; s++) x = x->next;
                list.move_to_front(x);
                int moved = ref[k];
                ref.erase(ref.begin() + k);
                ref.push_front(moved);
            }
            vector<int> expected(ref.begin(), ref.end());
            CHECK_EQ(forward(list), expected);
            CHECK_EQ(backward_reversed(list), expected);
        }
    }
    return t::summary("doubly_linked_list");
}
