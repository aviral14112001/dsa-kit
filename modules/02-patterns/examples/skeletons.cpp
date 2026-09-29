// Module 02 skeletons: the shape of each core pattern, on toy problems that are NOT in the plan,
// each tested against an obviously-correct brute force. NOTES.md section 2–3 pull in the [snippet] regions.
#include <bits/stdc++.h>
#include "test.hpp"
#include "binary_search.hpp"
using namespace std;
using ll = long long;

// ======================= section 2 two pointers: converging =======================
// [snippet:converging]
// How many pairs i < j have a[i] + a[j] < target? `a` is sorted ascending.
// Invariant: every pair that uses an index outside [l, r] is already counted or ruled out.
long long count_pairs_below(const vector<int>& a, long long target) {
    long long count = 0;
    int l = 0, r = (int)a.size() - 1;
    while (l < r) {
        if ((long long)a[l] + a[r] < target) {
            count += r - l;   // a[l] + a[j] <= a[l] + a[r] for every j in (l, r]: all r - l pairs count
            l++;              // a[l] is finished
        } else {
            r--;              // a[r] + a[i] >= a[l] + a[r] >= target for every i in [l, r): a[r] is finished
        }
    }
    return count;
}
// [/snippet]

// ======================= section 2 two pointers: same direction =======================
// [snippet:same_direction]
// Smallest |a[i] - b[j]| over two sorted, non-empty arrays. Both pointers only ever move right.
// If a[i] < b[j], every later b is even farther from a[i], so a[i] can never do better: drop it.
long long min_gap(const vector<int>& a, const vector<int>& b) {
    long long best = LLONG_MAX;
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        best = min(best, llabs((long long)a[i] - b[j]));
        if (a[i] < b[j]) i++;   // advance whichever side is smaller
        else j++;
    }
    return best;
}
// [/snippet]

// ======================= section 2 sliding window: fixed size =======================
// [snippet:fixed_window]
// Number of distinct values in every window of length k (1 <= k <= n), left to right.
// Each step one value enters on the right and one leaves on the left: update, never rebuild.
vector<int> distinct_in_windows(const vector<int>& a, int k) {
    unordered_map<int, int> count;                   // value -> occurrences inside the window
    vector<int> result;
    for (int r = 0; r < (int)a.size(); r++) {
        count[a[r]]++;                               // a[r] enters
        if (r >= k) {                                // a[r - k] leaves
            if (--count[a[r - k]] == 0) count.erase(a[r - k]);
        }
        if (r >= k - 1) result.push_back((int)count.size());   // window a[r-k+1..r] is complete
    }
    return result;
}
// [/snippet]

// ======================= section 2 sliding window: variable size (longest) =======================
// [snippet:variable_window]
// Length of the longest subarray with sum <= budget. Needs every a[i] >= 0 and budget >= 0.
// Removing elements never raises the sum, so once [l, r] is over budget the only fix is to move
// l right, and l never has to move back: each index enters once and leaves once. O(n).
int longest_within_budget(const vector<int>& a, long long budget) {
    long long sum = 0;
    int best = 0;
    for (int l = 0, r = 0; r < (int)a.size(); r++) {
        sum += a[r];                           // grow: take a[r]
        while (sum > budget) sum -= a[l++];    // shrink until the window is valid again
        best = max(best, r - l + 1);           // a[l..r] is the longest valid window ending at r
    }
    return best;
}
// [/snippet]

// ======================= section 2 fast & slow pointers =======================
// [snippet:fast_slow]
// Floyd's tortoise and hare on the sequence x0, f(x0), f(f(x0)), ...  If f maps a finite set to
// itself, the sequence must eventually repeat, so it ends in a cycle. Once both pointers are in
// the cycle, fast gains one position per step on slow, so they meet within one lap.
// Returns the cycle's length, using O(1) memory (no visited set).
template <class F>
int cycle_length(int x0, F f) {
    int slow = f(x0), fast = f(f(x0));
    while (slow != fast) {        // phase 1: meet somewhere inside the cycle
        slow = f(slow);
        fast = f(f(fast));
    }
    int length = 1;               // phase 2: walk once around the cycle from the meeting point
    for (int x = f(slow); x != slow; x = f(x)) length++;
    return length;
}
// [/snippet]

// ======================= section 2 binary search on the answer =======================
// [snippet:bs_answer]
// A saw blade at height H cuts every tree taller than H down to H and keeps the tops.
// What is the highest H that still collects at least `need` wood?
// Raising the blade never collects MORE wood, so enough(H) runs true...true false...false as H
// grows: last_true (templates/binary_search.hpp) finds the last true. O(n log(max height)).
long long highest_blade(const vector<int>& trees, long long need) {
    auto enough = [&](long long H) {                 // the feasibility test: O(n)
        long long wood = 0;
        for (int h : trees) wood += max(0LL, h - H);
        return wood >= need;
    };
    long long tallest = *max_element(trees.begin(), trees.end());
    return last_true(0LL, tallest, enough);          // -1 when even H = 0 is not enough
}
// [/snippet]

// ======================= section 3 BFS with a level-size snapshot =======================
// [snippet:bfs_grid]
// Fewest moves from (sr, sc) to (tr, tc) on a grid of '.' (open) and '#' (wall), moving up, down,
// left or right. The start must be open. Returns -1 if the target can't be reached.
int min_steps(const vector<string>& grid, int sr, int sc, int tr, int tc) {
    int rows = (int)grid.size(), cols = (int)grid[0].size();
    const int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
    vector<vector<bool>> seen(rows, vector<bool>(cols, false));
    queue<pair<int, int>> q;
    q.push({sr, sc});
    seen[sr][sc] = true;                           // mark when you PUSH, not when you pop
    for (int steps = 0; !q.empty(); steps++) {
        int levelSize = (int)q.size();             // snapshot: exactly the cells `steps` moves away
        for (int k = 0; k < levelSize; k++) {
            auto [r, c] = q.front();
            q.pop();
            if (r == tr && c == tc) return steps;  // first time we pop it = fewest moves
            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d], nc = c + dc[d];
                if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;   // off the grid
                if (grid[nr][nc] == '#' || seen[nr][nc]) continue;          // wall, or queued already
                seen[nr][nc] = true;
                q.push({nr, nc});
            }
        }
    }
    return -1;
}
// [/snippet]

// ======================= section 3 backtracking: choose -> explore -> unchoose =======================
// [snippet:backtracking]
// Every string of length n over `alphabet` in which no two neighbours are equal, in alphabet order.
// One shared `path` is extended and shrunk in place: no copy per call.
void extend(int n, const string& alphabet, string& path, vector<string>& out) {
    if ((int)path.size() == n) {                        // a complete candidate: record it
        out.push_back(path);
        return;
    }
    for (char c : alphabet) {
        if (!path.empty() && path.back() == c) continue;   // prune: this choice breaks the rule
        path.push_back(c);                                 // choose
        extend(n, alphabet, path, out);                    // explore
        path.pop_back();                                   // unchoose: restore the state exactly
    }
}

vector<string> no_equal_neighbours(int n, const string& alphabet) {
    vector<string> out;
    string path;
    extend(n, alphabet, path, out);
    return out;
}
// [/snippet]

// ======================= section 3 memoise -> tabulate =======================
// Toy: ways to tile a 1 x n strip with red squares, green squares (1 x 1) and blue dominoes (1 x 2).

// [snippet:dp_recursive]
// Look at the LAST tile: a square (2 colours) leaves a 1 x (n-1) strip; a domino leaves 1 x (n-2).
//   ways(n) = 2 * ways(n - 1) + ways(n - 2),   ways(0) = 1 (the empty tiling),   ways(1) = 2
long long ways_recursive(int n) {
    if (n == 0) return 1;
    if (n == 1) return 2;
    return 2 * ways_recursive(n - 1) + ways_recursive(n - 2);   // re-solves the same n again and again
}
// [/snippet]

// [snippet:dp_memo]
// Top-down: the same recursion, but each n is solved once and cached. memo[n] == -1: not solved yet.
long long ways_memo(int n, vector<long long>& memo) {
    if (n == 0) return 1;
    if (n == 1) return 2;
    if (memo[n] != -1) return memo[n];
    return memo[n] = 2 * ways_memo(n - 1, memo) + ways_memo(n - 2, memo);
}
// Call it as: vector<long long> memo(n + 1, -1); ways_memo(n, memo);
// [/snippet]

// [snippet:dp_table]
// Bottom-up: fill dp[0..n] in an order where everything dp[i] reads is already filled.
long long ways_table(int n) {
    vector<long long> dp(max(n + 1, 2));
    dp[0] = 1;
    dp[1] = 2;
    for (int i = 2; i <= n; i++) dp[i] = 2 * dp[i - 1] + dp[i - 2];
    return dp[n];
}
// [/snippet]

// [snippet:dp_rolling]
// dp[i] reads only dp[i-1] and dp[i-2], so keep two variables instead of the table: O(1) space.
long long ways_rolling(int n) {
    if (n == 0) return 1;
    long long prev2 = 1, prev1 = 2;    // ways(i-2) and ways(i-1), starting at i = 2
    for (int i = 2; i <= n; i++) {
        long long cur = 2 * prev1 + prev2;
        prev2 = prev1;
        prev1 = cur;
    }
    return prev1;
}
// [/snippet]

// ======================= brute forces for the stress tests =======================
long long count_pairs_brute(const vector<int>& a, long long target) {
    long long c = 0;
    for (size_t i = 0; i < a.size(); i++)
        for (size_t j = i + 1; j < a.size(); j++) c += (long long)a[i] + a[j] < target;
    return c;
}

long long min_gap_brute(const vector<int>& a, const vector<int>& b) {
    long long best = LLONG_MAX;
    for (int x : a)
        for (int y : b) best = min(best, llabs((long long)x - y));
    return best;
}

vector<int> distinct_brute(const vector<int>& a, int k) {
    vector<int> out;
    for (int s = 0; s + k <= (int)a.size(); s++) out.push_back((int)set<int>(a.begin() + s, a.begin() + s + k).size());
    return out;
}

int longest_brute(const vector<int>& a, long long budget) {
    int best = 0;
    for (int l = 0; l < (int)a.size(); l++) {
        long long sum = 0;
        for (int r = l; r < (int)a.size(); r++) {
            sum += a[r];
            if (sum <= budget) best = max(best, r - l + 1);
        }
    }
    return best;
}

template <class F>
int cycle_length_brute(int x0, F f) {        // remember the step at which each value first appeared
    map<int, int> firstSeen;
    int x = x0;
    for (int step = 0;; step++, x = f(x)) {
        if (firstSeen.count(x)) return step - firstSeen[x];
        firstSeen[x] = step;
    }
}

long long highest_blade_brute(const vector<int>& trees, long long need) {
    for (long long H = *max_element(trees.begin(), trees.end()); H >= 0; H--) {
        long long wood = 0;
        for (int h : trees) wood += max(0LL, h - H);
        if (wood >= need) return H;
    }
    return -1;
}

int min_steps_brute(const vector<string>& grid, int sr, int sc, int tr, int tc) {
    // Relax every edge rows*cols times (Bellman-Ford with unit weights): slow but obviously right.
    int rows = (int)grid.size(), cols = (int)grid[0].size();
    const int INF = INT_MAX / 2, dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
    vector<vector<int>> dist(rows, vector<int>(cols, INF));
    dist[sr][sc] = 0;
    for (int round = 0; round < rows * cols; round++)
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                if (dist[r][c] == INF) continue;
                for (int d = 0; d < 4; d++) {
                    int nr = r + dr[d], nc = c + dc[d];
                    if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && grid[nr][nc] != '#')
                        dist[nr][nc] = min(dist[nr][nc], dist[r][c] + 1);
                }
            }
    return dist[tr][tc] == INF ? -1 : dist[tr][tc];
}

vector<string> no_equal_brute(int n, const string& alphabet) {   // all |alphabet|^n strings, filtered
    int k = (int)alphabet.size();
    long long total = 1;
    for (int i = 0; i < n; i++) total *= k;
    vector<string> out;
    for (long long code = 0; code < total; code++) {
        string s(n, ' ');
        long long x = code;
        for (int i = n - 1; i >= 0; i--, x /= k) s[i] = alphabet[x % k];
        bool ok = true;
        for (int i = 1; i < n; i++) ok = ok && s[i] != s[i - 1];
        if (ok) out.push_back(s);
    }
    return out;
}

long long tilings_brute(int n) {
    // A tiling spelled cell by cell is a string over {R, G, B} whose runs of B all have even length
    // (each BB is one domino). Count those strings directly: no recurrence involved.
    long long total = 1, count = 0;
    for (int i = 0; i < n; i++) total *= 3;
    for (long long code = 0; code < total; code++) {
        long long x = code;
        int run = 0;
        bool ok = true;
        for (int i = 0; i < n; i++, x /= 3) {
            if (x % 3 == 2) run++;
            else { ok = ok && run % 2 == 0; run = 0; }
        }
        if (ok && run % 2 == 0) count++;
    }
    return count;
}

int main() {
    // ---- converging: the traced example from the notes ----
    CHECK_EQ(count_pairs_below({1, 2, 4, 6, 9}, 8), 4);
    CHECK_EQ(count_pairs_below({}, 5), 0);
    CHECK_EQ(count_pairs_below({7}, 100), 0);
    CHECK_EQ(count_pairs_below({-3, -3, -3}, -5), 3);
    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(0, 15), -20, 20);
        sort(a.begin(), a.end());
        ll target = t::rand_int(-45, 45);
        CHECK_EQ(count_pairs_below(a, target), count_pairs_brute(a, target));
    }

    // ---- same direction ----
    CHECK_EQ(min_gap({1, 4, 10}, {6, 13, 20}), 2);
    CHECK_EQ(min_gap({5}, {5}), 0);
    CHECK_EQ(min_gap({INT_MIN}, {INT_MAX}), 4294967295LL);     // why the difference is computed in 64 bits
    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(1, 10), -50, 50), b = t::rand_vec((int)t::rand_int(1, 10), -50, 50);
        sort(a.begin(), a.end());
        sort(b.begin(), b.end());
        CHECK_EQ(min_gap(a, b), min_gap_brute(a, b));
    }

    // ---- fixed window ----
    CHECK_EQ(distinct_in_windows({1, 2, 1, 3, 3}, 3), vector<int>{2, 3, 2});
    CHECK_EQ(distinct_in_windows({4, 4, 4}, 1), vector<int>{1, 1, 1});
    CHECK_EQ(distinct_in_windows({4, 5, 6}, 3), vector<int>{3});
    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(1, 15), 0, 5);
        int k = (int)t::rand_int(1, (int)a.size());
        CHECK_EQ(distinct_in_windows(a, k), distinct_brute(a, k));
    }

    // ---- variable window ----
    CHECK_EQ(longest_within_budget({3, 1, 2, 1, 4}, 5), 3);
    CHECK_EQ(longest_within_budget({6, 7}, 5), 0);                // nothing fits
    CHECK_EQ(longest_within_budget({}, 5), 0);
    CHECK_EQ(longest_within_budget({0, 0, 0}, 0), 3);
    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(0, 15), 0, 9);
        ll budget = t::rand_int(0, 30);
        CHECK_EQ(longest_within_budget(a, budget), longest_brute(a, budget));
    }

    // ---- fast & slow ----
    vector<int> next{1, 2, 3, 4, 2};                               // 0 -> 1 -> 2 -> 3 -> 4 -> 2 -> ...
    auto step = [&](int x) { return next[x]; };
    CHECK_EQ(cycle_length(0, step), 3);
    auto digitSquares = [](int x) {                                // pointers don't have to be pointers
        int s = 0;
        for (; x > 0; x /= 10) s += (x % 10) * (x % 10);
        return s;
    };
    CHECK_EQ(cycle_length(7, digitSquares), 1);                    // 7 -> 49 -> 97 -> 130 -> 10 -> 1 -> 1
    CHECK_EQ(cycle_length(2, digitSquares), 8);                    // 2 -> 4 -> 16 -> ... -> 20 -> 4
    for (int iter = 0; iter < 300; iter++) {                       // random functions on {0..n-1}
        int n = (int)t::rand_int(1, 20);
        auto f = t::rand_vec(n, 0, n - 1);
        auto g = [&](int x) { return f[x]; };
        int x0 = (int)t::rand_int(0, n - 1);
        CHECK_EQ(cycle_length(x0, g), cycle_length_brute(x0, g));
    }

    // ---- binary search on the answer ----
    CHECK_EQ(highest_blade({4, 12, 9, 7}, 6), 7);                  // H = 7: 5 + 2 + 0 = 7 >= 6; H = 8: 4 + 1 = 5
    CHECK_EQ(highest_blade({5, 5}, 10), 0);                        // must cut to the ground
    CHECK_EQ(highest_blade({5, 5}, 11), -1);                       // impossible
    CHECK_EQ(highest_blade({1'000'000'000, 1'000'000'000}, 2), 999'999'999);
    for (int iter = 0; iter < 300; iter++) {
        auto trees = t::rand_vec((int)t::rand_int(1, 8), 0, 30);
        ll need = t::rand_int(0, 120);
        CHECK_EQ(highest_blade(trees, need), highest_blade_brute(trees, need));
    }

    // ---- BFS ----
    vector<string> maze{"..#.",
                        ".##.",
                        "...."};
    CHECK_EQ(min_steps(maze, 0, 0, 0, 3), 7);                      // around the walls
    CHECK_EQ(min_steps(maze, 0, 0, 0, 0), 0);
    CHECK_EQ(min_steps({".#.", "##.", "..."}, 0, 0, 2, 2), -1);    // walled in
    for (int iter = 0; iter < 300; iter++) {
        int rows = (int)t::rand_int(1, 6), cols = (int)t::rand_int(1, 6);
        vector<string> g(rows, string(cols, '.'));
        for (auto& row : g)
            for (auto& cell : row) cell = t::rand_int(0, 3) == 0 ? '#' : '.';
        int sr = (int)t::rand_int(0, rows - 1), sc = (int)t::rand_int(0, cols - 1);
        int tr = (int)t::rand_int(0, rows - 1), tc = (int)t::rand_int(0, cols - 1);
        g[sr][sc] = '.';
        CHECK_EQ(min_steps(g, sr, sc, tr, tc), min_steps_brute(g, sr, sc, tr, tc));
    }

    // ---- backtracking ----
    CHECK_EQ(no_equal_neighbours(2, "ab"), vector<string>{"ab", "ba"});
    CHECK_EQ(no_equal_neighbours(3, "abc").size(), 12);              // 3 * 2 * 2
    CHECK_EQ(no_equal_neighbours(0, "abc"), vector<string>{""});     // one empty string
    for (int n = 0; n <= 6; n++)
        for (string alphabet : {"a", "ab", "abc", "abcd"}) CHECK_EQ(no_equal_neighbours(n, alphabet), no_equal_brute(n, alphabet));

    // ---- memoise -> tabulate: all four stages agree, and agree with direct counting ----
    CHECK_EQ(ways_table(4), 29);
    for (int n = 0; n <= 10; n++) CHECK_EQ(ways_recursive(n), tilings_brute(n));
    for (int n = 0; n <= 25; n++) {
        vector<ll> memo(n + 1, -1);
        ll expected = ways_recursive(n);
        CHECK_EQ(ways_memo(n, memo), expected);
        CHECK_EQ(ways_table(n), expected);
        CHECK_EQ(ways_rolling(n), expected);
    }
    for (int n = 26; n <= 49; n++) {                                // ways(49) still fits in long long
        vector<ll> memo(n + 1, -1);
        CHECK_EQ(ways_memo(n, memo), ways_table(n));
        CHECK_EQ(ways_rolling(n), ways_table(n));
    }
    CHECK(ways_table(49) > 0);
    return t::summary("02-skeletons");
}
