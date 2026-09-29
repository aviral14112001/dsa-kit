// 912. Sort an Array: https://leetcode.com/problems/sort-an-array/
// Pattern: write an O(n log n) sort yourself; heap sort is the one with O(1) extra memory. Module 05 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> sortArray(vector<int>& nums) {
        int n = nums.size();
        // Phase 1, heapify: sift down every internal node, the last one first. Leaves
        // (i >= n/2) are one-node heaps already, so when we reach i both subtrees are heaps.
        for (int i = n / 2 - 1; i >= 0; i--) siftDown(nums, i, n);
        // Phase 2. Invariant: nums[0, end] is a max-heap, and nums(end, n) holds the largest
        // values in sorted order. Swapping the root (the heap's max) into slot `end` grows
        // the sorted suffix by one; one sift-down repairs the smaller heap.
        for (int end = n - 1; end > 0; end--) {
            swap(nums[0], nums[end]);
            siftDown(nums, 0, end);
        }
        return nums;
    }

private:
    // Max-heap on a[0, n): the children of i are 2i+1 and 2i+2. Moves a[i] down until it is
    // not smaller than either child: one level per iteration, so O(log n).
    void siftDown(vector<int>& a, int i, int n) {
        while (true) {
            int largest = i, left = 2 * i + 1, right = 2 * i + 2;
            if (left < n && a[left] > a[largest]) largest = left;
            if (right < n && a[right] > a[largest]) largest = right;
            if (largest == i) return;
            swap(a[i], a[largest]);
            i = largest;
        }
    }
};
// [/snippet]

// Oracle for the stress test: the library sort.
vector<int> reference(vector<int> v) {
    sort(v.begin(), v.end());
    return v;
}

int main() {
    Solution sol;
    vector<int> ex1{5, 2, 3, 1}, ex2{5, 1, 1, 2, 0, 0};
    CHECK_EQ(sol.sortArray(ex1), vector<int>{1, 2, 3, 5});
    CHECK_EQ(sol.sortArray(ex2), vector<int>{0, 0, 1, 1, 2, 5});

    vector<int> one{7}, two{2, 1}, negatives{-3, 50000, -50000, 0, -3};
    CHECK_EQ(sol.sortArray(one), vector<int>{7});
    CHECK_EQ(sol.sortArray(two), vector<int>{1, 2});
    CHECK_EQ(sol.sortArray(negatives), vector<int>{-50000, -3, -3, 0, 50000});

    // The inputs that break careless quicksorts, at the maximum size (n = 5 * 10^4).
    const int n = 50000;
    vector<int> equal(n, -7), ascending(n), descending(n), zigzag(n);
    iota(ascending.begin(), ascending.end(), -25000);
    for (int i = 0; i < n; i++) {
        descending[i] = 25000 - i;
        zigzag[i] = (i % 2 == 0) ? i : -i;
    }
    for (auto* input : {&equal, &ascending, &descending, &zigzag}) {
        vector<int> copy = *input;
        CHECK_EQ(sol.sortArray(copy), reference(*input));
    }

    for (int iter = 0; iter < 300; iter++) {        // stress test against std::sort
        int len = (int)t::rand_int(1, 40);
        vector<int> v = (iter % 2) ? t::rand_vec(len, -3, 3) : t::rand_vec(len, -50000, 50000);
        vector<int> copy = v;
        CHECK_EQ(sol.sortArray(copy), reference(v));
    }
    return t::summary("0912-sort-an-array");
}
