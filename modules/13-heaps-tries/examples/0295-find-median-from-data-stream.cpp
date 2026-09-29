// 295. Find Median from Data Stream: https://leetcode.com/problems/find-median-from-data-stream/
// Pattern: two heaps: max-heap low half, min-heap high half, sizes kept balanced. Module 13 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class MedianFinder {
public:
    MedianFinder() {}

    void addNum(int num) {
        low.push(num);                      // 1. put it in the low half...
        high.push(low.top());               // 2. ...then hand the low half's max to the high half:
        low.pop();                          //    now every low value <= every high value again
        if (high.size() > low.size()) {     // 3. restore the size rule
            low.push(high.top());
            high.pop();
        }
    }

    double findMedian() {
        if (low.size() > high.size()) return low.top();    // odd count: the middle is low's max
        return (low.top() + (double)high.top()) / 2;       // even: convert BEFORE adding (no int overflow)
    }

private:
    // Invariants: every value in low <= every value in high, and
    //             low.size() == high.size() or low.size() == high.size() + 1.
    priority_queue<int> low;                                // max-heap: the smaller half
    priority_queue<int, vector<int>, greater<int>> high;    // min-heap: the larger half
};
// [/snippet]

// Brute force: keep a sorted vector (insert at lower_bound), read the middle directly.
struct Brute {
    vector<int> sorted;
    void add(int x) { sorted.insert(lower_bound(sorted.begin(), sorted.end(), x), x); }
    double median() const {
        size_t n = sorted.size();
        return n % 2 ? sorted[n / 2] : ((double)sorted[n / 2 - 1] + sorted[n / 2]) / 2;
    }
};

int main() {
    // official example
    MedianFinder mf;
    mf.addNum(1);
    mf.addNum(2);
    CHECK_NEAR(mf.findMedian(), 1.5, 1e-9);
    mf.addNum(3);
    CHECK_NEAR(mf.findMedian(), 2.0, 1e-9);
    // edge cases: one value, descending input, duplicates, values near the int limits
    MedianFinder one;
    one.addNum(-7);
    CHECK_NEAR(one.findMedian(), -7.0, 1e-9);
    MedianFinder desc;
    for (int x : {5, 4, 3, 2}) desc.addNum(x);
    CHECK_NEAR(desc.findMedian(), 3.5, 1e-9);
    MedianFinder big;
    big.addNum(INT_MAX);
    big.addNum(INT_MAX - 2);                // (a + b) in int would overflow here
    CHECK_NEAR(big.findMedian(), INT_MAX - 1.0, 1e-6);

    // stress against the brute force: small ranges (many duplicates) and wide ranges
    for (int iter = 0; iter < 200; iter++) {
        int range = iter % 2 ? 5 : 100000;
        MedianFinder mine;
        Brute brute;
        for (int op = 0; op < 40; op++) {
            int x = (int)t::rand_int(-range, range);
            mine.addNum(x);
            brute.add(x);
            CHECK_NEAR(mine.findMedian(), brute.median(), 1e-9);
        }
    }
    return t::summary("0295-find-median-from-data-stream");
}
