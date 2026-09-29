// 875. Koko Eating Bananas: https://leetcode.com/problems/koko-eating-bananas/
// Pattern: binary search on the answer (the minimum feasible speed). Module 11 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
#include "binary_search.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int minEatingSpeed(vector<int>& piles, int h) {
        // canFinish(speed) is false...false true...true over [1, max pile]: find the first true.
        // hi = max pile always works (one hour per pile, and h >= n), so no "not found" case.
        int lo = 1, hi = *max_element(piles.begin(), piles.end());
        while (lo < hi) {                            // the answer is in [lo, hi]
            int mid = lo + (hi - lo) / 2;
            if (canFinish(piles, h, mid)) hi = mid;  // mid works: the answer is mid or smaller
            else lo = mid + 1;                       // mid is too slow: the answer is bigger
        }
        return lo;
    }

    // The feasibility test: at this speed, is the total time within h hours?
    static bool canFinish(const vector<int>& piles, int h, int speed) {
        long long hours = 0;                         // 10^4 piles * 10^9 hours each: needs 64 bits
        for (int p : piles) {
            hours += (p + speed - 1LL) / speed;      // ceil(p / speed) in integers, computed in 64 bits
            if (hours > h) return false;             // already too slow: stop early
        }
        return true;
    }
};
// [/snippet]

// [snippet:with_template]
// The same search via templates/binary_search.hpp: only the predicate and the range are new.
int minEatingSpeedTemplate(vector<int>& piles, int h) {
    int maxPile = *max_element(piles.begin(), piles.end());
    return first_true(1, maxPile, [&](int speed) { return Solution::canFinish(piles, h, speed); });
}
// [/snippet]

// Brute force: try every speed from 1 up. O(max pile * n).
int brute(const vector<int>& piles, int h) {
    for (int speed = 1;; speed++)
        if (Solution::canFinish(piles, h, speed)) return speed;
}

int main() {
    Solution sol;
    vector<int> a{3, 6, 7, 11};
    CHECK_EQ(sol.minEatingSpeed(a, 8), 4);                        // the official examples
    vector<int> b{30, 11, 23, 4, 20};
    CHECK_EQ(sol.minEatingSpeed(b, 5), 30);
    CHECK_EQ(sol.minEatingSpeed(b, 6), 23);

    vector<int> one{1000000000};
    CHECK_EQ(sol.minEatingSpeed(one, 1), 1000000000);             // h == n: the biggest pile per hour
    CHECK_EQ(sol.minEatingSpeed(one, 1000000000), 1);             // lots of time: speed 1
    CHECK_EQ(sol.minEatingSpeed(one, 999999999), 2);              // ceil(10^9 / 2) = 5 * 10^8 <= h
    vector<int> huge(10000, 1000000000);                          // hours(1) = 10^13: overflows int
    CHECK_EQ(sol.minEatingSpeed(huge, 1000000000), 10000);        // 10^4 piles * 10^5 hours each = h
    CHECK_EQ(minEatingSpeedTemplate(huge, 1000000000), 10000);

    for (int iter = 0; iter < 300; iter++) {                      // stress test vs brute force
        auto piles = t::rand_vec((int)t::rand_int(1, 8), 1, 30);
        int h = (int)t::rand_int((int)piles.size(), 60);
        int expected = brute(piles, h);
        CHECK_EQ(sol.minEatingSpeed(piles, h), expected);
        CHECK_EQ(minEatingSpeedTemplate(piles, h), expected);
    }
    return t::summary("0875-koko-eating-bananas");
}
