// Module 08 section 4: a FIFO queue backed by a singly linked list with a tail pointer.
// Unlike LeetCode code, this structure owns its nodes, so it frees them.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:linked_queue]
// Push at the tail, pop at the head: both O(1). (Popping at the tail of a SINGLY linked list
// would be O(n): you'd need the node before the tail.)
class LinkedQueue {
public:
    LinkedQueue() = default;
    LinkedQueue(const LinkedQueue&) = delete;             // a copy would share the nodes, then
    LinkedQueue& operator=(const LinkedQueue&) = delete;  // both destructors would free them
    ~LinkedQueue() { while (!empty()) pop(); }

    bool empty() const { return head == nullptr; }
    int size() const { return count; }
    int front() const { return head->val; }  // precondition: !empty()

    void push(int x) {
        ListNode* node = new ListNode(x);
        if (tail) tail->next = node;  // non-empty: link after the old tail
        else head = node;             // empty: the new node is also the front
        tail = node;
        count++;
    }
    void pop() {                      // precondition: !empty()
        ListNode* old = head;
        head = head->next;
        if (!head) tail = nullptr;    // removed the last node: tail must not dangle
        delete old;
        count--;
    }

private:
    ListNode* head = nullptr;  // front: the next to pop
    ListNode* tail = nullptr;  // back: the last pushed
    int count = 0;
};
// [/snippet]

int main() {
    LinkedQueue q;
    CHECK(q.empty());
    q.push(1);
    q.push(2);
    CHECK_EQ(q.front(), 1);
    q.pop();
    q.pop();
    CHECK(q.empty());
    q.push(3);            // after emptying, tail must have been reset or this would write freed memory
    CHECK_EQ(q.front(), 3);
    CHECK_EQ(q.size(), 1);

    // stress vs std::queue
    for (int iter = 0; iter < 200; iter++) {
        LinkedQueue mine;
        queue<int> ref;
        for (int step = 0; step < 50; step++) {
            if (ref.empty() || t::rand_int(0, 2) > 0) {
                int x = (int)t::rand_int(0, 999);
                mine.push(x);
                ref.push(x);
            } else {
                CHECK_EQ(mine.front(), ref.front());
                mine.pop();
                ref.pop();
            }
            CHECK_EQ(mine.size(), ref.size());
            CHECK_EQ(mine.empty(), ref.empty());
        }
    }
    return t::summary("list_queue");
}
