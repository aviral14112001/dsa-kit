// 307. Range Sum Query - Mutable: https://leetcode.com/problems/range-sum-query-mutable/
// Pattern: Fenwick tree (point update, prefix sums). Module 17 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class NumArray {
    int n;
    vector<int> value;                   // current a[i]: turns "set a[i] = val" into "add val - a[i]"
    vector<int> tree;                    // 1-based Fenwick: tree[j] = sum of a over (j - lowbit(j), j]

    void add(int i, int delta) {
        for (int j = i + 1; j <= n; j += j & -j) tree[j] += delta;
    }
    int prefix(int k) const {            // a[0] + ... + a[k-1]
        int sum = 0;
        for (int j = k; j > 0; j -= j & -j) sum += tree[j];
        return sum;
    }

public:
    NumArray(vector<int>& nums) : n((int)nums.size()), value(nums), tree(n + 1, 0) {
        for (int i = 0; i < n; i++) add(i, nums[i]);
    }

    void update(int index, int val) {
        add(index, val - value[index]);
        value[index] = val;
    }

    int sumRange(int left, int right) {
        return prefix(right + 1) - prefix(left);
    }
};
// [/snippet]

int main() {
    // The official example.
    vector<int> nums{1, 3, 5};
    NumArray arr(nums);
    CHECK_EQ(arr.sumRange(0, 2), 9);
    arr.update(1, 2);
    CHECK_EQ(arr.sumRange(0, 2), 8);

    // Edge cases: one element; updating to the same value; negative values.
    vector<int> single{-7};
    NumArray one(single);
    CHECK_EQ(one.sumRange(0, 0), -7);
    one.update(0, -7);
    CHECK_EQ(one.sumRange(0, 0), -7);
    one.update(0, 100);
    CHECK_EQ(one.sumRange(0, 0), 100);

    // Stress: random updates and queries against a plain array.
    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(1, 30);
        vector<int> a = t::rand_vec(n, -100, 100);
        vector<int> copy_for_ctor = a;
        NumArray na(copy_for_ctor);
        for (int op = 0; op < 50; op++) {
            int l = (int)t::rand_int(0, n - 1), r = (int)t::rand_int(l, n - 1);
            if (t::rand_int(0, 1)) {
                a[l] = (int)t::rand_int(-100, 100);
                na.update(l, a[l]);
            } else {
                CHECK_EQ(na.sumRange(l, r), accumulate(a.begin() + l, a.begin() + r + 1, 0));
            }
        }
    }
    return t::summary("0307-range-sum-query-mutable");
}
