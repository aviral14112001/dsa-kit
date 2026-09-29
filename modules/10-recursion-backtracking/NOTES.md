# 10 · Recursion + Backtracking
> Search the whole space — then learn where to stop searching.

**Time:** ~12 h of Core work ([problems](problems.md)) · **Prereqs:** modules 01 (value semantics), 03 section 2–3 (recursion trees, stack space), 09 section 1 (stacks) · **You're done when:** you can write the subsets, combinations and permutations templates from a blank file in under 5 minutes each and give each one's complexity as *leaves × cost per leaf*; you can point at every line in N-Queens that undoes a change and say which change it undoes; given a correct but slow backtracking, you can name the pruning that applies (feasibility, ordering, symmetry, bound, memo).

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Recursion mechanics | Trust the smaller call (induction); every call is a stack frame | halving recursion; explicit stack with a `stage` field | 50 |
| 2 | Subsets, permutations & combinations | choose → explore → un-choose over a decision tree | subsets · combinations · permutations; same-depth duplicate skip | 78, 90, 46, 22, 17, 39, 40, 131 |
| 3 | Constraint problems | O(1) legality checks from bookkeeping arrays; restore on return | N-Queens sets; grid DFS with mark / restore | 79, 1219, 51, 37 |
| 4 | Pruning | cut subtrees that can't succeed (or can't win) as close to the root as possible | ordering + symmetry; branch and bound; memo → DP | 698 |

## 1. Recursion mechanics — base cases, the call stack, converting to iteration

### Concept

A recursive function answers a problem by answering a smaller instance of the same problem:

- **Base case:** an input small enough to answer directly. Every chain of calls must reach one.
- **Recursive case:** shrink the input, call yourself, combine the result.

**The leap of faith.** When you write `f(n)`, *assume* `f` already works on every smaller input, and check only three things:

1. The base cases return the right answer.
2. *If* the smaller calls return correct answers, the combination is correct.
3. Every call moves strictly toward a base case.

That's proof by induction: 1 is the base case, 2 is the inductive step, and 3 guarantees the chain of calls ends. Don't trace ten levels deep in your head; check the three points and move on. Tower of Hanoi is the classic demonstration: "park the n−1 smaller disks on the spare peg, move the biggest, put the n−1 back on top" is correct by induction and needs 2ⁿ − 1 moves.

<!-- snippet: modules/10-recursion-backtracking/examples/recursion_to_iteration.cpp#hanoi_recursive -->
```cpp
// Move a tower of n disks from peg `from` to peg `to`, using `via` as the spare peg.
// Leap of faith: trust hanoi(n - 1, ...) to move a smaller tower correctly; then n works too.
void hanoi(int n, char from, char to, char via, vector<Move>& moves) {
    if (n == 0) return;                      // base case: no disks, nothing to do
    hanoi(n - 1, from, via, to, moves);      // 1. park the n-1 smaller disks on the spare peg
    moves.push_back({from, to});             // 2. move the biggest disk to its target
    hanoi(n - 1, via, to, from, moves);      // 3. move the n-1 disks back on top of it
}
```
<!-- /snippet -->

**The call stack.** Each call gets a *frame*: its parameters, locals, and the return address (where the caller resumes). Calling pushes a frame; returning pops it; the caller sits suspended until then. The halving recursion from the worked example below, on `myPow(2, 10)`:

```text
frame (top = newest)      waiting for             returns
power(x=2, n=0)           -- base case --         1
power(x=2, n=1)           half = power(2, 0)      1 * 1 * 2  = 2
power(x=2, n=2)           half = power(2, 1)      2 * 2      = 4
power(x=2, n=5)           half = power(2, 2)      4 * 4 * 2  = 32
power(x=2, n=10)          half = power(2, 5)      32 * 32    = 1024   <- bottom: the first call
```

Stack space is **maximum depth × frame size**, not the total number of calls. Here the depth is ⌊log₂ n⌋ + 2 frames; the subsets recursion in section 2 makes 2ⁿ⁺¹ − 1 calls but is never more than n + 1 deep.

**Tail calls.** A call is a *tail call* when nothing happens after it (`return f(n - 1, acc * n);`), so the caller's frame could be reused. Some languages guarantee that reuse; C++ doesn't, and neither does C#. An optimiser may do it, but you can't see what the judge's build did, so assume one frame per level.

A measured example on this kit's machine (8 MB stack), `long long sumTo(int n) { return n == 0 ? 0 : n + sumTo(n - 1); }`:

| Build | depth 10⁵ | depth 2·10⁵ | depth 10⁶ |
|---|---|---|---|
| no optimisation (`-O0`) | fine | crashes | crashes |
| `-O1` (clang rewrote the recursion as a loop) | fine | fine | fine |
| this kit's flags (`-O1` + sanitizers) | fine | crashes | crashes |

Same code, three builds, different verdicts. (`sumTo` isn't even a tail call: the `n +` runs after the call returns.)

### Template

**Recursion → iteration.** When each call makes at most one recursive call and you can compute the answer bottom-up, a plain loop replaces the recursion; the iterative `myPow` below is that case. For anything else, keep your own stack of what the call stack was keeping: the arguments, plus *where to resume*. Hanoi does work between its two calls, so each frame carries a `stage`:

<!-- snippet: modules/10-recursion-backtracking/examples/recursion_to_iteration.cpp#hanoi_iterative -->
```cpp
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
```
<!-- /snippet -->

When each state carries everything it needs and nothing happens after the children return, you don't need a stage: pop a state, push its children.

<!-- snippet: modules/10-recursion-backtracking/examples/recursion_to_iteration.cpp#subsets_stack -->
```cpp
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
```
<!-- /snippet -->

Cost: Hanoi O(2ⁿ) moves with O(n) frames either way; subsets O(2ⁿ · n) time. The explicit stack lives on the heap (a `vector`), so it's limited by memory, not by the 8 MB call stack. It's LIFO: the child pushed last runs first, so push children in reverse if the output order matters.

**Depth limits.** The default stack is 8 MB for the main thread on Linux and macOS (`ulimit -s`) and 1 MB on Windows. A frame is tens to a few hundred bytes; large locals and sanitizers make it bigger.

| Recursion depth | Verdict |
|---|---|
| ≤ 10⁴ | safe everywhere |
| ~10⁵ | usually fine with 8 MB and small frames; risky with big locals, on 1 MB stacks, or in debug builds |
| ≥ 10⁶ | convert to iteration (explicit stack or BFS) |

Backtracking depth is the number of decisions (n ≤ 20), so it's never the problem. DFS down a linked list, a skewed tree or a path graph of 10⁵+ nodes is where recursion crashes (modules 12 and 14).

### Pitfalls

- **Unreachable base case:** `f(n) = ... f(n - 2)` with a base case only at 0 never ends for odd n. The crash shows up as `AddressSanitizer: stack-overflow` locally and as a runtime error on a judge.
- **Negating `INT_MIN`:** `-n` overflows `int` (UB). Widen to `long long` first (worked example 50).
- **Two calls where one will do:** `return power(x, n / 2) * power(x, n / 2);` gives T(n) = 2T(n/2) + O(1) = O(n). Compute `half` once.
- **Copying big arguments into every frame:** pass `const vector<int>&`, or `vector<int>&` when you mutate and undo.
- **A reference into your own stack vector across `push_back`:** it may reallocate and leave the reference dangling. `hanoiIterative` copies `stack.back()` for that reason.
- **State that outlives one test case:** LeetCode runs many tests in one process, so members and statics persist. Reset them at the top of the public method (N-Queens does).

### Recognize it when…

- The answer is defined by a smaller answer: "row n is built from row n−1", xⁿ from x^(n/2), "valid if the inside is valid".
- The input is nested: trees, bracket expressions, directory structures.
- n is huge (10⁹, 2³¹) and the operation is repeated application (power, doubling) → halving recursion, O(log n).
- "Generate all …" → section 2: backtracking is recursion over choices.

### Worked example: 50. Pow(x, n)
[LeetCode 50](https://leetcode.com/problems/powx-n/) · Medium

**Problem (paraphrased):** compute x raised to an integer power n, where n can be negative and as small as −2³¹.
**Signals:** |n| up to 2³¹ rules out anything linear in n; "−2³¹" is the `INT_MIN` trap; |xⁿ| ≤ 10⁴ means the double itself won't overflow.
**Brute force, and why it fails:** multiply x by itself |n| times: about 2·10⁹ multiplications, TLE.
**Key insight:** xⁿ = (x^(n/2))² for even n and (x^(n/2))² · x for odd n: one recursive call per halving, so O(log n). A negative n becomes (1/x)^(−n), negated in 64 bits.
**Dry run:** the call-stack table above (n = 10 → 5 → 2 → 1 → 0, returning 1, 2, 4, 32, 1024). The iterative version reads the bits of n = 10 = 1010₂, so x¹⁰ = x⁸ · x²:

| N (binary) | lowest bit | result | x after squaring |
|---|---|---|---|
| 1010 | 0 | 1 | 4 |
| 101 | 1 | 1 · 4 = 4 | 16 |
| 10 | 0 | 4 | 256 |
| 1 | 1 | 4 · 256 = 1024 | 65536 |

<!-- snippet: modules/10-recursion-backtracking/examples/0050-powx-n.cpp#solution -->
```cpp
class Solution {
public:
    double myPow(double x, int n) {
        long long N = n;                   // -n overflows int when n == INT_MIN; -N is fine in 64 bits
        if (N < 0) {
            x = 1 / x;
            N = -N;
        }
        return power(x, N);
    }

private:
    // x^n for n >= 0. Leap of faith: assume power(x, n / 2) is correct, and build x^n from it.
    double power(double x, long long n) {
        if (n == 0) return 1.0;            // base case: every call chain ends here
        double half = power(x, n / 2);     // ONE recursive call, reused: that's what makes it O(log n)
        return n % 2 == 0 ? half * half : half * half * x;
    }
};
```
<!-- /snippet -->

The same idea without recursion (binary exponentiation):

<!-- snippet: modules/10-recursion-backtracking/examples/0050-powx-n.cpp#iterative -->
```cpp
class Solution {
public:
    double myPow(double x, int n) {
        long long N = n;
        if (N < 0) {
            x = 1 / x;
            N = -N;
        }
        double result = 1.0;
        // Invariant: result * x^N == the answer. Each step moves the lowest bit of N into result.
        while (N > 0) {
            if (N & 1) result *= x;        // this bit is set: the current power of x is a factor
            x *= x;                        // x, x^2, x^4, x^8, ...
            N >>= 1;
        }
        return result;
    }
};
```
<!-- /snippet -->

**Complexity:** O(log |n|) time; O(log |n|) stack for the recursive version, O(1) for the loop.
**Edge cases:**
- n = 0 → 1.
- n = `INT_MIN` → widen before negating (tested with x = ±1 and 2).
- Negative x with odd n → the odd branch keeps the sign.
- Results that underflow (2^(−2³¹)) quietly become 0.

**Follow-ups:**
- *aᵇ mod m?* The same loop with `% m` after every multiplication, in `long long` (module 18).
- *Why not `std::pow`?* Fine in production; the interview is testing the algorithm. It's also a floating-point function, so large integer powers can round.
- *What else speeds up this way?* Any associative operation: raising a 2×2 matrix to the n-th power gives the n-th Fibonacci number in O(log n).

## 2. Subsets, permutations & combinations — the choose / don't-choose tree

### Concept

Backtracking is depth-first search over a tree of decisions. A node is a partial solution, its children are the ways to extend it, and the leaves (or every node, depending on the tree) are the candidates. You keep **one** mutable partial solution and walk it down and back up:

```c++
void backtrack(State& s) {
    if (complete(s)) { record(s); return; }
    for (Choice c : choices(s)) {
        if (!allowed(s, c)) continue;   // prune (Section 4)
        apply(s, c);                    // choose
        backtrack(s);                   // explore
        undo(s, c);                     // un-choose: s is exactly as it was before apply()
    }
}
```

The undo is the "back" in backtracking: when the recursive call returns, the state is back to what it was, so the next sibling starts clean. A backtracking solution is correct when the tree holds every candidate exactly once and every change is undone.

Three trees cover most problems:

1. **Subsets (choose / skip).** Depth n; at depth i you decide whether nums[i] is in. 2ⁿ leaves, one per subset.
2. **Combinations (k of n).** The *start-index* tree, where a node's children append one element from a later index, cut off at depth k. Add the bound "enough elements must remain": i ≤ n − (k − size).
3. **Permutations.** At depth d, pick any element not used yet: n choices, then n − 1, … → n! leaves. Track use with `used[]`, or swap the chosen element into position d.

```text
choose / skip on [1, 2, 3]: the leaves are the subsets
                   []                              decide 1
          skip /         \ take
       []                      [1]                 decide 2
     /     \                 /      \
  []        [2]         [1]          [1,2]         decide 3
 /  \      /   \       /   \         /   \
[]  [3]  [2]  [2,3]  [1]  [1,3]  [1,2]  [1,2,3]    8 leaves = 8 subsets

start-index on [1, 2, 3]: every node is a subset (8 nodes)
[]
├── [1]
│   ├── [1,2]
│   │   └── [1,2,3]
│   └── [1,3]
├── [2]
│   └── [2,3]
└── [3]
```

### Template

The subsets code is worked example 78 below (choose / skip, start-index and bitmask versions); permutations is worked example 46. The combinations template:

<!-- snippet: modules/10-recursion-backtracking/examples/combinations_template.cpp#combinations -->
```cpp
// All k-element combinations of items, each once, in index order (C(n, k) of them).
void combine(const vector<int>& items, int k, int start, vector<int>& cur, vector<vector<int>>& out) {
    if ((int)cur.size() == k) {                                 // a full combination
        out.push_back(cur);
        return;
    }
    int need = k - (int)cur.size();                             // elements still to pick
    for (int i = start; i <= (int)items.size() - need; i++) {   // beyond this, too few elements remain
        cur.push_back(items[i]);
        combine(items, k, i + 1, cur, out);                     // later elements only: no reorderings
        cur.pop_back();
    }
}
```
<!-- /snippet -->

**Complexity = leaves × work per leaf.** Copying an answer into the output costs its length:

| Tree | Leaves (= answers) | Time | Extra space |
|---|---|---|---|
| Subsets | 2ⁿ | O(2ⁿ · n) | O(n): recursion + path |
| Combinations | C(n, k) | O(C(n, k) · k) | O(k) |
| Permutations | n! | O(n! · n) | O(n): path + `used` |

Internal nodes don't change these bounds:
- The subsets tree has 2ⁿ⁺¹ − 1 nodes in total, fewer than twice the leaves.
- The permutation tree has n!/(n−d)! nodes at depth d, fewer than e · n! in total.
- The bounded combinations tree has exactly C(n+1, k) nodes. Without the loop bound, it visits every prefix of length ≤ k, which is Σ C(n, d) for d ≤ k. For n = 20, k = 18 that's 1,048,555 calls instead of 1,330 (the example file checks both formulas).

For a feel of the limits: 2²⁰ ≈ 10⁶ subsets is fine; 10! ≈ 3.6·10⁶ permutations is fine; 12! ≈ 4.8·10⁸ is not. When the output itself is exponential, no algorithm beats the enumeration.

**Duplicates in the input.** On [1, 2, 2] the plain templates produce [1, 2] twice, once with each 2. The fix: sort, then at each depth skip a value equal to the one just tried *at that same depth*:

```c++
sort(nums.begin(), nums.end());
// ...inside the start-index loop:
for (int i = start; i < (int)nums.size(); i++) {
    if (i > start && nums[i] == nums[i - 1]) continue;   // this value already rooted a branch here
    // choose nums[i], recurse with i + 1, un-choose
}
```

Two siblings that choose equal values root identical subtrees, so keep the first and skip the rest. Why `i > start` and not `i > 0`: the first choice at each depth is always allowed, even when it equals the element before it. That's how [2, 2] is built: the second 2 is the *first* choice one level deeper.

```text
start-index tree on [1, 2, 2] with the skip
[] ─ 1 → [1] ─ 2 → [1,2] ─ 2 → [1,2,2]
   │         └ 2 (skipped: equal to its left sibling)
   ├ 2 → [2] ─ 2 → [2,2]
   └ 2 (skipped)
6 distinct subsets: [], [1], [1,2], [1,2,2], [2], [2,2]
```

**Pruning a sum search on a sorted array.** When you pick numbers to reach a target sum and the candidates are sorted ascending, `cand[i] > remaining` means every later candidate is too big as well: `break` out of the loop, don't `continue`. If an element may be reused, the child call starts at `i` instead of `i + 1`. Those two lines, plus the duplicate skip, are what separate the combination-sum problems on your list from the combinations template.

The other trees on your list are the same skeleton with different choices:
- one level per input position, whose choices are that position's options (17);
- choices limited by running counts of what's still legal (22);
- choices that are cut positions, i.e. where the next piece ends (131).

### Pitfalls

- **Forgetting the undo** (`pop_back`, `used[i] = false`): state leaks into sibling branches.
- **`result.push_back(cur)` copies**, and that's what you want: a snapshot. The C# instinct is the opposite. `result.Add(cur)` stores a *reference*, so every entry aliases one list, and you'd need `new List<int>(cur)`. In C++ you only get that bug by storing pointers.
- **Passing `vector<int> cur` by value into each call** is correct but copies at every node. Pass it by reference and undo.
- **`build(start + 1)` instead of `build(i + 1)`** in the start-index loop gives wrong sets and duplicates.
- **The duplicate skip without sorting first**, or with `i > 0`, which loses answers like [2, 2].
- **`break` where only `continue` is valid**: the array isn't sorted, or the condition isn't monotone. That's a wrong answer, not just a slow one.
- **Deduplicating with `set<vector<int>>`:** correct, but it still explores every duplicate subtree and pays O(log) vector comparisons per insert. Interviewers expect the skip.
- **`vector<bool>` is bit-packed:** `used[i] = true` works, but `bool& b = used[i];` doesn't compile (it's a proxy object).

### Recognize it when…

- "Return **all** possible subsets / combinations / permutations / partitions", "generate every …", "in any order".
- Tiny n: ≤ 10 for permutations, ≤ 20 for subsets. An output that is exponential by definition is the tell.
- Choose k of n → combinations; order matters → permutations; any size → subsets.
- "Unique combinations" while the input has repeats → sort + same-depth skip.
- A target sum over candidates → the combination-sum shape; "may be used unlimited times" → recurse from `i`.

### Worked example: 78. Subsets
[LeetCode 78](https://leetcode.com/problems/subsets/) · Medium

**Problem (paraphrased):** given up to 10 distinct integers, return every subset (the power set) in any order.
**Signals:** "all possible subsets", n ≤ 10, distinct values → the choose / skip tree with 2ⁿ ≤ 1024 answers.
**Brute force, and why it fails:** it doesn't. Any correct method is Θ(2ⁿ · n) because that's the size of the output. The real question is which enumeration you can write without bugs: recursion (choose / skip or start-index) or bitmasks.
**Key insight:** each element is independently in or out: one binary decision per element, and a root-to-leaf path spells a subset.
**Dry run:** nums = [1, 2, 3]. The skip branch runs first, so the leaves come out as [], [3], [2], [2,3], [1], [1,3], [1,2], [1,2,3]:

| call | `cur` on entry | what happens |
|---|---|---|
| build(0) | [] | skip 1 → build(1) |
| build(1) | [] | skip 2 → build(2) |
| build(2) | [] | skip 3 → build(3) records []; take 3 → build(3) records [3]; pop → [] |
| build(1) | [] | take 2 → [2]; build(2) records [2] and [2,3]; pop → [] |
| build(0) | [] | take 1 → [1]; the same four leaves with 1 in front; pop → [] |

<!-- snippet: modules/10-recursion-backtracking/examples/0078-subsets.cpp#solution -->
```cpp
class Solution {
public:
    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> cur;                         // the path: elements chosen so far
        build(nums, 0, cur, result);
        return result;
    }

private:
    // Decide nums[i..]; cur holds the elements chosen from nums[0..i).
    void build(const vector<int>& nums, int i, vector<int>& cur, vector<vector<int>>& result) {
        if (i == (int)nums.size()) {             // every element decided: cur is one subset
            result.push_back(cur);               // copies cur: a snapshot, not a reference
            return;
        }
        build(nums, i + 1, cur, result);         // skip nums[i]
        cur.push_back(nums[i]);                  // choose nums[i]
        build(nums, i + 1, cur, result);
        cur.pop_back();                          // un-choose: hand cur back exactly as we got it
    }
};
```
<!-- /snippet -->

The start-index version, where every node is an answer (this is the shape that takes the duplicate skip):

<!-- snippet: modules/10-recursion-backtracking/examples/0078-subsets.cpp#start_index -->
```cpp
class Solution {
public:
    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> cur;
        build(nums, 0, cur, result);
        return result;
    }

private:
    // Every node of this tree is a subset: record it, then extend it by each later element.
    void build(const vector<int>& nums, int start, vector<int>& cur, vector<vector<int>>& result) {
        result.push_back(cur);
        for (int i = start; i < (int)nums.size(); i++) {
            cur.push_back(nums[i]);
            build(nums, i + 1, cur, result);     // i + 1, not start + 1: only later elements may follow
            cur.pop_back();
        }
    }
};
```
<!-- /snippet -->

The bitmask version, which is the "Power Set Bit Manipulation" redo that the sheet asks for:

<!-- snippet: modules/10-recursion-backtracking/examples/0078-subsets.cpp#bitmask -->
```cpp
// No recursion: bit j of mask says whether nums[j] is in the subset. (The tests use it as the brute force.)
vector<vector<int>> subsetsBitmask(const vector<int>& nums) {
    int n = nums.size();
    vector<vector<int>> result;
    for (int mask = 0; mask < (1 << n); mask++) {
        vector<int> subset;
        for (int j = 0; j < n; j++)
            if ((mask >> j) & 1) subset.push_back(nums[j]);
        result.push_back(subset);
    }
    return result;
}
```
<!-- /snippet -->

**Complexity:** O(2ⁿ · n) time (2ⁿ subsets, each copied in O(n)); O(n) extra space for the recursion and `cur`, besides the O(2ⁿ · n) output.
**Edge cases:**
- n = 1 → [[], [x]].
- The empty subset must appear: it's the all-skip leaf, or the root of the start-index tree.
- Negative values change nothing.

**Follow-ups:**
- *The input has duplicates?* That's 90 on your list: sort + the same-depth skip.
- *Only subsets of size k?* The combinations template.
- *Without recursion or bits?* Start from [[]]; for each element, append a copy of every existing subset with that element added.
- *n = 30?* 2³⁰ ≈ 10⁹ subsets can't be listed. Ask what's really needed: a count or a best subset points to DP or meet-in-the-middle.

### Worked example: 46. Permutations
[LeetCode 46](https://leetcode.com/problems/permutations/) · Medium

**Problem (paraphrased):** return every ordering of up to 6 distinct integers.
**Signals:** "all possible permutations", n ≤ 6 (6! = 720) → the permutation tree.
**Brute force, and why it fails:** as with subsets, the n! · n output is unavoidable. `std::next_permutation` on a sorted copy is the library route (the tests use it as the reference); interviewers want the backtracking.
**Key insight:** fill positions left to right; each position takes any element not used yet. `used[]` makes "not used yet" an O(1) question.
**Dry run:** nums = [1, 2, 3].

| depth | `used` | `cur` | next |
|---|---|---|---|
| 0 | F F F | [] | try 1 |
| 1 | T F F | [1] | try 2 (1 is used) |
| 2 | T T F | [1,2] | try 3 → record [1,2,3]; undo back to depth 1 |
| 1 | T F F | [1] | try 3 → [1,3], then 2 → record [1,3,2]; undo back to depth 0 |
| 0 | F F F | [] | try 2 → [2,1,3], [2,3,1]; then 3 → [3,1,2], [3,2,1] |

<!-- snippet: modules/10-recursion-backtracking/examples/0046-permutations.cpp#solution -->
```cpp
class Solution {
public:
    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> cur;
        vector<bool> used(nums.size(), false);           // used[i]: nums[i] is already in cur
        build(nums, used, cur, result);
        return result;
    }

private:
    void build(const vector<int>& nums, vector<bool>& used, vector<int>& cur, vector<vector<int>>& result) {
        if (cur.size() == nums.size()) {                 // every position filled
            result.push_back(cur);
            return;
        }
        for (int i = 0; i < (int)nums.size(); i++) {     // any unused element may go next
            if (used[i]) continue;
            used[i] = true;                              // choose
            cur.push_back(nums[i]);
            build(nums, used, cur, result);              // explore
            cur.pop_back();                              // un-choose: undo both changes,
            used[i] = false;                             // in reverse order
        }
    }
};
```
<!-- /snippet -->

The swapping version keeps the unused elements in nums[pos..] and swaps each candidate into position pos. Undoing the swap restores the order, so nums comes back unchanged (the tests check this):

<!-- snippet: modules/10-recursion-backtracking/examples/0046-permutations.cpp#swap -->
```cpp
class Solution {
public:
    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> result;
        build(nums, 0, result);
        return result;                                   // nums is back in its original order here
    }

private:
    // nums[0..pos) is fixed; nums[pos..] are the elements still available. Try each at position pos.
    void build(vector<int>& nums, int pos, vector<vector<int>>& result) {
        if (pos == (int)nums.size()) {
            result.push_back(nums);
            return;
        }
        for (int i = pos; i < (int)nums.size(); i++) {
            swap(nums[pos], nums[i]);                    // choose nums[i] for position pos
            build(nums, pos + 1, result);
            swap(nums[pos], nums[i]);                    // undo: restore the order for the next i
        }
    }
};
```
<!-- /snippet -->

**Complexity:** O(n! · n) time; O(n) extra space (`used`, `cur`, recursion).
**Edge cases:** n = 1 → [[x]]; negative values; the swap version must restore every swap.
**Follow-ups:**
- *Duplicates in the input?* Sort, and skip nums[i] when it equals nums[i−1] and nums[i−1] is *not* in use. Equal values are then always used left to right, so each distinct arrangement appears once.
- *Only the k-th permutation?* Don't enumerate: pick each position's element from factorial block sizes, giving O(n²).
- *The next permutation in lexicographic order?* 31 in module 04.

## 3. Constraint problems — N-Queens, Sudoku, word search, rat in a maze

### Concept

A constraint problem has variables to assign (the queen's column in each row, the digit in each empty cell, the next step of a path), each with a domain, and rules that forbid some combinations. Backtracking assigns one variable at a time and rejects a choice the moment it breaks a rule. The skill is making "is this choice legal?" an O(1) question with bookkeeping that you update on choose and restore on un-choose.

**The restore invariant.** When a backtracking call returns, every piece of shared state (board, marks, sets, counters, path) is exactly as it was when the call began. Every change made before the recursive call has a matching undo after it, in reverse order. Watch any `return` or `continue` that sits between a change and its undo.

**N-Queens.** Place one queen per row, so rows never conflict by construction. A queen at (r, c) attacks column c, its "\" diagonal (where r − c is constant) and its "/" diagonal (where r + c is constant). Three boolean arrays answer "attacked?" in O(1):

```text
"\" index: r - c + (n-1)        "/" index: r + c
     c: 0 1 2 3                      c: 0 1 2 3
r=0:    3 2 1 0                 r=0:    0 1 2 3
r=1:    4 3 2 1                 r=1:    1 2 3 4
r=2:    5 4 3 2                 r=2:    2 3 4 5
r=3:    6 5 4 3                 r=3:    3 4 5 6
```

Each index 0..2n−2 names one diagonal; the + (n − 1) shift keeps the "\" index non-negative.

**Sudoku (37 on your list; ideas only).**
- Keep three arrays of 9-bit masks, `rowMask[9]`, `colMask[9]`, `boxMask[9]`, with box = (r / 3) · 3 + c / 3. Bit d set means digit d + 1 is taken.
- Pick the empty cell with the **fewest** candidates (the most-constrained variable). A cell with zero candidates fails right away and a cell with one is forced. Scanning cells in reading order instead can explore orders of magnitude more nodes.
- Placing a digit sets its bit in all three masks; the undo clears it.

```c++
int box = (r / 3) * 3 + c / 3;
int allowed = ~(rowMask[r] | colMask[c] | boxMask[box]) & 0x1FF;  // bit d set: digit d+1 fits
int options = __builtin_popcount(allowed);                         // choose the cell minimising this
for (int m = allowed; m != 0; m &= m - 1) {                        // visit each set bit
    int bit = m & -m;                                              // lowest set bit
    int digit = __builtin_ctz(bit) + 1;
    // place `digit`, recurse, then clear `bit` from the three masks
}
```

**Word search (79 on your list; ideas only).** Run a DFS from every cell that matches the first letter. So that a path can't reuse a cell, overwrite the cell with a sentinel (such as '#') before recursing and put the letter back afterwards. In-place marking costs O(1) per step and no extra memory. Two cheap pre-checks:
- If the board has fewer copies of some letter than the word needs, answer false immediately.
- If the word's last letter is rarer on the board than its first, search the reversed word: fewer starting points.

**Rat in a maze.** List every path from the top-left to the bottom-right cell through open cells, moving D/L/R/U without revisiting a cell on the path. Direction arrays replace four copy-pasted branches with one loop. Listing the directions in alphabetical order (D, L, R, U) makes the paths come out sorted. The same mark / explore / restore loop, with "maximise the sum" instead of "record the path", is 1219 on your list.

### Template

N-Queens is worked example 51 below. The grid version:

<!-- snippet: modules/10-recursion-backtracking/examples/rat_in_a_maze.cpp#rat_in_a_maze -->
```cpp
// Directions listed in alphabetical order of their letters, so paths come out sorted.
const int DR[4] = {1, 0, 0, -1};
const int DC[4] = {0, -1, 1, 0};
const char DIR[4] = {'D', 'L', 'R', 'U'};

void explore(vector<vector<int>>& grid, int r, int c, string& path, vector<string>& paths) {
    int n = grid.size();
    if (r == n - 1 && c == n - 1) {                  // reached the exit
        paths.push_back(path);
        return;
    }
    grid[r][c] = 0;                                  // mark: this cell is on the current path
    for (int d = 0; d < 4; d++) {
        int nr = r + DR[d], nc = c + DC[d];
        if (nr < 0 || nr >= n || nc < 0 || nc >= n || grid[nr][nc] == 0) continue;   // bounds first
        path.push_back(DIR[d]);
        explore(grid, nr, nc, path, paths);
        path.pop_back();
    }
    grid[r][c] = 1;                                  // restore: other paths may pass through here
}

vector<string> ratInAMaze(vector<vector<int>> grid) {   // by value: we mark cells in our own copy
    int n = grid.size();
    vector<string> paths;
    if (n == 0 || grid[0][0] == 0 || grid[n - 1][n - 1] == 0) return paths;
    string path;
    explore(grid, 0, 0, path, paths);
    return paths;
}
```
<!-- /snippet -->

**Complexity:** exponential. After the first step, each cell has at most 3 unvisited neighbours, so there are at most O(3^(n²)) paths. An open 5×5 grid already has 8,512 corner-to-corner paths (the file checks 1, 2, 12, 184, 8512 for n = 1..5). Space O(n²): a path can visit every cell, so that's the recursion depth.

### Pitfalls

- **A negative diagonal index:** r − c needs the + (n − 1) shift. A `map` also works, but it's slower.
- **Bounds before contents:** `nr < 0 || nr >= n || … || grid[nr][nc] == 0` relies on short-circuit order. Read the cell first and you read out of bounds, which ASan reports.
- **Returning before restoring:** `return true` straight after marking leaves the board modified. That's harmless only if nobody looks at the board afterwards; restore first when unsure.
- **A sentinel that can occur in the data:** '#' is safe among letters. In a 0/1 maze, marking a visited cell with 0 works because a visited cell should behave exactly like a wall.
- **Undoing only some of the changes:** N-Queens sets three flags and a board cell, so it must clear all four.
- **`vector<bool>` + chained assignment** (`a[i] = b[j] = true`) is fine, but you can't take `bool&` into it. Use `vector<char>` if you need references.

### Recognize it when…

- "Place n items so that no two conflict" (queens, knights, colours) → one variable per row or item, plus conflict arrays.
- "Fill the grid so every row / column / box satisfies …" → cell-by-cell assignment with bitmasks; take the most constrained cell first.
- "Is there a path that spells / visits … without reusing a cell?" → grid DFS with in-place marking.
- "All paths" or "the best path" in a small grid (≤ 6×6, ≤ 25 useful cells) → backtracking with mark / restore. The tiny constraints are the tell.

### Worked example: 51. N-Queens
[LeetCode 51](https://leetcode.com/problems/n-queens/) · Hard

**Problem (paraphrased):** place n queens on an n×n board (n ≤ 9) so that no two attack each other; return every such board as a list of strings.
**Signals:** "all distinct solutions", n ≤ 9, pairwise conflicts → constraint backtracking.
**Brute force, and why it fails:** choosing any n of the n² cells means C(81, 9) ≈ 2.6·10¹¹ boards for n = 9. Even "one queen per row, any column" is nⁿ ≈ 3.9·10⁸.
**Key insight:** every solution has exactly one queen per row and per column, so the search is over column orders (at most n!). The O(1) column and diagonal checks cut most branches near the root: n = 8 visits 2,057 nodes, against 8! = 40,320 column orders (tested in the example file).
**Dry run:** n = 4 (placements as (row, column)):

| step | board so far | what happens |
|---|---|---|
| 1 | (0,0) | row 1: col 0 taken, col 1 on the "\" diagonal → place (1,2) |
| 2 | (0,0) (1,2) | row 2: every column attacked → dead end; undo (1,2) |
| 3 | (0,0) (1,3) | row 2: (2,1) is safe → place; row 3: all attacked → undo; (2,2) attacked → undo (1,3) |
| 4 | (0,0) | row 1 exhausted → undo (0,0) |
| 5 | (0,1) (1,3) (2,0) (3,2) | every row placed → record `.Q..`, `...Q`, `Q...`, `..Q.` |
| 6 | … | keep going: (0,2) leads to the mirror image, the second solution |

<!-- snippet: modules/10-recursion-backtracking/examples/0051-n-queens.cpp#solution -->
```cpp
class Solution {
public:
    vector<vector<string>> solveNQueens(int n) {
        board.assign(n, string(n, '.'));
        colUsed.assign(n, false);
        diagUsed.assign(2 * n - 1, false);      // "\" diagonals: r - c is constant (shifted by n - 1)
        antiUsed.assign(2 * n - 1, false);      // "/" diagonals: r + c is constant
        solutions.clear();
        placeRow(0);
        return solutions;
    }

private:
    vector<string> board;
    vector<bool> colUsed, diagUsed, antiUsed;
    vector<vector<string>> solutions;

    // Rows 0..r-1 hold one queen each, none attacking another. Place a queen in row r.
    void placeRow(int r) {
        int n = board.size();
        if (r == n) {                            // a queen in every row: a full solution
            solutions.push_back(board);
            return;
        }
        for (int c = 0; c < n; c++) {
            int d = r - c + n - 1, a = r + c;
            if (colUsed[c] || diagUsed[d] || antiUsed[a]) continue;   // attacked: O(1) check
            board[r][c] = 'Q';                                        // choose
            colUsed[c] = diagUsed[d] = antiUsed[a] = true;
            placeRow(r + 1);                                          // explore
            colUsed[c] = diagUsed[d] = antiUsed[a] = false;           // un-choose: undo every
            board[r][c] = '.';                                        // change made above
        }
    }
};
```
<!-- /snippet -->

**Complexity:** O(n!) nodes as an upper bound, far fewer in practice; each solution costs O(n²) to copy. Space O(n²) for the board, plus O(n) for the arrays and the recursion.
**Edge cases:**
- n = 1 → [["Q"]].
- n = 2 and n = 3 have no solutions: return an empty list.
- Calling `solveNQueens` twice on one object must not mix results; the reset at the top handles that.

**Follow-ups:**
- *Only count the solutions?* Drop the board and count at the leaves.
- *Faster?* Replace the three arrays with three `int` bitmasks, so the free columns are `~(cols | diag | anti) & ((1 << n) - 1)`. Shift the diagonal masks by one bit per row. It's the same tree with a much smaller constant.
- *n = 1000, one solution?* Backtracking won't do; explicit constructions place the queens by formula.

## 4. Pruning — feasibility checks and orderings that cut the tree early

### Concept

Backtracking costs **nodes visited × work per node**. Pruning removes whole subtrees, and a cut near the root removes exponentially more than a cut near the leaves, so the aim is to fail as early as possible. Five tools:

1. **Feasibility checks.** Before recursing, ask whether this partial state can still be completed. Run the cheap global checks once, up front (in 698: total % k ≠ 0, or the largest item exceeds the target), then the per-step checks (this bucket would overflow).
2. **Ordering.** Try the most constrained choices first, so failures surface near the root. Placing the largest items first makes an overflow show up after a couple of items instead of a dozen. Choosing Sudoku's most constrained cell is the same idea.
3. **Symmetry breaking.** If two choices lead to states that are identical up to relabelling, explore only one. Empty buckets are interchangeable: if an item failed in one empty bucket, it fails in every empty bucket. More generally, skip a bucket whose current sum equals one you already tried for this item; section 2's duplicate skip is the same idea.
4. **Branch and bound** (optimisation problems). Keep the best complete answer found so far. At each node, compute an *optimistic* bound on anything reachable below it; for a minimisation, that's a cost that no completion can go under. If even the bound can't beat the best, prune. A tighter bound prunes more; an invalid bound (one that claims more cost than some real completion has) can prune the optimum away.
5. **Memoisation → DP.** Suppose the rest of the search depends only on a compact state rather than on the path that reached it, and many paths reach the same state. Then cache the answer per state: that's dynamic programming. Exponential backtracking over *orders* becomes polynomial in the number of *states*: 698 goes from kⁿ assignments to 2ⁿ masks × n. The test is whether you can write the recursive function so that its result depends only on its arguments. If it reads or updates a global "best", you can't memoise it as is.

What each pruning buys on one input with no answer (so every variant must exhaust its tree), measured by 698's example file:

| Search on nums = [1,5,8,6,5,5,8,6,7,8,9,8], k = 4 | calls |
|---|---|
| feasibility check only | 275,149 |
| feasibility + largest first | 15,285 |
| feasibility + empty-bucket symmetry break | 11,491 |
| all three (worked example 698) | 643 |

### Template

Feasibility, ordering and symmetry are in worked example 698 below. **Branch and bound**, on the assignment problem: n workers and n jobs, cost[w][j] for worker w doing job j; give each worker a different job and minimise the total. It's the permutations template plus one bound check:

<!-- snippet: modules/10-recursion-backtracking/examples/branch_and_bound.cpp#branch_and_bound -->
```cpp
class AssignmentSolver {
public:
    int solve(const vector<vector<int>>& costs) {
        cost = costs;
        int n = cost.size();
        restMin.assign(n + 1, 0);           // restMin[w] = sum of the cheapest job of workers w..n-1
        for (int w = n - 1; w >= 0; w--)
            restMin[w] = restMin[w + 1] + *min_element(cost[w].begin(), cost[w].end());
        taken.assign(n, false);
        best = INT_MAX;
        nodes = 0;
        search(0, 0);
        return best;
    }

    long long nodes = 0;                    // search nodes visited, to measure the pruning

private:
    vector<vector<int>> cost;
    vector<int> restMin;
    vector<bool> taken;                     // taken[j]: job j already has a worker
    int best = INT_MAX;                     // cheapest complete assignment found so far

    void search(int w, int costSoFar) {     // workers 0..w-1 have jobs, costing costSoFar
        nodes++;
        int n = cost.size();
        if (w == n) {
            best = min(best, costSoFar);
            return;
        }
        // Bound: even if every remaining worker got their cheapest job (clashes ignored), this branch
        // costs at least costSoFar + restMin[w]. If that can't beat best, nothing below can: prune.
        if (costSoFar + restMin[w] >= best) return;
        for (int j = 0; j < n; j++) {       // branch: worker w tries every free job
            if (taken[j]) continue;
            taken[j] = true;
            search(w + 1, costSoFar + cost[w][j]);
            taken[j] = false;
        }
    }
};
```
<!-- /snippet -->

The bound `restMin[w]` is what the remaining workers would cost if each got their cheapest job with clashes ignored. It can only underestimate the true completion cost, so it's a valid lower bound. On the 8×8 instance in the example file, the search visits 1,700 nodes instead of the full tree's 109,601. The worst case is still O(n! · n): bounds help in practice, not in theory. Space O(n).

### Pitfalls

- **A bound that overestimates what a branch can reach** prunes the branch holding the optimum. That's a wrong answer that small tests may not catch; stress-test against brute force.
- **Symmetry breaking on things that aren't symmetric**, such as buckets with different capacities, or non-empty buckets with different sums.
- **`>=` versus `>` against the best:** `>=` is fine when you need only the optimal value; use `>` if you must list every optimal answer.
- **Memoising a function that depends on hidden state** (a global best, a path vector): the cache hands back answers from a different situation.
- **A memo key that drops part of the state:** in 698 the open bucket's fill is sum(used) % target, a function of the mask. That's why the mask alone is a valid key, and it's the argument to say out loud.
- **`1 << n` breaks at n ≥ 31:** `1 << 31` is negative, and shifting an `int` by 32 or more is UB. Write `1LL << n` if you need it, though bitmask memo tables are for n ≤ ~20–25 anyway (2²⁰ one-byte entries = 1 MB).

### Recognize it when…

- "Partition into k groups with equal sums", "can these sticks form a square", "split the work so every worker's load is equal" → bucket search: sort descending, check feasibility per bucket, break the empty-bucket symmetry.
- Optimisation over all assignments or orders with n ≤ ~12–15 ("minimum total cost", "smallest maximum") → branch and bound, or bitmask DP when the state is "which items are used".
- A correct backtracking TLEs on a few tests → add ordering and symmetry before rewriting.
- n ≤ 16–20 and the answer depends only on *which* items are used, not their order → a bitmask memo (module 16 section 4).

### Worked example: 698. Partition to K Equal Sum Subsets
[LeetCode 698](https://leetcode.com/problems/partition-to-k-equal-sum-subsets/) · Medium

**Problem (paraphrased):** can the array (n ≤ 16, values ≤ 10⁴) be split into k non-empty groups with equal sums?
**Signals:** n ≤ 16 says exponential search is intended (2¹⁶ = 65,536 masks); "k subsets with equal sum" → bucket search; the target is forced to be total / k.
**Brute force, and why it fails:** assign every item to one of k buckets and check: kⁿ = 16¹⁶ ≈ 1.8·10¹⁹ assignments at the limits.
**Key insight:** fill buckets up to target = total / k with the three prunings: reject overflow, place big items first, never try a second empty bucket. Every bucket stays ≤ target and the items sum to k · target, so placing every item means every bucket is exactly full.
**Dry run:** [4, 3, 2, 3, 5, 2, 1], k = 4 → target 5; sorted descending: [5, 4, 3, 3, 2, 2, 1].

| item | buckets after | why |
|---|---|---|
| 5 | [5, 0, 0, 0] | first bucket |
| 4 | [5, 4, 0, 0] | 5 + 4 > 5 |
| 3 | [5, 4, 3, 0] | 8 > 5, 7 > 5 |
| 3 | [5, 4, 3, 3] | bucket 2 would hold 6 |
| 2 | [5, 4, 5, 3] | 3 + 2 = 5 |
| 2 | [5, 4, 5, 5] | only bucket 3 fits |
| 1 | [5, 5, 5, 5] | all placed → true, with no backtracking at all |

<!-- snippet: modules/10-recursion-backtracking/examples/0698-partition-to-k-equal-sum-subsets.cpp#solution -->
```cpp
class Solution {
public:
    bool canPartitionKSubsets(vector<int>& nums, int k) {
        int total = accumulate(nums.begin(), nums.end(), 0);
        if (total % k != 0) return false;                     // feasibility, before any search
        target = total / k;
        sort(nums.begin(), nums.end(), greater<int>());       // ordering: big items fail fast
        if (nums[0] > target) return false;
        bucket.assign(k, 0);
        return place(nums, 0);
    }

private:
    int target = 0;
    vector<int> bucket;                                       // current sum of each bucket

    // Place nums[i..]. Invariant: every bucket sum is <= target.
    bool place(const vector<int>& nums, int i) {
        if (i == (int)nums.size()) return true;               // sums <= target and total = k * target,
                                                              // so every bucket is exactly target
        for (int b = 0; b < (int)bucket.size(); b++) {
            if (bucket[b] + nums[i] > target) continue;       // feasibility: this bucket would overflow
            bucket[b] += nums[i];
            if (place(nums, i + 1)) return true;
            bucket[b] -= nums[i];
            if (bucket[b] == 0) break;                        // symmetry: nums[i] failed in an empty
        }                                                     // bucket, so it fails in every empty one
        return false;
    }
};
```
<!-- /snippet -->

**Memoised version (bitmask DP).** Fill the buckets one at a time instead. Then the state is just the set of used items, because the open bucket's fill is sum(used) % target. Different placement orders reach the same set, so cache the answer per mask:

<!-- snippet: modules/10-recursion-backtracking/examples/0698-partition-to-k-equal-sum-subsets.cpp#memo -->
```cpp
class Solution {
public:
    bool canPartitionKSubsets(vector<int>& nums, int k) {
        int total = accumulate(nums.begin(), nums.end(), 0);
        if (total % k != 0) return false;
        target = total / k;
        memo.assign(1 << nums.size(), UNKNOWN);
        return canFinish(nums, 0, 0);
    }

private:
    enum Result : char { UNKNOWN, YES, NO };
    int target = 0;
    vector<Result> memo;                                      // memo[used] for each subset of items

    // Fill buckets one at a time. The items in `used` make up some full buckets plus the open one,
    // so the open bucket holds sum(used) % target: `fill` is determined by `used`, and so is the
    // answer for the remaining items. Many choice orders reach the same `used`: memoize on it.
    bool canFinish(const vector<int>& nums, int used, int fill) {
        if (used == (1 << nums.size()) - 1) return true;
        if (memo[used] != UNKNOWN) return memo[used] == YES;
        for (int i = 0; i < (int)nums.size(); i++) {
            if (((used >> i) & 1) || fill + nums[i] > target) continue;
            if (canFinish(nums, used | (1 << i), (fill + nums[i]) % target)) {
                memo[used] = YES;
                return true;
            }
        }
        memo[used] = NO;
        return false;
    }
};
```
<!-- /snippet -->

**Complexity:**
- Pruned search: exponential in the worst case (O(kⁿ)), but fast in practice. On 5,000 random n = 16 inputs (values 1–20, k = 2–6), the slowest took about 1.2·10⁵ calls. Space O(n + k).
- Memo version: a guaranteed O(2ⁿ · n) time (about 10⁶ steps at n = 16) and O(2ⁿ) memory.

**Edge cases:**
- total % k ≠ 0, or one item bigger than the target → false before any search.
- k = 1 → true; k = n → true only if all values are equal.
- Many equal values: the pruned search can still repeat work across equal items. The memo version is immune.

**Follow-ups:**
- *Four equal sides from sticks?* The same code with k = 4.
- *Minimise the largest bucket instead of requiring equal sums?* The same bucket search with branch and bound on the current maximum, or binary search on the answer (module 11).
- *Why descending order?* Big items have the fewest legal placements, so failing on them happens near the root, where a cut removes the most nodes.

## Common mistakes

- **A missing undo, or undos in the wrong order**, so state leaks into sibling branches.
- **`build(start + 1)` instead of `build(i + 1)`** in a start-index loop.
- **The duplicate skip with `i > 0`**, or without sorting first.
- **`continue` instead of `break`** on a sorted sum search (slow), or `break` on unsorted data (wrong).
- **`power(x, n / 2) * power(x, n / 2)`:** two calls make it O(n).
- **`-n` on `INT_MIN`:** widen to `long long` first.
- **Recursing 10⁵–10⁶ deep on list- or path-shaped input:** stack overflow. Debug and sanitizer builds overflow sooner than optimised ones.
- **Members or statics not reset between calls** on the same `Solution` object.
- **Passing vectors by value** through every recursive call.
- **A `set` to remove duplicates** instead of pruning them.
- **Unqualified `move(x)`:** this kit's flags (`-Werror`) reject it, so write `std::move`.

## Say it out loud

A talk track for N-Queens:

1. "I need every placement of n non-attacking queens, n ≤ 9, returned as boards."
2. "Brute force picks n of the n² cells: C(81, 9), about 10¹¹. But every solution has exactly one queen per row, so I'll place row by row: at most n! column orders."
3. "To test a square in O(1), I keep three boolean arrays: columns, the r − c diagonals shifted by n − 1, and the r + c diagonals."
4. "For each column in the current row: if it's free, place, mark the three arrays, recurse, then unmark and clear the cell. The undo restores the state for the next column."
5. "Time is bounded by n! nodes, plus O(n²) to copy each solution; the pruning keeps it tiny, about 2,000 nodes for n = 8. Space O(n²) for the board."
6. "Edge cases: n = 1 gives one board, n = 2 and 3 give none."

Follow-ups interviewers ask:
- **"What's the time complexity?"** Describe the tree: leaves × work per leaf (2ⁿ · n, n! · n, C(n, k) · k).
- **"Can you avoid duplicates without a set?"** Sort + the same-depth skip, and explain why `i > start`.
- **"Can you do it without recursion?"** An explicit stack of states (Section 1), or bitmasks for subsets.
- **"How would you speed it up?"** Ordering, symmetry breaking, a bound, bitmasks for O(1) checks, and memoisation when states repeat.
- **"What if n were 10⁵?"** You can't list an exponential output. Ask whether they want a count or an optimum, which usually means DP.

## Self-check

1. Instead of tracing a recursive function, what three things do you check?
<details><summary>Answer</summary>The base cases are right; <em>assuming</em> the smaller calls are right, the combination is right; every call moves strictly toward a base case. That's induction.</details>

2. Why is `return power(x, n / 2) * power(x, n / 2);` O(n) and not O(log n)?
<details><summary>Answer</summary>Two calls per level: T(n) = 2T(n/2) + O(1) = O(n). Compute <code>half</code> once and square it.</details>

3. Why does `myPow` copy n into a `long long` before negating it?
<details><summary>Answer</summary>n can be <code>INT_MIN</code>, and −<code>INT_MIN</code> doesn't fit in <code>int</code>: signed overflow, which is UB. In 64 bits it's just 2³¹.</details>

4. How many nodes and how many leaves does the choose / skip subsets tree have for n elements, and how deep does the recursion go?
<details><summary>Answer</summary>2ⁿ⁺¹ − 1 nodes, 2ⁿ leaves, depth n + 1 frames. Stack space is O(n) even though the time is O(2ⁿ · n).</details>

5. In the duplicate skip, why `i > start` and not `i > 0`?
<details><summary>Answer</summary>The first choice at each depth must always be allowed, even when it equals the element before it; otherwise [2, 2] could never be built. Only later siblings with an equal value are skipped, because they would root identical subtrees.</details>

6. Converting Hanoi to an explicit stack needs a `stage` field in each frame; converting subsets doesn't. Why?
<details><summary>Answer</summary>Hanoi does work <em>between and after</em> its recursive calls, so a frame must remember where to resume. Each subsets state carries its whole partial subset and nothing happens after its children finish, so you pop it and push the children.</details>

7. Why is `if (bucket[b] == 0) break;` correct in 698?
<details><summary>Answer</summary>Empty buckets are interchangeable: if the current item couldn't complete a partition from one empty bucket, it can't from any other. Empty buckets also always form a suffix (items only ever go into the first empty one), so nothing after it needs trying.</details>

8. What makes a bound valid in branch and bound for a minimisation?
<details><summary>Answer</summary>It must never exceed the true cost of any completion of the branch, i.e. it's optimistic. Then "bound ≥ best" proves the branch can't win. <code>restMin</code> qualifies because it ignores job clashes.</details>

9. When can you memoise a backtracking function, and what does that turn it into?
<details><summary>Answer</summary>When its result depends only on its arguments (not on the path taken or on a global best) and the same arguments recur. The result is DP over those states: 698 goes from exponential assignments to O(2ⁿ · n) over used-masks.</details>

10. Your grid DFS returns `true` as soon as it finds the word, straight after marking the cell. What's the risk, and when does it matter?
<details><summary>Answer</summary>The board is left with '#' marks. On LeetCode that's harmless because the search is over; in a reusable function, or if the caller searches again, it breaks later searches. Restore the cell before returning.</details>
