// LeetCode-style practice file. Copy it (e.g. practice/06/0003-longest-substring.cpp), paste the
// problem's function signature into Solution, add the examples as CHECK_EQs, then:
//     make run F=practice/06/0003-longest-substring.cpp
// That builds with -Wall + AddressSanitizer + UBSan, so out-of-bounds reads, signed overflow and
// use-after-free crash loudly here instead of passing quietly and failing on LeetCode.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"   // ListNode / TreeNode + make_list, make_tree, to_vector, tree_to_string
using namespace std;

class Solution {
public:
    int solve(vector<int>& nums) {
        return (int)nums.size();
    }
};

int main() {
    Solution sol;
    vector<int> ex1{1, 2, 3};
    CHECK_EQ(sol.solve(ex1), 3);
    // Add the edge cases before you submit: empty input, a single element, all equal values,
    // negatives, and the largest values the constraints allow.
    return t::summary("practice");
}
