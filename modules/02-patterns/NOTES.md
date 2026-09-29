# 02 · Common DSA Patterns
> The dozen shapes almost every problem collapses into, once you can see them.

**Time:** ~5 h of Core work ([problems](problems.md)) · **Prereqs:** module 01 (C++ syntax, `vector`, `string`, `unordered_map`) · **You're done when:** you classify all 30 drill statements (A and B) with the right pattern and a one-line reason; you can write each section 2–section 3 skeleton from a blank file in under 5 minutes; you've solved 125, 643, 69 and 70 without hints.

This module is the map. Every later module zooms into one region of it, so here you learn to *recognize* each pattern and to write its skeleton; the proofs and hard variants come later.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Pattern recognition | Four questions (shape, ask, constraints, special feature) select one row of the master table | master table (Section 1) | Drill A |
| 2 | The core set | Use order or contiguity to throw away candidates without looking at them: one pass instead of a nested loop | [skeletons.cpp](examples/skeletons.cpp) | 125, 643, 69 |
| 3 | Traversal & state patterns | Visit states in a disciplined order: by distance (BFS), by choice (backtracking), by subproblem (DP) | [skeletons.cpp](examples/skeletons.cpp) | 70 |
| 4 | Choosing between them | `n` sets the complexity budget; signal words pick the family | constraint + signal tables (Section 4) | Drill B |

## 1. Pattern recognition — mapping a problem statement onto a known template

### Concept

Most interview problems are one of about twenty patterns in costume. Spend the first minutes of a problem **classifying**, not coding, and do it out loud: the interviewer grades your reasoning as much as your code.

**The four-question triage.** Answer these in order; the answers point at one row of the master table below.

1. **What is the input's shape?** Array or string (sorted?), matrix/grid, linked list, tree, graph (edges, prerequisites), intervals, a stream, a single big number.
2. **What is asked?**
   - *count* ("how many") → prefix sums + hash map, window counting, DP (number of ways), combinatorics
   - *min / max* → greedy, DP, binary search on the answer, BFS (fewest steps)
   - *exists / is it possible* → two pointers, hash set, BFS/DFS reachability, binary search
   - *all* ("list every") → backtracking; the output alone is exponential, so exponential time is expected
   - *k-th / top k* → heap, quickselect, binary search on the value
   - *construct* (an order, an arrangement) → greedy, topological sort, constructive reasoning
3. **What do the constraints allow?** `n` fixes your budget before you think of any algorithm (Section 4, module 03 section 4): n ≤ 20 → exponential is fine; n ≤ 5·10³ → O(n²); n ≤ 10⁵–10⁶ → O(n log n) or O(n); values up to 10⁹ → you can't index an array by value.
4. **What is special?** The feature that unlocks a pattern: *sorted* (binary search, two pointers), *contiguous* (window, prefix sums), *pairs / complements* (hash map, two pointers), *dependencies* (topological sort), *prefixes of words* (trie), *intervals* (sort + sweep), *grid* (BFS/DFS with direction arrays), *next greater* (monotonic stack), *repeated subproblems* (DP).

**Triage in action.** "An app logs n ≤ 10⁵ login timestamps in increasing order. Answer q ≤ 10⁵ queries: how many logins happened between t₁ and t₂?"
Shape: a sorted array plus queries. Asked: a count. Constraints: scanning per query is n·q = 10¹⁰, too slow, so each query must cost O(log n). Special: *sorted*. → Binary search: `upper_bound(t₂) − lower_bound(t₁)` per query, O(q log n) (module 05 section 4).

### Template: the master pattern table

| Signal in the statement | Pattern | Taught in | Template |
|---|---|---|---|
| sorted array; pair or triplet with a target sum; palindrome; reverse in place | Two pointers, converging | 02 section 2, 06 section 1 | `examples/skeletons.cpp` |
| two sorted sequences walked together; filter an array in place | Two pointers, same direction | 02 section 2, 04 section 1, 06 section 1 | `examples/skeletons.cpp` |
| contiguous subarray/substring; longest, shortest or count under a constraint | Sliding window | 02 section 2, 06 section 2 | `examples/skeletons.cpp` |
| many range sums on data that doesn't change | Prefix sums | 06 section 3 | `templates/prefix_sum.hpp` |
| subarray sum equals k, negatives allowed; divisible by k | Prefix sums + hash map | 07 section 4 | — |
| many range updates, values read once at the end | Difference array, sweep line | 06 section 4 | `templates/prefix_sum.hpp` |
| middle of a linked list, cycles, O(1) memory; a process that must repeat | Fast & slow pointers | 02 section 2, 08 section 2 | `examples/skeletons.cpp` |
| sorted data; "minimum X such that…" with an easy yes/no check | Binary search (on the index or on the answer) | 02 section 2, 05 section 4, 11 section 1 | `templates/binary_search.hpp` |
| fewest steps, unweighted grid or graph; spreading one step per minute | BFS | 02 section 3, 14 section 2 | `examples/skeletons.cpp` |
| islands, regions, connected groups, merging groups | DFS / BFS / union–find | 14 section 2, section 4 | `templates/dsu.hpp` |
| "list all" subsets, permutations, combinations; n ≤ ~20 | Backtracking | 02 section 3, 10 | `examples/skeletons.cpp` |
| number of ways; best value over a sequence of choices | Dynamic programming | 02 section 3, 16 | `examples/skeletons.cpp` |
| k-th largest, top k, merge k sorted lists, running median | Heap | 13 section 1–2 | `templates/heap.hpp` |
| next greater / smaller element, spans, histogram areas | Monotonic stack | 09 section 2 | `templates/monotonic.hpp` |
| max or min of every window | Monotonic deque | 09 section 4 | `templates/monotonic.hpp` |
| meetings, bookings, overlapping ranges | Sort + sweep / merge | 11 section 3, 06 section 4 | — |
| prerequisites, build order, dependencies | Topological sort | 14 section 3 | — |
| prefixes, autocomplete, dictionary of words | Trie | 13 section 3 | `templates/trie.hpp` |
| shortest path with weights | Dijkstra (0-1 BFS for 0/1 weights) | 15 section 1 | `templates/graph.hpp` |
| range queries mixed with updates | Fenwick tree / segment tree | 17 | `templates/fenwick.hpp`, `templates/segment_tree.hpp` |
| a locally best choice that provably never hurts | Greedy + exchange argument | 11 section 2 | — |
| seen before? frequency? complement? | Hash map / set | 07 section 1 | — |
| matching brackets, nesting, undo | Stack | 09 section 1 | — |

### Pitfalls

- **Matching on one word.** "Subarray" suggests a window, but if values can be negative the window is wrong (06 section 2 shows the counterexample); you need prefix sums + a hash map.
- **"Sorted" versus "sortable".** If the answer doesn't depend on the original order, sorting it yourself (O(n log n)) unlocks two pointers and binary search. If you need original indices, sort `(value, index)` pairs.
- **Greedy by gut feeling.** A greedy rule needs a proof or at least a failed search for a counterexample; otherwise it's DP (drill A, statement 7).
- **Forgetting the output size.** When the task is "list all", no algorithm beats the size of the list; don't hunt for a polynomial one.
- **Coding before classifying.** Say the pattern and the target complexity before the first line of code.

### Recognize it when…

- You can name the brute force but it's a double loop over pairs or subarrays and n ≈ 10⁵: look for what's special (sorted? contiguous? monotone?).
- The statement contains a word from the section 4 signal table: start from that row, then check it against the constraints.
- Two patterns both fit: pick by the constraints, then by which invariant you can state in one sentence.

### Classification drill A

For each statement: name the pattern, give a one-line reason, then open the answer. The statements are paraphrased and unnamed on purpose; each answer names the module that teaches the pattern.

1. You get an array of positive integers and a target. Return the length of the shortest contiguous stretch whose sum is at least the target (0 if none). n ≤ 10⁵.
<details><summary>Answer</summary>

**Variable sliding window (shortest).** Contiguous, and all values are positive: growing a window only raises its sum and shrinking only lowers it, so once a window reaches the target you shrink from the left to find the shortest one ending at each r. O(n). **Module 06 section 2.**
</details>

2. Count the contiguous subarrays whose sum is exactly k. Values may be negative. n ≤ 2·10⁴.
<details><summary>Answer</summary>

**Prefix sums + hash map.** Negatives mean a window can't know when to shrink. Since sum(l..r) = P[r+1] − P[l], for each r count how many earlier prefix sums equal P[r+1] − k. O(n). **Module 07 section 4.**
</details>

3. There are piles of bananas and h hours. Each hour you choose one pile and eat up to s bananas from it (a smaller pile is finished and the rest of the hour is wasted). Find the smallest s that finishes every pile within h hours.
<details><summary>Answer</summary>

**Binary search on the answer.** "Smallest s such that it's feasible", and feasibility is monotone (eating faster never hurts); checking one s costs O(n). O(n log max). **Module 11 section 1.**
</details>

4. A grid holds empty cells, fresh oranges and rotten ones. Every minute, each rotten orange rots its fresh neighbours (up, down, left, right). How many minutes until nothing fresh is left, or −1 if something never rots?
<details><summary>Answer</summary>

**Multi-source BFS.** Spreading one step per minute from several starting points is BFS by levels, with every rotten cell in the queue at minute 0; the number of levels is the answer. O(R·C). **Module 14 section 2.**
</details>

5. There are n courses and pairs "to take a, you must first take b". Can every course be completed?
<details><summary>Answer</summary>

**Topological sort (cycle detection).** Prerequisites form a directed graph; everything is completable exactly when there's no cycle. Kahn's algorithm (repeatedly take a course with no unmet prerequisite) finishes all n nodes iff the graph is acyclic. O(V + E). **Module 14 section 3.**
</details>

6. Given n, produce every string of n opening and n closing brackets that is correctly balanced.
<details><summary>Answer</summary>

**Backtracking.** "Produce every…" means the output itself is exponential. Build the string one character at a time and prune any prefix that closes more brackets than it opened. **Module 10 section 2.**
</details>

7. Given coin values (unlimited supply) and an amount, return the fewest coins that make exactly the amount, or −1.
<details><summary>Answer</summary>

**DP (unbounded knapsack).** A best value over choices, with the same sub-amounts reached many ways: best(x) = 1 + min over coins c of best(x − c). Greedy (largest coin first) fails: coins {1, 3, 4} and amount 6 give 4+1+1 greedily, but 3+3 is better. **Module 16 section 3.**
</details>

8. Return the k values that occur most often in an array.
<details><summary>Answer</summary>

**Hash map of counts + heap** (a size-k min-heap, O(n log k)), or buckets indexed by count (O(n)). "Top k" is the heap signal. **Module 13 section 2.**
</details>

9. Given a list of daily temperatures, report for each day how many days you'd wait for a strictly warmer one (0 if it never comes).
<details><summary>Answer</summary>

**Monotonic stack.** It's "next greater element to the right" for every position. A stack of indices with decreasing temperatures pushes and pops each index once. O(n). **Module 09 section 2.**
</details>

10. Given time ranges [start, end], merge every group of overlapping ranges and return the result.
<details><summary>Answer</summary>

**Sort by start, then sweep:** extend the current range while the next one starts before it ends, otherwise close it and start a new one. O(n log n). **Module 11 section 3.**
</details>

11. Numbers arrive one at a time. After each one, report the median of everything seen so far.
<details><summary>Answer</summary>

**Two heaps:** a max-heap holds the lower half and a min-heap the upper half, with sizes kept within one; the median sits on top. O(log n) per number. **Module 13 section 2.**
</details>

12. A tree on n nodes had one extra edge added. Return an edge whose removal leaves a tree (among several, the one that appears last in the input).
<details><summary>Answer</summary>

**Union–find (DSU).** Add the edges in input order; the first edge whose endpoints are already connected closes the cycle, and it's the last cycle edge in the input. Nearly O(n). **Module 14 section 4.**
</details>

13. A singly linked list may loop back on itself. Return the node where the loop begins (or null) using O(1) extra memory.
<details><summary>Answer</summary>

**Fast & slow pointers (Floyd).** O(1) memory rules out a visited set; the two runners meet inside the loop, and a short distance argument turns the meeting point into the entrance. **Module 08 section 2.**
</details>

14. Return every distinct triple of values (taken from different positions) that sums to zero, with no triple repeated. n ≤ 3000.
<details><summary>Answer</summary>

**Sort + fix one element + converging two pointers, skipping duplicates.** n ≤ 3000 allows O(n²). Sorting turns "the other two sum to −x" into a two-pointer scan and puts duplicates next to each other. **Module 06 section 1.**
</details>

15. A signal starts at one node and travels along one-way links, each with its own travel time. How long until every node has it (−1 if some node never does)?
<details><summary>Answer</summary>

**Dijkstra.** Single-source shortest paths with non-negative weights; the answer is the largest distance. O(E log V). **Module 15 section 1.**
</details>

## 2. The core set — two pointers, sliding window, fast-slow, binary search on answer

### Concept

The brute force for most array problems tries every pair, every subarray or every candidate answer. Each pattern below rests on an **invariant** (a fact that stays true after every step) that lets you **discard candidates without looking at them**:

| Pattern | What one step discards | Brute force → pattern |
|---|---|---|
| Converging two pointers | a whole row or column of the pair grid | O(n²) → O(n) after sorting |
| Same-direction two pointers | an element that can never be part of a better answer | O(n·m) → O(n + m) |
| Sliding window | every window starting left of l (never needed again) | O(n²) or O(n³) → O(n) |
| Fast & slow pointers | nothing: it replaces the visited set, not the time | O(n) memory → O(1) |
| Binary search on the answer | half of the remaining candidate answers | O(range) checks → O(log range) |

A `while` loop inside a `for` loop is **not** automatically O(n²). If each pointer only moves forward and moves at most n times in total, the whole thing is O(n): the cost is counted over the entire run (amortized), not per outer iteration.

### Template

All skeletons below live in [examples/skeletons.cpp](examples/skeletons.cpp), each stress-tested against a brute force. They solve toy problems that are *not* in the plan, so the practice problems are left for you.

#### Two pointers: converging

**Reach for it when** the array is sorted (or you may sort it) and the question is about pairs: a sum, a difference, an area, or symmetry (palindromes, reversal). Pointers start at both ends and move inward.

**Why it works.** Draw all pairs (i, j) with i < j as the upper triangle of a grid. Sortedness means one comparison at (l, r) settles a whole row or column: if a[l] + a[r] is too small, it's too small with *every* partner j ≤ r, so row l is finished. Invariant: every pair that uses an index outside [l, r] is already counted or ruled out. Each step removes one index, so at most n − 1 steps.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#converging -->
```cpp
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
```
<!-- /snippet -->

**Trace** on a = [1, 2, 4, 6, 9], target 8:

| l | r | a[l] + a[r] | action | count |
|---|---|---|---|---|
| 0 | 4 | 10 | ≥ 8: a[4] = 9 is finished, r-- | 0 |
| 0 | 3 | 7 | < 8: pairs (0,1), (0,2), (0,3) all count, l++ | 3 |
| 1 | 3 | 8 | ≥ 8: r-- | 3 |
| 1 | 2 | 6 | < 8: pair (1,2) counts, l++ | 4 |
| 2 | 2 | — | l == r: stop | **4** |

O(n) after sorting, O(1) space. → [125. Valid Palindrome](https://leetcode.com/problems/valid-palindrome/) uses the same loop comparing instead of counting; your only decision is what to do when a pointer sits on a character that doesn't count.

#### Two pointers: same direction

**Reach for it when** two sorted sequences are walked together (merge, intersect, closest pair across them), or one array is scanned by a fast *reader* while a slow *writer* builds the answer in place (module 04 section 1). Both pointers only move forward.

**Why it works** (for the toy below): suppose a[i] < b[j]. Every later b is even farther above a[i]. Every earlier b was dropped while paired with some a[i'] ≤ a[i] that was ≥ it, so its gap to a[i] is no smaller than a gap already recorded. So a[i] has no better partner left: drop it.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#same_direction -->
```cpp
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
```
<!-- /snippet -->

**Trace** on a = [1, 4, 10], b = [6, 13, 20]:

| i | j | a[i] | b[j] | gap | best | move |
|---|---|---|---|---|---|---|
| 0 | 0 | 1 | 6 | 5 | 5 | a smaller: i++ |
| 1 | 0 | 4 | 6 | 2 | 2 | a smaller: i++ |
| 2 | 0 | 10 | 6 | 4 | 2 | b smaller: j++ |
| 2 | 1 | 10 | 13 | 3 | 2 | a smaller: i++, a is used up |

O(n + m), O(1) space.

#### Sliding window: fixed size

**Reach for it when** the statement says "every window of size k", "k consecutive", "subarray of length exactly k".

**Why it works.** Neighbouring windows share k − 1 elements. Keep a summary of the window (a sum, a count map, a counter) and update it for the one element entering and the one leaving: O(1) per step instead of O(k).

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#fixed_window -->
```cpp
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
```
<!-- /snippet -->

**Trace** on a = [1, 2, 1, 3, 3], k = 3:

| r | enters | leaves | window | counts | distinct |
|---|---|---|---|---|---|
| 0 | 1 | — | [1] | {1:1} | (not full yet) |
| 1 | 2 | — | [1, 2] | {1:1, 2:1} | (not full yet) |
| 2 | 1 | — | [1, 2, 1] | {1:2, 2:1} | 2 |
| 3 | 3 | 1 | [2, 1, 3] | {1:1, 2:1, 3:1} | 3 |
| 4 | 3 | 2 | [1, 3, 3] | {1:1, 3:2} | 2 |

O(n) expected (hash map), O(k) space. → In [643. Maximum Average Subarray I](https://leetcode.com/problems/maximum-average-subarray-i/) the window's summary is a single number.

#### Sliding window: variable size

**Reach for it when** you're asked for the longest (or shortest) contiguous piece satisfying a condition, and the condition is *monotone*: for "longest", cutting elements off a valid window keeps it valid; for "shortest", adding elements to a valid window keeps it valid.

**Why it works.** For the longest version: if [l, r + 1] is valid then [l, r] is valid, so the smallest valid left end for r + 1 is never left of the one for r. The left pointer only moves right; each index enters once and leaves once, so the nested `while` costs O(n) in total.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#variable_window -->
```cpp
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
```
<!-- /snippet -->

**Trace** on a = [3, 1, 2, 1, 4], budget 5:

| r | a[r] | sum after adding | shrink | window | best |
|---|---|---|---|---|---|
| 0 | 3 | 3 | — | [0, 0] | 1 |
| 1 | 1 | 4 | — | [0, 1] | 2 |
| 2 | 2 | 6 | drop 3 → 3 | [1, 2] | 2 |
| 3 | 1 | 4 | — | [1, 3] | 3 |
| 4 | 4 | 8 | drop 1 → 7, drop 2 → 5 | [3, 4] | 3 |

O(n), O(1). The *shortest* version records inside the shrink loop while the window is still valid; module 06 section 2 has it, plus counting windows. The precondition matters: with a negative number the sum can drop when the window grows, and the window silently returns wrong answers (06 section 2 has the counterexample).

#### Fast & slow pointers

**Reach for it when** a linked-list question asks for the middle or about cycles with O(1) extra memory, or when a value keeps being fed through a function (digit-square sums, "use each value as the next index") and must eventually repeat.

**Why it works.** The hare moves two steps per tick, the tortoise one. On a list of length n, when the hare has covered all of it the tortoise has covered half. If the sequence cycles, once both are in the cycle the hare gains exactly one position per tick, so the gap shrinks 1 at a time and they meet within one lap; it can't jump over the tortoise.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#fast_slow -->
```cpp
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
```
<!-- /snippet -->

**Trace** with next = [1, 2, 3, 4, 2] from x0 = 0 (the walk is 0 → 1 → 2 → 3 → 4 → 2 → …):

| tick | slow | fast |
|---|---|---|
| start | f(0) = 1 | f(f(0)) = 2 |
| 1 | 2 | f(f(2)) = 4 |
| 2 | 3 | f(f(4)) = 3: they meet at 3 |

From 3 the lap is 4, 2, 3: length **3**. Module 08 runs the same two speeds on linked lists, which *end*: there the hare can fall off, so it must check it has room before each double step.

#### Binary search on the answer

**Reach for it when** you're asked for the minimum (or maximum) value X such that some condition holds, checking one X is easy, and the check is monotone: if X works, every larger X works (or every smaller one).

**Why it works.** A monotone yes/no over an ordered range looks like `false false … true true`; it has exactly one boundary, and halving the range finds it in O(log range) checks. The kit's template does the halving, so you only write the check:

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

Applied to a toy: the highest saw-blade height that still collects enough wood. "Highest H such that…" with `true … true false … false` is `last_true`:

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#bs_answer -->
```cpp
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
```
<!-- /snippet -->

**Trace** on trees = [4, 12, 9, 7], need = 6. `last_true` runs `first_true` on "not enough" over [0, 12], then subtracts 1:

| range [lo, hi) | mid H | wood cut at H | not enough? | next range |
|---|---|---|---|---|
| [0, 13) | 6 | 0 + 6 + 3 + 1 = 10 | no | [7, 13) |
| [7, 13) | 10 | 0 + 2 + 0 + 0 = 2 | yes | [7, 10) |
| [7, 10) | 8 | 0 + 4 + 1 + 0 = 5 | yes | [7, 8) |
| [7, 8) | 7 | 0 + 5 + 2 + 0 = 7 | no | [8, 8) |

The first "not enough" height is 8, so the answer is **7**: four checks instead of thirteen. O(n log(max height)). → [69. Sqrt(x)](https://leetcode.com/problems/sqrtx/) is "largest x with x·x ≤ n"; choose the range and an overflow-safe comparison yourself.

### Pitfalls

- **Converging pointers on unsorted data** give wrong answers silently. Sort first; if the answer needs original indices, sort `(value, index)` pairs or use a hash map instead (module 07).
- **`a[l] + a[r]` overflows `int`** when values approach 2³¹. The skeletons compute sums and differences in `long long`.
- **`a.size() - 1` on an empty vector** is `size_t` arithmetic and wraps around to 2⁶⁴ − 1. Write `(int)a.size() - 1`.
- **Windows need their monotone condition.** Negative numbers break "sum ≤ budget"; use prefix sums + a hash map (07 section 4) or a deque (09).
- **Stale map entries.** When a window count drops to 0, `erase` the key, or `count.size()` keeps counting values that left.
- **Fixed windows:** emit a result only once the window is full (`r >= k - 1`), and remove `a[r - k]`, not `a[r - k + 1]`.
- **Fast & slow on a list** needs a null check before every hop; on a function sequence there is no null.
- **Binary search:** `(lo + hi) / 2` can overflow (use `lo + (hi - lo) / 2`); a predicate that isn't monotone gives garbage with no error; hand-written bounds are where off-by-ones live, so reuse `first_true` / `last_true`.

### Recognize it when…

| Cue | Skeleton |
|---|---|
| "sorted", "pair", "two numbers", "palindrome", "reverse in place" | converging pointers |
| two sorted arrays; "merge"; "in place, return the new length" | same-direction pointers |
| "every window of size k", "k consecutive elements" | fixed window |
| "longest / shortest substring or subarray such that…" with non-negative values or counts | variable window |
| linked list + O(1) memory; "cycle", "middle"; a process that repeats | fast & slow |
| "minimum / maximum value such that…", huge answer range, easy check | binary search on the answer |

## 3. Traversal & state patterns — BFS levels, DFS with backtracking, memoise-then-tabulate

### Concept

These three explore a space of states instead of an array:

- **BFS** explores by distance. A queue is first-in first-out, so every state at distance d is dequeued before any state at distance d + 1. With unit-cost moves, the first time you dequeue the target you've found the fewest moves.
- **Backtracking** explores a decision tree depth-first. The path from the root is "the choices made so far"; recording at the leaves enumerates every complete choice sequence. Cost ≈ (number of tree nodes) × (work per node); pruning deletes whole subtrees.
- **Memoise → tabulate** is for recursions that solve the same subproblem again and again. Cache each answer (memo) so it's computed once; then compute the subproblems bottom-up (table); then keep only what the next step reads (rolling variables).

### Template

#### BFS: queue, visited-on-push, level snapshot

**Reach for it when** you need the fewest moves, steps or transformations with every move costing the same: grids, unweighted graphs, word or lock puzzles, or something spreading one step per minute.

**Why mark on push.** Marking a cell when you *enqueue* it means each cell enters the queue once. Mark on pop instead and a cell can be enqueued by several neighbours before its first pop; unless you also skip cells already visited when popping, those copies multiply (in an open grid, as fast as the number of shortest paths).

**Why the level snapshot.** `levelSize = q.size()` taken at the start of a level is exactly the set of cells at distance `steps`; processing that many before incrementing `steps` gives distances without a distance array.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#bfs_grid -->
```cpp
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
```
<!-- /snippet -->

**Trace** from S = (0, 0) to T = (0, 3):

```
S . # T        steps 0: (0,0)           steps 4: (2,2)
. # # .        steps 1: (1,0) (0,1)     steps 5: (2,3)
. . . .        steps 2: (2,0)           steps 6: (1,3)
               steps 3: (2,1)           steps 7: (0,3) popped = T → 7
```

O(R·C) time and space. On an adjacency list, the direction loop becomes `for (int v : adj[u])`. Module 14 reuses this skeleton for islands, flood fills and multi-source spreading; when you only need to *visit* cells (not count steps), drop the level loop, or use DFS.

#### DFS with backtracking: choose → explore → unchoose

**Reach for it when** the statement says "all possible", "every combination / permutation / arrangement", or asks you to place things under constraints, and n is small (≈ 10–20).

**Why it works.** The recursion visits every root-to-leaf path of the decision tree. Choosing and unchoosing on **one shared** `path` keeps the state exactly equal to "the choices on the current path", with no copying per call.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#backtracking -->
```cpp
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
```
<!-- /snippet -->

**Trace** for n = 2 over "abc" (✗ = pruned):

```
                  ""
        a/        b|        \c
      "a"         "b"        "c"
     ✗a b c     a ✗b c     a b ✗c
     "ab""ac"   "ba""bc"   "ca""cb"     → 6 strings = 3 · 2
calls: push a, push b, record "ab", pop b, push c, record "ac", pop c, pop a, push b, ...
```

Cost: 3 · 2ⁿ⁻¹ leaves, each copied into `out` in O(n). In general, backtracking costs O(tree nodes × work per node + output size).

The other common shape is **include / skip**: at element i, either take it or don't. Same discipline, two recursive calls:

```c++
void dfs(int i) {                   // decide about element i
    if (i == n) { /* one complete subset is in `chosen` */ return; }
    chosen.push_back(a[i]);         // choose: include a[i]
    dfs(i + 1);                     // explore
    chosen.pop_back();              // unchoose
    dfs(i + 1);                     // explore the branch without a[i]
}
```

Module 10 builds its subset and combination templates on this shape; often you carry a running value (a sum, an XOR) down the tree instead of a vector.

#### Memoise → tabulate: recursion → memo → table → rolling variables

**Reach for it when** you're asked for a number of ways, a minimum cost or a maximum value, the answer for a state depends on answers for smaller states of the same form, and the plain recursion revisits states.

The recipe, on a toy: **how many ways can you tile a 1 × n strip with red squares, green squares (1 × 1) and blue dominoes (1 × 2)?**

1. **State:** ways(n) = number of tilings of a strip of length n.
2. **Transition:** look at the *last* tile. A square (2 colours) leaves length n − 1; a domino leaves n − 2. So ways(n) = 2·ways(n − 1) + ways(n − 2).
3. **Base cases:** ways(0) = 1 (the empty tiling), ways(1) = 2.

Stage 1, plain recursion: correct, but exponential. The call tree re-solves the same n again and again, about 3·10⁸ calls for n = 40.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#dp_recursive -->
```cpp
// Look at the LAST tile: a square (2 colours) leaves a 1 x (n-1) strip; a domino leaves 1 x (n-2).
//   ways(n) = 2 * ways(n - 1) + ways(n - 2),   ways(0) = 1 (the empty tiling),   ways(1) = 2
long long ways_recursive(int n) {
    if (n == 0) return 1;
    if (n == 1) return 2;
    return 2 * ways_recursive(n - 1) + ways_recursive(n - 2);   // re-solves the same n again and again
}
```
<!-- /snippet -->

Stage 2, memo: the same recursion plus a cache, so each n is solved once.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#dp_memo -->
```cpp
// Top-down: the same recursion, but each n is solved once and cached. memo[n] == -1: not solved yet.
long long ways_memo(int n, vector<long long>& memo) {
    if (n == 0) return 1;
    if (n == 1) return 2;
    if (memo[n] != -1) return memo[n];
    return memo[n] = 2 * ways_memo(n - 1, memo) + ways_memo(n - 2, memo);
}
// Call it as: vector<long long> memo(n + 1, -1); ways_memo(n, memo);
```
<!-- /snippet -->

Stage 3, table: the same recurrence filled bottom-up. No recursion, so no stack depth to worry about.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#dp_table -->
```cpp
// Bottom-up: fill dp[0..n] in an order where everything dp[i] reads is already filled.
long long ways_table(int n) {
    vector<long long> dp(max(n + 1, 2));
    dp[0] = 1;
    dp[1] = 2;
    for (int i = 2; i <= n; i++) dp[i] = 2 * dp[i - 1] + dp[i - 2];
    return dp[n];
}
```
<!-- /snippet -->

Stage 4, rolling variables: `dp[i]` reads only the previous two entries, so two variables replace the table.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#dp_rolling -->
```cpp
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
```
<!-- /snippet -->

| Stage | Time | Extra space | Note |
|---|---|---|---|
| recursion | exponential (≈ 1.62ⁿ calls) | O(n) stack | the specification: obviously correct |
| memo | O(n) | O(n) memo + O(n) stack | easiest to write from the recursion |
| table | O(n) | O(n) | no recursion depth limit |
| rolling | O(n) | O(1) | only when the recurrence reads a fixed window of earlier states |

**Trace** of the table for n = 4: dp = [1, 2, 5, 12, 29]. Check dp[2] = 5 by hand: RR, RG, GR, GG and one domino. The test file also counts tilings by a completely different method (strings over {R, G, B} whose B-runs have even length) and compares. → [70. Climbing Stairs](https://leetcode.com/problems/climbing-stairs/) is this ladder with its own recurrence: ask what the last move was, then walk all four stages.

### Pitfalls

- **BFS:** mark on push; bounds-check before you index; take the level snapshot *before* the inner loop (`q.size()` changes as you push). BFS gives fewest moves only when every move costs the same; with weights, use Dijkstra or 0-1 BFS (module 15).
- **DFS depth:** a recursive DFS over a 1000 × 1000 grid can go 10⁶ frames deep and overflow the stack. Use BFS or an explicit stack when the depth can reach ~10⁵.
- **Forgetting to unchoose** leaks one branch's choices into its siblings. Every `push_back` before the recursive call needs a `pop_back` after it.
- **C# habit:** in C#, `result.Add(path)` stores a *reference* to the list you keep mutating (you need `new List<int>(path)`). In C++, `out.push_back(path)` stores a copy, because containers have value semantics.
- **Passing `string path` by value** into every call works but copies O(n) per call. Pass by reference and unchoose.
- **Memo sentinel:** `-1` means "not computed" only if −1 can never be a real answer; otherwise use a separate `bool` array or `optional`.
- **Counting DPs overflow fast.** ways(49) still fits in `long long`, ways(50) doesn't. Real problems ask for the answer mod 10⁹ + 7; apply `%` after every addition.
- **Table order:** fill states so everything `dp[i]` reads is already computed; size the table `n + 1`, not `n`.

### Recognize it when…

| Cue | Pattern |
|---|---|
| "minimum number of steps / moves / operations", each move costs the same | BFS |
| "spreads", "infects", "every minute" | BFS from all sources at once |
| "all possible", "generate every", "list all combinations", n ≤ ~20 | backtracking |
| "number of ways", "minimum cost to reach", "maximum total with a restriction" | DP |
| a recursion that calls itself twice on overlapping arguments | memoise it |

## 4. Choosing between them — the signals in the constraints that pick your approach

### Concept

Read the constraints **before** you design anything: they tell you the complexity budget. A common rule of thumb is about 10⁸ simple operations per second for C++ (treat it as a rough scale, not a promise). Then the statement's wording narrows the family. Module 03 section 4 goes through the constraint table in detail.

### Template: two tables

**Constraint → complexity** (largest n → what fits):

| Largest n | Budget | Typical techniques |
|---|---|---|
| ≤ 10–12 | O(n!), O(n·n!) | all permutations, brute force over orders |
| ≤ 20–25 | O(2ⁿ), O(n·2ⁿ) | subsets, backtracking with pruning, bitmask DP |
| ≤ 100–500 | O(n³) | Floyd–Warshall, interval DP, triple loops |
| ≤ 2·10³–5·10³ | O(n²) | 2D DP (LCS, edit distance), all pairs |
| ≤ 10⁵–10⁶ | O(n log n), O(n) | sorting, heaps, binary search, two pointers, windows, prefix sums, hashing, BFS/DFS |
| ≥ 10⁹ (up to 10¹⁸) | O(log n), O(√n), O(1) | binary search on the answer, maths, fast exponentiation, digit DP |

Also check: with q queries, the cost is build + q × (cost per query). Values up to 10⁹ can't index an array (hash map or coordinate compression). A grid is R·C cells: 1000 × 1000 = 10⁶ is fine for BFS.

**Signal words → pattern:**

| Signal words | Reach for | Module |
|---|---|---|
| "contiguous", "subarray", "substring" | sliding window, prefix sums | 06 |
| "sorted", "non-decreasing" | binary search, two pointers | 05, 06 |
| "all possible", "every combination", "generate" | backtracking | 10 |
| "minimum steps", "fewest moves", "shortest" (no weights) | BFS | 14 |
| "number of ways", "best value with choices" | DP | 16 |
| "top k", "k-th largest / smallest" | heap, quickselect | 13, 05 |
| "prefix", "starts with", "dictionary of words" | trie | 13 |
| "connected", "groups", "islands", "friends of friends" | DFS, union–find | 14 |
| "order", "dependencies", "prerequisites" | topological sort | 14 |
| "intervals", "meetings", "overlapping", "bookings" | sort + sweep | 11, 06 |
| "next greater", "next smaller", "previous warmer" | monotonic stack | 09 |
| "subarray sum = k", "sum divisible by k" | prefix sums + hash map | 07 |
| "median of a stream" | two heaps | 13 |
| "range queries with updates" | Fenwick / segment tree | 17 |

### Pitfalls

- **Coding O(n²) against n = 10⁵** and hoping: that's 10¹⁰ operations. If the budget says no, find a different idea before typing.
- **Over-engineering small inputs:** with n ≤ 100, an O(n³) brute force is the intended answer, and far easier to get right.
- **Several test cases per input:** what matters is the bound on the *total* n across tests, when the statement gives one.
- **Memory:** 10⁸ `int`s is 400 MB, more than judges usually allow; a 10⁵ × 10⁵ table is impossible whatever the time limit.

### Recognize it when…

- n tiny and the task says "all" → the exponential brute force is intended.
- n or the values reach 10⁹ and beyond → no array of that size; think maths or binary search on the answer.
- Many queries on data that doesn't change → preprocess once (prefix sums, sorting); queries mixed with updates → a tree structure (module 17).

### Classification drill B

The constraints decide these. Name the approach and the reason, then check.

1. n ≤ 15 jobs with durations are assigned to k workers (any job to any worker). Minimize the busiest worker's total.
<details><summary>Answer</summary>

**Backtracking with pruning** (or bitmask DP over subsets of jobs). n ≤ 15 allows exponential search; prune any partial assignment already worse than the best found. No simple greedy is correct. **Module 10 section 4, 16 section 4.**
</details>

2. Same goal, but n ≤ 10⁵ jobs in a fixed order, and each worker takes one contiguous block of them.
<details><summary>Answer</summary>

**Binary search on the answer** (the maximum load) with a greedy O(n) check: fill each worker until the next job would exceed the limit. O(n log(total)). Exponential search is impossible at 10⁵. **Module 11 section 1, section 4.**
</details>

3. n ≤ 1000 points in the plane: find the two closest to each other.
<details><summary>Answer</summary>

**Check all pairs:** about 5·10⁵ distance computations. The brute force *is* the answer here; nothing cleverer is needed. **Module 03 section 4.**
</details>

4. An array of n ≤ 10⁵ integers and q ≤ 10⁵ queries "sum of a[l..r]"; the array never changes.
<details><summary>Answer</summary>

**Prefix sums:** O(n) once, O(1) per query. Summing each query directly costs n·q = 10¹⁰. **Module 06 section 3.**
</details>

5. Same, but queries are mixed with updates "set a[i] = x".
<details><summary>Answer</summary>

**Fenwick tree (or segment tree):** O(log n) per update and per query. A prefix-sum array would need an O(n) rebuild after every update. **Module 17 section 2.**
</details>

6. Longest palindromic substring of a string of length ≤ 1000. What changes if the length can be 10⁶?
<details><summary>Answer</summary>

**1000: expand around each center**, O(n²) = 10⁶ steps. **10⁶:** O(n²) is 10¹² steps, so you need **Manacher's algorithm**, O(n). **Module 04 section 3, 18 section 4.**
</details>

7. Count pairs i < j with a[i] + a[j] ≤ K, for n ≤ 10⁵ and |a[i]| ≤ 10⁹.
<details><summary>Answer</summary>

**Sort + converging two pointers**, O(n log n); all pairs would be 5·10⁹. The sums reach 2·10⁹, past `INT_MAX`: compute them in `long long`. **Module 02 section 2, 06 section 1.**
</details>

8. Can n ≤ 20 numbers be split into two groups with equal sums?
<details><summary>Answer</summary>

**Enumerate subsets:** 2²⁰ ≈ 10⁶, by backtracking or bitmasks. **Module 10 section 2.**
</details>

9. The same question with n ≤ 200 numbers, each between 1 and 100.
<details><summary>Answer</summary>

2²⁰⁰ is impossible, but the sums are small (at most 20 000): **subset-sum DP** over reachable sums, about n · sum/2 = 2·10⁶ steps. **Module 16 section 3.**
</details>

10. A grid up to 1000 × 1000 with walls: fewest moves from the top-left corner to the bottom-right.
<details><summary>Answer</summary>

**BFS**, O(R·C) = 10⁶. Trying all paths with DFS is exponential. **Module 14 section 2.**
</details>

11. n ≤ 10⁵ values up to 10⁹: for each window of k consecutive values, report how many distinct values it holds.
<details><summary>Answer</summary>

**Fixed window + hash map of counts**, O(n). An array indexed by value would need 10⁹ slots. **Module 02 section 2, 07 section 1.**
</details>

12. 10⁵ updates "add v to every position in [l, r]" with positions up to 10⁹, then report the largest value anywhere.
<details><summary>Answer</summary>

**Difference updates stored in a map keyed by position** (or coordinate compression + a difference array), then one sweep in key order: O(n log n). A 10⁹-slot array doesn't fit in memory. **Module 06 section 4.**
</details>

13. A graph with 10⁵ nodes and 2·10⁵ edges, every weight 0 or 1: shortest distance from node 1 to all nodes.
<details><summary>Answer</summary>

**0-1 BFS with a deque**, O(V + E): 0-weight edges go to the front, 1-weight edges to the back. Dijkstra also works in O(E log V); plain BFS doesn't, because edges cost different amounts. **Module 15 section 1.**
</details>

14. All-pairs shortest travel times between n ≤ 400 cities.
<details><summary>Answer</summary>

**Floyd–Warshall**: n³ = 6.4·10⁷, fine. At n = 10⁵ it would be 10¹⁵; run Dijkstra only from the sources you need instead. **Module 15 section 2.**
</details>

15. Count the integers in [1, N] whose digits are all different, for N ≤ 10¹⁸.
<details><summary>Answer</summary>

You can't loop to 10¹⁸. **Digit DP** over N's ≈ 19 digits: state = (position, "still equal to N's prefix?", mask of digits used). **Module 16 section 4.**
</details>

## Common mistakes

- Starting to code before stating the pattern and the target complexity from `n`.
- Running a sliding window over data with negative numbers.
- Running two pointers over unsorted data, or sorting when the answer needs the original indices.
- Marking BFS cells visited when popping instead of when pushing.
- Forgetting the "unchoose" step, so one branch's choices leak into the next.
- Filling a DP table in an order that reads entries not yet computed.
- `int` sums and counts that overflow: pair sums of values near 10⁹, n(n+1)/2 subarrays for n = 10⁵, counting DPs.
- Off-by-ones in window length (`r - l + 1`) and in hand-written binary-search bounds (use the template).

## Say it out loud

A talk track for "longest subarray with sum ≤ budget, values non-negative, n ≤ 10⁵":

1. *Restate:* "I need the longest contiguous block whose total stays within the budget. All values are non-negative, and n can be 10⁵."
2. *Brute force:* "Every subarray with a running sum is O(n²), about 5·10⁹ steps at 10⁵. Too slow."
3. *Insight:* "Because values are non-negative, removing elements never increases a sum, so a valid window stays valid when it shrinks. That's the sliding-window condition: extend on the right, shrink from the left while over budget."
4. *Complexity:* "Each index enters and leaves the window at most once: O(n) time, O(1) space."
5. *Edge cases:* "Empty input and nothing fitting both give 0; a zero budget still admits runs of zeros; sums go in `long long`."

Follow-ups interviewers commonly ask:

- *"What if values can be negative?"* The window breaks. Use prefix sums: a subarray fits when P[r+1] − P[l] ≤ budget, i.e. P[l] ≥ P[r+1] − budget; running maxima of the prefix array are sorted, so a binary search finds the earliest such l, O(n log n).
- *"Your while loop is inside a for loop. Isn't that O(n²)?"* No: l only moves forward, at most n times over the whole run.
- *"Prove the two-pointer step never skips the answer."* State the invariant (every pair outside [l, r] is already decided) and show each move preserves it.
- *"Can you do it without recursion?"* BFS and tabulation need none; DFS can use an explicit stack.
- *"What if the input isn't sorted?"* Sort it (O(n log n)) if the answer doesn't need original positions, or use a hash map (module 07).

## Self-check

1. What are the four triage questions, in order?
<details><summary>Answer</summary>

Input shape; what's asked (count / min-max / exists / all / k-th / construct); what the constraints allow; what's special (sorted, contiguous, pairs, dependencies, prefixes, intervals, grid).
</details>

2. Why does the converging two-pointer scan never miss a valid pair?
<details><summary>Answer</summary>

Invariant: every pair using an index outside [l, r] is already counted or impossible. With sorted data, if a[l] + a[r] is too small it's too small with every partner ≤ r, so dropping l discards only pairs that can't work (symmetrically for r). Each move preserves the invariant.
</details>

3. The variable window has a `while` inside a `for`. Why is it O(n)?
<details><summary>Answer</summary>

Both pointers only move forward, and each moves at most n times across the entire run, so the inner loop executes at most n times in total (amortized), not n times per outer iteration.
</details>

4. Give an input on which the longest-window skeleton fails, and name the fix.
<details><summary>Answer</summary>

Negative numbers: [2, 2, −10, 2] with budget 3. The window drops index 0 when the sum hits 4, but the whole array (sum −4) is valid, so it reports 3 instead of 4. Use prefix sums (+ binary search or a hash map, depending on the question).
</details>

5. Why mark BFS cells visited when you push them, not when you pop them?
<details><summary>Answer</summary>

Marking on push puts each cell in the queue exactly once. Marking on pop lets several neighbours enqueue the same cell before it's popped; without an extra skip, the duplicates multiply and blow up time and memory.
</details>

6. What does `int levelSize = q.size()` buy you?
<details><summary>Answer</summary>

It freezes the number of cells at the current distance before new ones are pushed, so you process exactly one level per `steps` value and know every cell's distance without a distance array.
</details>

7. What goes wrong if you forget the "unchoose" step in backtracking?
<details><summary>Answer</summary>

The shared state keeps the old choice, so sibling branches start from a wrong state: results contain leftover elements, and pruning checks use a path that isn't the real one.
</details>

8. Name the four stages from recursion to rolling variables, with each one's time and space.
<details><summary>Answer</summary>

Plain recursion (exponential time, O(n) stack) → memo (O(n) time, O(n) memo + stack) → table (O(n) time, O(n) space, no recursion) → rolling variables (O(n) time, O(1) space, when each state reads only a fixed number of earlier ones).
</details>

9. The same "choose a subset" question comes with n ≤ 20 and with n ≤ 10⁵. What changes?
<details><summary>Answer</summary>

n ≤ 20: enumerating all 2ⁿ ≈ 10⁶ subsets is fine. n ≤ 10⁵: exponential is impossible; you need structure (DP over small values, greedy with a proof, sorting + two pointers, binary search on the answer).
</details>

10. What property must the check have for binary search on the answer, and how do you choose `first_true` or `last_true`?
<details><summary>Answer</summary>

It must be monotone over the answer range: one switch from false to true (or true to false). "Smallest x that works" with `false…true` is `first_true`; "largest x that works" with `true…false` is `last_true`.
</details>
