// Module 09 section 3: a bounded blocking queue for producer-consumer, built on std::mutex and
// std::condition_variable. The test runs real threads, but only checks results that don't
// depend on scheduling (count, sum, FIFO order), so it's deterministic.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:blocking_queue]
// push waits while the queue is full; pop waits while it's empty.
// close() wakes every waiter: later pushes fail, and pop drains what's left, then returns nullopt.
template <class T>
class BlockingQueue {
public:
    explicit BlockingQueue(size_t capacity) : cap(capacity) {}

    bool push(T item) {
        unique_lock<mutex> lock(m);
        // wait() unlocks while asleep, relocks on wakeup, and re-checks the condition, so a
        // spurious wakeup or a stolen slot just sends it back to sleep
        not_full.wait(lock, [&] { return q.size() < cap || closed; });
        if (closed) return false;
        q.push(std::move(item));
        not_empty.notify_one();  // wake one consumer, if any is waiting
        return true;
    }

    optional<T> pop() {
        unique_lock<mutex> lock(m);
        not_empty.wait(lock, [&] { return !q.empty() || closed; });
        if (q.empty()) return nullopt;  // closed and fully drained
        T item = std::move(q.front());
        q.pop();
        not_full.notify_one();          // wake one producer, if any is waiting
        return item;
    }

    void close() {
        {
            lock_guard<mutex> lock(m);  // change shared state only under the lock
            closed = true;
        }
        not_full.notify_all();
        not_empty.notify_all();
    }

private:
    size_t cap;
    queue<T> q;
    bool closed = false;
    mutex m;                            // guards q and closed
    condition_variable not_full, not_empty;
};
// [/snippet]

int main() {
    {   // single thread: FIFO within capacity, then close() drains and refuses new items
        BlockingQueue<string> q(2);
        CHECK(q.push("a"));
        CHECK(q.push("b"));
        CHECK_EQ(q.pop(), optional<string>("a"));
        q.close();
        CHECK(!q.push("c"));                      // closed: rejected
        CHECK_EQ(q.pop(), optional<string>("b"));  // what was queued is still delivered
        CHECK_EQ(q.pop(), optional<string>());    // then nullopt, without blocking
    }
    {   // one producer, one consumer, 10000 items through a queue of capacity 16
        const int N = 10000;
        BlockingQueue<int> q(16);
        long long sum = 0;
        int count = 0;
        bool in_order = true;
        thread consumer([&] {
            int expected = 1;
            while (optional<int> x = q.pop()) {   // stops at nullopt: closed and drained
                sum += *x;
                count++;
                in_order &= *x == expected++;
            }
        });
        thread producer([&] {
            for (int i = 1; i <= N; i++) q.push(i);
            q.close();
        });
        producer.join();
        consumer.join();                           // join() makes the consumer's writes visible here
        CHECK_EQ(count, N);
        CHECK_EQ(sum, (long long)N * (N + 1) / 2);
        CHECK(in_order);                           // one producer + one consumer: FIFO survives
    }
    {   // capacity 1 (the most blocking), 2 producers and 2 consumers: nothing lost, nothing doubled
        const int per_producer = 5000;
        BlockingQueue<int> q(1);
        mutex totals;
        long long sum = 0;
        int count = 0;
        auto consume = [&] {
            long long my_sum = 0;
            int my_count = 0;
            while (optional<int> x = q.pop()) { my_sum += *x; my_count++; }
            lock_guard<mutex> lock(totals);
            sum += my_sum;
            count += my_count;
        };
        auto produce = [&](int first) {
            for (int i = 0; i < per_producer; i++) q.push(first + i);
        };
        thread c1(consume), c2(consume);
        thread p1(produce, 1), p2(produce, per_producer + 1);  // values 1..5000 and 5001..10000
        p1.join();
        p2.join();
        q.close();                                 // only after every producer has finished
        c1.join();
        c2.join();
        CHECK_EQ(count, 2 * per_producer);
        CHECK_EQ(sum, 10000LL * 10001 / 2);
    }
    return t::summary("blocking_queue");
}
