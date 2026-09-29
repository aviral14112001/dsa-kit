// 698. Partition to K Equal Sum Subsets: https://leetcode.com/problems/partition-to-k-equal-sum-subsets/
// Pattern: bucket backtracking + pruning (feasibility, largest-first ordering, symmetry breaking);
// then memoizing on the used-mask, which turns the search into bitmask DP. Module 10 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace buckets {
// [snippet:solution]
class Solution {
public:
    bool canPartitionKSubsets(vector<int>& nums, int k) {
        int total = accumulate(nums.begin(), nums.end(), 0);
        if (total % k != 0) return false;                     // feasibility, before any search
        target = total / k;
        sort(nums.begin(), nums.end(), greater<int>());       // ordering: big items fail fast
        if (nums[0] > target) return false;
        bucket.assign(k, 0);
        return place(nums, 0);
    }

private:
    int target = 0;
    vector<int> bucket;                                       // current sum of each bucket

    // Place nums[i..]. Invariant: every bucket sum is <= target.
    bool place(const vector<int>& nums, int i) {
        if (i == (int)nums.size()) return true;               // sums <= target and total = k * target,
                                                              // so every bucket is exactly target
        for (int b = 0; b < (int)bucket.size(); b++) {
            if (bucket[b] + nums[i] > target) continue;       // feasibility: this bucket would overflow
            bucket[b] += nums[i];
            if (place(nums, i + 1)) return true;
            bucket[b] -= nums[i];
            if (bucket[b] == 0) break;                        // symmetry: nums[i] failed in an empty
        }                                                     // bucket, so it fails in every empty one
        return false;
    }
};
// [/snippet]
}  // namespace buckets

namespace memo {
// [snippet:memo]
class Solution {
public:
    bool canPartitionKSubsets(vector<int>& nums, int k) {
        int total = accumulate(nums.begin(), nums.end(), 0);
        if (total % k != 0) return false;
        target = total / k;
        memo.assign(1 << nums.size(), UNKNOWN);
        return canFinish(nums, 0, 0);
    }

private:
    enum Result : char { UNKNOWN, YES, NO };
    int target = 0;
    vector<Result> memo;                                      // memo[used] for each subset of items

    // Fill buckets one at a time. The items in `used` make up some full buckets plus the open one,
    // so the open bucket holds sum(used) % target: `fill` is determined by `used`, and so is the
    // answer for the remaining items. Many choice orders reach the same `used`: memoize on it.
    bool canFinish(const vector<int>& nums, int used, int fill) {
        if (used == (1 << nums.size()) - 1) return true;
        if (memo[used] != UNKNOWN) return memo[used] == YES;
        for (int i = 0; i < (int)nums.size(); i++) {
            if (((used >> i) & 1) || fill + nums[i] > target) continue;
            if (canFinish(nums, used | (1 << i), (fill + nums[i]) % target)) {
                memo[used] = YES;
                return true;
            }
        }
        memo[used] = NO;
        return false;
    }
};
// [/snippet]
}  // namespace memo

// Instrumented copy of the bucket search: counts calls with each pruning switched on or off.
struct Counter {
    int target = 0;
    vector<int> bucket;
    bool symmetry = true;
    long long calls = 0;

    bool place(const vector<int>& nums, int i) {
        calls++;
        if (i == (int)nums.size()) return true;
        for (int b = 0; b < (int)bucket.size(); b++) {
            if (bucket[b] + nums[i] > target) continue;
            bucket[b] += nums[i];
            if (place(nums, i + 1)) return true;
            bucket[b] -= nums[i];
            if (symmetry && bucket[b] == 0) break;
        }
        return false;
    }
};

long long countCalls(vector<int> nums, int k, bool sortDescending, bool symmetry) {
    Counter c;
    c.target = accumulate(nums.begin(), nums.end(), 0) / k;
    c.bucket.assign(k, 0);
    c.symmetry = symmetry;
    if (sortDescending) sort(nums.begin(), nums.end(), greater<int>());
    c.place(nums, 0);
    return c.calls;
}

// Brute force: try every assignment of items to buckets (k^n of them), no pruning at all.
bool bruteAssign(const vector<int>& nums, int i, vector<int>& sums, int target) {
    if (i == (int)nums.size())
        return all_of(sums.begin(), sums.end(), [&](int s) { return s == target; });
    for (int& s : sums) {
        s += nums[i];
        bool ok = bruteAssign(nums, i + 1, sums, target);
        s -= nums[i];
        if (ok) return true;
    }
    return false;
}

bool brute(const vector<int>& nums, int k) {
    int total = accumulate(nums.begin(), nums.end(), 0);
    if (total % k != 0) return false;
    vector<int> sums(k, 0);
    return bruteAssign(nums, 0, sums, total / k);
}

// Half the random cases are built to be feasible: k buckets, each split into 1-2 random parts.
vector<int> randomCase(int& k) {
    k = (int)t::rand_int(1, 4);
    vector<int> nums;
    if (t::rand_int(0, 1) == 0) {
        int target = (int)t::rand_int(2, 12);
        for (int b = 0; b < k; b++) {
            int part = (int)t::rand_int(1, target);
            if (part == target || t::rand_int(0, 1) == 0) nums.push_back(target);
            else { nums.push_back(part); nums.push_back(target - part); }
        }
        shuffle(nums.begin(), nums.end(), t::rng());
    } else {
        nums = t::rand_vec((int)t::rand_int(k, 8), 1, 10);
    }
    return nums;
}

template <class S>
void run(S& sol) {
    vector<int> a{4, 3, 2, 3, 5, 2, 1};
    CHECK_EQ(sol.canPartitionKSubsets(a, 4), true);           // the official examples
    vector<int> b{1, 2, 3, 4};
    CHECK_EQ(sol.canPartitionKSubsets(b, 3), false);
    vector<int> c{2, 2, 2, 2, 3, 4, 5};                       // sum divisible by k, still impossible
    CHECK_EQ(sol.canPartitionKSubsets(c, 4), false);
    vector<int> d{7};
    CHECK_EQ(sol.canPartitionKSubsets(d, 1), true);           // k = 1: the whole array
    vector<int> e{5, 5, 5, 5};
    CHECK_EQ(sol.canPartitionKSubsets(e, 4), true);           // k = n
    vector<int> f{1, 1, 1, 9};
    CHECK_EQ(sol.canPartitionKSubsets(f, 2), false);          // one item bigger than the target
    vector<int> g{1, 5, 8, 6, 5, 5, 8, 6, 7, 8, 9, 8};
    CHECK_EQ(sol.canPartitionKSubsets(g, 4), false);          // the pruning demo input below
    vector<int> big{10000, 10000, 10000, 10000, 9999, 9999, 9999, 9999,
                    1, 1, 1, 1, 2, 2, 2, 2};                  // n = 16, the maximum: {10000, 9999, 2, 1} x 4
    CHECK_EQ(sol.canPartitionKSubsets(big, 4), true);
    vector<int> evens{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 2, 2};
    CHECK_EQ(sol.canPartitionKSubsets(evens, 4), false);      // target 21 is odd, every value is even
    vector<int> big3{9998, 9998, 9998, 9998, 1, 1, 1, 1, 1, 1, 1, 1};
    CHECK_EQ(sol.canPartitionKSubsets(big3, 4), true);

    for (int iter = 0; iter < 300; iter++) {                  // stress test vs exhaustive assignment
        int k;
        auto nums = randomCase(k);
        auto copy = nums;
        CHECK_EQ(sol.canPartitionKSubsets(copy, k), brute(nums, k));
    }
}

int main() {
    buckets::Solution s1;
    memo::Solution s2;
    run(s1);
    run(s2);

    // n = 16 is too big for the brute force, so the two independent solutions check each other.
    for (int iter = 0; iter < 30; iter++) {
        int k = (int)t::rand_int(2, 6);
        auto nums = t::rand_vec(16, 1, 20);
        int total = accumulate(nums.begin(), nums.end(), 0);
        nums[0] += (k - total % k) % k;                       // make the sum divisible by k
        auto a = nums, b = nums;
        CHECK_EQ(s1.canPartitionKSubsets(a, k), s2.canPartitionKSubsets(b, k));
    }

    // What each pruning buys on an input with no answer (so every variant must exhaust its tree).
    vector<int> demo{1, 5, 8, 6, 5, 5, 8, 6, 7, 8, 9, 8};
    CHECK_EQ(countCalls(demo, 4, false, false), 275149LL);    // feasibility check only
    CHECK_EQ(countCalls(demo, 4, true, false), 15285LL);      // + largest first
    CHECK_EQ(countCalls(demo, 4, false, true), 11491LL);      // + empty-bucket symmetry break
    CHECK_EQ(countCalls(demo, 4, true, true), 643LL);         // both: the solution above
    return t::summary("0698-partition-to-k-equal-sum-subsets");
}
