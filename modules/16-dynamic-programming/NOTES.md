# 16 · Dynamic Programming
> The topic that decides most offers — taught as a recipe, not a flash of insight.

**Time:** ~20 h of Core work ([problems](problems.md)) · **Prereqs:** modules 10 (recursion, choose / skip trees), 02 section 3 and 03 section 2 (memo → table on 70 and 509), 05 (`lower_bound`), 12 (post-order recursion, for section 4) · **You're done when:** (1) before coding any Core problem you can write its five recipe lines (state in words, transition, base cases, order, answer) and say why the transition misses nothing; (2) from a blank file you can write 0/1 and unbounded knapsack, the O(n log n) LIS and the LCS table, and explain each loop's direction; (3) you re-solve 322, 1143 and 337 in under 25 minutes each, plus one follow-up (less memory, or the actual solution).

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | State design | Name the subproblem in words; build it from the **last decision** | the five-step recipe · 198 | 746, 198, 139 |
| 2 | Memoisation → tabulation | One recurrence, three evaluations: memo, table, rolling row | 62 (memo / table / row) | 62, 63 |
| 3 | Classic families | Recognise the family; inherit its state and loop order | `knapsack.cpp` · 416 · 322 · 300 · 1143 · 72 | 416, 494, 1049, 518, 322, 300, 673, 1143, 718, 516, 72, 44, 714, 188 |
| 4 | Harder shapes | The state is a range, a subtree, a subset or a digit prefix | 312 · 337 · 1986 · `tsp_held_karp.cpp` · 2376 | 1039, 312, 132, 337, 1986, 2376 |

Every file in [examples/](examples/) is stress-tested against a brute force; `make test M=16-` runs them all.

## 1. State design — defining dp[i], the transition, and proving nothing is missed

### Concept

**The decision-tree view.** A brute force (module 10) walks a tree of decisions: each node makes one choice and recurses on what's left. Label every node with *what's left to decide, plus whatever about the past still matters*. DP applies when:
- **Overlapping subproblems:** the same label appears in many places. Nodes with equal labels have identical subtrees, so solve each label once and cache it.
- **Optimal substructure:** a node's best value is the best, over its choices, of (that choice's value + the child's best value). This holds when the label captures everything the future depends on: how you reached a node can't change what's best from there.

House Robber, labelled f(i) = best loot from the first i houses, n = 5:

```text
                     f(5)
         skip h4  /        \  rob h4
              f(4)          f(3)
             /    \         /   \
          f(3)    f(2)    f(2)  f(1)
          /  \
       f(2)  f(1)       f(2) already appears three times. The tree has ~1.6^n nodes,
                        but only n + 1 distinct labels: with a cache, O(n).
```

**Cost of any DP = (number of distinct states) × (work per state, usually the number of choices).** Every complexity in this module comes from that product.

**The recipe.** Write these five lines before any code. They are the spine of every worked example below.

| Step | Ask | House Robber (198) |
|---|---|---|
| 1. State, in words | "dp[…] = the best / count / whether for …": which part of the input, plus what the future needs | best[i] = most money from the first i houses |
| 2. Transition | Take any solution of this state. What was its **last decision**? Each option leaves a smaller state | house i−1 skipped → best[i−1]; robbed → best[i−2] + nums[i−1] |
| 3. Base cases | The smallest states, where the transition would read outside the table | best[0] = 0, best[1] = nums[0] |
| 4. Order | Every state after the states it reads | i = 2, 3, …, n |
| 5. Answer | Which state (or max over which states) is the whole problem | best[n] |

**Proving nothing is missed.** Every valid solution decomposes by its last step. The transition is right when:
1. **Exhaustive:** every solution of state S ends with one of the listed decisions.
2. **Reducible and extendable:** removing the last decision leaves a valid solution of the smaller state S′, and *any* solution of S′ plus that decision is valid for S.
3. **Disjoint** (counting only): no solution falls under two decisions, or you double count.

Then optimum(S) = best over decisions d of (value(d) + optimum(S′_d)). Why the child must be optimal too: if the S′ part of an optimal solution for S weren't the best for S′, you could swap in a better S′ solution and improve S, a contradiction (the "cut and paste" argument). For counting, count(S) = Σ_d count(S′_d).

"Extendable" is what fails when the state is too small. Take LIS with best[i] = "longest increasing subsequence inside nums[0..i)": to append nums[i] you need the last chosen value to be smaller, and best[i] doesn't remember it. Fix: put the missing fact in the state, "longest increasing subsequence *ending at* index i" (Section 3).

**Finding the state variables: "what must I know to decide the rest?"**
1. Write the brute force as a function that **returns the value of the remaining work**, not one that carries a running total. `void go(int i, int lootSoFar)` has states (i, loot) that almost never repeat; `int best(int i)` depends on i alone: n states.
2. Its parameters are the state. Position in the input first (i; i and j; l and r), then whatever limits the next choice: the last item taken (LIS), the remaining capacity (knapsack), holding a stock or not, which items are used (bitmask), "still equal to N's prefix" (digit DP).
3. Test it: can two histories with the same state have different best futures? Then a variable is missing. Can you drop one without that happening? Drop it: every variable multiplies the state count.
4. Cost check: states × choices ≲ 10⁸ simple steps.

**Common state shapes**

| Shape | State | Reads | Order | Typical cost | Examples |
|---|---|---|---|---|---|
| 1D prefix / index | dp[i] over a[0..i) | i−1, i−2, or every j < i | i upward | O(n), O(n²) | 198, 746, 139, 300 |
| Two sequences | dp[i][j] over a[0..i), b[0..j) | (i−1, j−1), (i−1, j), (i, j−1) | row by row | O(n·m) | 1143, 72, 718, 44 |
| Grid | dp[r][c] | up, left | row by row | O(R·C) | 62, 63 |
| Knapsack capacity | dp[i][c]: first i items, capacity / sum c | skip or take item i | item by item (Section 3 for one row) | O(n·W) | 416, 494, 1049, 322, 518 |
| Interval | dp[l][r] over a[l..r] | a split point / last operation k | by increasing length | O(n³) | 312, 1039, 516 |
| Tree node | dp[v][flag] for v's subtree | its children | post-order | O(n) | 337 |
| Bitmask | dp[mask] or dp[mask][last] | mask minus one element | mask upward | O(2ⁿ·n), O(2ⁿ·n²) | 1986, TSP |
| Digit / tight | f(pos, tight, started, extra) | the next digit, 0–9 | top-down memo | O(digits · 4 · extra · 10) | 2376 |

### Template

```c++
// 1. best[i] = <quantity> for <the first i items>, in words. Size n + 1: best[0] is the empty prefix.
vector<long long> best(n + 1);
best[0] = /* 3. base case */;
for (int i = 1; i <= n; i++)             // 4. order: best[i] reads only smaller indices
    best[i] = /* 2. combine over the last decision: best[i-1] + ..., best[i-2] + ..., best[j] + ... */;
return best[n];                           // 5. answer (or the max over all i)
```

Time = states × choices; space = states, often reducible (Section 2). The compiled version of this shape is 198 below.

### Pitfalls

- **A vague state.** "dp[i] = answer at i": including element i or not? Prefix length or index? Write the definition as a comment and size the table to match (n + 1 for prefix lengths).
- **Base cases guessed instead of derived.** best[1] = nums[0] follows from the definition; best[1] = max(nums[0], nums[1]) mixes prefix lengths with indices.
- **Answer in the wrong cell.** With "ending at i" states the answer is the max over all i, not dp[n−1].
- **Double counting.** Counting sets by "which element is last" counts each set once per ordering; make the decisions disjoint.
- **Sentinels.** A min-DP's INF must survive INF + cost (amount + 1, 1e9 or LLONG_MAX / 4, never INT_MAX); a max-DP with negative values needs −INF, not 0.
- **A running total as a parameter** (the module-10 habit): the states stop repeating and the memo does nothing.

### Recognize it when…

DP in general:
- The question asks for a **count**, a **min / max**, or **whether it's possible**, over choices that interact (taking one limits another). If it says "list all", the output itself is exponential: backtracking.
- The brute force is a recursion tree, and you can see labels repeating.
- Greedy has a small counterexample (coins {1, 3, 4}, amount 6: greedy's 4+1+1 is 3 coins, 3+3 is 2).
- "Subsequence" (may skip elements), not "subarray" (contiguous: usually sliding window, Kadane or prefix sums).
- Constraints: n ≤ 10⁵ → 1D DP with O(1) or O(log n) transitions; n ≤ 1000–5000 → O(n²); n ≤ 300–500 with ranges → O(n³) interval; n ≤ 20 → bitmask; a small value bound (sums ≤ 10⁴) → the value becomes a dimension; N up to 10¹⁸ with "count numbers ≤ N" → digit DP.

1D shapes (this section's problems):
- "Maximise … with no two adjacent / consecutive chosen" → take / skip (198).
- "Minimum total cost to reach position i, paying for each move" → dp[i] = the best over the last move (746).
- "Number of ways to reach position i" → dp[i] = the sum over the last step (the Climbing Stairs shape from module 02).
- "Can the string be split into dictionary words?" → boolean prefix reachability (139).

### Worked example: 198. House Robber
[LeetCode 198](https://leetcode.com/problems/house-robber/) · Medium

**Problem (paraphrased):** Houses in a row hold non-negative amounts of money; robbing two adjacent houses sets off an alarm. Return the largest total you can take.

**Signals:** "maximum", items in a line, "no two adjacent": each choice constrains only its neighbours. n ≤ 100.

**Brute force, and why it fails:** try every subset with no two adjacent houses. There are Fibonacci-many, about 1.6ⁿ ≈ 10²¹ for n = 100.

**Key insight:** look at the last house. Either it's skipped (the best for the first n−1 houses) or robbed (house n−2 is off limits: the best for the first n−2, plus it). Both leftovers are prefixes, so there are only n + 1 states.

**Recipe:** the table in the Concept: best[i] over the first i houses · skip or rob house i−1 · best[0] = 0, best[1] = nums[0] · i upward · best[n]. Nothing is missed: every valid set either contains house i−1 or not; removing it leaves a valid set of a shorter prefix, and any such set extends back.

**Dry run:** nums = [2, 7, 9, 3, 1]

| i | 0 | 1 | 2 | 3 | 4 | 5 |
|---|---|---|---|---|---|---|
| nums[i−1] | – | 2 | 7 | 9 | 3 | 1 |
| best[i] | 0 | 2 | max(2, 0+7) = 7 | max(7, 2+9) = 11 | max(11, 7+3) = 11 | max(11, 11+1) = **12** |

Answer 12: houses 0, 2 and 4. The table version maps line by line onto the recipe:

<!-- snippet: modules/16-dynamic-programming/examples/0198-house-robber.cpp#table -->
```cpp
class Solution {
public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        // 1. State: best[i] = the most money from the first i houses (houses 0..i-1).
        vector<int> best(n + 1);
        // 3. Base cases: no houses -> 0; one house -> rob it.
        best[0] = 0;
        best[1] = nums[0];
        // 4. Order: best[i] reads best[i-1] and best[i-2], so fill left to right.
        for (int i = 2; i <= n; i++) {
            // 2. Transition, by the last decision: skip house i-1, or rob it (then house i-2 is off limits).
            best[i] = max(best[i - 1], best[i - 2] + nums[i - 1]);
        }
        // 5. Answer: the state that covers all n houses.
        return best[n];
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time, O(n) space. best[i] reads only best[i−1] and best[i−2], so two variables (`twoBack`, `oneBack`) bring the space down to O(1); that's the version to hand in (`Solution` in the example file).

**Edge cases:**
- n = 1: the table's loop never runs and best[1] = nums[0]. (n ≥ 1 by the constraints; the two-variable version even returns 0 for an empty array.)
- [2, 1, 1, 2] → 4: the optimum skips two houses in a row, so "the better of the even and odd indices" is wrong.
- All zeros → 0.

**Follow-ups:**
- *Which houses?* Keep the table and walk back from i = n: if best[i] == best[i−1], house i−1 was skipped (i −= 1); otherwise it was robbed (record it, i −= 2).
- *Houses in a circle?* The first and last houses become neighbours: break the circle into two straight rows (drop the first house, or drop the last) and reuse this solution on each.
- *Negative amounts?* The transition copes (skipping is always allowed), but the base case must become best[1] = max(0, nums[0]); the two-variable version already does that.

## 2. Memoisation → tabulation — recursion first, then bottom-up, then space-optimised

### Concept

One recurrence can be evaluated three ways. Same states, same transitions, same answer; they differ in order, memory and constant factors.

**Top-down (memoisation).** Write the recursion straight from the recurrence; look a state up before computing it, store it after. Each state is computed once, so time = states × choices. You get the evaluation order for free (the recursion reaches dependencies first), and you only touch states reachable from the start.
- **Memo in a `vector`** indexed by the state, with a sentinel meaning "not computed": the default. The sentinel must be impossible as an answer (−1 for counts and lengths); if every value is possible, keep a separate `vector<char> done`.
- **Memo in a hash map** only when the state space is huge and sparse, or isn't a small integer tuple: pack the state into one key (`(long long)i << 32 | j`). Several times slower than a vector, and `unordered_map<pair<int, int>, int>` doesn't even compile without a custom hash.
- **Recursion depth** = the longest chain of dependent states (n for a 1D DP, m + n for a grid). Stacks are typically 1–8 MB: depth ~10⁴ is fine, 10⁵–10⁶ frames can overflow (sanitizer builds make frames bigger). For n ≥ 10⁵, go bottom-up.

**Bottom-up (tabulation).** Allocate the table, fill the base cases, then loop over states so that every state comes after the states it reads (a topological order of the dependency graph). The Order column of section 1's shapes table gives one for each shape; intervals are the classic trap: by increasing length (or l downward, r upward), never l and r both upward.

Memo → table, mechanically: the memo array becomes the table; the recursion's base cases become initial cells; each recursive call becomes a cell read; order the loops so those cells are filled first; return the answer cell.

**Rolling arrays** (keep only the rows you still read):
- **Two rows.** If row r reads only row r−1, keep `prev` and `cur` and swap them after each row: O(columns) memory.
- **One row.** Enough when the inner loop runs in a direction where each overwritten cell is never read again. In 62, when you compute `row[c]`, it still holds the previous row (up) and `row[c−1]` already holds this row (left).
- **One row + a saved diagonal.** LCS and edit distance also read the up-left cell, which was overwritten one step earlier: save it in a variable before overwriting (1143).
- **Two variables.** A 1D chain that reads only i−1 and i−2 (198).

The price: rolling gives up reconstruction, because walking back needs the full table.

**Which one to write**

| Top-down wins when… | Bottom-up wins when… |
|---|---|
| few states are reachable out of a huge space (digit DP, games, (i, remaining) pairs that mostly can't occur) | the recursion would be too deep (n ≥ 10⁵) |
| the order is awkward (intervals, trees, DAG-shaped states) | the time limit is tight: no call overhead, sequential memory |
| you're under time pressure: it's the brute force plus three lines | you need rolling arrays (they exist only bottom-up) |
| | a further speedup scans states in order (prefix sums over dp, a monotonic deque, binary search) |

Interview default: derive the recurrence, write the memo version (quick, low risk), then offer the table and the rolling version as the follow-up.

### Template

```c++
vector<int> memo(numStates, -1);        // -1 = "not computed"; must be impossible as a real answer
int solve(int state) {
    if (/* base case */) return /* its value */;
    int& res = memo[state];             // one lookup; safe because memo never resizes during the recursion
    if (res != -1) return res;
    return res = /* combine solve(smaller states) over the last decision */;
}
```

The three compiled stages of 62 below are the full template: memo, table, one row.

### Pitfalls

- **A sentinel that is also a real answer** (0 is a valid count): the memo never hits, and you're back to exponential time with correct output, the hardest kind of slowness to spot.
- **A reference into a growing container:** `int& res = memo[s]` is safe only if nothing resizes `memo` during the recursion (a `vector` you `push_back` into invalidates it).
- **The memo copied on every call:** make it a member or pass it by reference; a `vector` parameter taken by value copies the whole memo each time.
- **A memo carried across test cases:** a global or static memo survives between tests when a judge runs them in one process. Assign it inside the entry method.
- **Big local arrays:** `int dp[5000][5000]` inside a function lives on the stack and crashes; use `vector` (heap).
- **Rolling mistakes:** reading a cell you already overwrote (the diagonal), or not resetting `cur` when the transition doesn't write every cell.

### Recognize it when…

- You have a correct brute-force recursion whose arguments take few distinct values → memoise it as is.
- n ≥ 10⁵ with a 1D recurrence → bottom-up (recursion depth).
- "Use O(1) extra space" or "can you reduce memory?" → rolling rows or variables.
- Grids where each cell depends on neighbours in fixed directions (62, 63) → table first, then one row.

### Worked example: 62. Unique Paths
[LeetCode 62](https://leetcode.com/problems/unique-paths/) · Medium

**Problem (paraphrased):** A robot starts at the top-left of an m × n grid and moves only right or down. Count the distinct paths to the bottom-right cell.

**Signals:** "number of distinct paths", moves in two fixed directions (so no cell can be revisited: the dependencies have no cycles). m, n ≤ 100; the answer is guaranteed ≤ 2·10⁹.

**Brute force, and why it fails:** walk every path recursively: one leaf per path. There are C(m+n−2, m−1) paths; the guarantee caps that at 2·10⁹, which is still 2·10⁹ leaves (and without the cap, 100 × 100 would have about 10⁵⁸).

**Key insight:** every path into (r, c) arrives from (r−1, c) or from (r, c−1), never both, so the counts add. Only m·n states.

**Recipe:** paths(r, c) = number of paths from (0, 0) to (r, c) · the last move was down or right → paths(r−1, c) + paths(r, c−1) (disjoint and exhaustive: no double counting) · row 0 and column 0 are 1 · row by row · paths(m−1, n−1).

**Dry run:** m = 3, n = 7

```text
1  1  1  1  1  1  1
1  2  3  4  5  6  7
1  3  6 10 15 21 28    → 28
```

Stage 1, memo: the recursion as written, plus a cache.

<!-- snippet: modules/16-dynamic-programming/examples/0062-unique-paths.cpp#memo -->
```cpp
class Solution {
    vector<vector<int>> memo;                  // memo[r][c] = paths from (0,0) to (r,c); -1 = not computed yet

    int paths(int r, int c) {
        if (r == 0 || c == 0) return 1;        // first row or column: one straight path
        int& cached = memo[r][c];              // safe: memo is never resized during the recursion
        if (cached != -1) return cached;
        return cached = paths(r - 1, c) + paths(r, c - 1);   // the last move came from above or from the left
    }

public:
    int uniquePaths(int m, int n) {
        memo.assign(m, vector<int>(n, -1));
        return paths(m - 1, n - 1);
    }
};
```
<!-- /snippet -->

Stage 2, table: the same recurrence, loops in dependency order.

<!-- snippet: modules/16-dynamic-programming/examples/0062-unique-paths.cpp#table -->
```cpp
class Solution {
public:
    int uniquePaths(int m, int n) {
        vector<vector<int>> paths(m, vector<int>(n, 1));    // row 0 and column 0 stay 1: the base cases
        for (int r = 1; r < m; r++)
            for (int c = 1; c < n; c++)                      // row by row: up and left are already final
                paths[r][c] = paths[r - 1][c] + paths[r][c - 1];
        return paths[m - 1][n - 1];
    }
};
```
<!-- /snippet -->

Stage 3, one row: row r reads only row r−1 and itself.

<!-- snippet: modules/16-dynamic-programming/examples/0062-unique-paths.cpp#row -->
```cpp
class Solution {
public:
    int uniquePaths(int m, int n) {
        vector<int> row(n, 1);                 // row 0
        for (int r = 1; r < m; r++)
            for (int c = 1; c < n; c++)
                row[c] += row[c - 1];          // before: row[c] = up (last row), row[c-1] = left (this row)
        return row[n - 1];
    }
};
```
<!-- /snippet -->

**Complexity:** O(m·n) time for all three. Space: memo O(m·n) plus O(m + n) stack depth; table O(m·n); one row O(n) (loop over the shorter side for O(min(m, n))).

**Edge cases:**
- m = 1 or n = 1: one path; the base row or column covers it and the loops don't run.
- Overflow: counts only grow along rows and columns, so every cell ≤ the final answer ≤ 2·10⁹ < 2³¹ − 1. `int` is safe here only because of that guarantee.

**Follow-ups:**
- *Closed form?* A path is m−1 downs and n−1 rights in some order: C(m+n−2, m−1). Compute it incrementally in `long long` (res = res·(N−k+i)/i stays an exact integer at every step).
- *Obstacles (63)?* That's your practice problem. Decide how many paths reach a blocked cell, then check whether the "row 0 and column 0 are all 1" base case still holds.
- *Cheapest path instead of counting?* The same shape: min instead of +, plus the cell's cost.

## 3. Classic families — knapsack, LIS, LCS, edit distance, coin change, partitions

Most interview DPs belong to a handful of families. Recognise the family and you inherit its state, transition and loop order. The knapsack templates below live in [knapsack.cpp](examples/knapsack.cpp), stress-tested against exhaustive search; LIS, LCS and edit distance are taught through their worked examples at the end of the section.

| Family | State | Last decision | Loop order | Where |
|---|---|---|---|---|
| 0/1 knapsack, subset sum | best[i][c] → one row best[c] | skip item i−1, or take it once | items; capacity **downward** in one row | `knapsack.cpp`, 416 |
| Unbounded knapsack | best[c] | the last item added, reusable | capacity **upward** | `knapsack.cpp`, 322 |
| Counting totals | ways[t] | depends on what counts as "different" | the nesting decides | below |
| LIS | endingAt[i], or the tails array | the previous element, some j < i | i upward | 300 |
| LCS | lcs[i][j] over prefixes | pair the last characters, or drop one | row by row | 1143 |
| Edit distance | dist[i][j] over prefixes | replace / delete / insert at the end | row by row | 72 |
| Stock trading | value[day][state] | the arrow taken today | day by day | below |

### 0/1 knapsack: the 2D table

Items with weight w and value v, capacity W, each item at most once; maximise the value.
1. best[i][c] = max value using only the first i items with total weight ≤ c.
2. Item i−1 is skipped (best[i−1][c]) or taken once (best[i−1][c−w] + v, if w ≤ c).
3. best[0][c] = 0: no items, no value.
4. i upward; within a row any order (row i reads only row i−1).
5. best[n][W].

<!-- snippet: modules/16-dynamic-programming/examples/knapsack.cpp#knapsack_2d -->
```cpp
// 0/1 knapsack: each item at most once, total weight <= W, maximise the total value.
int knapsack01Table(const vector<int>& weight, const vector<int>& value, int W) {
    int n = weight.size();
    // best[i][c] = max value using only the first i items with total weight <= c
    vector<vector<int>> best(n + 1, vector<int>(W + 1, 0));       // row 0: no items -> 0
    for (int i = 1; i <= n; i++) {
        int w = weight[i - 1], v = value[i - 1];
        for (int c = 0; c <= W; c++) {
            best[i][c] = best[i - 1][c];                             // skip item i-1
            if (w <= c) best[i][c] = max(best[i][c], best[i - 1][c - w] + v);   // or take it, once
        }
    }
    return best[n][W];
}
```
<!-- /snippet -->

O(n·W) time and space. This is **pseudo-polynomial**: W is a number, not an input size. W up to ~10⁶ is fine; W = 10⁹ needs a different state (dp over value, or meet-in-the-middle for n ≤ 40).

### 0/1 knapsack in one row: why the loop runs downward

Row i reads row i−1 at c and at c − w, which is to the left. Keep one row and overwrite it in place: when you write best[c], best[c − w] must still hold the *previous row*. Going from W down to w guarantees it, because the cells left of c haven't been touched yet this round.

```text
processing item (w, v):   best[c] = max(best[c], best[c - w] + v)

downward, W → w:   [ old  old  old ... best[c-w] ... best[c] ← new  new ]   c-w is still row i-1: item used at most once
upward,   w → W:   [ new  new  new ... best[c-w] ... best[c] ← old  old ]   c-w may already hold item i: item reused
```

<!-- snippet: modules/16-dynamic-programming/examples/knapsack.cpp#knapsack_1d -->
```cpp
int knapsack01(const vector<int>& weight, const vector<int>& value, int W) {
    vector<int> best(W + 1, 0);                    // one row, overwritten in place: row i-1 becomes row i
    for (size_t i = 0; i < weight.size(); i++)
        for (int c = W; c >= weight[i]; c--)       // DOWNWARD: best[c - w] still holds row i-1 (item unused)
            best[c] = max(best[c], best[c - weight[i]] + value[i]);
    return best[W];
}
```
<!-- /snippet -->

Same O(n·W) time, O(W) memory.

### Unbounded knapsack: the same row, upward

Unlimited copies of each item. The one-row loop run **upward** is exactly right: best[c − w] may already include item i, which is "take item i again".

<!-- snippet: modules/16-dynamic-programming/examples/knapsack.cpp#unbounded -->
```cpp
int knapsackUnbounded(const vector<int>& weight, const vector<int>& value, int W) {
    vector<int> best(W + 1, 0);
    for (size_t i = 0; i < weight.size(); i++)
        for (int c = weight[i]; c <= W; c++)       // UPWARD: best[c - w] may already contain item i -> reuse
            best[c] = max(best[c], best[c - weight[i]] + value[i]);
    return best[W];
}
```
<!-- /snippet -->

The equivalent "last item" form: best[c] = max over items with w ≤ c of best[c − w] + v, with c upward. Coin Change (322) is this family with a min count instead of a max value.

### Counting: combinations vs ordered sequences

When a DP counts the ways to reach a total with repeatable items, the same update line can count two different things; the loop nesting decides which:
- **Items outer, total inner** → each multiset counted once (**combinations**, as 518 asks). Items are introduced in a fixed order, so {1, 2} can only be built as "the 1s, then the 2s".
- **Total outer, items inner** → the inner loop asks "which item comes *last*?", so 1+2 and 2+1 are different last steps (**ordered sequences**).

Coins {1, 2}, total 3: 2 combinations ({1,1,1}, {1,2}) but 3 sequences (1+1+1, 1+2, 2+1). Hand-trace both nestings on this example before you solve 518. For min or max (322) the nesting doesn't matter: the best over orders equals the best over multisets.

Counts grow fast, and the cells for intermediate totals can exceed the final answer: count in `long long` (or `unsigned long long`), or apply the given modulus after every addition.

### Subset sum and partition

A 0/1 knapsack where the value is yes / no: reachable[s] = some subset of the items so far sums to exactly s. Downward loop, as above. Equal partition (416) asks whether total/2 is reachable; "± signs" (494, which counts subsets instead of asking yes / no) and "split into two piles as evenly as possible" (1049) reduce to the same table of sums. `std::bitset` does a whole row in one line, `reach |= reach << x`, 64 sums per machine word: O(n·S/64) (416's follow-up).

### Palindromic subsequences

The problem list's hint for 516 names two routes: LCS against the reversed string, or an interval state over s[l..r] that decides about the two ends, filled by increasing length (the section 4 order). Before coding the first, convince yourself *why* a common subsequence of s and its reverse can be taken to be a palindrome. Palindromic *substrings* (contiguous) need a different tool: expand around centres (module 04) or a boolean isPal[l][r] table (132 needs one).

### Stock trading as a state machine

Model each day as a move between a few states; each state's value on day i = the max over its incoming arrows of (the source state's value on day i−1 + the arrow's cash).

```text
                 buy: −price
    ┌──────┐  ───────────────▶  ┌─────────┐
    │ free │                    │ holding │
    └──────┘  ◀───────────────  └─────────┘
     rest ↺     sell: +price       ↺ rest
```

Each arrow becomes one term of a max, so each state becomes one line of code, computed from *yesterday's* values of all states. Then run the recipe: which states make sense before the first day (base cases), and which state is the answer on the last one? Variants change the machine, not the method: a fee is a cost on one arrow, a cooldown is an extra state, and a cap on transactions adds a counter to the state. Draw the machine first, then write one line per state; 714 and 188 are the practice problems for this.

### Pitfalls

- **Knapsack loop direction.** The 1D 0/1 loop must go downward; upward silently becomes unbounded (and still passes samples where no item fits twice).
- **2D tables that don't fit.** 200 × 10⁴ ints is fine; 10³ × 10⁵ = 10⁸ ints = 400 MB is not: use the 1D row.
- **INF + 1.** With INF = INT_MAX, `fewest[a - c] + 1` overflows (UB); use amount + 1.
- **Counting.** The wrong nesting (combinations vs sequences); intermediate overflow; a modulus forgotten on one of the additions.
- **LIS.** `lower_bound` (strict) vs `upper_bound` (non-decreasing); returning tails as if it were the subsequence.
- **String tables.** Tables are indexed by prefix length, strings by position: compare a[i−1] with b[j−1]. Edit distance's row and column 0 are i and j, not 0.
- **Recursive brute forces taking `string` by value** copy both strings on every call; use `const string&`.

### Recognize it when…

- "Each item at most once", "a subset with sum …", "split into two groups with equal sum / minimum difference", "assign + or −" → 0/1 knapsack / subset sum (416, 494, 1049).
- "Unlimited supply", "coins", "fewest to make exactly …" → unbounded knapsack (322, 518).
- "Number of combinations" vs "number of orderings" → the loop nesting (518).
- "Longest increasing / chain / nested", possibly after sorting; "how many longest" → LIS, LIS with counts (300, 673).
- Two strings with "common subsequence", "common substring", "minimum edits", "pattern with ? and *" → a two-sequence table (1143, 718, 72, 44).
- "Buy and sell" with a fee, a cooldown or at most k trades → state machine (714, 188).
- "Palindromic subsequence" → LCS with the reverse, or interval DP (516).

### Worked example: 416. Partition Equal Subset Sum
[LeetCode 416](https://leetcode.com/problems/partition-equal-subset-sum/) · Medium

**Problem (paraphrased):** Given positive integers, decide whether they can be split into two groups with equal sums.

**Signals:** "split into two subsets", "equal sum" → subset sum. n ≤ 200 and values ≤ 100, so the total is ≤ 20 000: small enough to be a table dimension.

**Brute force, and why it fails:** try all 2ⁿ subsets: 2²⁰⁰.

**Key insight:** each half must sum to total/2, so the question is "does some subset sum to total/2?" That's a 0/1 knapsack over sums with booleans: O(n · total/2) ≤ 200 · 10⁴ = 2·10⁶.

**Recipe:** reachable[s] = some subset of the items processed so far sums to s · item x unused (reachable[s] stays) or used once (reachable[s − x]) · reachable[0] = true (the empty subset) · items one by one, s downward · reachable[total/2]; an odd total is false immediately.

**Dry run:** nums = [1, 5, 11, 5], total 22, target 11

| after item | reachable sums ≤ 11 |
|---|---|
| (start) | 0 |
| 1 | 0, 1 |
| 5 | 0, 1, 5, 6 |
| 11 | 0, 1, 5, 6, **11** |
| 5 | 0, 1, 5, 6, 10, 11 |

Why downward matters: on [1, 2, 5] (target 4, answer false), an upward loop for the item 1 marks 1 from 0, then 2 from 1, 3 from 2 and 4 from 3, "using" the single 1 four times, and returns true.

<!-- snippet: modules/16-dynamic-programming/examples/0416-partition-equal-subset-sum.cpp#solution -->
```cpp
class Solution {
public:
    bool canPartition(vector<int>& nums) {
        int total = accumulate(nums.begin(), nums.end(), 0);
        if (total % 2 != 0) return false;            // an odd total can't split into two equal halves
        int target = total / 2;
        vector<bool> reachable(target + 1, false);   // reachable[s]: some subset of the items so far sums to s
        reachable[0] = true;                         // the empty subset
        for (int x : nums)
            for (int s = target; s >= x; s--)        // downward, so each item is used at most once
                if (reachable[s - x]) reachable[s] = true;
        return reachable[target];
    }
};
```
<!-- /snippet -->

**Complexity:** O(n·S) time with S = total/2 ≤ 10⁴; O(S) space.

**Edge cases:**
- An odd total → false before any DP.
- A single element → false (the other half would be empty).
- An element larger than total/2: its inner loop never runs, correctly, since it can't fit in a half.

**Follow-ups:**
- *Faster?* A bitset row: bit s set ⇔ sum s reachable, and one line per item, `reach |= reach << x`, updates 64 sums per machine word: O(n·S/64). A `bitset` needs a compile-time size, so take it from the constraints (`bitset<10001>`); `bitset_version::Solution` in the example file is tested alongside the main one.
- *Return the two groups?* Keep the 2D table (or, for each sum, the item that first reached it) and walk back from the target.
- *k equal groups?* The state would need every group's running sum: that's the backtracking / bitmask problem 698 from module 10.

### Worked example: 322. Coin Change
[LeetCode 322](https://leetcode.com/problems/coin-change/) · Medium

**Problem (paraphrased):** Given coin denominations with unlimited supply and an amount, return the fewest coins that sum exactly to the amount, or −1 if it can't be done.

**Signals:** "fewest", "an infinite number of each kind" → unbounded knapsack (min). amount ≤ 10⁴, at most 12 coins.

**Brute force, and why it fails:** recurse on "which coin is last" without a cache: up to 12 branches, depth up to the amount, so exponential. Greedy (largest coin first) is fast and wrong: coins {1, 3, 4}, amount 6 → 4+1+1, but 3+3 is better.

**Key insight:** the fewest coins for amount a depend only on a, and the last coin c leaves a − c. So fewest[a] = 1 + min over coins c ≤ a of fewest[a − c]: amount + 1 states × 12 choices.

**Recipe:** fewest[a] = fewest coins summing to exactly a · last coin c → fewest[a − c] + 1 · fewest[0] = 0, everything else starts at INF · a upward · fewest[amount], or −1 if it's still INF.

**Dry run:** coins [1, 2, 5], amount 11

| a | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| fewest[a] | 0 | 1 | 1 | 2 | 2 | 1 | 2 | 2 | 3 | 3 | 2 | **3** |

fewest[11] = 1 + min(fewest[10], fewest[9], fewest[6]) = 1 + 2 = 3 (5 + 5 + 1).

<!-- snippet: modules/16-dynamic-programming/examples/0322-coin-change.cpp#solution -->
```cpp
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        const int INF = amount + 1;                  // more coins than any real answer, and INF + 1 can't overflow
        vector<int> fewest(amount + 1, INF);         // fewest[a] = fewest coins summing to exactly a
        fewest[0] = 0;
        for (int a = 1; a <= amount; a++)            // upward: every fewest[a - c] is final before fewest[a]
            for (int c : coins)
                if (c <= a) fewest[a] = min(fewest[a], fewest[a - c] + 1);   // c is the last coin
        return fewest[amount] == INF ? -1 : fewest[amount];
    }
};
```
<!-- /snippet -->

**Complexity:** O(amount · k) time for k coins, O(amount) space.

**Edge cases:**
- amount = 0 → 0: the base case, no iterations.
- Unreachable (coins [2], amount 3) → stays INF → −1.
- A huge coin (2³¹ − 1): `c <= a` skips it, and nothing ever computes a − c < 0.
- INF = amount + 1, not INT_MAX, so fewest[a − c] + 1 can't overflow.

**Follow-ups:**
- *Coins in the outer loop?* With an upward amount loop it gives the same minimum; for *counting*, the nesting matters (combinations vs sequences, above).
- *Which coins?* Record the last coin whenever fewest[a] improves, then walk back from the amount.
- *As a graph?* Amounts are nodes and coins are edges of weight 1: the fewest coins is the BFS distance from 0 to the amount (module 14). Same O(amount · k), and it can stop as soon as it reaches the amount.

### Worked example: 300. Longest Increasing Subsequence
[LeetCode 300](https://leetcode.com/problems/longest-increasing-subsequence/) · Medium

**Problem (paraphrased):** Return the length of the longest strictly increasing subsequence (elements keep their order but needn't be adjacent).

**Signals:** "subsequence", "strictly increasing", "longest". n ≤ 2500 allows O(n²); the follow-up asks for O(n log n).

**Brute force, and why it fails:** choose or skip every element while remembering the last value taken: 2ⁿ = 2²⁵⁰⁰ subsequences.

**Key insight:** the future only needs the last value taken, so the state is "ending at i": O(n²). Sharper: for each length only the smallest possible tail matters (anything that extends a larger tail also extends a smaller one), and those tails are sorted, so each new element is placed by binary search: O(n log n).

**Recipe (O(n²)):** endingAt[i] = LIS ending exactly at i · the element before nums[i] is some nums[j] < nums[i] with j < i → endingAt[j] + 1, or there's none → 1 · everything starts at 1 · i upward · the max over all i.

**Dry run:** nums = [10, 9, 2, 5, 3, 7, 101, 18]

| x | 10 | 9 | 2 | 5 | 3 | 7 | 101 | 18 |
|---|---|---|---|---|---|---|---|---|
| endingAt | 1 | 1 | 1 | 2 | 2 | 3 | 4 | 4 |
| tails after x | [10] | [9] | [2] | [2,5] | [2,3] | [2,3,7] | [2,3,7,101] | [2,3,7,18] |

Answer 4. On [3, 4, 1] tails ends as [1, 4]: the right length, but not a subsequence. tails is bookkeeping, not an answer.

O(n²), the recipe verbatim:

<!-- snippet: modules/16-dynamic-programming/examples/0300-longest-increasing-subsequence.cpp#quadratic -->
```cpp
class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        int n = nums.size();
        vector<int> endingAt(n, 1);          // endingAt[i] = longest strictly increasing subsequence ending at i
        for (int i = 0; i < n; i++)
            for (int j = 0; j < i; j++)      // the element before nums[i] is some smaller nums[j], j < i
                if (nums[j] < nums[i]) endingAt[i] = max(endingAt[i], endingAt[j] + 1);
        return *max_element(endingAt.begin(), endingAt.end());   // the LIS may end anywhere
    }
};
```
<!-- /snippet -->

O(n log n):

<!-- snippet: modules/16-dynamic-programming/examples/0300-longest-increasing-subsequence.cpp#solution -->
```cpp
class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        vector<int> tails;      // tails[k] = smallest tail of any increasing subsequence of length k+1 so far
        for (int x : nums) {
            auto it = lower_bound(tails.begin(), tails.end(), x);   // first tail >= x
            if (it == tails.end()) tails.push_back(x);               // x extends the longest one
            else *it = x;                                            // x: a smaller tail for that length
        }
        return tails.size();
    }
};
```
<!-- /snippet -->

Reconstruction: keep indices in tails and a parent pointer for each element.

<!-- snippet: modules/16-dynamic-programming/examples/0300-longest-increasing-subsequence.cpp#reconstruct -->
```cpp
// One longest strictly increasing subsequence (its values), in O(n log n).
vector<int> longestIncreasingSubsequence(const vector<int>& nums) {
    vector<int> tailIdx;                     // tailIdx[k] = index of the smallest tail for length k+1
    vector<int> parent(nums.size(), -1);     // parent[i] = the index before i in its subsequence
    for (int i = 0; i < (int)nums.size(); i++) {
        int k = lower_bound(tailIdx.begin(), tailIdx.end(), nums[i],
                            [&](int idx, int value) { return nums[idx] < value; }) - tailIdx.begin();
        if (k > 0) parent[i] = tailIdx[k - 1];           // extend the best subsequence of length k
        if (k == (int)tailIdx.size()) tailIdx.push_back(i);
        else tailIdx[k] = i;
    }
    vector<int> lis;
    for (int i = tailIdx.empty() ? -1 : tailIdx.back(); i != -1; i = parent[i]) lis.push_back(nums[i]);
    reverse(lis.begin(), lis.end());
    return lis;
}
```
<!-- /snippet -->

**Complexity:** O(n²) or O(n log n) time; O(n) space.

**Edge cases:**
- All equal ([7, 7, 7]) → 1: `lower_bound` finds the equal tail and overwrites it, so equal values never extend a run.
- Strictly decreasing → 1; strictly increasing → n; negative values need nothing special.

**Follow-ups:**
- *Non-decreasing?* `upper_bound` instead of `lower_bound`.
- *How many LIS are there (673)?* Keep a (length, count) pair per index in the O(n²) version, as the list's hint says; the care is in how counts combine when two j give the same length.
- *Pairs, like (width, height) boxes that nest?* Sort by one coordinate, then run LIS on the other; ties in the sorted coordinate need thought, since equal widths can't nest.

### Worked example: 1143. Longest Common Subsequence
[LeetCode 1143](https://leetcode.com/problems/longest-common-subsequence/) · Medium

**Problem (paraphrased):** Return the length of the longest sequence of characters that appears, in order but not necessarily contiguously, in both strings.

**Signals:** two strings, "common subsequence". Lengths ≤ 1000 → O(n·m) = 10⁶.

**Brute force, and why it fails:** every subsequence of one string (2ⁿ), each checked against the other in O(m): 2¹⁰⁰⁰.

**Key insight:** compare the last characters. Equal: some LCS pairs them, so take the diagonal + 1. Different: at least one of the two isn't in the LCS, so drop one side or the other and keep the better.

**Recipe:** lcs[i][j] over prefixes a[0..i), b[0..j) · match → lcs[i−1][j−1] + 1, else max(lcs[i−1][j], lcs[i][j−1]) · row 0 and column 0 are 0 · row by row · lcs[n][m]. Why the match case is safe: take an LCS that doesn't end with the pair (a[i−1], b[j−1]). If it uses neither character, append the pair: longer, a contradiction. If it uses a[i−1] matched to an earlier b[k], rematch it to b[j−1] (same character, still in order): same length, now ending with the pair. The case where it uses only b[j−1] is symmetric.

**Dry run:** a = "abcde", b = "ace", then walk back from the bottom-right cell:

```text
        ""  a  c  e
   ""    0  0  0  0
   a     0  1  1  1        (5,3) e = e → take 'e', go to (4,2)
   b     0  1  1  1        (4,2) d ≠ c, up 2 ≥ left 1 → (3,2)
   c     0  1  2  2        (3,2) c = c → take 'c', go to (2,1)
   d     0  1  2  2        (2,1) b ≠ a, up 1 ≥ left 0 → (1,1)
   e     0  1  2  3        (1,1) a = a → take 'a'   → "ace", length 3
```

<!-- snippet: modules/16-dynamic-programming/examples/1143-longest-common-subsequence.cpp#solution -->
```cpp
class Solution {
public:
    int longestCommonSubsequence(string text1, string text2) {
        int n = text1.size(), m = text2.size();
        // lcs[i][j] = LCS length of the prefixes text1[0..i) and text2[0..j); row 0 / column 0: empty prefix
        vector<vector<int>> lcs(n + 1, vector<int>(m + 1, 0));
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= m; j++)
                if (text1[i - 1] == text2[j - 1])
                    lcs[i][j] = lcs[i - 1][j - 1] + 1;                 // last chars match: pair them up
                else
                    lcs[i][j] = max(lcs[i - 1][j], lcs[i][j - 1]);     // one of the two last chars is unused
        return lcs[n][m];
    }
};
```
<!-- /snippet -->

Reconstruction, by walking the full table back:

<!-- snippet: modules/16-dynamic-programming/examples/1143-longest-common-subsequence.cpp#reconstruct -->
```cpp
// One LCS as a string: fill the same table, then walk back from (n, m).
string lcsString(const string& a, const string& b) {
    int n = a.size(), m = b.size();
    vector<vector<int>> lcs(n + 1, vector<int>(m + 1, 0));
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= m; j++)
            lcs[i][j] = a[i - 1] == b[j - 1] ? lcs[i - 1][j - 1] + 1 : max(lcs[i - 1][j], lcs[i][j - 1]);
    string out;
    for (int i = n, j = m; i > 0 && j > 0;) {
        if (a[i - 1] == b[j - 1]) { out += a[i - 1]; i--; j--; }    // this pair is in the LCS
        else if (lcs[i - 1][j] >= lcs[i][j - 1]) i--;               // go where the value came from
        else j--;
    }
    reverse(out.begin(), out.end());
    return out;
}
```
<!-- /snippet -->

**Complexity:** O(n·m) time; O(n·m) space with the table, which reconstruction needs. For the length alone, keep one row over the shorter string plus a variable that saves the old diagonal before it's overwritten: O(min(n, m)) memory (`rolling::Solution` in the example file, the section 2 rolling pattern).

**Edge cases:** no common character → 0; identical strings → n; an empty string → 0 via row or column 0 (outside the constraints, handled anyway).

**Follow-ups:**
- *Shortest common supersequence?* Its length is n + m − LCS; build it with the same walk back, emitting the unmatched characters too.
- *Fewest deletions to make the strings equal?* n + m − 2·LCS.
- *Longest common substring, contiguous (718)?* That's your practice problem: work out what a mismatch does to a *contiguous* match, and where the answer ends up.

### Worked example: 72. Edit Distance
[LeetCode 72](https://leetcode.com/problems/edit-distance/) · Medium

**Problem (paraphrased):** Return the fewest single-character inserts, deletes and replacements that turn word1 into word2.

**Signals:** two strings, "minimum number of operations", three edit types. Lengths ≤ 500 → O(n·m) = 2.5·10⁵.

**Brute force, and why it fails:** try every operation at every position: about three branches per step and up to n + m steps, 3¹⁰⁰⁰.

**Key insight:** fix the strings from the end. Equal last characters can simply stay. Otherwise the last operation fixed the ends in one of three ways, each leaving a smaller pair of prefixes: replace (i−1, j−1), delete a[i−1] (i−1, j), insert b[j−1] (i, j−1).

**Recipe:** dist[i][j] = edits turning a[0..i) into b[0..j) · equal ends → dist[i−1][j−1], else 1 + min(the three neighbours) · dist[i][0] = i, dist[0][j] = j · row by row · dist[n][m].

**Dry run:** "horse" → "ros"

```text
        ""  r  o  s
   ""    0  1  2  3
   h     1  1  2  3
   o     2  2  1  2
   r     3  2  2  2
   s     4  3  3  2
   e     5  4  4  3    → 3: replace h by r, delete r, delete e
```

<!-- snippet: modules/16-dynamic-programming/examples/0072-edit-distance.cpp#solution -->
```cpp
class Solution {
public:
    int minDistance(string word1, string word2) {
        int n = word1.size(), m = word2.size();
        // dist[i][j] = fewest edits turning word1[0..i) into word2[0..j)
        vector<vector<int>> dist(n + 1, vector<int>(m + 1));
        for (int i = 0; i <= n; i++) dist[i][0] = i;       // delete all i chars
        for (int j = 0; j <= m; j++) dist[0][j] = j;       // insert all j chars
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= m; j++) {
                if (word1[i - 1] == word2[j - 1])
                    dist[i][j] = dist[i - 1][j - 1];       // the ends already agree: no edit
                else
                    dist[i][j] = 1 + min({dist[i - 1][j - 1],   // replace word1[i-1] by word2[j-1]
                                          dist[i - 1][j],       // delete word1[i-1]
                                          dist[i][j - 1]});     // insert word2[j-1] at the end
            }
        return dist[n][m];
    }
};
```
<!-- /snippet -->

**Complexity:** O(n·m) time and space; O(min(n, m)) space with one row and a saved diagonal, exactly as in 1143.

**Edge cases:** an empty string → the other one's length (the base row or column; LeetCode allows length 0 here); equal strings → 0; "ab" → "ba" is 2, since there's no swap operation.

**Follow-ups:**
- *Why is it safe to skip the min when the ends match?* Adding one character to either string changes the distance by at most 1, so dist[i−1][j−1] ≤ 1 + dist[i−1][j] and ≤ 1 + dist[i][j−1]: the diagonal already is the minimum.
- *Print the operations?* Walk back from (n, m), each time moving to the neighbour that produced the value.
- *Different costs per operation?* Replace each 1 by that operation's cost; nothing else changes.

**How it's tested:** the stress test compares against a BFS over strings (one edit per edge), which is the definition of edit distance implemented without any cleverness. A good brute force is exactly that: the definition, done stupidly.

## 4. Harder shapes — interval DP, DP on trees, bitmask DP, digit DP

The same recipe with less obvious states: a contiguous range, a subtree, a subset, a prefix of digits.

### Interval DP

State dp[l][r] = the answer for the contiguous piece a[l..r]. Transition: choose the split point, or which operation happens first or last in the range, and combine two smaller intervals. Order: by increasing length, so both halves are ready. Usually O(n²) states × O(n) splits = O(n³): n ≤ ~300–500.

**The "last operation" trick (312).** When an operation changes the neighbourhood (bursting a balloon makes its neighbours adjacent), splitting on the *first* operation leaves two sides that interact later. Split on the *last* balloon k burst inside the open interval (l, r) instead: until k goes it acts as a wall, so (l, k) and (k, r) are independent; and when k finally bursts, its neighbours are exactly l and r.

### DP on trees

Root the tree; a node's answer depends only on its children's answers, so compute in post-order. When the parent's choice constrains the child (rob the parent ⇒ can't rob the child), return **one value per state of the node**: a pair or tuple such as {best if this node is taken, best if not}. O(n) time, O(height) stack. Without the tuple you either recurse into grandchildren from every node (exponential) or memoise on `TreeNode*` in a hash map (correct, slower).

**Rerooting** (an answer for every node as the root, e.g. each node's sum of distances to all others): root the tree once and compute down[v], the answer inside v's subtree, in post-order. Then a pre-order pass hands each child c the contribution of everything *outside* c's subtree: its parent's full answer minus c's own contribution, or a prefix / suffix combination of the siblings when "minus" doesn't exist (max, min). Moving the root across one edge costs O(1), so all n answers cost O(n) instead of O(n²).

### Bitmask DP

When n ≤ ~20 and the state must remember *which* elements are used, store the set as an int: bit i set ⇔ element i used. 2²⁰ ≈ 10⁶ states.

| Idiom | Meaning |
|---|---|
| `mask >> i & 1` | is i in the set? |
| `mask \| 1 << i` | add i |
| `mask & ~(1 << i)` | remove i |
| `(1 << n) - 1` | all n elements |
| `__builtin_popcount(mask)` | the set's size |
| `for (int s = mask; s > 0; s = (s - 1) & mask)` | every non-empty subset of mask (3ⁿ steps over all masks) |

Numeric order is a valid evaluation order: adding an element only makes the number bigger.

**TSP (Held–Karp).** best[mask][last] = the shortest path that starts at city 0, visits exactly the cities in mask and ends at last. Extend by one unvisited city; close the tour back to 0 at the end. O(2ⁿ·n²) instead of O(n!): for n = 16, 1.7·10⁷ steps vs 15! ≈ 1.3·10¹². The same (visited set, last) state fits any "order or visit every item exactly once" problem with a cost between consecutive items.

<!-- snippet: modules/16-dynamic-programming/examples/tsp_held_karp.cpp#tsp -->
```cpp
// Shortest tour that starts at city 0, visits every city exactly once and returns to 0.
// dist is an n x n matrix (it may be asymmetric); n <= ~16.
long long shortestTour(const vector<vector<int>>& dist) {
    int n = dist.size();
    if (n == 1) return 0;
    const long long INF = LLONG_MAX / 4;
    // best[mask][last] = shortest path from 0 through exactly the cities in mask, ending at last
    vector<vector<long long>> best(1 << n, vector<long long>(n, INF));
    best[1][0] = 0;                                   // visited {0}, standing on 0
    for (int mask = 1; mask < (1 << n); mask++)       // supersets come later numerically
        for (int last = 0; last < n; last++) {
            if (best[mask][last] == INF) continue;    // unreachable (e.g. mask without city 0)
            for (int next = 0; next < n; next++) {
                if (mask >> next & 1) continue;       // already visited
                int grown = mask | 1 << next;
                best[grown][next] = min(best[grown][next], best[mask][last] + dist[last][next]);
            }
        }
    long long tour = INF;
    for (int last = 1; last < n; last++)              // close the cycle back to city 0
        tour = min(tour, best[(1 << n) - 1][last] + dist[last][0]);
    return tour;
}
```
<!-- /snippet -->

### Digit DP

"Count the integers in [0, N] whose digits satisfy P", with N up to 10⁹–10¹⁸; a range is [L, R] = f(R) − f(L − 1). Build the number from the most significant digit. The state:
- **pos:** which digit you're placing.
- **tight:** the digits so far equal N's prefix, so this digit can only go up to N[pos]; otherwise 0–9. Place a smaller digit once and tight stays false forever.
- **started:** a non-zero digit has been placed. Leading zeros aren't real digits: they mustn't count as "0 used", or as the number's first digit.
- **whatever P needs:** a digit sum mod k, the previous digit, a used-digit mask, a count.

Memoise on the whole state (pos, tight, started, extra). Only one path is ever tight, so the memo pays off on the non-tight states. Cost = states × 10.

### DP + binary search: weighted interval scheduling

Intervals with weights; choose non-overlapping ones with maximum total weight. Greedy by end time (module 11's activity selection) maximises the *count*, not the weight. The recipe:
1. Sort by end time. best[i] = max weight using only the first i intervals.
2. Last decision: interval i−1 is out (best[i−1]) or in (its weight w, plus the best over the intervals that end before it starts).
3. Because ends are sorted, the intervals that end before interval i−1 starts form a *prefix* of the order; a binary search on the end times finds its length j. So best[i] = max(best[i−1], w + best[j]).
4. O(n log n) overall. Decide whether touching endpoints overlap (`upper_bound` vs `lower_bound`).

The same "sort, prefix DP, binary search for the last compatible item" shape covers job scheduling with profits and choosing non-overlapping events.

### Pitfalls

- **Interval order.** l and r both upward reads dp[k][r] before it exists: loop by length (or l downward, r upward).
- **Interval bounds.** Pick closed [l, r] or open (l, r) (312 uses open, with padding) and keep every loop consistent.
- **A tree DP returning one number** when the parent needs two: wrong answers or exponential time.
- **`1 << n` on an `int`:** n = 31 already gives a negative number, and n ≥ 32 is UB. Use `1LL << n` for wider masks (bitmask DP stays at n ≤ ~20 anyway).
- **Precedence:** `mask & 1 << i == 0` parses as `mask & ((1 << i) == 0)`. Write `!(mask >> i & 1)`.
- **Memory:** best[1 << 20][20] of `long long` is 160 MB: use `int`, or drop a dimension.
- **Digit DP:** counting leading zeros as digits; forgetting whether 0 itself is in range; reusing the memo for a different N without resetting it (pos counts from N's first digit).

### Recognize it when…

- **Interval:** "burst / merge / remove / cut, and the cost depends on the current neighbours", "the best way to parenthesise / triangulate", n ≤ 500 (312, 1039).
- **Partition a string into pieces with a property:** "fewest cuts so every piece is a palindrome" → a precomputed isPal[l][r] table plus a DP over the cut positions (132).
- **Trees:** a tree plus "no parent and child both chosen", "cover every node", or "for every node compute …" (rerooting) (337).
- **Bitmask:** n ≤ 15–20 with "assign each task / person / city exactly once", "visit all", "fewest groups / sessions" (1986, TSP).
- **Digit:** "count the integers in [1, N] or [L, R] whose digits …", N up to 10⁹–10¹⁸ (2376).
- **DP + binary search:** weighted intervals, "non-overlapping", "maximum profit from jobs".

### Worked example: 312. Burst Balloons
[LeetCode 312](https://leetcode.com/problems/burst-balloons/) · Hard

**Problem (paraphrased):** Balloons in a row carry numbers. Bursting one earns (left neighbour) × (its number) × (right neighbour), counting a missing neighbour as 1; afterwards its two neighbours become adjacent. Burst them all for the most coins.

**Signals:** the neighbours change after every removal; n ≤ 300 → interval DP, O(n³) ≈ 4.5·10⁶ steps.

**Brute force, and why it fails:** try every burst order: n! = 300!.

**Key insight:** choose the balloon that bursts *last* inside an open interval (l, r). While the others burst it separates the two sides, so they're independent, and when it bursts its neighbours are exactly l and r. Padding with a 1 at each end turns "no neighbour" into a real index.

**Recipe:** best[l][r] = the most coins from bursting everything strictly between l and r (padded indices) · last balloon k → best[l][k] + val[l]·val[k]·val[r] + best[k][r] · best[l][l+1] = 0 (nothing inside) · by increasing r − l · best[0][n+1].

**Dry run:** nums = [3, 1, 5, 8] → val = [1, 3, 1, 5, 8, 1]
- best[1][3] (just the 1, between 3 and 5) = 3·1·5 = 15; best[2][4] (just the 5, between 1 and 8) = 1·5·8 = 40.
- best[1][4] (the 1 and the 5, between 3 and 8): 5 last → 15 + 3·5·8 + 0 = 135; 1 last → 0 + 3·1·8 + 40 = 64. So 135.
- best[0][4] (3, 1, 5, between the left pad and 8): 3 last → 0 + 1·3·8 + 135 = 159, the best choice.
- best[0][5] (everything): 8 last → 159 + 1·8·1 + 0 = **167**. As an order: burst 1, 5, 3, 8 → 15 + 120 + 24 + 8.

<!-- snippet: modules/16-dynamic-programming/examples/0312-burst-balloons.cpp#solution -->
```cpp
class Solution {
public:
    int maxCoins(vector<int>& nums) {
        int n = nums.size();
        vector<int> val(n + 2, 1);                   // pad with a virtual 1-balloon at each end
        for (int i = 0; i < n; i++) val[i + 1] = nums[i];
        // best[l][r] = most coins from bursting every balloon strictly between l and r (l and r stay)
        vector<vector<int>> best(n + 2, vector<int>(n + 2, 0));   // r = l + 1: nothing inside -> 0
        for (int len = 2; len <= n + 1; len++)       // by increasing gap r - l: inner intervals are final
            for (int l = 0; l + len <= n + 1; l++) {
                int r = l + len;
                for (int k = l + 1; k < r; k++)      // k = the LAST balloon burst inside (l, r)
                    best[l][r] = max(best[l][r], best[l][k] + val[l] * val[k] * val[r] + best[k][r]);
            }
        return best[0][n + 1];
    }
};
```
<!-- /snippet -->

**Complexity:** O(n³) time (about n³/6 ≈ 4.5·10⁶ inner steps for n = 300), O(n²) space.

**Edge cases:**
- One balloon → 1·x·1 = x.
- Zeros earn nothing but still separate their neighbours; the recurrence needs no special case.
- Overflow: each burst ≤ 100³ = 10⁶, so the total ≤ 3·10⁸ < 2³¹.

**Follow-ups:**
- *Why not split on the first balloon burst?* After it bursts its two neighbours become adjacent, so the left and right parts share a boundary that keeps changing: they aren't independent.
- *Top-down?* memo[l][r] with the same transition; the loop-order question disappears.
- *The same skeleton elsewhere?* 1039, your practice problem: an interval over polygon vertices, choosing the triangle on edge (i, j), as the list's hint says.

### Worked example: 337. House Robber III
[LeetCode 337](https://leetcode.com/problems/house-robber-iii/) · Medium

**Problem (paraphrased):** The houses form a binary tree; robbing two directly connected houses (a parent and its child) sets off the alarm. Return the most money you can take.

**Signals:** a binary tree plus "no two directly linked" → tree DP. Up to 10⁴ nodes.

**Brute force, and why it fails:** at each node, rob it (and jump to the grandchildren) or skip it (and go to the children). The grandchildren are solved from both branches, again and again: exponential.

**Key insight:** return two numbers from each subtree: the best if its root is robbed and the best if it isn't. That's exactly what the parent needs: robbing the parent needs the children's "skipped" values; skipping it lets each child take its better value.

**Recipe:** state (rob, skip) for v's subtree · rob = v.val + skip(L) + skip(R); skip = max(rob(L), skip(L)) + max(rob(R), skip(R)) · a null child is (0, 0) · post-order · max(rob, skip) at the root.

**Dry run:** [3,2,3,null,3,null,1]

```text
         3              leaf 3  → (3, 0)          leaf 1 → (1, 0)
        / \             node 2  → (2 + 0, max(3, 0))      = (2, 3)
       2   3            node 3  → (3 + 0, max(1, 0))      = (3, 1)
        \   \           root 3  → (3 + 3 + 1, max(2, 3) + max(3, 1)) = (7, 6)  → 7
         3   1
```

<!-- snippet: modules/16-dynamic-programming/examples/0337-house-robber-iii.cpp#solution -->
```cpp
class Solution {
    // For the subtree rooted at node: {best if node is robbed, best if node is skipped}.
    pair<int, int> dfs(TreeNode* node) {
        if (!node) return {0, 0};
        auto [robLeft, skipLeft] = dfs(node->left);       // post-order: children first
        auto [robRight, skipRight] = dfs(node->right);
        int robThis = node->val + skipLeft + skipRight;   // robbing node rules out both children
        int skipThis = max(robLeft, skipLeft) + max(robRight, skipRight);   // children choose freely
        return {robThis, skipThis};
    }

public:
    int rob(TreeNode* root) {
        auto [robRoot, skipRoot] = dfs(root);
        return max(robRoot, skipRoot);
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time (each node once), O(h) stack: h ≤ 10⁴ even for a chain, which is fine.

**Edge cases:** a single node → its value; a chain → 198 on a path; zeros everywhere → 0; the best set may skip two levels in a row (the tests include one).

**Follow-ups:**
- *Memoise instead?* `unordered_map<TreeNode*, int>` over "best from this node" is O(n) too, but it hashes on every call; the pair is cleaner.
- *Three states instead of two?* Covering problems (place cameras so every node is watched) return {has a camera, covered, not covered} per node: the same post-order shape.
- *The answer for every node as root?* Rerooting (above).

### Worked example: 1986. Minimum Number of Work Sessions to Finish the Tasks
[LeetCode 1986](https://leetcode.com/problems/minimum-number-of-work-sessions-to-finish-the-tasks/) · Medium

**Problem (paraphrased):** Tasks take integer times; a work session lasts at most sessionTime; each task must be finished inside one session, in any order. Return the fewest sessions.

**Signals:** n ≤ 14 (tiny), "any order", "minimum number of sessions" → bitmask DP over the set of finished tasks. It's bin packing, NP-hard in general, hence the tiny n.

**Brute force, and why it fails:** try every split of the tasks into sessions: Bell(14) ≈ 1.9·10⁸ set partitions, each to check (and all n! orders is worse).

**Key insight:** finish tasks one at a time with a "current session": the next task joins it if it fits, otherwise it opens a new one. For a given set of finished tasks, only two numbers matter for the future: the sessions opened and the minutes used in the current one. A smaller pair is never worse, so best[mask] = the smallest (sessions, used), compared lexicographically: sessions first, minutes only on a tie (exactly what `std::pair`'s `<` does).

**Why one pair per mask is enough:**
1. *Some order reaches the optimum.* List an optimal split session by session; on that task order, "join if it fits, else open" never uses more sessions than the split.
2. *Smaller pairs stay smaller.* Add the same task to two pairs, the first smaller: the result from the first is still no larger. (Fewer sessions stay ahead, or at worst tie with fewer minutes used; equal sessions with fewer minutes used fit at least as much.)

So the smallest pair for a mask comes from the smallest pairs of the masks one task smaller, and best[all] is the best over every order. Compare the *whole* pair: keeping only the session count fails on tasks [4, 3, 4, 7, 1] with sessionTime 10 (it returns 3; the answer is 2: {3, 7} and {4, 4, 1}).

**Recipe:** best[mask] as above · add a task i not in mask: it fits → (s, x + tᵢ), else (s + 1, tᵢ) · best[0] = (0, sessionTime), a "full" dummy so the first task opens session 1 · mask upward · best[all].first.

**Dry run:** tasks [1, 2, 3], sessionTime 3 (mask bits: task 2, task 1, task 0)

| mask | tasks done | best (sessions, used) | from |
|---|---|---|---|
| 000 | – | (0, 3) | base |
| 001 | 1 | (1, 1) | 000 + task 0, which opens a session |
| 010 | 2 | (1, 2) | 000 + task 1 |
| 011 | 1, 2 | (1, 3) | 001 + task 1, which fits |
| 100 | 3 | (1, 3) | 000 + task 2 |
| 101 | 1, 3 | (2, 1) | 100 + task 0 opens a session; beats 001 + task 2 → (2, 3) |
| 110 | 2, 3 | (2, 2) | 100 + task 1 |
| 111 | all | **(2, 3)** | → 2 sessions |

<!-- snippet: modules/16-dynamic-programming/examples/1986-minimum-number-of-work-sessions-to-finish-the-tasks.cpp#solution -->
```cpp
class Solution {
public:
    int minSessions(vector<int>& tasks, int sessionTime) {
        int n = tasks.size();
        // best[mask] = smallest {sessions opened, minutes used in the current session} over every
        // order of finishing exactly the tasks in mask (pairs compare lexicographically)
        vector<pair<int, int>> best(1 << n, {INT_MAX, INT_MAX});
        best[0] = {0, sessionTime};                     // no session yet: a "full" one, so task 1 opens one
        for (int mask = 0; mask < (1 << n); mask++) {   // a task only ever makes the mask bigger
            auto [sessions, used] = best[mask];
            for (int i = 0; i < n; i++) {
                if (mask >> i & 1) continue;            // task i already done
                pair<int, int> next = used + tasks[i] <= sessionTime
                                          ? pair{sessions, used + tasks[i]}   // fits in the current session
                                          : pair{sessions + 1, tasks[i]};     // open a new one for it
                best[mask | 1 << i] = min(best[mask | 1 << i], next);
            }
        }
        return best[(1 << n) - 1].first;
    }
};
```
<!-- /snippet -->

**Complexity:** O(2ⁿ·n) time ≈ 2.3·10⁵ for n = 14; O(2ⁿ) space.

**Edge cases:** one task → 1; every task equal to sessionTime → n; everything fits in one session → 1.

**Follow-ups:**
- *Another formulation?* fewest[mask] = the fewest sessions to finish mask, where the *last session* is any submask whose total fits, enumerated with `for (int s = mask; s > 0; s = (s - 1) & mask)`. O(3ⁿ) ≈ 4.8·10⁶ for n = 14: the canonical "DP over subsets" (`submask::Solution` in the example file, tested against the same brute force).
- *Why not greedy, largest task first into the first session where it fits?* That's a bin-packing heuristic, not an exact algorithm: [3, 3, 2, 2, 2, 2] with sessionTime 7 needs 2 sessions ({3, 2, 2} twice), and the greedy uses 3.

### Worked example: 2376. Count Special Integers
[LeetCode 2376](https://leetcode.com/problems/count-special-integers/) · Hard

**Problem (paraphrased):** Count the positive integers ≤ n whose decimal digits are all different. n ≤ 2·10⁹.

**Signals:** "count the integers in [1, n]" with a property of their digits, and n far too big to loop over → digit DP.

**Brute force, and why it fails:** check every number up to n: 2·10⁹ numbers × 10 digits.

**Key insight:** build the number digit by digit from the left. The future depends only on the position, whether you're still tight against n's prefix, whether a real digit has started, and which digits are used (a 10-bit mask).

**Recipe:** countFrom(pos, tight, started, used) = the ways to fill positions pos.. validly · try each digit d ≤ (tight ? n[pos] : 9): a leading zero keeps started false and the mask unchanged; a real digit must not be in used · at the end, count 1 if started (0 isn't positive) · memo on all four · countFrom(0, true, false, 0).

**Dry run:** n = 20 (answer 19: everything except 11)
- d = 0 at pos 0 (a leading zero, no longer tight): the one-digit numbers 1–9 → 9.
- d = 1 (not tight): the second digit is 0–9 except 1 → 9.
- d = 2 (still tight, so the next limit is 0): only "20" → 1.
- Total 9 + 9 + 1 = **19**.

<!-- snippet: modules/16-dynamic-programming/examples/2376-count-special-integers.cpp#solution -->
```cpp
class Solution {
    string digits;        // n in decimal, most significant digit first
    vector<int> memo;     // memo[state index] = ways, -1 = not computed yet

    // Ways to fill positions pos.. so that the whole number is <= n and its digits are distinct.
    //   tight:   the digits so far equal n's prefix, so this digit may not exceed digits[pos]
    //   started: a non-zero digit has been placed (leading zeros are not real digits)
    //   used:    bitmask of the real digits placed so far
    int countFrom(int pos, bool tight, bool started, int used) {
        if (pos == (int)digits.size()) return started ? 1 : 0;   // "all zeros" is 0, which isn't in [1, n]
        int& cached = memo[((pos * 2 + tight) * 2 + started) * 1024 + used];
        if (cached != -1) return cached;
        int limit = tight ? digits[pos] - '0' : 9;
        int ways = 0;
        for (int d = 0; d <= limit; d++) {
            bool nextTight = tight && d == limit;
            if (!started && d == 0)
                ways += countFrom(pos + 1, nextTight, false, used);         // another leading zero
            else if (!(used >> d & 1))
                ways += countFrom(pos + 1, nextTight, true, used | 1 << d);  // a real digit, still unused
        }
        return cached = ways;
    }

public:
    int countSpecialNumbers(int n) {
        digits = to_string(n);
        memo.assign(digits.size() * 2 * 2 * 1024, -1);   // fresh memo: pos is counted from n's first digit
        return countFrom(0, true, false, 0);
    }
};
```
<!-- /snippet -->

**Complexity:** at most 10 · 2 · 2 · 2¹⁰ ≈ 4·10⁴ states × 10 digits ≈ 4·10⁵ steps (far fewer are reachable); O(states) memory.

**Edge cases:**
- n < 10 → n; n = 10 → 10 (10 is special); n = 11 → 10.
- The largest n, 2·10⁹ → 5 974 650. It fits in `int` easily: only 8 877 690 special integers exist at all (at most 10 digits long, since there are only 10 digits).

**Follow-ups:**
- *A range [L, R]?* f(R) − f(L − 1).
- *Without a memo?* Count all the shorter lengths with permutation counts, then walk n's digits and count the smaller choices at each position: the tight flag, unrolled by hand.
- *Numbers with at least one repeated digit?* n minus this answer.

## Common mistakes

- **Coding before the state is written in words**, then patching the recurrence until the samples pass.
- **Index drift:** a prefix length in one line, an element index in the next; off-by-one base cases and answer cells follow.
- **The wrong loop direction or nesting:** 0/1 vs unbounded, combinations vs sequences, intervals not by length.
- **Overflow:** INT_MAX as INF, counts past 2³¹, a modulus missing from one addition.
- **Greedy where DP is needed:** test the greedy idea on a tiny counterexample first (coins {1, 3, 4}).
- **A missing state variable:** the transition silently assumes something the state doesn't record (the last value, holding a stock, the used set).
- **The wrong answer cell:** dp[n−1] instead of the max over all i; dp[n] instead of dp[0][n+1].
- **A memo carried across test cases**, or rebuilt inside a loop when once per test is enough.

### DP debugging checklist

When the answer is wrong:
1. **Print the table on a tiny input** (n ≤ 5) and compare it with a hand computation. Guard the print with `#ifdef LOCAL` (`make run` defines `LOCAL`), so the same file can be pasted into the judge:

```c++
#ifdef LOCAL
for (int i = 0; i <= n; i++) {
    for (int j = 0; j <= m; j++) cerr << setw(4) << dp[i][j];
    cerr << '\n';
}
#endif
```

2. **Check the base cases** against the state definition: the empty prefix, capacity 0, one element, row 0 and column 0.
3. **Check the iteration order:** for one cell, list the cells it reads and confirm your loops fill them earlier (intervals: by length; the 1D 0/1 knapsack: downward).
4. **Check for overflow and sentinels:** INF + cost, products, counts beyond 2³¹. `make run` builds with UBSan, which reports signed overflow at the exact line.
5. **Check the answer cell** against the state definition.
6. **Stress-test against a brute force.** The module-10 choose / skip recursion is the easiest one to write, and random tiny inputs find the counterexample for you. Every example in this module does it; from 300:

<!-- snippet: modules/16-dynamic-programming/examples/0300-longest-increasing-subsequence.cpp#brute -->
```cpp
// Brute force: module 10's choose / skip recursion, no memo. O(2^n): fine for n <= ~15.
// (Memoise it on (i, prev) and you have the O(n^2) DP.)
int brute(const vector<int>& nums, int i, int prev) {      // prev = index of the last element taken, -1 if none
    if (i == (int)nums.size()) return 0;
    int best = brute(nums, i + 1, prev);                                       // skip nums[i]
    if (prev == -1 || nums[prev] < nums[i]) best = max(best, 1 + brute(nums, i + 1, i));   // take it
    return best;
}
```
<!-- /snippet -->

<!-- snippet: modules/16-dynamic-programming/examples/0300-longest-increasing-subsequence.cpp#stress -->
```cpp
for (int iter = 0; iter < 300; iter++) {    // stress test: random tiny arrays vs the brute force
    vector<int> nums = t::rand_vec((int)t::rand_int(1, 12), -5, 5);   // tiny range forces duplicates
    int expected = brute(nums, 0, -1);
    CHECK_EQ(sol.lengthOfLIS(nums), expected);
    CHECK_EQ(quad.lengthOfLIS(nums), expected);
    vector<int> lis = longestIncreasingSubsequence(nums);
    CHECK_EQ((int)lis.size(), expected);
    CHECK(isIncreasingSubsequence(lis, nums));
}
```
<!-- /snippet -->

A tiny value range (here −5..5) forces duplicates and ties, where most DP bugs live. Skew the random sizes toward the cases that matter too: 1986's stress test uses 4–9 tasks, because with fewer tasks the pair tie-break almost never decides the answer. When a check fails, print the input and shrink it by hand; the fixed seed replays the same case on every run.

## Say it out loud

A DP talk track (Coin Change):
1. "Restating: the fewest coins summing exactly to the amount, unlimited coins of each kind, −1 if impossible; amount ≤ 10⁴, up to 12 coins."
2. "Brute force: pick the last coin and recurse on the rest: exponential. Greedy isn't safe: {1, 3, 4} for 6 gives 3 coins instead of 2."
3. "What's left depends only on the remaining amount, so there are amount + 1 subproblems. State: fewest[a] = the fewest coins for exactly a."
4. "Transition by the last coin: fewest[a] = 1 + min fewest[a − c]. Base fewest[0] = 0; fill a upward; the answer is fewest[amount], or −1."
5. "O(amount × coins) time, O(amount) space. INF is amount + 1, so INF + 1 can't overflow."
6. "Edge cases: amount 0 → 0; unreachable → −1; a coin bigger than the amount is skipped."

Follow-ups interviewers commonly ask:
- *Can you use less memory?* Rolling rows or variables; say which cells each state reads.
- *Can you return the actual solution?* Parent pointers, or walk back through the full table.
- *Top-down or bottom-up, and why?* Depth, sparsity and evaluation order (Section 2).
- *Why is the recurrence complete?* Every solution has a last decision; removing it leaves a solution of the smaller state (Section 1).
- *Count the ways instead?* A sum instead of a min; check the decisions are disjoint (no double counting), watch overflow, and pick the loop nesting (combinations vs sequences).
- *What if n is 10⁵ or the values are 10⁹?* Recount the states: a value-sized dimension no longer fits, so look for a different state, or for structure (greedy, binary search) instead.

## Self-check

1. What are the five steps of the recipe, and which step does "extendable" in the correctness argument test?
   <details><summary>Answer</summary>State in words; transition by the last decision; base cases; evaluation order; where the answer is. "Extendable" (any solution of the smaller state plus the decision is valid) tests step 1: whether the state records everything the next decision depends on. When it fails, add the missing fact to the state (LIS: "ending at i").</details>
2. Why does the one-row 0/1 knapsack loop run downward, and what does the same loop compute when it runs upward?
   <details><summary>Answer</summary>best[c] reads best[c − w], which must still be the previous row (the item not yet used). Going downward, the cells to the left haven't been overwritten this round. Going upward, best[c − w] may already include the current item, so it can be taken again and again: that's the unbounded knapsack.</details>
3. Counting the ways to reach a total with repeatable items: which loop nesting counts combinations, which counts sequences, and why?
   <details><summary>Answer</summary>Items outer → combinations: items are introduced in a fixed order, so each multiset is built exactly one way. Total outer → sequences: ways[t] sums over which item comes last, so different orders are different last steps. The update line is the same in both.</details>
4. In the O(n log n) LIS, what does tails[k] mean, why is tails sorted, and why isn't it an LIS?
   <details><summary>Answer</summary>tails[k] = the smallest tail of any increasing subsequence of length k+1 seen so far. A subsequence of length k+2 contains one of length k+1 ending at a smaller value, so tails[k] < tails[k+1]. It isn't an LIS because replacements put later, smaller values into early slots: [3, 4, 1] ends with tails = [1, 4], and 1 comes after 4 in the array.</details>
5. How do you compute LCS in O(min(n, m)) memory, and what extra variable does the one-row version need?
   <details><summary>Answer</summary>Keep one row over the shorter string. Before overwriting row[j], remember its old value (lcs[i−1][j]); it becomes the diagonal lcs[i−1][j−1] for column j+1. You give up reconstruction, which needs the full table.</details>
6. In Burst Balloons, why must k be the *last* balloon burst in (l, r) rather than the first?
   <details><summary>Answer</summary>The last one stays until the end, so it walls off (l, k) from (k, r): their bursts never see each other, and when k bursts its neighbours are exactly l and r, so its coins are known. If k bursts first, the neighbours of later balloons depend on what happens on the other side.</details>
7. In House Robber III, why does each call return two numbers?
   <details><summary>Answer</summary>The parent's options depend on the child's state: robbing the parent needs the child's best when the child is skipped; skipping the parent needs the child's best overall. One number can't answer both, and recomputing grandchildren instead is exponential.</details>
8. Name two situations where top-down is the better choice, and two where bottom-up is.
   <details><summary>Answer</summary>Top-down: few reachable states in a huge space (digit DP, games); an awkward evaluation order (intervals, trees); a quick upgrade of an existing brute force. Bottom-up: recursion too deep (n ≥ 10⁵); rolling arrays to save memory; tight constant factors; further speedups that scan states in order.</details>
9. In digit DP, what does `tight` mean, and why does 2376 need a `started` flag?
   <details><summary>Answer</summary>tight = the digits so far equal n's prefix, so the current digit is capped at n[pos]; after a smaller digit, every later digit is free. Leading zeros aren't digits of the number: without `started`, "005" would mark 0 as used and then reject the second 0 as a repeat, so the number 5 would never be counted.</details>
10. A problem says n ≤ 16 and "visit every node exactly once, minimum total cost". What's the state, and what's the complexity?
    <details><summary>Answer</summary>Bitmask DP over (set of visited nodes, current node): 2ⁿ·n states with n transitions each, O(2ⁿ·n²) ≈ 1.7·10⁷ for n = 16, instead of n! orders (Held–Karp TSP).</details>
