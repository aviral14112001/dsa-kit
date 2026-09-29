// 703. Kth Largest Element in a Stream: https://leetcode.com/problems/kth-largest-element-in-a-stream/
// Pattern: size-k min-heap: the top is the k-th largest. Module 13 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class KthLargest {
public:
    KthLargest(int k, vector<int>& nums) : k(k) {
        for (int x : nums) add(x);
    }

    int add(int val) {
        top_k.push(val);
        if ((int)top_k.size() > k) top_k.pop();   // evict the smallest: it can't be among the k largest
        return top_k.top();                        // the smallest of the k largest = the k-th largest
    }

private:
    int k;
    priority_queue<int, vector<int>, greater<int>> top_k;   // min-heap holding the k largest values so far
};
// [/snippet]

// Brute force: keep everything, sort descending on every query.
struct Brute {
    int k;
    vector<int> all;
    int add(int val) {
        all.push_back(val);
        vector<int> sorted = all;
        sort(sorted.rbegin(), sorted.rend());
        return sorted[k - 1];
    }
};

int main() {
    // official examples
    vector<int> nums1{4, 5, 8, 2};
    KthLargest a(3, nums1);
    CHECK_EQ(a.add(3), 4);
    CHECK_EQ(a.add(5), 5);
    CHECK_EQ(a.add(10), 5);
    CHECK_EQ(a.add(9), 8);
    CHECK_EQ(a.add(4), 8);
    vector<int> nums2{7, 7, 7, 7, 8, 3};
    KthLargest b(4, nums2);
    CHECK_EQ(b.add(2), 7);
    CHECK_EQ(b.add(10), 7);
    CHECK_EQ(b.add(9), 7);
    CHECK_EQ(b.add(9), 8);
    // edge cases: k = 1 (a running max), and nums shorter than k (k = nums.size() + 1 is allowed)
    vector<int> none;
    KthLargest c(1, none);
    CHECK_EQ(c.add(-3), -3);
    CHECK_EQ(c.add(-5), -3);
    CHECK_EQ(c.add(10), 10);
    vector<int> one{5};
    KthLargest d(2, one);
    CHECK_EQ(d.add(1), 1);
    CHECK_EQ(d.add(7), 5);

    // stress against the brute force; every add happens with at least k values stored
    for (int iter = 0; iter < 300; iter++) {
        int k = (int)t::rand_int(1, 8);
        vector<int> start = t::rand_vec((int)t::rand_int(k - 1, 12), -20, 20);
        KthLargest mine(k, start);
        Brute brute{k, start};
        for (int op = 0; op < 20; op++) {
            int val = (int)t::rand_int(-20, 20);
            CHECK_EQ(mine.add(val), brute.add(val));
        }
    }
    return t::summary("0703-kth-largest-element-in-a-stream");
}
