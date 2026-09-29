// 739. Daily Temperatures: https://leetcode.com/problems/daily-temperatures/
// Pattern: monotonic stack of indices (next strictly greater element). Module 09 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
#include "monotonic.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    vector<int> dailyTemperatures(vector<int>& temperatures) {
        int n = (int)temperatures.size();
        vector<int> answer(n, 0);  // 0 = no warmer day later
        vector<int> waiting;       // days still waiting for a warmer day; their temperatures
                                   // never increase from bottom to top
        for (int day = 0; day < n; day++) {
            // Today answers every waiting day that is strictly colder. They're all on top.
            while (!waiting.empty() && temperatures[day] > temperatures[waiting.back()]) {
                answer[waiting.back()] = day - waiting.back();
                waiting.pop_back();
            }
            waiting.push_back(day);
        }
        return answer;             // days still waiting keep 0
    }
};
// [/snippet]

// Brute force: scan forward from every day. O(n^2).
vector<int> brute(const vector<int>& temps) {
    int n = (int)temps.size();
    vector<int> ans(n, 0);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (temps[j] > temps[i]) { ans[i] = j - i; break; }
    return ans;
}

int main() {
    Solution sol;
    // official examples
    vector<int> ex1{73, 74, 75, 71, 69, 72, 76, 73};
    CHECK_EQ(sol.dailyTemperatures(ex1), vector<int>{1, 1, 4, 2, 1, 1, 0, 0});
    vector<int> ex2{30, 40, 50, 60};
    CHECK_EQ(sol.dailyTemperatures(ex2), vector<int>{1, 1, 1, 0});
    vector<int> ex3{30, 60, 90};
    CHECK_EQ(sol.dailyTemperatures(ex3), vector<int>{1, 1, 0});
    // edge cases
    vector<int> one{50};
    CHECK_EQ(sol.dailyTemperatures(one), vector<int>{0});
    vector<int> flat{50, 50, 50};                   // "warmer" is strict: equal days don't count
    CHECK_EQ(sol.dailyTemperatures(flat), vector<int>{0, 0, 0});
    vector<int> falling{90, 80, 70, 100};           // one day answers three waiting days at once
    CHECK_EQ(sol.dailyTemperatures(falling), vector<int>{3, 2, 1, 0});

    // stress vs brute force, and vs the template's next_greater (strict)
    for (int iter = 0; iter < 400; iter++) {
        vector<int> temps = t::rand_vec((int)t::rand_int(0, 30), 30, 40);  // narrow range: many ties
        vector<int> expected = brute(temps);
        CHECK_EQ(sol.dailyTemperatures(temps), expected);
        int n = (int)temps.size();
        vector<int> ng = next_greater(temps), from_template(n);
        for (int i = 0; i < n; i++) from_template[i] = ng[i] == n ? 0 : ng[i] - i;
        CHECK_EQ(from_template, expected);
    }
    // largest input size: 10^5 days that never get warmer (100 down to 31, so the stack grows
    // to almost n), then a final 100 that answers every day colder than 100 in one sweep
    vector<int> big(100000);
    for (int i = 0; i < 99999; i++) big[i] = 100 - (int)((long long)i * 70 / 100000);
    big[99999] = 100;
    vector<int> big_answer = sol.dailyTemperatures(big);
    CHECK_EQ(big_answer[1428], 0);          // days 0..1428 are 100: nothing is strictly warmer
    CHECK_EQ(big_answer[1429], 99999 - 1429);
    CHECK_EQ(big_answer[99998], 1);
    return t::summary("0739-daily-temperatures");
}
