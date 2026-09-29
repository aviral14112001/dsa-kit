// 622. Design Circular Queue: https://leetcode.com/problems/design-circular-queue/
// Pattern: ring buffer (fixed array, head index + count, indices modulo capacity). Module 09 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class MyCircularQueue {
public:
    MyCircularQueue(int k) : buf(k), cap(k) {}

    bool enQueue(int value) {
        if (isFull()) return false;
        buf[(head + count) % cap] = value;  // the slot just past the rear, wrapping around
        count++;
        return true;
    }
    bool deQueue() {
        if (isEmpty()) return false;
        head = (head + 1) % cap;            // abandon the front slot; enQueue will reuse it
        count--;
        return true;
    }
    int Front() { return isEmpty() ? -1 : buf[head]; }
    int Rear() { return isEmpty() ? -1 : buf[(head + count - 1) % cap]; }
    bool isEmpty() { return count == 0; }
    bool isFull() { return count == cap; }

private:
    vector<int> buf;
    int cap;
    int head = 0;   // index of the front element
    int count = 0;  // number of elements: tells "empty" from "full", which look alike by indices
};
// [/snippet]

int main() {
    {   // the official example
        MyCircularQueue q(3);
        CHECK(q.enQueue(1));
        CHECK(q.enQueue(2));
        CHECK(q.enQueue(3));
        CHECK(!q.enQueue(4));   // full
        CHECK_EQ(q.Rear(), 3);
        CHECK(q.isFull());
        CHECK(q.deQueue());
        CHECK(q.enQueue(4));    // wraps into the slot the 1 left behind
        CHECK_EQ(q.Rear(), 4);
    }
    {   // empty queue and capacity 1
        MyCircularQueue q(1);
        CHECK(q.isEmpty());
        CHECK_EQ(q.Front(), -1);
        CHECK_EQ(q.Rear(), -1);
        CHECK(!q.deQueue());
        CHECK(q.enQueue(7));
        CHECK(q.isFull());
        CHECK_EQ(q.Front(), 7);
        CHECK_EQ(q.Rear(), 7);
        CHECK(q.deQueue());
        CHECK(q.isEmpty());
    }
    // stress vs std::deque with an explicit capacity check; many wrap-arounds per run
    for (int iter = 0; iter < 300; iter++) {
        int k = (int)t::rand_int(1, 5);
        MyCircularQueue q(k);
        deque<int> ref;
        for (int step = 0; step < 60; step++) {
            int op = (int)t::rand_int(0, 2);
            if (op == 0) {
                int v = (int)t::rand_int(0, 1000);
                bool fits = (int)ref.size() < k;
                if (fits) ref.push_back(v);
                CHECK_EQ(q.enQueue(v), fits);
            } else if (op == 1) {
                bool had = !ref.empty();
                if (had) ref.pop_front();
                CHECK_EQ(q.deQueue(), had);
            }
            CHECK_EQ(q.Front(), ref.empty() ? -1 : ref.front());
            CHECK_EQ(q.Rear(), ref.empty() ? -1 : ref.back());
            CHECK_EQ(q.isEmpty(), ref.empty());
            CHECK_EQ(q.isFull(), (int)ref.size() == k);
        }
    }
    return t::summary("0622-design-circular-queue");
}
