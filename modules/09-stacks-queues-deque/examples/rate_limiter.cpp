// Module 09 section 3: rate limiting. An exact sliding-log limiter (a queue of timestamps) next to a
// fixed-window counter, with a test that shows the fixed window's burst at a boundary.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:sliding_log]
// Allow at most `limit` requests in any `window` time units. A request at time t is allowed iff
// fewer than `limit` ALLOWED requests happened in (t - window, t]. Times must not decrease.
class SlidingLogLimiter {
public:
    SlidingLogLimiter(int max_requests, long long window_len) : limit(max_requests), window(window_len) {}

    bool allow(long long t) {
        while (!log.empty() && log.front() <= t - window) log.pop_front();  // expired: out of the window
        if ((int)log.size() >= limit) return false;  // denied requests are not logged
        log.push_back(t);
        return true;
    }

private:
    int limit;
    long long window;
    deque<long long> log;  // times of allowed requests still inside the window, oldest first
};
// [/snippet]

// [snippet:fixed_window]
// Count requests per fixed window [w * window, (w + 1) * window). O(1) memory, but a burst that
// straddles a boundary gets up to 2 * limit requests through in a short span.
class FixedWindowLimiter {
public:
    FixedWindowLimiter(int max_requests, long long window_len) : limit(max_requests), window(window_len) {}

    bool allow(long long t) {  // t >= 0
        long long id = t / window;
        if (id != current) {   // a new window starts: forget the old count
            current = id;
            used = 0;
        }
        if (used >= limit) return false;
        used++;
        return true;
    }

private:
    int limit;
    long long window;
    long long current = -1;  // id of the window being counted
    int used = 0;
};
// [/snippet]

int main() {
    {   // 3 requests per 10 time units
        SlidingLogLimiter lim(3, 10);
        CHECK(lim.allow(1));
        CHECK(lim.allow(2));
        CHECK(lim.allow(3));
        CHECK(!lim.allow(4));    // 1, 2, 3 are all within (-6, 4]
        CHECK(!lim.allow(10));   // (0, 10] still holds 1, 2, 3
        CHECK(lim.allow(11));    // (1, 11]: the request at 1 expired
        CHECK(!lim.allow(11));   // 2, 3, 11
        CHECK(lim.allow(13));    // (3, 13]: only 11 is left inside
        CHECK(lim.allow(20));    // (10, 20]: 11, 13
        CHECK(!lim.allow(20));   // 11, 13, 20: full
    }
    {   // The boundary burst: 3 per 10, requests at 7, 8, 9 and then 10, 11, 12.
        SlidingLogLimiter sliding(3, 10);
        FixedWindowLimiter fixed(3, 10);
        int sliding_ok = 0, fixed_ok = 0;
        for (long long at : {7, 8, 9, 10, 11, 12}) {
            sliding_ok += sliding.allow(at);
            fixed_ok += fixed.allow(at);
        }
        CHECK_EQ(sliding_ok, 3);  // exact: never more than 3 in any window of length 10
        CHECK_EQ(fixed_ok, 6);    // 3 in window [0, 10) plus 3 in [10, 20), all within 6 time units
    }

    // stress: the sliding log vs a brute force that recounts all allowed requests every time,
    // plus the guarantee itself: no window of length `window` ever holds more than `limit`.
    for (int iter = 0; iter < 300; iter++) {
        int limit = (int)t::rand_int(1, 4);
        long long window = t::rand_int(1, 12);
        SlidingLogLimiter lim(limit, window);
        vector<long long> allowed;
        long long now = 0;
        for (int step = 0; step < 40; step++) {
            now += t::rand_int(0, 4);
            int in_window = 0;
            for (long long a : allowed) in_window += a > now - window;
            bool expected = in_window < limit;
            CHECK_EQ(lim.allow(now), expected);
            if (expected) allowed.push_back(now);
        }
        bool never_over = true;
        for (size_t i = 0; i < allowed.size(); i++) {
            int inside = 0;
            for (size_t j = i; j < allowed.size() && allowed[j] < allowed[i] + window; j++) inside++;
            never_over &= inside <= limit;
        }
        CHECK(never_over);
    }
    return t::summary("rate_limiter");
}
