// Recursion -> iteration with an explicit stack. Module 10 section 1.
//   Tower of Hanoi: the call does work BETWEEN its two recursive calls, so each explicit frame
//   needs a `stage` field that says where to resume after a child call returns.
//   Subsets: each state carries everything it needs, so pushing states is enough (no stage).
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

using Move = pair<char, char>;   // (from peg, to peg)

// [snippet:hanoi_recursive]
// Move a tower of n disks from peg `from` to peg `to`, using `via` as the spare peg.
// Leap of faith: trust hanoi(n - 1, ...) to move a smaller tower correctly; then n works too.
void hanoi(int n, char from, char to, char via, vector<Move>& moves) {
    if (n == 0) return;                      // base case: no disks, nothing to do
    hanoi(n - 1, from, via, to, moves);      // 1. park the n-1 smaller disks on the spare peg
    moves.push_back({from, to});             // 2. move the biggest disk to its target
    hanoi(n - 1, via, to, from, moves);      // 3. move the n-1 disks back on top of it
}
// [/snippet]

// [snippet:hanoi_iterative]
// The same algorithm with the call stack made explicit. A frame holds a call's arguments plus
// `stage`: how far that call has got, i.e. where to resume when its child call returns.
vector<Move> hanoiIterative(int n) {
    struct Frame { int n; char from, to, via; int stage; };
    vector<Move> moves;
    vector<Frame> stack = {{n, 'A', 'C', 'B', 0}};
    while (!stack.empty()) {
        Frame f = stack.back();              // a copy: push_back below may reallocate the vector
        if (f.n == 0) {                      // base case: "return" to the caller
            stack.pop_back();
        } else if (f.stage == 0) {           // about to make the first call
            stack.back().stage = 1;
            stack.push_back({f.n - 1, f.from, f.via, f.to, 0});
        } else if (f.stage == 1) {           // first call returned: move the disk, make the second
            stack.back().stage = 2;
            moves.push_back({f.from, f.to});
            stack.push_back({f.n - 1, f.via, f.to, f.from, 0});
        } else {                             // second call returned: this call is finished
            stack.pop_back();
        }
    }
    return moves;
}
// [/snippet]

// [snippet:subsets_stack]
// All subsets without recursion. A state is (next index to decide, subset built so far);
// popping a state and pushing its two children is exactly what the choose/skip call does.
vector<vector<int>> subsetsIterative(const vector<int>& nums) {
    vector<vector<int>> result;
    vector<pair<int, vector<int>>> stack = {{0, {}}};
    while (!stack.empty()) {
        auto [i, cur] = std::move(stack.back());
        stack.pop_back();
        if (i == (int)nums.size()) {         // leaf: every element decided
            result.push_back(std::move(cur));
            continue;
        }
        stack.push_back({i + 1, cur});       // child 1: skip nums[i]
        cur.push_back(nums[i]);
        stack.push_back({i + 1, std::move(cur)});  // child 2: take nums[i]
    }
    return result;
}
// [/snippet]

// Replays moves on three pegs: every move takes a top disk onto an empty peg or a bigger disk,
// and at the end the whole tower sits on peg C.
bool validHanoi(int n, const vector<Move>& moves) {
    map<char, vector<int>> peg;
    for (int d = n; d >= 1; d--) peg['A'].push_back(d);
    for (auto [from, to] : moves) {
        if (peg[from].empty()) return false;
        int disk = peg[from].back();
        if (!peg[to].empty() && peg[to].back() < disk) return false;
        peg[from].pop_back();
        peg[to].push_back(disk);
    }
    return (int)peg['C'].size() == n;
}

// Subsets by bitmask, sorted, as the reference answer.
vector<vector<int>> canonicalSubsets(const vector<int>& nums) {
    int n = nums.size();
    vector<vector<int>> out;
    for (int mask = 0; mask < (1 << n); mask++) {
        vector<int> s;
        for (int j = 0; j < n; j++)
            if ((mask >> j) & 1) s.push_back(nums[j]);
        sort(s.begin(), s.end());
        out.push_back(s);
    }
    sort(out.begin(), out.end());
    return out;
}

vector<vector<int>> canonical(vector<vector<int>> sets) {
    for (auto& s : sets) sort(s.begin(), s.end());
    sort(sets.begin(), sets.end());
    return sets;
}

int main() {
    for (int n = 0; n <= 12; n++) {
        vector<Move> rec;
        hanoi(n, 'A', 'C', 'B', rec);
        CHECK_EQ(rec.size(), (1u << n) - 1);          // 2^n - 1 moves
        CHECK(validHanoi(n, rec));
        CHECK_EQ(hanoiIterative(n), rec);             // identical move sequence, no recursion
    }
    CHECK_EQ(hanoiIterative(2), vector<Move>{{'A', 'B'}, {'A', 'C'}, {'B', 'C'}});

    CHECK_EQ(subsetsIterative({}), vector<vector<int>>{{}});
    CHECK_EQ(canonical(subsetsIterative({1, 2, 3})),
             vector<vector<int>>{{}, {1}, {1, 2}, {1, 2, 3}, {1, 3}, {2}, {2, 3}, {3}});
    for (int iter = 0; iter < 200; iter++) {           // stress test vs bitmask enumeration
        auto nums = t::rand_vec((int)t::rand_int(0, 10), -10, 10);
        CHECK_EQ(canonical(subsetsIterative(nums)), canonicalSubsets(nums));
    }
    return t::summary("recursion_to_iteration");
}
