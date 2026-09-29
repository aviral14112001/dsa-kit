// 238. Product of Array Except Self: https://leetcode.com/problems/product-of-array-except-self/
// Pattern: prefix products x suffix products, no division. Module 06 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> productExceptSelf(vector<int>& nums) {
        int n = (int)nums.size();
        vector<int> answer(n, 1);
        for (int i = 1; i < n; i++)                 // pass 1: answer[i] = nums[0] * ... * nums[i-1]
            answer[i] = answer[i - 1] * nums[i - 1];
        int suffix = 1;                             // nums[i+1] * ... * nums[n-1], built right to left
        for (int i = n - 1; i >= 0; i--) {          // pass 2: multiply in everything to the right
            answer[i] *= suffix;
            suffix *= nums[i];
        }
        return answer;
    }
};
// [/snippet]

// Brute force: multiply everything except position i, for each i. O(n^2).
vector<int> brute(const vector<int>& nums) {
    vector<int> out;
    for (size_t i = 0; i < nums.size(); i++) {
        long long p = 1;
        for (size_t j = 0; j < nums.size(); j++)
            if (j != i) p *= nums[j];
        out.push_back((int)p);
    }
    return out;
}

int main() {
    Solution sol;
    vector<int> ex1{1, 2, 3, 4}, ex2{-1, 1, 0, -3, 3};
    CHECK_EQ(sol.productExceptSelf(ex1), vector<int>{24, 12, 8, 6});
    CHECK_EQ(sol.productExceptSelf(ex2), vector<int>{0, 0, 9, 0, 0});

    vector<int> twoZeros{0, 4, 0}, oneZero{2, 0, 5}, two{-3, 7};
    CHECK_EQ(sol.productExceptSelf(twoZeros), vector<int>{0, 0, 0});   // division by the total would break here
    CHECK_EQ(sol.productExceptSelf(oneZero), vector<int>{0, 10, 0});
    CHECK_EQ(sol.productExceptSelf(two), vector<int>{7, -3});

    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(2, 16), -2, 2);            // |product| <= 2^16: no overflow
        CHECK_EQ(sol.productExceptSelf(a), brute(a));
    }
    return t::summary("0238-product-except-self");
}
