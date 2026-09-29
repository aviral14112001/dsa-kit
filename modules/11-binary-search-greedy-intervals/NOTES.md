# 11 · Binary Search + Greedy + Intervals
> Three techniques that turn “too slow” into “fast enough”.

**Time:** ~12 h of Core work ([problems](problems.md)) · **Prereqs:** modules 05 section 4 (`first_true`, lower / upper bound), 03 section 4 (reading constraints), 06 section 4 (difference arrays), 01 section 3 (comparators, `priority_queue`) · **You're done when:** you can turn "the minimum X such that …" into a predicate plus bounds for `first_true` / `last_true` and defend monotonicity in one sentence; you can prove a greedy with an exchange argument out loud, or break it with a stress test against brute force; you can solve merge intervals and minimum rooms in under 15 minutes each, saying whether the intervals are closed or half-open.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Binary search on the answer | a monotone yes/no question over the answer range; search for where it flips | `first_true` / `last_true` / `first_true_real` + a feasibility check | 875, 1552, 410, 2387, 378 |
| 2 | Greedy proofs | commit to the locally best move, then prove nothing beats it (exchange, stays ahead) or break it with a counterexample | sort + scan; stress test vs brute force | 55, 678, 135 |
| 3 | Interval problems | sort by start to merge, by end to select; max overlap = min rooms | merge; min-heap of end times; sweep line | 56, 57, 435, 2406 |
| 4 | Scheduling & allocation | earliest end / deadline first; keep the committed set in a heap; binary search on capacity | deadline heap; frame counting | 1011, 1353 |

## 1. Binary search on the answer — minimum feasible value, predicate monotonicity

### Concept

Many optimisation problems ask for the smallest (or largest) value that satisfies a condition: minimum speed, minimum capacity, maximum gap. Suppose you can answer ok(x) = "does x work?" quickly, and ok is **monotone**: once true, it stays true as x grows. Then the answers look like this:

```text
x:      1   2   3   4   5   6   7   8  ...
ok(x):  F   F   F   T   T   T   T   T       the answer is the first T
```

Binary search finds the flip with O(log range) calls to ok, for O(cost of ok · log range) in total. You never build the optimum directly; you only *check* guesses. The monotonicity proof is one sentence, and you should say it out loud: "if Koko finishes at speed s, she finishes at any faster speed"; "if capacity C ships everything in D days, any bigger capacity does too".

### Template

From `templates/binary_search.hpp` (module 05 section 4):

<!-- snippet: templates/binary_search.hpp#first_true -->
```cpp
// Smallest x in [lo, hi] with ok(x) true, for ok shaped false...false true...true.
// Returns hi + 1 when ok is false everywhere. Keep hi + 1 representable (use long long if unsure).
template <class T, class Pred>
T first_true(T lo, T hi, Pred ok) {
    hi++;                               // search the half-open range [lo, hi)
    while (lo < hi) {
        T mid = lo + (hi - lo) / 2;     // not (lo + hi) / 2, which can overflow
        if (ok(mid)) hi = mid;          // mid works: the answer is mid or left of it
        else lo = mid + 1;              // mid fails: the answer is right of it
    }
    return lo;
}
```
<!-- /snippet -->

<!-- snippet: templates/binary_search.hpp#last_true -->
```cpp
// Largest x in [lo, hi] with ok(x) true, for ok shaped true...true false...false.
// Returns lo - 1 when ok is false everywhere.
template <class T, class Pred>
T last_true(T lo, T hi, Pred ok) {
    return first_true(lo, hi, [&](T x) { return !ok(x); }) - 1;
}
```
<!-- /snippet -->

**Choosing lo and hi.**
- `lo` is the smallest value that could be the answer: 1 for a speed; the largest item for a capacity, since every item must fit on its own.
- `hi` is a value that certainly works: the largest pile for Koko, the total for a capacity, or 10⁹ / 10¹⁸ when unsure.
- If nothing in the range works, `first_true` returns hi + 1. Check for that whenever feasibility isn't guaranteed.
- Loose bounds are cheap: a range of 10⁹ costs ~30 iterations and 10¹⁸ costs ~60.

**Overflow, in two places.**
1. **In `mid`:** `lo + (hi - lo) / 2`, as the template does. `(lo + hi) / 2` overflows near `INT_MAX`, and so does the template's internal `hi + 1` if hi is `INT_MAX`: use `long long` bounds then.
2. **In the check:** Koko's hours reach 10⁴ piles × 10⁹ hours = 10¹³, far past `INT_MAX` (≈ 2.1·10⁹). Sums of counts, days or trips, and products like mid · mid or rate · time, belong in `long long`, preferably with an early exit once over budget.

**Minimise the maximum vs maximise the minimum.**

| Goal | ok(x) asks | Shape | Call |
|---|---|---|---|
| smallest x that works: "minimum speed", "minimise the largest part" | can we do it with limit x? | F…F T…T | `first_true(lo, hi, ok)` |
| largest x that works: "maximise the smallest gap", "the biggest piece" | can we guarantee at least x? | T…T F…F | `last_true(lo, hi, ok)` |

On your list, 410 minimises the largest subarray sum and 1552 maximises the smallest gap. Each checks feasibility with a greedy pass (its list line says which), and the search around it is exactly Koko's. A maximise example (not a list problem) that also shows the overflow trap:

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/answer_search_demos.cpp#cube_root -->
```cpp
// Largest x >= 0 with x^3 <= n, for 0 <= n <= 10^18. ok(x) = "x^3 <= n" is true...true false...false.
long long cubeRoot(long long n) {
    // hi = n is a lazy but safe bound (only ~60 halvings). The price: mid can be ~5 * 10^17, where
    // mid * mid * mid overflows long long (UB). Divide instead: for x >= 1, x^3 <= n  <=>  x <= n / x / x.
    auto ok = [&](long long x) { return x == 0 || x <= n / x / x; };
    return last_true(0LL, n, ok);            // first_true inside computes n + 1: fine for n <= 10^18
}
```
<!-- /snippet -->

**Searching the value instead of an index.** To find the k-th smallest element, or a median, of something you can't cheaply sort, binary-search the *value* v. count(≤ v), the number of elements ≤ v, grows with v, so the answer is the smallest v with count(≤ v) ≥ k. 2387 and 378 on your list have this shape.

**Real-valued answers** (a rate, a length, a time): halve a fixed number of times instead of looping until `hi - lo < eps`.

<!-- snippet: templates/binary_search.hpp#first_true_real -->
```cpp
// Real-valued answers: a fixed number of halvings instead of an epsilon loop.
// 100 halvings shrink the range by 2^100 ≈ 1e30, far beyond the ~16 digits a double resolves.
template <class Pred>
double first_true_real(double lo, double hi, Pred ok, int iters = 100) {
    for (int i = 0; i < iters; i++) {
        double mid = (lo + hi) / 2;
        if (ok(mid)) hi = mid;
        else lo = mid;
    }
    return hi;
}
```
<!-- /snippet -->

Why a fixed count? 100 halvings divide the range by 2¹⁰⁰ ≈ 10³⁰, far more than a double's ~16 significant digits can resolve, and the loop always ends. An eps loop can spin forever: near 10⁹, consecutive doubles are about 1.2·10⁻⁷ apart, so `hi - lo < 1e-9` never becomes true. A real-valued example from your day job: the interest rate behind an EMI has no closed form, but the EMI grows with the rate.

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/answer_search_demos.cpp#emi_rate -->
```cpp
// EMI (equated monthly instalment) for principal P at monthly rate r over m months:
//     emi(r) = P * r * (1 + r)^m / ((1 + r)^m - 1)
// emi grows with r, but there is no formula for r given the EMI: binary-search it.
double emi(double principal, double rate, int months) {
    if (rate == 0) return principal / months;
    double growthMinus1 = expm1(months * log1p(rate));    // (1 + r)^m - 1, accurate even for tiny r
    return principal * rate * (growthMinus1 + 1) / growthMinus1;
}

// The monthly rate whose EMI equals `payment`: the first r in [0, 1] with emi(r) >= payment.
// Assumes principal / months <= payment <= emi(1.0), i.e. a rate between 0% and 100% a month.
double monthlyRate(double principal, double payment, int months) {
    return first_true_real(0.0, 1.0, [&](double r) { return emi(principal, r, months) >= payment; });
}
```
<!-- /snippet -->

### Pitfalls

- **A predicate that isn't monotone:** binary search silently returns garbage. Ask whether a bigger x could ever make ok false again.
- **Bounds that exclude the answer:** Koko's lo is 1, not the smallest pile, because with lots of hours the answer can be below every pile.
- **An `int` sum in the check that overflows** goes negative, and the predicate lies.
- **Floating-point ceilings:** `ceil((double)p / s)` can misround for big values. Use `(p + s - 1) / s`, computed in 64 bits if p + s can overflow.
- **Hand-rolled loops** that return lo when they mean hi, or loop forever with `lo = mid`: use the template, or copy its shape exactly.
- **Printing a real answer:** `cout << fixed << setprecision(9)`, or whatever precision the judge asks for.

### Recognize it when…

- "Minimum … such that …", "maximum … such that …", "at least", "within h hours / D days / k groups".
- Checking a candidate is easy (a greedy pass, a count) but constructing the optimum directly isn't.
- The answer range is huge (10⁹–10¹⁸) while n is ~10⁵: the target is O(n log range).
- "Minimise the largest …" or "maximise the smallest …".
- "k-th smallest" or "median" of an implicit sorted structure (a matrix, pair sums): search the value and count ≤ mid.

### Worked example: 875. Koko Eating Bananas
[LeetCode 875](https://leetcode.com/problems/koko-eating-bananas/) · Medium

**Problem (paraphrased):** there are n piles of bananas and h ≥ n hours. Each hour Koko eats up to s bananas from a single pile; if the pile has fewer, she finishes it and idles for the rest of the hour. Find the smallest integer s that finishes every pile within h hours.
**Signals:** "minimum speed", "within h hours", piles up to 10⁹, n up to 10⁴. You can't try every speed, but checking one speed is a single O(n) pass.
**Brute force, and why it fails:** try s = 1, 2, 3, … up to the largest pile: O(n · max pile) = 10⁴ · 10⁹, hopeless.
**Key insight:** hours(s) = Σ ⌈p / s⌉ never increases as s grows. So ok(s) = hours(s) ≤ h is F…F T…T over [1, max pile], and you want the first T. The largest pile always works (one hour per pile, and h ≥ n).
**Dry run:** piles = [3, 6, 7, 11], h = 8.

| lo | hi | mid | hours(mid) | ok? | next |
|---|---|---|---|---|---|
| 1 | 11 | 6 | 1+1+2+2 = 6 | yes | hi = 6 |
| 1 | 6 | 3 | 1+2+3+4 = 10 | no | lo = 4 |
| 4 | 6 | 5 | 1+2+2+3 = 8 | yes | hi = 5 |
| 4 | 5 | 4 | 1+2+2+3 = 8 | yes | hi = 4 |
| 4 | 4 | | | | return 4 |

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/0875-koko-eating-bananas.cpp#solution -->
```cpp
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
```
<!-- /snippet -->

The loop is `first_true` inlined, so the class can be pasted into LeetCode. Because hi is known to work, it skips the hi + 1 "not found" slot. With the template, only the predicate and the range are new:

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/0875-koko-eating-bananas.cpp#with_template -->
```cpp
// The same search via templates/binary_search.hpp: only the predicate and the range are new.
int minEatingSpeedTemplate(vector<int>& piles, int h) {
    int maxPile = *max_element(piles.begin(), piles.end());
    return first_true(1, maxPile, [&](int speed) { return Solution::canFinish(piles, h, speed); });
}
```
<!-- /snippet -->

**Complexity:** O(n log(max pile)): about 30 checks of 10⁴ piles each, ≈ 3·10⁵ steps; O(1) extra space.
**Edge cases:**
- h == n forces speed = the largest pile.
- A huge h gives speed 1.
- 10⁴ piles of 10⁹ make hours(1) = 10¹³: the sum must be 64-bit (tested).
- Use the integer ceiling, not floating point.

**Follow-ups:**
- *What if h < n were allowed?* Nothing works; `first_true` returns max + 1, so return −1.
- *Tighter bounds?* Every hour eats at most s bananas, so s ≥ ⌈Σp / h⌉. Starting lo there saves a few iterations and changes nothing else.
- *A real-valued speed?* `first_true_real` with the same predicate.

## 2. Greedy proofs — the exchange argument, and when local optimum is actually safe

### Concept

A greedy algorithm builds the answer from a sequence of locally best choices that it never takes back, usually after a sort. It's short and fast (often O(n log n)), and it is wrong surprisingly often. Treat every greedy idea as a claim that needs a proof or a stress test.

Two properties together make a greedy optimal:
- **Greedy-choice property:** some optimal solution starts with the greedy's first choice.
- **Optimal substructure:** after that choice, what's left is the same problem on a smaller input, and "greedy choice + an optimal solution of the rest" is optimal overall.

Induction on the input size then proves the whole greedy.

**The exchange argument, step by step.** Problem: n jobs, where job i takes tᵢ, run back to back on one machine; job i's completion time Cᵢ is when it finishes. Minimise ΣCᵢ. Claim: shortest job first.

1. Take any optimal order. If it isn't sorted by t, it has two **adjacent** jobs, a then b, with tₐ > t_b (an inversion).
2. Swap them. Jobs before the pair don't move, and the pair still fills the same span, so jobs after it don't move either. Only Cₐ and C_b change. Let T be the pair's start time. Before the swap, Cₐ = T + tₐ and C_b = T + tₐ + t_b. After it, C_b = T + t_b and Cₐ = T + t_b + tₐ. The total changes by t_b − tₐ < 0.
3. The swap strictly improves an "optimal" order, which is a contradiction. So an optimal order has no inversions: it's sorted by t. (Swapping equal times changes nothing.) ∎

With weights (minimise Σ wᵢCᵢ), the same swap changes the total by wₐt_b − w_b·tₐ. So a belongs before b exactly when tₐ / wₐ ≤ t_b / w_b: sort by time per unit weight (Smith's rule). In code, cross-multiply in integers:

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/greedy_stress_test.cpp#greedy -->
```cpp
// Smith's rule: run a before b when a.time / a.weight < b.time / b.weight. Cross-multiplied in
// 64 bits, so there's no floating-point error, and `<` (never `<=`) keeps it a strict weak ordering.
long long smithsRule(vector<Job> jobs) {
    sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b) {
        return (long long)a.time * b.weight < (long long)b.time * a.weight;
    });
    return weightedCompletion(jobs);
}
```
<!-- /snippet -->

**Greedy stays ahead.** Pick a measure of progress. Show by induction that after every step the greedy's measure is at least as good as any other solution's after the same number of steps, then conclude that the greedy finishes no later. Worked example 45 below is this pattern: after k jumps, the greedy reaches as far as any strategy can with k jumps.

**When greedy fails.**
- **Coins {1, 3, 4}, amount 6:** largest-coin-first takes 4 + 1 + 1 (3 coins), but 3 + 3 uses 2. No optimal answer for 6 contains a 4, so the greedy-choice property fails; the fix is DP (worked example 322 in module 16). For "canonical" coin sets such as {1, 2, 5, 10, 20, 50}, greedy happens to be optimal (the example file checks every amount up to 100), but that's a property of the coin set, not of greedy.
- **0/1 knapsack:** capacity 10, items (weight 6, value 30), (5, 20), (5, 20). Taking the best value per kg first grabs the 6 kg item (5 per kg), and then nothing else fits: 30. The two 5 kg items give 40. If items could be split (fractional knapsack), the same greedy *is* optimal: moving capacity from a lower-density item to a higher-density one never loses value. Indivisible items break that exchange.

**Stress-test a greedy.** This is the fastest way to find out a greedy is wrong:
1. Write an obviously correct brute force for tiny inputs (all orders, all subsets).
2. Generate random tiny inputs and compare the two.
3. The first mismatch is a counterexample small enough to study by hand.

In the example file, Smith's rule passes 500 random cases, while "shortest first" and "heaviest first" fail within a handful. For example, with jobs (t = 1, w = 1) and (t = 2, w = 5), shortest-first costs 16 and the optimum is 13.

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/greedy_stress_test.cpp#stress -->
```cpp
// The stress test: random tiny inputs, greedy vs brute force. Returns the first input where they
// disagree, i.e. a counterexample. Print it, shrink it by hand, and you'll see why the rule fails.
template <class Greedy>
optional<vector<Job>> findCounterexample(Greedy greedy, int iterations) {
    for (int iter = 0; iter < iterations; iter++) {
        int n = (int)t::rand_int(1, 6);                  // tiny, so the brute force stays fast
        vector<Job> jobs(n);
        for (Job& j : jobs) j = {(int)t::rand_int(1, 10), (int)t::rand_int(1, 10)};
        if (greedy(jobs) != bruteForce(jobs)) return jobs;
    }
    return nullopt;                                      // no counterexample found (not a proof!)
}
```
<!-- /snippet -->

A passing stress test is evidence, not proof; a failing one *is* proof that the greedy is wrong. The skill carries into online assessments: when a greedy idea is shaky, five minutes on a brute force plus a random loop is cheaper than a wrong submission.

### Template

The two snippets above: a greedy is a sort by the key your exchange argument produced, then one scan; the stress harness checks it.

### Pitfalls

- **Sorting by the wrong key** (start vs end, weight vs ratio). The key is whatever your exchange argument says.
- **Comparing ratios as `double`s:** cross-multiply in 64 bits (`(long long)a.t * b.w < (long long)b.t * a.w`). 10⁵ × 10⁵ = 10¹⁰ doesn't fit in `int`.
- **A comparator that isn't a strict weak ordering:** `<=` inside `sort`'s comparator is undefined behaviour and can crash (module 05 section 3). Use `<`.
- **Trusting the examples:** a problem's examples rarely contain the counterexample to your greedy.
- **Carrying a greedy over to a variant:** unit weights vs weights, fractional vs 0/1. It may not survive.

### Recognize it when…

- n up to 10⁵–10⁶ plus "minimum number of …", "maximum number of …", "earliest", "can you reach …": the target is a sort + scan in O(n log n) or a single O(n) pass.
- There's a natural best next move that never closes off options: the farthest reach, the smallest item that still fits, the earliest finish.
- A running quantity must stay within bounds (reachability, bracket balance). Track its extreme, or the range of values it can still take: 55 and 678 on your list.
- Each element has a constraint against both neighbours: handle one direction per pass (135 on your list).
- No exchange argument in a couple of minutes, or a stress test breaks the idea → switch to DP.

### Worked example: 45. Jump Game II
[LeetCode 45](https://leetcode.com/problems/jump-game-ii/) · Medium

> Solve [55. Jump Game](https://leetcode.com/problems/jump-game/) from your list **before** reading this: 45's
> solution contains 55's answer.

**Problem (paraphrased):** from index 0, you may jump forward from index i by at most nums[i] positions. Find the fewest jumps that reach the last index; reaching it is guaranteed.
**Signals:** "minimum number of jumps", n ≤ 10⁴, forward moves only. It's a shortest path in a graph where each index's edges cover a contiguous range.
**Brute force, and why it fails:** a DP, dp[j] = min(dp[i] + 1) over the i that can reach j, costs O(n · max jump) ≈ 10⁷ here. That passes, but it's O(n²) once jumps can reach n. The greedy is O(n) and it's the expected answer.
**Key insight:** it's BFS without a queue. The indices reachable in exactly k jumps form a contiguous window, and the next window runs from just past it to farthest = max(i + nums[i]) over the current window. Count windows until one covers n − 1.
**Why it's optimal (stays ahead):** let Rₖ be the farthest index reachable with at most k jumps.
- Every index ≤ Rₖ is reachable within k jumps, because any jump can be shortened.
- R₀ = 0, and Rₖ₊₁ = max(i + nums[i]) over all i ≤ Rₖ.
- The loop's `levelEnd` after k jumps is exactly Rₖ: it scans every i ≤ Rₖ and keeps that max.
- No strategy is ever past Rₖ after k jumps, so the first k with Rₖ ≥ n − 1, which is what the loop counts, is the minimum.

**Dry run:** nums = [2, 3, 1, 1, 4].

| i | i + nums[i] | farthest | i == levelEnd? | jumps | levelEnd |
|---|---|---|---|---|---|
| 0 | 2 | 2 | yes | 1 | 2 |
| 1 | 4 | 4 | no | 1 | 2 |
| 2 | 3 | 4 | yes | 2 | 4 |
| 3 | 4 | 4 | no | 2 | 4 |

The loop stops before the last index: 2 jumps.

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/0045-jump-game-ii.cpp#solution -->
```cpp
class Solution {
public:
    int jump(vector<int>& nums) {
        int n = nums.size();
        int jumps = 0;          // levels finished so far
        int levelEnd = 0;       // last index reachable with `jumps` jumps
        int farthest = 0;       // last index reachable with jumps + 1 jumps (from what we've scanned)
        for (int i = 0; i < n - 1; i++) {        // the last index never needs a jump out of it
            farthest = max(farthest, i + nums[i]);
            if (i == levelEnd) {                 // scanned the whole current level:
                jumps++;                         // one more jump reaches everything up to farthest
                levelEnd = farthest;
            }
        }
        return jumps;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time, O(1) space.
**Edge cases:**
- n = 1 → 0 jumps (the loop never runs).
- Zeros inside a window are harmless, because other indices in the window reach past them.
- One big first jump → 1.
- 10⁴ ones → 9,999 jumps (tested).

**Follow-ups:**
- *The end might be unreachable?* That's 55 on your list, and the same `farthest` idea solves it.
- *Return the path, not just the count?* In each window, remember the index that produced `farthest`.
- *Jumps in both directions, or to equal values?* The windows stop being contiguous: use a real BFS with a queue (module 14).

## 3. Interval problems — merging, insertion, minimum meeting rooms

### Concept

An interval is a pair [s, e]. Before writing any code, decide whether the endpoints belong to it:
- **Closed [s, e]:** [1, 2] and [2, 3] share the point 2, so they overlap.
- **Half-open [s, e):** a meeting that ends at 2 and one that starts at 2 don't clash.

Two intervals overlap when a.s ≤ b.e && b.s ≤ a.e (closed), or a.s < b.e && b.s < a.e (half-open). After sorting by start (a.s ≤ b.s), only one comparison is needed: b.s ≤ a.e (closed) or b.s < a.e (half-open).

| Problem | Endpoints | [1, 2] and [2, 3] |
|---|---|---|
| 56. Merge Intervals | closed | overlap → merged into [1, 3] |
| 435. Non-overlapping Intervals | touching is allowed | compatible |
| 2406. Divide Intervals Into Minimum Number of Groups | closed | clash → different groups |
| 253. Meeting Rooms II (the Premium twin of 2406) | half-open | can share a room |

Settle the convention from the statement and the examples; getting it wrong is the most common interval bug.

**Sort by start to merge.** After sorting by start, anything that overlaps the current merged block comes right after it, so one pass either extends the block or starts a new one (worked example 56). Inserting one interval into an already sorted, disjoint list needs no sort at all: that's 57 on your list.

**Sort by end to select.** To keep the most pairwise non-overlapping intervals (equivalently, to remove the fewest), keep choosing the compatible interval that **ends first**. Ending first leaves the most room for everything after it. The proof is activity selection in section 4, and 435 on your list is this problem, counted as removals.

**Counting overlaps.** The minimum number of rooms (groups, platforms) equals the maximum number of intervals that share a single point:
- **Lower bound:** intervals that share a point all need different rooms.
- **Achievable:** the heap greedy below never opens a room unless that many intervals share a point.

Two ways to compute it:
- **Sweep line:** turn each interval into events, +1 at its start and −1 at its end, then sort and scan with a running count. The largest count is the answer. Ties encode the convention: for closed integer intervals, put the −1 at e + 1; for half-open ones, process −1 before +1 at the same coordinate. (Module 06's difference arrays are the same idea for small coordinates.)
- **Min-heap of end times:** sort by start and keep one heap entry per room: the end of its last interval. If the room that frees up first is free before this interval starts, reuse it (pop); then push this interval's end. The heap's size is the room count.

Why the heap count is optimal: suppose the greedy opens room number r. The earliest-ending room wasn't free, so every existing room's last interval ends at or after the new start. Those r − 1 intervals also started no later (the input is sorted by start), so r intervals share the point `start` and any valid assignment needs at least r rooms.

C# note: `priority_queue` is a **max**-heap by default, while .NET's `PriorityQueue<TElement, TPriority>` is a min-heap. For a min-heap in C++, write `priority_queue<int, vector<int>, greater<int>>`.

### Template

Merging is worked example 56; the heap and the sweep are both in worked example 2406.

### Pitfalls

- **Closed vs half-open**, i.e. `<` vs `<=`: check the table above against the statement.
- **`merged.back()[1] = iv[1]` instead of `max(...)`:** [1, 10] followed by [2, 3] would shrink the block to [1, 3].
- **A comparator that takes `vector<int>` by value** copies two vectors per comparison. Take `const vector<int>&`, or use the default `sort`, which is lexicographic: by start, then by end.
- **The wrong sort key:** sorting by start for a selection problem, or by end for a merge.
- **`priority_queue` is a max-heap** unless you pass `greater<>`.
- **Sweep tie order:** sorting (x, delta) pairs puts −1 before +1 at equal x. That's right for half-open intervals or with the e + 1 shift, and wrong for closed intervals without the shift.
- **e + 1 near `INT_MAX`** overflows. Use `long long`, or order the ties explicitly.

### Recognize it when…

- The input is a list of [start, end] pairs: meetings, bookings, time ranges, segments, balloons, trains.
- "Merge", "union", "total covered length" → sort by start, then merge.
- "Insert a new interval into a sorted list" → three phases: before, overlapping, after.
- "Maximum number of non-overlapping …", "minimum removals so none overlap", "minimum points to hit them all" → sort by end.
- "Minimum rooms / groups / platforms", "maximum concurrent …" → sweep line or min-heap of end times.

### Worked example: 56. Merge Intervals
[LeetCode 56](https://leetcode.com/problems/merge-intervals/) · Medium

**Problem (paraphrased):** merge all overlapping intervals (touching counts as overlapping) and return the disjoint result.
**Signals:** "merge all overlapping", unsorted input, n up to 10⁴.
**Brute force, and why it fails:** repeatedly find any overlapping pair and merge it until nothing changes: O(n²) per pass and up to n passes. Marking a coverage array only works for small coordinates, and it needs care to keep [1, 2] and [3, 4] apart.
**Key insight:** after sorting by start, intervals that overlap one merged block are contiguous in the sorted order. The invariant: `merged` holds disjoint, sorted blocks, and only the last block can still grow, because everything after it starts at or after its start.
**Dry run:** [[1,3], [2,6], [8,10], [15,18]] (already sorted).

| interval | merged after | why |
|---|---|---|
| [1,3] | [[1,3]] | first block |
| [2,6] | [[1,6]] | 2 ≤ 3: extend to max(3, 6) |
| [8,10] | [[1,6], [8,10]] | 8 > 6: a gap |
| [15,18] | [[1,6], [8,10], [15,18]] | 15 > 10: a gap |

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/0056-merge-intervals.cpp#solution -->
```cpp
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        sort(intervals.begin(), intervals.end());               // by start (ties: by end)
        vector<vector<int>> merged;
        // Invariant: merged holds disjoint blocks, sorted; only the last one can still grow,
        // because every interval still to come starts at or after its start.
        for (const auto& iv : intervals) {
            if (!merged.empty() && iv[0] <= merged.back()[1])   // closed intervals: touching overlaps
                merged.back()[1] = max(merged.back()[1], iv[1]); // max: iv may lie entirely inside
            else
                merged.push_back(iv);                            // a gap: start a new block
        }
        return merged;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n log n) for the sort plus O(n) for the scan; O(n) for the output.
**Edge cases:**
- Containment: [1,10] with [2,3] and [4,5] → [1,10].
- Touching: [1,4] with [4,5] → [1,5].
- Unsorted input: [[4,7],[1,4]] → [[1,7]].
- Point intervals such as [2,2], and duplicates.
- A gap of one unit ([1,2] and [3,4]) stays two blocks.

**Follow-ups:**
- *Insert one interval into a sorted, disjoint list?* No re-sort needed: 57 on your list.
- *Intervals arrive one at a time?* Keep a `map<int, int>` from start to end, and on each insert merge with the neighbours found by `lower_bound`: O(log n) amortized.
- *Total covered length?* Merge, then sum e − s.
- *Half-open intervals?* Use `<` instead of `<=` in the overlap test.

### Worked example: 2406. Divide Intervals Into Minimum Number of Groups
[LeetCode 2406](https://leetcode.com/problems/divide-intervals-into-minimum-number-of-groups/) · Medium

**Problem (paraphrased):** split closed intervals [l, r] into as few groups as possible so that no two intervals in the same group intersect. Sharing an endpoint counts as intersecting.
**Signals:** "minimum number of groups", n up to 10⁵, coordinates up to 10⁶. It's the free twin of Meeting Rooms II and the sheet's "minimum platforms".
**Brute force, and why it fails:** "put each interval into the first group where it fits" works if you process the intervals sorted by start, but scanning every group per interval is O(n · groups), i.e. O(n²) when everything overlaps. Assigning groups by backtracking is exponential.
**Key insight:** the answer is the maximum number of intervals covering one point. Compute it with a min-heap of group end times, or with a sweep line.
**Dry run:** [[5,10], [6,8], [1,5], [2,3], [1,10]], sorted by start: [1,5], [1,10], [2,3], [5,10], [6,8].

| interval | heap top before | action | heap after (groups) |
|---|---|---|---|
| [1,5] | — | new group | {5} (1) |
| [1,10] | 5 ≥ 1 | new group | {5, 10} (2) |
| [2,3] | 5 ≥ 2 | new group | {3, 5, 10} (3) |
| [5,10] | 3 < 5 | reuse the group that ended at 3 | {5, 10, 10} (3) |
| [6,8] | 5 < 6 | reuse the group that ended at 5 | {8, 10, 10} (3) |

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/2406-divide-intervals-into-minimum-number-of-groups.cpp#solution -->
```cpp
class Solution {
public:
    int minGroups(vector<vector<int>>& intervals) {
        sort(intervals.begin(), intervals.end());                    // by start
        priority_queue<int, vector<int>, greater<int>> groupEnds;    // min-heap: the group that frees up first
        for (const auto& iv : intervals) {
            if (!groupEnds.empty() && groupEnds.top() < iv[0])       // closed: must end strictly before
                groupEnds.pop();                                     // reuse that group
            groupEnds.push(iv[1]);                                   // iv is now that group's last interval
        }
        return groupEnds.size();                                     // one heap entry per group
    }
};
```
<!-- /snippet -->

The sweep line computes the maximum overlap directly:

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/2406-divide-intervals-into-minimum-number-of-groups.cpp#sweep -->
```cpp
class Solution {
public:
    int minGroups(vector<vector<int>>& intervals) {
        vector<pair<int, int>> events;                   // (coordinate, +1 = starts, -1 = has ended)
        for (const auto& iv : intervals) {
            events.push_back({iv[0], +1});
            events.push_back({iv[1] + 1, -1});           // [l, r] covers the integers l..r: gone at r + 1
        }
        sort(events.begin(), events.end());              // same coordinate: -1 before +1 (leave, then enter)
        int active = 0, most = 0;
        for (const auto& e : events) {
            active += e.second;
            most = max(most, active);                    // the most intervals covering one point
        }
        return most;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n log n) time for either version; O(n) space.
**Edge cases:**
- All disjoint → 1; all identical → n.
- Touching endpoints clash: [1,5] and [5,8] → 2 (tested); [1,5] and [6,8] → 1.
- r + 1 ≤ 10⁶ + 1 fits comfortably in `int`.

**Follow-ups:**
- *Half-open meetings (253)?* In the heap version, reuse a room when `top() <= start`; in the sweep, drop the +1 shift and keep −1 before +1 at equal times.
- *Which room does each meeting get (the lowest-numbered free one)?* Two heaps: free rooms by number, busy rooms by end time.
- *Small coordinates?* A difference array over the coordinates: O(n + C) (module 06 section 4).

## 4. Scheduling & allocation — activity selection, deadlines, ship-within-D-days

### Concept

**Activity selection.** Keep the maximum number of pairwise non-overlapping intervals. The greedy: sort by end, scan, and keep an interval whenever it's compatible with the last one kept (using the problem's closed or half-open rule).

*Exchange proof.* Let g₁ be the interval that ends first, and take any optimal selection sorted by end, with first element o₁.
1. end(g₁) ≤ end(o₁), so swapping o₁ for g₁ keeps the selection valid (g₁ ends no later, so it can't collide with o₂, …) and the same size. So some optimal solution starts with g₁: that's the greedy-choice property.
2. Remove g₁ and every interval that overlaps it. What's left is the same problem, and g₁ plus an optimal selection of the rest is optimal: that's optimal substructure.
3. Induction does the rest.

*Stays-ahead proof.* Let greedy pick g₁, g₂, … and an optimal solution pick o₁, o₂, …, both sorted by end.
1. By induction, end(gᵢ) ≤ end(oᵢ) for every i: oᵢ₊₁ starts after end(oᵢ) ≥ end(gᵢ), so it was still available to greedy, which picked something ending no later.
2. If the optimum had an extra interval oₖ₊₁, it would still have been available after gₖ, and greedy would have taken it. So greedy keeps at least as many.

Other rules fail. Earliest start, on [1,10], [2,3], [4,5], keeps 1 interval where 2 fit. Shortest first, on [1,5], [4,7], [6,10], keeps only [4,7] where [1,5] and [6,10] fit.

**Deadlines with a heap.** When jobs have deadlines, process them in deadline order and keep the set you've committed to in a heap. When the set stops fitting, drop its worst member, which the heap finds in O(log n). Dropping the worst is safe because every kept job's deadline is ≤ the current one, so the kept jobs are interchangeable among the slots; removing the least valuable keeps the most value (an exchange argument). The template below does this for unit-time jobs with profits: the classic job-sequencing problem.

A related shape arises when each item is available over a window of time steps and you handle one per step: at each step, the available item with the earliest deadline is the safe pick (earliest deadline first, justified by the same exchange). That's 1353 on your list; deciding what goes into the heap, and when it leaves, is the exercise.

**Binary search on capacity (the ship-within-D-days family).** Split a sequence, in order, into at most D contiguous groups (days, workers, subarrays) so that the largest group total is as small as possible.
- **Predicate:** ok(C) = "with capacity C, a greedy left-to-right fill, starting a new group only when the next item doesn't fit, uses ≤ D groups".
- **Monotone:** more capacity never needs more groups.
- **Bounds:** lo = the largest item (items can't be split), hi = the total (everything in one group).
- **Cost:** O(n log(total)).
- **Why greedy filling is the right check:** it stays ahead. Packing each group as full as possible never makes a later group start later.

1011 (here) and 410 (Section 1) on your list are exactly this shape, and 1552 is its mirror image: maximise the minimum with `last_true`. No code here, because those are your practice.

**The task-scheduler idea (frames).** Identical tasks need a cooldown of n slots between them. Let f be the highest frequency and m the number of tasks tied at f. The most frequent task forces f − 1 gaps of at least n slots between its copies:

```text
tasks A×3, B×3, C×1, cooldown n = 2
A B C | A B _ | A B        frames of n + 1 = 3 slots; the last frame holds only the tasks tied at f
(3 − 1) · (2 + 1) + 2 = 8 slots
```

So you need at least (f − 1)(n + 1) + m slots, and at least one slot per task. The answer is the larger of the two, and it's achievable: fill the frames column by column in order of frequency, which puts copies of a task in different frames, at least n + 1 apart. If the frames overflow, widen them, and no idle slot is needed at all. Tested against a BFS over every schedule:

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/0621-task-scheduler.cpp#solution -->
```cpp
class Solution {
public:
    int leastInterval(vector<char>& tasks, int n) {
        int freq[26] = {};
        for (char c : tasks) freq[c - 'A']++;
        int maxFreq = *max_element(freq, freq + 26);
        int tiedForMax = count(freq, freq + 26, maxFreq);
        // Lower bound 1: the most frequent task needs maxFreq - 1 gaps of n slots between its copies:
        // maxFreq - 1 frames of n + 1 slots, then a last frame with every task tied for the max.
        int frames = (maxFreq - 1) * (n + 1) + tiedForMax;
        // Lower bound 2: every task takes a slot. The answer is the larger bound, and it's achievable.
        return max(frames, (int)tasks.size());
    }
};
```
<!-- /snippet -->

### Template

<!-- snippet: modules/11-binary-search-greedy-intervals/examples/deadline_scheduling.cpp#deadline_heap -->
```cpp
// Max total profit: one job per time slot 1, 2, 3, ..., each job finished by its deadline.
long long maxProfit(vector<Job> jobs) {
    sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b) { return a.deadline < b.deadline; });
    priority_queue<int, vector<int>, greater<int>> kept;   // min-heap: the cheapest kept job on top
    long long total = 0;
    // Invariant: `kept` is a most profitable set of the jobs seen so far that fits in the slots.
    for (const Job& j : jobs) {
        kept.push(j.profit);
        total += j.profit;
        if ((int)kept.size() > j.deadline) {                // more jobs than slots 1..deadline:
            total -= kept.top();                            // drop the least profitable one
            kept.pop();
        }
    }
    return total;
}
```
<!-- /snippet -->

Why it's correct: a set of unit jobs fits exactly when, for every d, at most d of its jobs have deadline ≤ d. Processing in deadline order, adding job j can only break that condition at d = j.deadline, and dropping any one job fixes it. Dropping the cheapest keeps the most profit. Complexity O(n log n) time, O(n) space.

Trace on (deadline, profit) = (1,20), (1,40), (2,15), (2,30), (3,5), (3,25):

| job | kept (min-heap) | size > deadline? | total |
|---|---|---|---|
| (1,20) | {20} | no | 20 |
| (1,40) | {20, 40} → drop 20 → {40} | yes | 40 |
| (2,15) | {15, 40} | no | 55 |
| (2,30) | {15, 30, 40} → drop 15 → {30, 40} | yes | 70 |
| (3,5) | {5, 30, 40} | no | 75 |
| (3,25) | {5, 25, 30, 40} → drop 5 → {25, 30, 40} | yes | 95 |

### Pitfalls

- **`priority_queue` is a max-heap:** the deadline template needs `greater<int>` to see the cheapest kept job.
- **Off-by-one on deadlines:** deadline d means slots 1..d, so at most d kept jobs have deadline ≤ d.
- **Binary search on capacity with lo = 1:** the greedy fill meets an item bigger than the capacity and loops or miscounts. Start lo at the largest item, or have the check return false.
- **Summing loads in `int`** when totals can pass 2·10⁹.
- **Frame counting:** forgetting the tie count m, or forgetting to take the max with the number of tasks.
- **Sorting by start for activity selection:** earliest start is a wrong greedy (counterexample above).

### Recognize it when…

- "Maximum number of events / meetings you can attend, one at a time" → sort by end. If each event takes one time unit and can happen anywhere in a window, go earliest deadline first with a min-heap.
- Each job has a deadline plus a profit or duration → sort by deadline, keep a heap, drop the worst when the set stops fitting.
- "Within D days", "at most m workers / students / pieces", "minimise the largest load" → binary search on capacity + greedy fill.
- "Cooldown between identical tasks", "no two equal items adjacent" → frequency counting and frames.

## Common mistakes

- **A non-monotone predicate fed to binary search:** always state the monotonicity sentence.
- **`int` sums inside a feasibility check** (Koko's hours reach 10¹³).
- **hi too small** (the answer is excluded) or **lo too big** (Koko's lo = min pile is wrong).
- **Epsilon loops for real-valued answers** instead of a fixed iteration count.
- **Believing a greedy because it passes the examples:** prove it, or stress-test it.
- **Floating-point ratio comparators:** cross-multiply in `long long`.
- **`<=` in a `sort` comparator.**
- **Closed vs half-open** confusion in interval overlap tests and sweep tie-breaking.
- **`max()` forgotten** when extending a merged interval.
- **`priority_queue` used as if it were a min-heap.**
- **Sorting intervals by start** for a max-non-overlapping selection.

## Say it out loud

A talk track for Koko Eating Bananas:

1. "I need the smallest integer speed that finishes every pile within h hours; up to 10⁴ piles of up to 10⁹ bananas."
2. "Brute force tries every speed from 1 to the biggest pile: O(n · max) ≈ 10¹³. Too slow."
3. "If a speed works, every faster speed works too, so feasibility is monotone: false, then true. I'll binary-search the first speed that works."
4. "The check sums ⌈p / speed⌉ over the piles, in `long long` because it can reach 10¹³, and stops once it passes h."
5. "The range is [1, max pile]; max pile always works because h ≥ n. That's O(n log max), about 3·10⁵ operations, with O(1) space."
6. "Edge cases: h == n forces the biggest pile; a huge h gives 1; integer ceiling, no floating point."

Follow-ups interviewers ask:
- **"Why is it monotone?"** Say the one-sentence argument; if you can't, the approach is suspect.
- **"What if there were no guarantee that an answer exists?"** Check the hi + 1 sentinel and return −1.
- **"Can you tighten the bounds?"** lo = ⌈Σp / h⌉.
- **"Prove your greedy."** An exchange argument (swap an adjacent inversion) or stays ahead, or show the stress test against brute force.
- **"Are the endpoints inclusive?"** Ask before coding any interval problem.

## Self-check

1. What two things must hold before you can binary-search an answer?
<details><summary>Answer</summary>A monotone predicate (once true, it stays true as x grows, or the reverse), and bounds [lo, hi] known to contain the answer, ideally with hi known to work. Without monotonicity the search silently returns garbage.</details>

2. Minimise the maximum vs maximise the minimum: which template, and what shape does the predicate have?
<details><summary>Answer</summary>Minimise: ok(x) = "can do it with limit x", shaped F…F T…T → <code>first_true</code>. Maximise: ok(x) = "can guarantee at least x", shaped T…T F…F → <code>last_true</code>.</details>

3. Why is Koko's `hours` a `long long`, and what else keeps the sum small?
<details><summary>Answer</summary>Up to 10⁴ piles × 10⁹ hours each = 10¹³, far beyond <code>INT_MAX</code>. The early exit (return false once hours &gt; h) also caps it, but don't rely on a cap you haven't proved.</details>

4. Why does real-valued binary search use a fixed number of iterations?
<details><summary>Answer</summary>100 halvings shrink any range below double precision, and the loop always terminates. An eps loop can run forever when eps is smaller than the spacing between doubles near the answer (about 1.2·10⁻⁷ near 10⁹).</details>

5. Give the exchange argument for shortest-job-first in three steps.
<details><summary>Answer</summary>(1) A non-sorted optimal order has an adjacent inversion a, b with tₐ &gt; t_b. (2) Swapping them moves nobody else and changes the total by t_b − tₐ &lt; 0. (3) That contradicts optimality, so the optimal order is sorted.</details>

6. Give a counterexample to greedy coin change and one to 0/1 knapsack by value density.
<details><summary>Answer</summary>Coins {1, 3, 4}, amount 6: greedy uses 4+1+1 (3 coins), but 3+3 uses 2. Knapsack with capacity 10 and items (6 kg, 30), (5 kg, 20), (5 kg, 20): greedy gets 30, the optimum is 40.</details>

7. Why is the number of heap entries in 2406 exactly the minimum number of groups?
<details><summary>Answer</summary>Intervals that share a point need different groups, so max overlap is a lower bound. When the greedy opens group r, every existing group's last interval ends at or after the new start and started before it, so r intervals share that point. The greedy never exceeds the lower bound.</details>

8. [1, 2] and [2, 3]: do they conflict in 56, in 2406, and in Meeting Rooms II (253)?
<details><summary>Answer</summary>56: yes, they merge into [1, 3] (closed). 2406: yes, they need different groups (closed). 253: no, half-open meetings can share a room.</details>

9. For "keep the most non-overlapping intervals", why sort by end and not by start or by length?
<details><summary>Answer</summary>The interval that ends first leaves the most room for the rest, and an exchange argument puts it into some optimal solution. Earliest start fails on [1,10], [2,3], [4,5]; shortest first fails on [1,5], [4,7], [6,10].</details>

10. In the deadline heap, why is dropping the minimum profit correct when the heap grows past the current deadline?
<details><summary>Answer</summary>Every kept job has deadline ≤ the current one, so any d of them fit into slots 1..d, which makes them interchangeable. One must go, and dropping the cheapest keeps the largest total: an exchange argument.</details>
