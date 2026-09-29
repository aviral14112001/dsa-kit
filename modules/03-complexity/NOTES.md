# 03 · Complexity Analysis & Patterns
> Reason about cost before you write a line — and defend the number out loud.

**Time:** ~5 h of Core work ([problems](problems.md)) · **Prereqs:** module 01 (C++ containers and what their operations cost) · **You're done when:** (1) you answer drill snippets 1–8 and recurrences 1–8 correctly, each with a one-sentence reason; (2) given a constraints block, you name the target complexity and a technique that fits before reading the rest of the statement (9 of the 10 drill blocks); (3) you can prove amortized O(1) `push_back` with the aggregate or the accounting argument, and derive why naive `fib(n)` makes 2·F(n+1) − 1 calls.

Every `cpp` block below is pulled from a file in [`examples/`](examples/) that `make test M=modules/03-` compiles and runs, and the complexity claims in the drills are measured there too.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Big-O, Big-Θ, Big-Ω | Count the dominant operation as n grows; amortize over whole sequences, not single operations | [amortized_vector.cpp](examples/amortized_vector.cpp), [analyze_snippets.cpp](examples/analyze_snippets.cpp) | drill: snippets 1–8 |
| 2 | Recurrences | Sum the recursion tree level by level (the master theorem does it for you); memoization costs distinct states × work per state | [0509-fibonacci-number.cpp](examples/0509-fibonacci-number.cpp) | 509 📖 · drill: recurrences 1–8 |
| 3 | Space complexity | Auxiliary vs total; the recursion stack counts, the output usually doesn't | the tables in section 3 | none: concepts only |
| 4 | Reading constraints | n, the value range and the query count fix the target complexity before you choose an algorithm | the n → complexity table | 2006, 2367 · drill: ten constraint blocks |

## 1. Big-O, Big-Θ, Big-Ω — worst, average and amortized analysis

### What you're measuring

The running time T(n) is the number of basic steps (a comparison, an addition, an array access) as a function of the input size n. Constants depend on the machine and the compiler; the growth rate decides which n you can handle at all. Going from O(n²) to O(n log n) at n = 10⁵ is the difference between 10¹⁰ steps (minutes) and 2·10⁶ (milliseconds).

### The three bounds

| Notation | Formal: there exist c > 0 and n₀ such that for all n ≥ n₀ | Says | Read it as |
|---|---|---|---|
| f(n) = O(g(n)) | 0 ≤ f(n) ≤ c·g(n) | upper bound | "grows no faster than g" |
| f(n) = Ω(g(n)) | f(n) ≥ c·g(n) ≥ 0 | lower bound | "grows at least as fast as g" |
| f(n) = Θ(g(n)) | both of the above (with two constants c₁, c₂) | tight bound | "grows exactly like g" |

Example: f(n) = 3n² + 10n is Θ(n²). For n ≥ 10 we have 10n ≤ n², so 3n² ≤ f(n) ≤ 4n²: take c₁ = 3, c₂ = 4, n₀ = 10. And n = O(n²) is true (a loose upper bound), while n² = O(n) is false: no constant c keeps n² ≤ c·n once n > c.

Rules you'll use every day:
- Drop constants and lower-order terms: 5n² + n log n + 100 = Θ(n²).
- Code in sequence adds, and the biggest term wins: O(n) + O(n log n) = O(n log n).
- Nested loops: sum the inner loop's cost over the outer iterations. It's a product only when the inner count doesn't depend on the outer variable.
- The log base doesn't matter: log₂ n and log₁₀ n differ by a constant factor.
- Keep independent inputs separate: O(n + m), O(n·m), O(V + E). Don't call everything n.
- Strings cost their length: comparing or hashing a string of length L is O(L). Sorting n strings is O(n log n · L); a `map<string, int>` lookup is O(L log n).

In interviews "O" is used to mean the tight bound. State the tightest bound you can justify: "O(n²)" for a linear algorithm is technically true and will be heard as a mistake.

| n | log₂ n | √n | n log₂ n | n² | 2ⁿ |
|---|---|---|---|---|---|
| 10 | 3.3 | 3.2 | 33 | 100 | 1 024 |
| 10³ | 10 | 32 | 10⁴ | 10⁶ | ≈ 10³⁰¹ |
| 10⁶ | 20 | 1 000 | 2·10⁷ | 10¹² | hopeless |

### Best, worst and average case

- **Worst case**: the maximum cost over all inputs of size n. The default in interviews.
- **Best case**: rarely informative (insertion sort on already-sorted input is O(n)).
- **Average (expected) case**: the cost averaged over a distribution of inputs, or over an algorithm's own random choices. Quicksort with random pivots is O(n log n) expected and O(n²) worst; hash-table operations are O(1) average and O(n) worst.

Say which one you mean: "O(1) average for the hash lookups, O(n) in the worst case if every key collides."

### Amortized analysis

Amortized cost is the average cost per operation **over a worst-case sequence** of operations. No probability is involved: the bound holds for every sequence. That's what separates it from average case.

The standard example is `vector::push_back`. When the vector is full, it allocates a bigger buffer (twice the capacity in libc++ and libstdc++; MSVC uses 1.5×), moves every element across and frees the old one. That push costs O(n). Yet n pushes cost O(n) in total, so each is O(1) *amortized*. Two ways to prove it:

- **Aggregate method** (add up the whole sequence): starting empty and doubling, reallocations happen at sizes 1, 2, 4, …, 2ᵏ < n, moving 1 + 2 + 4 + … + 2ᵏ < 2n elements in total. Add the n writes themselves: fewer than 3n steps for n pushes, O(1) each on average.
- **Accounting method** (overcharge the cheap pushes, bank the difference, spend it on the expensive one): charge every push 3 coins. One pays for writing the element; two are saved on it. Right after the capacity grows to 2k, the vector holds k elements and no savings. The next reallocation happens after k more pushes, which bank 2k coins: exactly enough to move all 2k elements. The bank never goes negative, so the true total is at most 3n.

<!-- snippet: modules/03-complexity/examples/amortized_vector.cpp#count_reallocations -->
```cpp
GrowthStats push_n(int n) {
    GrowthStats stats;
    vector<int> v;
    size_t cap = v.capacity();
    for (int i = 0; i < n; i++) {
        if (v.size() == v.capacity()) stats.moves += (long long)v.size();   // full: growing moves them all
        v.push_back(i);
        if (v.capacity() != cap) {
            stats.reallocations++;
            cap = v.capacity();
        }
    }
    return stats;
}
```
<!-- /snippet -->

`amortized_vector.cpp` runs this for n up to 10⁶ and checks: at most log₁.₅(n) + 3 reallocations (the base of the log is the growth factor, and every mainstream library grows by at least 1.5×), at most 3n moves in total, and a copy-counting element type that measures exactly the moves this function infers. `make run F=modules/03-complexity/examples/amortized_vector.cpp` also prints the capacities: 1 2 4 8 … 1024. C#'s `List<T>.Add` works the same way (capacity 4, then doubling), and is amortized O(1) for the same reason.

The growth must be **geometric**. Grow by a constant instead and the moves add up to step + 2·step + 3·step + … ≈ n²/(2·step), so each push costs Θ(n/step) on average:

<!-- snippet: modules/03-complexity/examples/amortized_vector.cpp#additive_growth -->
```cpp
// A hypothetical vector that grows by a fixed `step` slots instead of by a factor.
long long moves_with_additive_growth(long long n, long long step) {
    long long moves = 0, cap = 0;
    for (long long size = 0; size < n; size++) {
        if (size == cap) {        // full: allocate cap + step slots and carry over all `size` elements
            moves += size;
            cap += step;
        }
    }
    return moves;
}
```
<!-- /snippet -->

With n = 10⁵ and step = 64 that's about 780 moves per element, against fewer than 3 with doubling (both checked). When you know the final size, skip the reallocations entirely:

<!-- snippet: modules/03-complexity/examples/amortized_vector.cpp#reserve -->
```cpp
vector<int> v;
v.reserve(1000);                   // one allocation up front...
size_t cap = v.capacity();
for (int i = 0; i < 1000; i++) v.push_back(i);
CHECK(cap >= 1000);
CHECK_EQ(v.capacity(), cap);       // ...so no push_back ever reallocated
```
<!-- /snippet -->

The same aggregate argument powers two patterns you'll meet constantly. A nested loop is **not** automatically O(n²):

- **Monotonic stack** (module 09 section 2): each index is pushed once and popped at most once, so the inner `while` runs at most n times *over the whole run*: O(n) in total.

  ```c++
  for (int i = 0; i < n; i++) {
      while (!st.empty() && /* st.top() can never matter once a[i] is here */)
          st.pop();       // each index is popped at most once, ever
      st.push(i);         // each index is pushed exactly once
  }
  ```

- **Two pointers / sliding window** (module 06): `left` only moves forward and never passes `n`, so however many times the inner loop runs for one `right`, it runs at most n times in total: O(n).

  ```c++
  for (int right = 0, left = 0; right < n; right++) {
      /* add a[right] to the window */
      while (/* the window is invalid */) {
          /* remove a[left] */
          left++;         // at most n increments over the whole run
      }
  }
  ```

(A third method, the potential method, formalizes the bank as a function of the structure's state. Interviews don't ask for it.)

**Pitfalls: costs that hide inside one line**

| Code | Looks like | Actually costs |
|---|---|---|
| `s = s + c` in a loop | O(1) per iteration | O(\|s\|): builds a new string, so O(n²) total. `s += c` is amortized O(1) |
| `s.substr(i, len)` | O(1) | O(len): it copies |
| `v.insert(v.begin(), x)`, `v.erase(v.begin())` | O(1) | O(n): every element shifts |
| `find`, `count` on a vector | cheap | O(n) |
| `std::lower_bound(s.begin(), s.end(), x)` on a `set` | O(log n) | O(n): set iterators can't jump. Use `s.lower_bound(x)` |
| `m[key]` on a `map` | O(1) | O(log n) comparisons, each O(L) for string keys |
| `unordered_map` operations | O(1) | O(1) average, O(n) worst; hashing a string key is O(L) |
| passing a `vector` by value | free | O(n) per call; inside recursion it multiplies |
| `vector<int> tmp(n)` or `fill` inside a loop | cheap | O(n) every time |

**Recognize it when…**
- A nested loop whose inner pointer never moves backwards → count the pointer's total moves: usually O(n) overall.
- Each element enters and leaves a stack, queue, deque or heap at most once → total = n × (cost per push/pop).
- A loop variable that doubles or halves → a log factor; one bounded by `i * i ≤ n` → √n; an inner loop to n/i → harmonic, n log n.
- "Design a structure with amortized O(1) operations" (a queue built from two stacks, a dynamic array) → the interviewer expects the aggregate or accounting argument.

### Drill: analyze snippets 1–8

For each snippet, give the tight Θ running time in terms of n with a one-sentence reason. `ops++` marks each pass through the innermost statement, but your answer is the total running time, including whatever each line costs by itself. Commit to an answer before you open it. `examples/analyze_snippets.cpp` measures every answer below.

**Snippet 1**

<!-- snippet: modules/03-complexity/examples/analyze_snippets.cpp#s1 -->
```cpp
for (int i = 0; i < n; i++)
    for (int j = 1; j < n; j *= 2)
        ops++;
```
<!-- /snippet -->

<details><summary>Answer</summary>

**Θ(n log n).** The inner loop doesn't depend on `i`: `j` takes the values 1, 2, 4, … below n, so it runs ⌈log₂ n⌉ times. n outer iterations × ⌈log₂ n⌉ = n·⌈log₂ n⌉, which is exactly what the test checks.

</details>

**Snippet 2**

<!-- snippet: modules/03-complexity/examples/analyze_snippets.cpp#s2 -->
```cpp
for (int i = 1; i * i <= n; i++)
    ops++;
```
<!-- /snippet -->

<details><summary>Answer</summary>

**Θ(√n).** The loop runs while i² ≤ n, so i goes up to ⌊√n⌋. For n near `INT_MAX`, `i * i` overflows `int` (undefined behaviour): write `i <= n / i` or `1LL * i * i <= n`.

</details>

**Snippet 3**

<!-- snippet: modules/03-complexity/examples/analyze_snippets.cpp#s3 -->
```cpp
for (int i = 1; i <= n; i++)
    for (int j = i; j <= n; j += i)
        ops++;
```
<!-- /snippet -->

<details><summary>Answer</summary>

**Θ(n log n).** For each i, `j` visits the multiples of i up to n: ⌊n/i⌋ values. The total is ⌊n/1⌋ + ⌊n/2⌋ + … + ⌊n/n⌋, just under n/1 + n/2 + … + n/n = n·Hₙ, and the harmonic number Hₙ = 1 + 1/2 + … + 1/n ≈ ln n. Exactly 27 operations at n = 10, 482 at 100, 7069 at 1000. The same sum appears in sieves and in "for every divisor" loops.

</details>

**Snippet 4**

<!-- snippet: modules/03-complexity/examples/analyze_snippets.cpp#s4 -->
```cpp
string s;
for (int i = 0; i < n; i++) {
    s = s + 'x';
    ops++;
}
```
<!-- /snippet -->

<details><summary>Answer</summary>

**Θ(n²).** `ops` says n, and it's lying: `s + 'x'` builds a brand-new string, copying all of `s` first. Iteration i copies i characters, so the total is 0 + 1 + … + (n − 1) = n(n − 1)/2. The test measures the bytes allocated: doubling n multiplies them by 3.99. With `s += 'x'` (amortized O(1)) the whole loop is Θ(n), and doubling n doubles the bytes.

</details>

**Snippet 5**

<!-- snippet: modules/03-complexity/examples/analyze_snippets.cpp#s5 -->
```cpp
map<int, int> position;          // a holds n distinct values
for (int i = 0; i < n; i++) {
    position[a[i]] = i;
    ops++;
}
```
<!-- /snippet -->

<details><summary>Answer</summary>

**Θ(n log n).** Each `map` operation is O(log size), and the size grows to n: log 1 + log 2 + … + log n = log(n!) = Θ(n log n). The test counts the comparisons: about 1.4·n·log₂ n. With an `unordered_map` it's Θ(n) on average; if `a` had only k distinct values, Θ(n log k).

</details>

**Snippet 6**

<!-- snippet: modules/03-complexity/examples/analyze_snippets.cpp#s6 -->
```cpp
int f(int n, long long& ops) {
    ops++;
    if (n == 0) return 1;
    return f(n - 1, ops) + f(n - 1, ops);
}
```
<!-- /snippet -->

<details><summary>Answer</summary>

**Θ(2ⁿ).** Each call makes two calls on n − 1: C(n) = 1 + 2·C(n − 1) with C(0) = 1, so C(n) = 2ⁿ⁺¹ − 1. The trap: `return 2 * f(n - 1, ops);` computes the same value with one call per level, Θ(n). The number of recursive calls per level sets the cost, not the arithmetic.

</details>

**Snippet 7**

<!-- snippet: modules/03-complexity/examples/analyze_snippets.cpp#s7 -->
```cpp
while (n > 0) {
    n /= 2;
    ops++;
}
```
<!-- /snippet -->

<details><summary>Answer</summary>

**Θ(log n).** n halves every iteration, so the loop runs ⌊log₂ n⌋ + 1 times: the number of bits in n. That's 31 iterations for n = `INT_MAX`.

</details>

**Snippet 8**

<!-- snippet: modules/03-complexity/examples/analyze_snippets.cpp#s8 -->
```cpp
vector<bool> composite(n + 1, false);
for (int i = 2; i <= n; i++)
    if (!composite[i])
        for (long long j = 1LL * i * i; j <= n; j += i) {
            composite[j] = true;
            ops++;
        }
```
<!-- /snippet -->

<details><summary>Answer</summary>

**Θ(n log log n).** The inner loop only runs for primes p ≤ √n, about n/p times each: n·(1/2 + 1/3 + 1/5 + …) ≈ n·ln ln n (Mertens' theorem). ln ln n stays below 4 for any n you'll ever meet, so this is nearly linear: 2.1·n marks at n = 10⁶, and doubling n just over doubles the work (2.05×). The outer loop adds Θ(n). This is the sieve of Eratosthenes (module 18).

</details>

## 2. Recurrences — recursion trees, the master theorem, divide-and-conquer costs

A recursive function's cost satisfies a **recurrence**: T(n) = (the cost of its recursive calls) + (the work it does itself). Divide and conquer has the shape T(n) = a·T(n/b) + f(n): a subproblems of size n/b, plus f(n) to split the input and combine the answers.

### Recursion trees

Draw the calls as a tree, write each node's own work next to it, and add up level by level. Merge sort, T(n) = 2T(n/2) + cn:

```text
level 0                  n                       cn
level 1           n/2          n/2               2 · c(n/2) = cn
level 2        n/4   n/4    n/4   n/4            4 · c(n/4) = cn
  ...                    ...                     ...
level log₂n    1  1  1  1  ...  1  1  (n leaves) n · c      = cn
                                                 total: cn · (log₂ n + 1) = Θ(n log n)
```

In general, level i has aⁱ nodes of size n/bⁱ doing aⁱ·f(n/bⁱ) work, the tree is log_b n deep, and it has a^(log_b n) = n^(log_b a) leaves. The level sums follow one of three shapes:
- they **shrink** geometrically going down → the root dominates → Θ(f(n));
- they're **equal** → (work per level) × (number of levels) → Θ(f(n) · log n);
- they **grow** geometrically → the leaves dominate → Θ(n^(log_b a)).

### The master theorem

For T(n) = a·T(n/b) + Θ(nᵈ) with a ≥ 1, b > 1, d ≥ 0, compare d with log_b a (the exponent of the leaf count):

| Case | Condition | T(n) | Which part dominates |
|---|---|---|---|
| 1 | d < log_b a | Θ(n^(log_b a)) | the leaves |
| 2 | d = log_b a | Θ(nᵈ log n) | every level equally |
| 3 | d > log_b a | Θ(nᵈ) | the root |

Case 2 extends to f(n) = Θ(nᵈ logᵏ n) with d = log_b a: T(n) = Θ(nᵈ logᵏ⁺¹ n).

| Algorithm | Recurrence | a, b, d | log_b a | Case | T(n) |
|---|---|---|---|---|---|
| Binary search | T(n/2) + O(1) | 1, 2, 0 | 0 | 2 | Θ(log n) |
| Merge sort | 2T(n/2) + O(n) | 2, 2, 1 | 1 | 2 | Θ(n log n) |
| Visit every node of a balanced tree, or the max of an array by halves | 2T(n/2) + O(1) | 2, 2, 0 | 1 | 1 | Θ(n) |
| Karatsuba multiplication | 3T(n/2) + O(n) | 3, 2, 1 | log₂ 3 ≈ 1.585 | 1 | Θ(n^1.585) |
| Halve the input after linear work (quickselect, on average) | T(n/2) + O(n) | 1, 2, 1 | 0 | 3 | Θ(n) |

The theorem needs subproblems of equal size n/b. For uneven splits such as T(n) = T(n/3) + T(2n/3) + n, draw the tree.

### When the master theorem doesn't apply

When the size shrinks by subtraction, unroll the recurrence:

| Recurrence | Unrolls to | T(n) | Where you meet it |
|---|---|---|---|
| T(n − 1) + O(1) | n constant steps | Θ(n) | recursion along a list |
| T(n − 1) + O(n) | n + (n − 1) + … + 1 | Θ(n²) | selection sort; quicksort's worst case |
| 2T(n − 1) + O(1) | 1 + 2 + 4 + … + 2ⁿ | Θ(2ⁿ) | all subsets; Tower of Hanoi |
| T(n − 1) + T(n − 2) + O(1) | grows like φⁿ | Θ(φⁿ), φ ≈ 1.618 | naive Fibonacci |
| n·T(n − 1) + O(1) | n · (n − 1) · … | Θ(n!) | all permutations |
| T(n/2) + O(n) | n + n/2 + n/4 + … ≤ 2n | Θ(n) | also master case 3 |

Rule of thumb for backtracking: branching factor b and depth D give about bᴰ nodes; multiply by the work per node. Output counts too: listing all 2ⁿ subsets, each copied at O(n), is Θ(n·2ⁿ).

### Memoization collapses the tree

When a recursion tree solves the same subproblem many times, caching each answer means every **distinct state** is expanded once, and the rest are lookups:

**cost = (number of distinct states) × (work per state, not counting the recursive calls)**

- `fib(n)`: n + 1 states × O(1) = Θ(n), down from Θ(φⁿ) calls.
- Paths through an R × C grid, `dp(r, c)`: R·C states × O(1).
- An interval recursion `dp(i, j)` that loops over a split point k: about n²/2 states × O(n) = Θ(n³) (recurrence 8 below).

Space is the table plus the deepest chain of calls on the stack.

### Worked example: 509. Fibonacci Number
[LeetCode 509](https://leetcode.com/problems/fibonacci-number/) · Easy

**Problem (paraphrased):** return F(n), where F(0) = 0, F(1) = 1 and F(n) = F(n − 1) + F(n − 2), for 0 ≤ n ≤ 30.
**Signals:** the definition is a recurrence with two self-calls, and its subproblems overlap: F(n − 2) is needed by both F(n) and F(n − 1).
**Brute force, and why it fails:** transcribe the definition. It's correct, but it makes C(n) = 2·F(n + 1) − 1 calls, because C(n) = 1 + C(n − 1) + C(n − 2) is the Fibonacci recurrence again, plus one. That's 21,891 calls for n = 20 and 2,692,537 for n = 30, so it passes here; at n = 50 it would be 40,730,022,147.
**Key insight:** there are only n + 1 distinct subproblems, each recomputed over and over. Cache them (Θ(n)). Then notice each state needs only the previous two: two variables, O(1) space.
**Dry run:** the tree for n = 4 has 9 calls (= 2·F(5) − 1); `fib(2)` is computed twice, and with a memo the second is a lookup.

```text
                 fib(4)
              /          \
         fib(3)          fib(2)
        /      \         /     \
    fib(2)   fib(1)   fib(1)  fib(0)
    /    \
fib(1)  fib(0)
```

The loop version for n = 5:

| after step i | prev | cur |
|---|---|---|
| start | 0 | 1 |
| 2 | 1 | 1 |
| 3 | 1 | 2 |
| 4 | 2 | 3 |
| 5 | 3 | 5 |

<!-- snippet: modules/03-complexity/examples/0509-fibonacci-number.cpp#naive -->
```cpp
int fib_naive(int n) {                  // T(n) = T(n-1) + T(n-2) + O(1): Θ(φ^n) calls, φ ≈ 1.618
    calls++;
    if (n < 2) return n;
    return fib_naive(n - 1) + fib_naive(n - 2);
}
```
<!-- /snippet -->

<!-- snippet: modules/03-complexity/examples/0509-fibonacci-number.cpp#memo -->
```cpp
int fib_memo(int n, vector<int>& memo) {   // n + 1 distinct states × O(1) work each = Θ(n)
    calls++;
    if (n < 2) return n;
    if (memo[n] != -1) return memo[n];     // solved before: answer without recursing
    return memo[n] = fib_memo(n - 1, memo) + fib_memo(n - 2, memo);
}
```
<!-- /snippet -->

<!-- snippet: modules/03-complexity/examples/0509-fibonacci-number.cpp#solution -->
```cpp
class Solution {
public:
    int fib(int n) {
        if (n < 2) return n;
        int prev = 0, cur = 1;             // F(0), F(1)
        for (int i = 2; i <= n; i++) {     // after each step: cur = F(i), prev = F(i - 1)
            int sum = prev + cur;
            prev = cur;
            cur = sum;
        }
        return cur;
    }
};
```
<!-- /snippet -->

The call counts, measured (the test also checks both formulas for every n from 0 to 30):

<!-- snippet: modules/03-complexity/examples/0509-fibonacci-number.cpp#count_calls -->
```cpp
calls = 0;
CHECK_EQ(fib_naive(20), 6765);
CHECK_EQ(calls, 21891);                // = 2·F(21) − 1: the recursion tree has 21891 nodes

calls = 0;
vector<int> memo(21, -1);
CHECK_EQ(fib_memo(20, memo), 6765);
CHECK_EQ(calls, 39);                   // = 2·20 − 1: states 2..20 expand once (19 calls); the other 20 return at once
```
<!-- /snippet -->

**Complexity:** naive Θ(φⁿ) time and Θ(n) stack; memoized Θ(n) time, Θ(n) for the table plus Θ(n) of stack; the loop Θ(n) time and Θ(1) space.
**Edge cases:** n = 0 and n = 1 return directly; n = 30 gives 832,040; the loop stays inside `int` up to n = 46 (F(46) = 1,836,311,903), and F(47) would overflow.
**Follow-ups:**
- *n up to 90?* Switch to `long long`, which holds up to F(92).
- *n up to 10¹⁸, answer modulo 10⁹ + 7?* Raise the matrix [[1, 1], [1, 0]] to the n-th power by repeated squaring: O(log n) 2×2 multiplications (fast exponentiation, module 18).
- *Why is naive exponential when each call does O(1) work?* The number of calls is exponential: count the nodes of the tree, not the work per node.
- *What about the closed form (Binet's formula)?* It needs φⁿ in floating point; with `double` it's first wrong at n = 71.

### Drill: solve recurrences 1–8

Give the tight bound, and name the tool: master theorem case, recursion tree, or unrolling.

1. T(n) = T(n/3) + O(1)
<details><summary>Answer</summary>

**Θ(log n).** Master: a = 1, b = 3, d = 0, and log₃ 1 = 0 = d, so case 2: Θ(n⁰ log n). Any search that keeps one third of the range per step has this shape.

</details>

2. T(n) = 4T(n/2) + O(n)
<details><summary>Answer</summary>

**Θ(n²).** Master: log₂ 4 = 2 > d = 1, case 1: the leaves dominate, and there are 4^(log₂ n) = n² of them.

</details>

3. T(n) = 2T(n/2) + O(n²)
<details><summary>Answer</summary>

**Θ(n²).** Master: log₂ 2 = 1 < d = 2, case 3. Level sums shrink: n², n²/2, n²/4, …, a geometric series bounded by 2n².

</details>

4. T(n) = 2T(n/2) + O(n log n)
<details><summary>Answer</summary>

**Θ(n log² n).** Extended case 2 (d = log₂ 2 = 1, k = 1). By the tree: level i costs about n·(log n − i), and summing over the log n levels gives about n·log² n / 2.

</details>

5. T(n) = T(n/4) + T(3n/4) + O(n)
<details><summary>Answer</summary>

**Θ(n log n).** Unequal splits, so draw the tree: every complete level costs cn in total, the shortest branch is log₄ n deep and the longest log₄/₃ n. So T(n) is between cn·log₄ n and cn·log₄/₃ n, and both are Θ(n log n). A quicksort that always splits 1 : 3 is still n log n.

</details>

6. T(n) = T(n − 1) + O(log n)
<details><summary>Answer</summary>

**Θ(n log n).** Unroll: log n + log(n − 1) + … + log 1 = log(n!), which is Θ(n log n). (It's at most n log n, and its larger half alone is at least (n/2)·log(n/2).)

</details>

7. T(n) = 3T(n − 1) + O(1)
<details><summary>Answer</summary>

**Θ(3ⁿ).** Unroll: 1 + 3 + 9 + … + 3ⁿ = (3ⁿ⁺¹ − 1)/2. Branching factor 3, depth n.

</details>

8. A memoized `solve(i, j)` for 0 ≤ i ≤ j < n, where each call loops over every k from i to j − 1 and combines `solve(i, k)` with `solve(k + 1, j)`.
<details><summary>Answer</summary>

**Θ(n³).** Distinct states × work per state: n(n + 1)/2 pairs (i, j), each doing O(j − i) work, and the sum is about n³/6. That's interval DP (module 16).

</details>

## 3. Space complexity — auxiliary vs total, recursion stack, in-place transforms

### What counts

- **Total space** = the input + everything else. **Auxiliary space** = everything else. "O(1) extra space" in an interview means auxiliary.
- **The output** usually doesn't count: "O(1) extra space, not counting the returned array" is the standard convention. Say it explicitly.
- **What does count:** every container you allocate, memo tables, hidden copies (`substr`, a `vector` passed by value, sorting a copy), and the **recursion stack**: maximum depth × frame size.

| Algorithm | Auxiliary space | Why |
|---|---|---|
| A loop, two pointers | O(1) | a few indices |
| Hash set of the values seen | O(n) | in the worst case every value goes in |
| `std::sort` | O(log n) | the recursion stack of introsort (quicksort that switches to heapsort if it recurses too deep) |
| Merge sort on an array | O(n) | the merge buffer (plus O(log n) of stack) |
| Quicksort | O(log n) on average, O(n) worst | the stack is as deep as the recursion; recursing into the smaller half first guarantees O(log n) |
| Recursive DFS on a tree of height h | O(h) | h = log n when balanced, n when it's a path |
| Recursive DFS on a graph | O(V) | a path graph is V calls deep |
| BFS | O(V) | the queue holds up to a whole level |
| Memoized recursion | O(states) + O(depth) | table plus stack |
| 2D DP over n × m | O(n·m), often O(m) | when row i only reads row i − 1, keep two rows |

### The recursion stack

Each call's frame holds its parameters, locals and return address: tens to hundreds of bytes. The main thread's stack is about 8 MB on Linux and macOS. Measured in this kit, a minimal recursive function survived depth 150,000 but not 200,000 under the sanitizer build, and not 300,000 even at `-O2`. So:
- depth up to ~10⁴: always safe;
- ~10⁵: fine with small frames, risky with big ones (a local array, many locals);
- ~10⁶: convert to iteration with an explicit stack (a `vector` on the heap). Module 10 section 1 shows the conversion.

Some judges raise the stack limit and many don't: never rely on it. When you claim "O(1) space" for a recursive solution, you're wrong by the stack: a recursive inorder traversal is O(h), not O(1).

### In-place transforms

Working inside the input instead of allocating a second array: reverse with two pointers; partition by swapping; compact with a write index; rotate by three reversals (module 04). When you need extra information per slot and the values leave room, pack it in: two values in [0, n) fit in one number as `a + n·b` (read back with `% n` and `/ n`; n² must fit in the type), and a sign bit can serve as a "seen" flag. The costs: you destroy the input (ask whether that's allowed) and the code gets subtler.

### Memory limits

- A typical limit of 256 MB holds about 6.7·10⁷ `int`s or 3.3·10⁷ `long long`s. Leave headroom: `vector<int>(10'000'000)` is 40 MB and fine; 10⁸ `int`s is 400 MB and isn't.
- A 5000 × 5000 `int` table is 100 MB: it fits in the limit, but only as a global or a `vector`, never as a local array (the stack is 8 MB).
- Every inner `vector` in a `vector<vector<int>>` costs its 24-byte header plus a separate heap allocation (tens of bytes of overhead each). A million tiny adjacency lists spend more on overhead than on data.
- `vector<bool>` packs 8 flags per byte: 10⁸ flags take 12.5 MB instead of 100 MB as `vector<char>`. The price: slower bit-level access, and elements are proxies (no `auto&`, no `bool*`). For a size fixed at compile time, `bitset<N>` is packed too and gives fast whole-set operations.

**Pitfalls**
- Claiming O(1) space for a recursive solution: the stack is O(depth).
- Hidden copies: `substr` in a loop, `auto row = grid[i]`, passing containers by value.
- A large local array (`int dp[5000][5000];` inside a function): stack overflow before your first line runs.
- Allocating inside a loop, per query or per test case: time and memory churn.
- Counting elements but forgetting their size: `long long` doubles it, and each inner `vector` has its own overhead.

**Recognize it when…**
- "O(1) extra space", "in-place", "without using another array", a `void` function that modifies its argument → two pointers, swaps, a write index, or packing values.
- Recursion depth equal to n, with n up to 10⁵–10⁶ → plan the iterative version.
- An OA states a memory limit and n is 10⁷–10⁸ → count the bytes: 4n for `int`, n/8 with bits.
- A DP table with 10⁸ cells → roll it (keep two rows) or find a smaller state.

## 4. Reading constraints — deriving the target complexity from n before you code

### The budget

Rule of thumb: optimized C++ does about **10⁸ simple operations per second**, and time limits are usually 1–2 seconds. So the total operation count at the maximum input should stay around 10⁸ or below. LeetCode doesn't publish per-problem limits, but the same budget works there. Heavy operations cost many simple ones: a `map`/`set` operation or a hash lookup is tens of steps, and an allocation more.

| n up to | Target complexity | Operations at that n | Typical techniques |
|---|---|---|---|
| 10–11 | O(n!), O(n!·n) | 11! ≈ 4·10⁷ | all permutations, backtracking |
| 20 | O(2ⁿ·n) | 2²⁰·20 ≈ 2·10⁷ | subsets, bitmask enumeration, bitmask DP |
| 40 | O(2^(n/2)·n) | 2²⁰·20 ≈ 2·10⁷ | meet in the middle |
| 500 | O(n³) | 1.25·10⁸ | triple loops, Floyd–Warshall, interval DP |
| 5 000 | O(n²) | 2.5·10⁷ | all pairs, 2D DP over two sequences |
| 10⁵ | O(n log n), even O(n√n) | 1.7·10⁶ / 3·10⁷ | sorting, heaps, binary search, trees |
| 10⁶ | O(n), O(n log n) | 10⁶ / 2·10⁷ | two pointers, prefix sums, hashing, counting, fast I/O |
| 10⁹ and beyond | O(log n), O(1); O(√n) up to ~10¹² | log₂ 10¹⁸ ≈ 60 | binary search on the answer, formulas, fast exponentiation |

Use it in both directions. Forwards: once you know the target, whole families of approaches drop out. Backwards: an unusually small limit is a hint. n ≤ 20 almost always means the intended solution is exponential; n ≤ 500 invites O(n³).

### Value ranges

- **Sums and products:** values up to 10⁹ with n up to 10⁵ sum to 10¹⁴: `long long`. A product of two values up to 10⁹ reaches 10¹⁸ and still fits in `long long` (max ≈ 9.2·10¹⁸); a product of three doesn't.
- **Small values** (up to 10⁶, letters, digits): index a counting array instead of using a map or sorting: O(n + V).
- **Large values used as indices** (up to 10⁹): coordinate-compress them (replace each value by its rank among the distinct values, so indices run 0…n−1), or use a hash map.
- **The value range can enter the complexity:** binary search on the answer over [lo, hi] costs (the check) × log₂(hi − lo), a factor of about 30 for values up to 10⁹.
- **"Return the answer modulo 10⁹ + 7":** the true count is astronomically large, so you count with DP or combinatorics instead of enumerating.

### Queries and test cases

- **q queries:** the total is (preprocessing) + q × (cost per query). With n = q = 10⁵, O(n) per query is 10¹⁰: too slow. Precompute so each query is O(1) or O(log n) (prefix sums in module 06, range structures in module 17, sort + binary search).
- **"The sum of n over all test cases is at most 2·10⁵":** the per-test work must scale with *that test's* n. Clearing a max-size array at the start of each of 10⁴ tests costs 10⁴ × 2·10⁵ = 2·10⁹ steps, even when every test is tiny.

**Pitfalls**
- Reading only n, and ignoring q, the value range or the number of test cases.
- Trusting the table at its edges: constants matter there. O(n log n) with `map<string, …>` at n = 10⁶, or O(n²) with a hash lookup inside at n = 5000, can both be too slow.
- Forgetting that the output bounds you from below: listing every subset is at least Θ(n·2ⁿ), whatever the algorithm.

**Recognize it when…**
- n ≤ 10, 20, 40 → exponential (permutations, subsets, meet in the middle).
- n ≤ 500 → cubic; n ≤ 5000 → quadratic; n ≤ 10⁵–10⁶ → n log n or linear; n ≥ 10⁹ → logarithmic or a formula.
- "q queries" on data that doesn't change → precompute.
- "The sum of n over all test cases…" → per-test work proportional to n, never to the maximum n.
- Values ≤ 10⁶ → a counting array; "modulo 10⁹ + 7" → DP or combinatorics.

Practice: [2006](https://leetcode.com/problems/count-number-of-pairs-with-absolute-difference-k/) and [2367](https://leetcode.com/problems/number-of-arithmetic-triplets/) are exercises in making this argument out loud. Before writing any code, compute the brute force's operation count at the maximum n and say whether it fits the budget; then decide whether "better" is needed or just nice.

### Drill: ten constraint blocks

For each block, write the target complexity and a candidate technique, then open the answer.

1. `1 ≤ n ≤ 8`. Return every ordering of n distinct items.
<details><summary>Answer</summary>

**O(n·n!)**: backtracking over permutations. The output alone is n! lists of length n (8!·8 ≈ 3.2·10⁵ numbers), so nothing faster is possible.

</details>

2. `1 ≤ n ≤ 20`, `1 ≤ w[i] ≤ 10⁹`. Is there a subset whose sum is exactly K?
<details><summary>Answer</summary>

**O(2ⁿ)**, or O(2ⁿ·n) with a bitmask loop: about 10⁶ subsets. Weights up to 10⁹ rule out a DP indexed by sum.

</details>

3. The same question with `1 ≤ n ≤ 40`.
<details><summary>Answer</summary>

2⁴⁰ ≈ 10¹² is too many, so **meet in the middle, O(2^(n/2)·n)**: list the 2²⁰ subset sums of each half, sort one list, and for each sum s in the other, binary-search for K − s. About 2·10⁷ steps.

</details>

4. `1 ≤ n ≤ 400` cities joined by weighted roads; answer up to 10⁵ queries "shortest distance from u to v".
<details><summary>Answer</summary>

**O(n³) once, then O(1) per query**: all-pairs shortest paths (Floyd–Warshall, module 15) is 6.4·10⁷ steps. Running a single-source search for each query instead would repeat work 10⁵ times.

</details>

5. Two strings, each of length at most 2000. What's the minimum number of single-character insertions, deletions and replacements that turns one into the other?
<details><summary>Answer</summary>

**O(n·m) = 4·10⁶**: a 2D DP over pairs of prefixes (edit distance, module 16).

</details>

6. `1 ≤ n, q ≤ 10⁵`. An array that never changes; each query asks for the sum of `a[l..r]`.
<details><summary>Answer</summary>

**O(n + q)**: prefix sums built once, then O(1) per query. Summing each range directly is O(n·q) = 10¹⁰.

</details>

7. `1 ≤ n ≤ 2·10⁵`, `|a[i]| ≤ 10⁹`. Count the pairs i < j with `a[i] + a[j] ≤ K`.
<details><summary>Answer</summary>

**O(n log n)**: sort, then two pointers (or a binary search per element); the O(n²) pair loop would be 2·10¹⁰. Watch the types: the count can reach about 2·10¹⁰ and `a[i] + a[j]` can reach 2·10⁹, so both need `long long`.

</details>

8. `1 ≤ n ≤ 10⁶`, `0 ≤ a[i] ≤ 10⁶`. Print the values in non-decreasing order.
<details><summary>Answer</summary>

**O(n + V)** with a counting sort, or O(n log n) with `sort` (about 2·10⁷ comparisons): both fit. At this size the I/O matters as much as the algorithm: a million numbers in and out need the fast-I/O lines and `'\n'`.

</details>

9. `1 ≤ T ≤ 10⁴` test cases, each with `1 ≤ n ≤ 2·10⁵`; the sum of n over all test cases is at most 2·10⁵.
<details><summary>Answer</summary>

**O(n log n) or O(n) per test**, since the total is bounded by Σn. The trap is any per-test cost tied to the *maximum* n: clearing a 2·10⁵-element array for every test is 2·10⁹ steps. Size and reset everything by this test's n.

</details>

10. `1 ≤ n ≤ 10¹⁸`. Print F(n) modulo 10⁹ + 7.
<details><summary>Answer</summary>

**O(log n)**: fast exponentiation of the 2×2 Fibonacci matrix, about 60 squarings. Anything linear in n would take 10¹⁸ steps.

</details>

## Common mistakes

| Mistake | Why it's wrong | Instead |
|---|---|---|
| "Two nested loops, so O(n²)" for a sliding window | the inner pointer never moves back: at most n moves in total | count total pointer moves (amortized O(n)) |
| Counting iterations and ignoring what each line costs | `s + c`, `substr`, `insert` at the front and copies are O(n) each | know the cost of every library call you make |
| Ignoring the recursion stack in the space bound | a recursive DFS on a path is O(n) space | add depth × frame to the bill |
| Confusing average-case with amortized | amortized holds for every sequence; average depends on the input distribution | say which one, and why |
| "O(2n)", "O(n + n/2)" | constants don't belong inside O | O(n) |
| One n for two inputs | O(n²) vs O(n·m) matters when n ≫ m | name each input |
| Hashing or comparing strings as O(1) | each costs O(L) | multiply by the string length |
| Checking only n against the table | q, the value range and Σn over tests change the target | read the whole constraints block |
| Resetting max-size arrays per test case | 10⁴ tests × 2·10⁵ = 2·10⁹ steps | reset only the first n entries |
| Trusting `double` for exact integer answers | exact only up to 2⁵³; Binet's formula fails at n = 71 | integers, or verify with integers |

## Say it out loud

How to narrate complexity, using [509. Fibonacci Number](https://leetcode.com/problems/fibonacci-number/):

> "The definition gives a recursive solution directly. Each call makes two more, and the call count follows the Fibonacci recurrence itself: 2·F(n + 1) − 1 calls, so it's exponential, about 1.6ⁿ. At n = 30 that's 2.7 million calls, which passes, but it doesn't scale. There are only n + 1 distinct arguments, so memoizing brings it to O(n) time, with O(n) space for the table and the stack. And each value only needs the previous two, so a loop with two variables gives O(n) time and O(1) space. Edge cases: n = 0 and 1, and `int` overflows past F(46)."

A reusable shape for any solution: *"The outer loop runs n times. The inner pointer only moves forward, so across the whole run it moves at most n times: O(n) total, not O(n²). The map holds at most k keys, so O(k) space."*

Follow-ups interviewers ask:
- *Can you do better?* Compare with a lower bound: you must read the input (Ω(n)); comparison sorting needs Ω(n log n); the output size is a floor.
- *What's the space, including the recursion?* Depth × frame, plus every container you allocate.
- *Worst case or average?* Hash maps are O(1) on average and O(n) in the worst case.
- *Is that amortized?* `push_back`, a monotonic stack and a two-stack queue are: say so, and name the argument.
- *What if there are q queries?* Move work into preprocessing so each query gets cheaper.
- *What if the input doesn't fit in memory?* Process it as a stream in one pass with O(1) state, or sort it in chunks on disk (external sort).

## Self-check

1. State the formal definition of f(n) = O(g(n)). Is n = O(n²)? Is n² = O(n)?
<details><summary>Answer</summary>

There exist c > 0 and n₀ such that 0 ≤ f(n) ≤ c·g(n) for all n ≥ n₀. n = O(n²) is true (a loose upper bound, with c = 1, n₀ = 1). n² = O(n) is false: n² ≤ c·n fails as soon as n > c.

</details>

2. What's the difference between average-case and amortized cost? Give an example of each.
<details><summary>Answer</summary>

Average case averages over a distribution of inputs (or random choices): hash lookups are O(1) on average, quicksort with random pivots O(n log n) expected. Amortized averages over a worst-case sequence of operations, with no probability involved: n `push_back`s cost O(n) in total, whatever the sequence.

</details>

3. Why must a dynamic array grow by a factor rather than by a constant amount? What would growing by 1000 slots cost for n pushes?
<details><summary>Answer</summary>

With a factor, the moves form a geometric series below 3n: O(1) amortized. With +1000, a reallocation happens every 1000 pushes and moves everything: 1000 + 2000 + … ≈ n²/2000 moves in total, Θ(n) per push on average.

</details>

4. Solve T(n) = 2T(n/2) + O(n) and T(n) = 3T(n/2) + O(n).
<details><summary>Answer</summary>

First: log₂ 2 = 1 = d, case 2: Θ(n log n) (merge sort). Second: log₂ 3 ≈ 1.585 > 1, case 1: Θ(n^(log₂ 3)) ≈ Θ(n^1.585) (Karatsuba).

</details>

5. How many calls does naive `fib(n)` make, and why does memoization make it Θ(n)?
<details><summary>Answer</summary>

C(n) = 2·F(n + 1) − 1, since C(n) = 1 + C(n − 1) + C(n − 2): exponential, about φⁿ. With a memo, each of the n + 1 distinct arguments is expanded once at O(1) work, and every other call is a lookup.

</details>

6. What's the auxiliary space of a recursive DFS on a path of n nodes, and of the same DFS with an explicit stack?
<details><summary>Answer</summary>

Both are O(n): the call stack reaches depth n, and the explicit stack can hold n entries. The explicit stack lives on the heap, though, so it doesn't overflow the ~8 MB thread stack at n = 10⁶.

</details>

7. What's the target complexity for n ≤ 20? For n ≤ 5000? For n ≤ 10⁵?
<details><summary>Answer</summary>

O(2ⁿ·n) (subsets, bitmasks); O(n²); O(n log n).

</details>

8. Is `std::lower_bound(s.begin(), s.end(), x)` on a `std::set` O(log n)?
<details><summary>Answer</summary>

No, it's O(n): set iterators are bidirectional, so each "jump" in the binary search walks step by step. The member `s.lower_bound(x)` walks the tree in O(log n).

</details>

9. What does sorting n strings of length L cost?
<details><summary>Answer</summary>

O(n log n) comparisons, each O(L) in the worst case: O(L·n log n).

</details>

10. The sum of n over 10⁴ test cases is at most 2·10⁵, yet your solution times out. Your code clears a global array of 2·10⁵ entries at the start of every test. Why is that the problem?
<details><summary>Answer</summary>

The clearing costs 2·10⁵ per test regardless of that test's n: 10⁴ × 2·10⁵ = 2·10⁹ steps. Clear only the first n entries, or allocate per test at size n.

</details>
