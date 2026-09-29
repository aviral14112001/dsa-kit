// 46. Permutations: https://leetcode.com/problems/permutations/
// Pattern: backtracking, pick any unused element for the next position (n! leaves). Module 10 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

namespace used_array {
// [snippet:solution]
class Solution {
public:
    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> cur;
        vector<bool> used(nums.size(), false);           // used[i]: nums[i] is already in cur
        build(nums, used, cur, result);
        return result;
    }

private:
    void build(const vector<int>& nums, vector<bool>& used, vector<int>& cur, vector<vector<int>>& result) {
        if (cur.size() == nums.size()) {                 // every position filled
            result.push_back(cur);
            return;
        }
        for (int i = 0; i < (int)nums.size(); i++) {     // any unused element may go next
            if (used[i]) continue;
            used[i] = true;                              // choose
            cur.push_back(nums[i]);
            build(nums, used, cur, result);              // explore
            cur.pop_back();                              // un-choose: undo both changes,
            used[i] = false;                             // in reverse order
        }
    }
};
// [/snippet]
}  // namespace used_array

namespace swapping {
// [snippet:swap]
class Solution {
public:
    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> result;
        build(nums, 0, result);
        return result;                                   // nums is back in its original order here
    }

private:
    // nums[0..pos) is fixed; nums[pos..] are the elements still available. Try each at position pos.
    void build(vector<int>& nums, int pos, vector<vector<int>>& result) {
        if (pos == (int)nums.size()) {
            result.push_back(nums);
            return;
        }
        for (int i = pos; i < (int)nums.size(); i++) {
            swap(nums[pos], nums[i]);                    // choose nums[i] for position pos
            build(nums, pos + 1, result);
            swap(nums[pos], nums[i]);                    // undo: restore the order for the next i
        }
    }
};
// [/snippet]
}  // namespace swapping

vector<vector<int>> canonical(vector<vector<int>> perms) {
    sort(perms.begin(), perms.end());
    return perms;
}

// Brute force: std::next_permutation walks every permutation of a sorted copy in order.
vector<vector<int>> brute(vector<int> nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> out;
    do out.push_back(nums);
    while (next_permutation(nums.begin(), nums.end()));
    return out;
}

vector<int> distinctValues(int n) {
    vector<int> pool;
    for (int v = -10; v <= 10; v++) pool.push_back(v);
    shuffle(pool.begin(), pool.end(), t::rng());
    return vector<int>(pool.begin(), pool.begin() + n);
}

template <class S>
void run(S& sol) {
    vector<int> a{1, 2, 3};
    CHECK_EQ(canonical(sol.permute(a)),
             vector<vector<int>>{{1, 2, 3}, {1, 3, 2}, {2, 1, 3}, {2, 3, 1}, {3, 1, 2}, {3, 2, 1}});
    vector<int> b{0, 1};
    CHECK_EQ(canonical(sol.permute(b)), vector<vector<int>>{{0, 1}, {1, 0}});
    vector<int> c{1};
    CHECK_EQ(sol.permute(c), vector<vector<int>>{{1}});

    for (int iter = 0; iter < 200; iter++) {                 // stress test vs next_permutation
        auto nums = distinctValues((int)t::rand_int(1, 6));
        auto before = nums;
        auto got = sol.permute(nums);
        CHECK_EQ(nums, before);                              // every change was undone
        CHECK_EQ(canonical(got), brute(nums));
    }
}

int main() {
    used_array::Solution s1;
    swapping::Solution s2;
    run(s1);
    run(s2);
    vector<int> a{1, 2, 3};                                  // the used[] version emits them in
    CHECK_EQ(s1.permute(a), brute(a));                       // lexicographic order for sorted input
    return t::summary("0046-permutations");
}
