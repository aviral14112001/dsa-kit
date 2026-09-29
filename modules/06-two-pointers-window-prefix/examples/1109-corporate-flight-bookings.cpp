// 1109. Corporate Flight Bookings: https://leetcode.com/problems/corporate-flight-bookings/
// Pattern: difference array (range add in O(1), one prefix sum at the end). Module 06 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> corpFlightBookings(vector<vector<int>>& bookings, int n) {
        vector<int> diff(n + 1, 0);           // 0-indexed flights; slot n absorbs the "-seats" after flight n
        for (auto& b : bookings) {
            int first = b[0] - 1, last = b[1] - 1, seats = b[2];
            diff[first] += seats;             // seats switch on at `first`...
            diff[last + 1] -= seats;          // ...and off right after `last`
        }
        vector<int> total(n);
        int running = 0;                      // at most 2*10^4 bookings * 10^4 seats = 2*10^8: fits in int
        for (int i = 0; i < n; i++) {
            running += diff[i];
            total[i] = running;
        }
        return total;
    }
};
// [/snippet]

// Brute force: add every booking to every flight it covers. O(bookings * n) = 4 * 10^8 at the limits.
vector<int> brute(const vector<vector<int>>& bookings, int n) {
    vector<int> total(n, 0);
    for (auto& b : bookings)
        for (int f = b[0]; f <= b[1]; f++) total[f - 1] += b[2];
    return total;
}

int main() {
    Solution sol;
    vector<vector<int>> ex1{{1, 2, 10}, {2, 3, 20}, {2, 5, 25}}, ex2{{1, 2, 10}, {2, 2, 15}};
    CHECK_EQ(sol.corpFlightBookings(ex1, 5), vector<int>{10, 55, 45, 25, 25});
    CHECK_EQ(sol.corpFlightBookings(ex2, 2), vector<int>{10, 25});

    vector<vector<int>> whole{{1, 3, 7}}, lastOnly{{4, 4, 1}};
    CHECK_EQ(sol.corpFlightBookings(whole, 3), vector<int>{7, 7, 7});       // last == n: uses the spare slot
    CHECK_EQ(sol.corpFlightBookings(lastOnly, 4), vector<int>{0, 0, 0, 1});

    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(1, 12);
        vector<vector<int>> bookings;
        for (int k = (int)t::rand_int(1, 8); k > 0; k--) {
            int first = (int)t::rand_int(1, n), last = (int)t::rand_int(first, n);
            bookings.push_back({first, last, (int)t::rand_int(1, 10000)});
        }
        CHECK_EQ(sol.corpFlightBookings(bookings, n), brute(bookings, n));
    }
    return t::summary("1109-corporate-flight-bookings");
}
