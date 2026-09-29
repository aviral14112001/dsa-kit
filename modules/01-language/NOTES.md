# 01 · Programming Language Concepts (C++)
> Pick one language and know it well enough that syntax never costs you a round.

**Time:** ~7 h of Core work ([problems](problems.md)) · **Prereqs:** none. You know C#; this module maps it onto C++ · **You're done when:** (1) you can type the LeetCode skeleton and the stdin template from memory in under 2 minutes each; (2) you can predict every `CHECK` in `examples/value_vs_reference.cpp` and `examples/overflow.cpp` before running them; (3) every Core box is ticked, and you can re-solve 1636 and CSES 1068 from a blank file without looking anything up.

Every `cpp` block below is pulled from a file in [`examples/`](examples/) that `make test M=modules/01-` compiles and runs, so it's known to work. Read `CHECK_EQ(actual, expected)` as "this is what you get".

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Language choice | C++ is fast and its STL has every structure you need; judges run GCC/Clang; this kit's sanitizers turn silent bugs into crashes | [`_template.cpp`](../../practice/_template.cpp), [`_cp_template.cpp`](../../practice/_cp_template.cpp) | 1929, 1480 · worked example: CSES 1068 |
| 2 | Types & memory | Every type copies like a C# struct unless you write `&`; signed overflow is undefined behaviour | [value_vs_reference.cpp](examples/value_vs_reference.cpp), [overflow.cpp](examples/overflow.cpp) | 7, 1822 |
| 3 | Standard library fluency | Each C# collection has an STL twin; `sort` plus a comparator replaces LINQ ordering | [stl_tour.cpp](examples/stl_tour.cpp), [comparators.cpp](examples/comparators.cpp) | 217, 242, 1636 📖, 1768 |
| 4 | Idioms & fast I/O | Fast I/O, the three input shapes, lambdas, `MOD`/`INF`, grids | [fast_io.cpp](examples/fast_io.cpp), [idioms.cpp](examples/idioms.cpp) | the Drill: both templates from memory |

## 1. Language choice — C++, the judge environment, this kit's toolchain

### Why C++, and what it costs

| | Strengths in a DSA round | Costs |
|---|---|---|
| **C++** | Fastest mainstream option; the STL (Standard Template Library: C++'s built-in containers and algorithms) has a hash map, a balanced BST with `lower_bound`, a heap and a deque; most competitive-programming editorials and references use it | Bugs are silent: an out-of-bounds read returns garbage instead of throwing, and overflow is undefined; no big integers; long template error messages |
| Python | Shortest code; big integers built in | Often 10× or more slower in tight loops, so O(n) at n = 10⁶ can time out; default recursion limit of 1000 |
| Java | C#'s speed class; `TreeMap` has floor/ceiling | Verbose; boxing overhead in collections; needs a fast input reader |
| C# | The language you know best; `SortedSet`, `SortedDictionary`, `PriorityQueue` (.NET 6+) | Fewer interview resources use it; `SortedSet` has no iterator-style `lower_bound` (you get `GetViewBetween`) |

Interviewers grade correctness and communication, not the language. C++ buys you speed and the STL; you pay with bugs that don't announce themselves. The worst kind is **undefined behaviour (UB)**: code the standard gives no meaning to (signed overflow, reading past an array's end). The program may crash, print garbage, or appear to work until the judge's input. The sanitizers in this kit give you C#'s safety net back while you practise.

### What judges give you

- **LeetCode:** you write only `class Solution`. The standard headers are already included, and a hidden `main` calls your method once per test case.
- **OA platforms (HackerRank, HackerEarth, CodeSignal) and CSES/Codeforces:** often a whole program: read stdin, print stdout, and the output is compared with the expected text. Some platforms instead give you a function stub with `main` already written: fill in the function and print nothing extra.
- **Compiler:** usually a recent GCC (sometimes Clang) with C++17 or C++20 picked from a dropdown. If you can't tell which, avoid C++20-only features or know the fallback: `m.count(k)` for `m.contains(k)`, a hand-written `operator<` for `<=>`, the erase–remove idiom for `erase_if`.
- **`#include <bits/stdc++.h>`** is a GCC header that pulls in the whole standard library, so GCC-based judges have it. Apple clang doesn't, which is why this kit ships a shim in `include/bits/`.
- Judges compile with optimisation and **without** sanitizers: undefined behaviour can pass locally and fail there, or the reverse.

### This kit's toolchain

| Command | What it does |
|---|---|
| `make run F=practice/01/x.cpp` | Compile with `clang++ -std=c++20`, sanitizers, warnings and `-DLOCAL`, then run |
| `make run F=… IN=x.in` | The same, with `x.in` fed to stdin |
| `make test` / `make test M=modules/01-` | Build and run every example and template test / only paths containing `modules/01-`. Stdin examples are diffed against their `.out` files |

Why those flags (see the [Makefile](../../Makefile)):

- `-fsanitize=address` (ASan): out-of-bounds reads and writes, use-after-free and stack overflow stop the program with a file:line report, instead of reading garbage or passing by luck.
- `-fsanitize=undefined` + `-fno-sanitize-recover=undefined` (UBSan): signed overflow, invalid shifts and null dereferences abort at the first occurrence. Change `ll n` to `int n` in the worked example below, feed it 113383, and you get:
  ```text
  runtime error: signed integer overflow: 3 * 827370449 cannot be represented in type 'int'
  ```
  A judge would only say "Wrong Answer".
- `-g -fno-omit-frame-pointer -O1`: readable stack traces, still fast enough for large tests.
- `-Wall -Wextra -Wshadow`: warnings. `make test` adds `-Werror`; `make run` only prints them. Read them: `-Wsign-compare` and `-Wshadow` point at real bugs.
- `-DLOCAL` (`make run` only) switches on the `dbg()` macro, which prints to stderr on your machine and compiles to nothing on the judge (Section 4).

The sanitizers do **not** catch reads of uninitialized locals (`int x;` holds garbage; the compiler warns about some cases). Initialize everything: `int best = 0;`, `int cnt[26] = {};`.

### Editor setup

- Open `dsa/` itself as the VS Code folder (File → Open Folder… → `dsa`). `.vscode/settings.json` is read only from the workspace root; it sets C++20, clang++ and the `include/` + `templates/` paths so `#include "test.hpp"` resolves. Open a parent folder instead and every include gets a red squiggle.
- The Microsoft C/C++ extension reads `.vscode/settings.json`; the clangd extension reads `compile_flags.txt`. Run one engine: with clangd installed, set `"C_Cpp.intelliSenseEngine": "disabled"`.
- Run `make` from the integrated terminal, which opens in `dsa/`.

### Your workflow

```bash
mkdir -p practice/01
cp practice/_template.cpp practice/01/1929-concatenation-of-array.cpp   # LeetCode-style: Solution + CHECK_EQ
cp practice/_cp_template.cpp practice/01/cses-1068.cpp                  # stdin/stdout
make run F=practice/01/1929-concatenation-of-array.cpp
make run F=practice/01/cses-1068.cpp IN=modules/01-language/examples/cses-1068-weird-algorithm.in
```

In the LeetCode template, paste the method signature into `Solution`, type the examples as `CHECK_EQ`s, add edge cases, run. The stdin template already has the fast-I/O lines, `using ll`, the `dbg` macro and a `solve()` per test case (Section 4).

**Pitfalls**
- Debug output on stdout fails an exact-match judge. Print to `cerr` (the template's `dbg` does), behind `#ifdef LOCAL`.
- A prompt like `cout << "Enter n: "` is output too. Never print what the statement didn't ask for.
- C++20 features on a judge set to C++17: a compile error in the middle of an OA. Check the dropdown first.
- LeetCode runs every test case in one process, so global and static variables keep their values between cases. Reset them, or keep state in locals and members.

**Recognize it when…**
- A class with one method signature → the LeetCode shape: no reading, no printing, return the answer.
- "The first line contains…", "Print…", a sample input block → a full stdin/stdout program with fast I/O.
- A stub function under a locked `main` → fill in the function only.
- `a[i] ≤ 10⁹` and anything summed or multiplied → `long long`, before you write another line (Section 2).

### Worked example: CSES 1068. Weird Algorithm
[CSES 1068](https://cses.fi/problemset/task/1068) · Easy

**Problem (paraphrased):** start from n (n ≤ 10⁶). While n ≠ 1, halve it if it's even, otherwise replace it with 3n + 1. Print every value, from n down to the final 1, on one line.
**Signals:** "print every value" means simulate; a `3n + 1` step means values can outgrow n; an input/output format means a full program.
**Brute force, and why it fails:** the simulation *is* the solution, and the sequences are short (the longest for n ≤ 10⁶ has 525 values, starting from 837799). What fails is the type: from 113383 the sequence passes 2³¹ − 1, and from 704511 it peaks at 56,991,483,520. With `int` that's undefined behaviour, and on a judge usually a wrong answer.
**Key insight:** size your types by the largest *intermediate* value, not by the input.
**Dry run:** n = 3.

| n | parity | next |
|---|---|---|
| 3 | odd | 10 |
| 10 | even | 5 |
| 5 | odd | 16 |
| 16, 8, 4, 2 | even | halve until 1 |

Output: `3 10 5 16 8 4 2 1`.

<!-- snippet: modules/01-language/examples/cses-1068-weird-algorithm.cpp#solution -->
```cpp
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ll n;                     // NOT int: starting from 113383 (or higher), 3n + 1 passes 2^31 - 1
    cin >> n;
    cout << n;
    while (n != 1) {
        n = (n % 2 == 0) ? n / 2 : 3 * n + 1;
        cout << ' ' << n;     // one line, space-separated; '\n' only once at the end
    }
    cout << '\n';
}
```
<!-- /snippet -->

**Complexity:** O(L) time for a sequence of L values (L ≤ 525 here), O(1) space: each value is printed as it's produced.
**Edge cases:**
- n = 1: print just `1`; the loop never runs (`cses-1068-weird-algorithm.3.in`).
- n = 704511: the peak 56,991,483,520 needs 64 bits (`.2.in`).

**Follow-ups:**
- *Why `long long` when n ≤ 10⁶?* The intermediate values decide the type, not the input.
- *What if n could be 10¹⁸?* `3n + 1` can overflow even `long long`: test `n > (LLONG_MAX - 1) / 3` before multiplying, or use GCC/Clang's `__int128`.
- *Printing millions of numbers?* Keep `'\n'` rather than `endl`, and the two fast-I/O lines (Section 4).

## 2. Types & memory — value vs reference semantics, mutability, integer overflow

### Everything is a value type

C# has two kinds of types: classes (assignment copies a reference, so both names share one object) and structs (assignment copies the value). In C++ **every** type, including `vector`, `string`, `map` and your own structs, behaves like a C# struct. Assignment, pass-by-value and `auto x = …` copy the whole object, down to the last element of a vector. Sharing is always explicit: a reference (`&`) or a pointer (`*`).

<!-- snippet: modules/01-language/examples/value_vs_reference.cpp#copy_on_assign -->
```cpp
vector<int> a{1, 2, 3};
vector<int> b = a;       // deep copy. In C#, `var b = a;` would share one List
b[0] = 99;
CHECK_EQ(a[0], 1);       // a is untouched
CHECK_EQ(b[0], 99);
```
<!-- /snippet -->

| C# | C++ |
|---|---|
| `var b = a;` (a `List`) shares | `auto& b = a;` shares · `auto b = a;` copies every element |
| `void F(List<int> xs)` shares | `void f(vector<int>& xs)` shares · `void f(vector<int> xs)` copies |
| `ref int x` | `int& x` |
| `in` (read-only reference) | `const T& x` |
| `null` | `nullptr`, for pointers only: a reference can't be null |
| `string` is immutable | `std::string` is mutable (`s[0] = 'b'` works), and copied on assignment like everything else |

Parameter rule: to read a big object, `const T&`; to modify the caller's object, `T&`; for small types (`int`, `char`, `double`, `pair<int,int>`) or a copy you mean to change locally, by value. LeetCode hands you `vector<int>& nums`: you may sort or overwrite it in place, and the caller sees the change.

`auto` drops references, so `auto x = v[i]` is a copy. It bites hardest in range-for:

<!-- snippet: modules/01-language/examples/value_vs_reference.cpp#auto_copy -->
```cpp
vector<int> v{1, 2, 3};
auto x = v[0];           // auto drops the reference: x is a copy
x = 100;
CHECK_EQ(x, 100);
CHECK_EQ(v[0], 1);       // the copy changed, v didn't
auto& y = v[0];          // auto& is an alias for v[0]
y = 100;
CHECK_EQ(v[0], 100);

vector<vector<int>> grid{{1, 2}, {3, 4}};
for (auto row : grid) row[0] = 0;    // row is a COPY of each row: grid unchanged
CHECK_EQ(grid[0][0], 1);
for (auto& row : grid) row[0] = 0;   // row aliases the real row
CHECK_EQ(grid[0][0], 0);
```
<!-- /snippet -->

`for (auto row : grid)` copies every row it visits, so each pass over the grid copies the whole grid. Default to `for (const auto& row : grid)`, and `auto&` when you modify.

### References vs pointers

| | Reference: `int& r = x;` | Pointer: `int* p = &x;` |
|---|---|---|
| Can be null | no | yes (`nullptr`) |
| Can be re-pointed | no: bound once, at creation | yes: `p = &y;` |
| Access | `r` is just a second name | `*p`, and `p->field` for structs |
| Use it for | parameters, aliases in loops | linked structures: LeetCode's `ListNode*`, `TreeNode*` |

Pointers are the part of C++ that behaves like C# references: copying a pointer shares the node. Copying the *struct* is shallow: its pointer members still point at the same nodes.

<!-- snippet: modules/01-language/examples/value_vs_reference.cpp#node_pointers -->
```cpp
ListNode* head = make_list({1, 2, 3});
ListNode* p = head;      // copies the address: both name the same node (like C# references)
p->val = 10;
CHECK_EQ(head->val, 10);
ListNode copy = *head;   // copies the struct: val AND the next pointer (a shallow copy)
copy.val = 20;
copy.next->val = 30;     // ...so this reaches the real second node
CHECK_EQ(head->val, 10);
CHECK_EQ(head->next->val, 30);
```
<!-- /snippet -->

### Stack, heap, and RAII

- Locals live on the **stack**: created at their declaration, destroyed at the end of their scope. The main thread gets about 8 MB of stack on Linux and macOS.
- A `vector` object is three pointers (24 bytes) wherever it lives; its elements are on the **heap**. So `vector<int> v(10'000'000)` (40 MB) is fine as a local, but a local `int a[10'000'000];` puts 40 MB on the stack and crashes. Big fixed arrays go global, or into a vector.
- There's no garbage collector. Every object is destroyed at a known point, and its destructor releases what it owns: containers free their memory, files close. That's **RAII** (resource acquisition is initialization): C#'s `using`/`Dispose`, applied automatically to every object. `value_vs_reference.cpp#raii_demo` creates five objects three ways (a local, a `unique_ptr`, a `vector` of three) and checks that all five are destroyed at the same `}`.
- `new T(…)` / `delete p` allocate and free by hand. In DSA you only write `new` for LeetCode nodes (`new ListNode(x)`), and you never `delete` them: LeetCode doesn't check, and this kit's test runner turns leak detection off. `unique_ptr`/`shared_ptr` automate the `delete`, but LeetCode's node types use raw pointers, so stay raw.
- **Returning a local container by value is cheap.** The compiler either builds it directly in the caller's variable or moves it, and a move hands over the buffer pointer in O(1) (`value_vs_reference.cpp` checks that the buffer's address survives). Write `vector<int> f()`, not output parameters. Never return a reference or pointer to a local: the local dies at its `}`.
- To move explicitly, write `std::move(x)`, with the `std::`: clang warns about a bare `move(x)` (`value_vs_reference.cpp#move`).

### Integer types

| Type | Size here | Range | Use for |
|---|---|---|---|
| `int` | 32 bits | ±2,147,483,647 (≈ 2.1·10⁹) | values and indices that provably stay under ~2·10⁹ |
| `long long` | 64 bits | ±9.22·10¹⁸ | sums, products, counts of pairs: anything that can pass 2·10⁹ |
| `size_t` (what `.size()` returns) | 64 bits, **unsigned** | 0 … 1.8·10¹⁹ | container sizes. `0 - 1` wraps to 1.8·10¹⁹ |
| `char` | 8 bits | −128 … 127 here (signedness varies by platform) | characters; `c - 'a'` for indices |
| `double` | 64 bits | ~15–16 significant digits; integers exact only up to 2⁵³ | geometry and averages, never exact counts |

C#'s `int` and `long` are always 32 and 64 bits; C#'s `long` is C++'s `long long`. Avoid plain `long`: 64-bit on Linux and macOS, 32-bit on Windows. A C# `char` is a 16-bit UTF-16 unit; a C++ `char` is one byte.

### Overflow is undefined behaviour, not wrap-around

C# wraps silently by default and throws inside `checked`. In C++, **signed** overflow is undefined behaviour: the optimiser assumes it can't happen, so anything goes, including deleting your overflow check `if (x + 1 < x)`. **Unsigned** arithmetic wraps modulo 2^bits (2⁶⁴ for `size_t`): defined, but just as wrong for your answer.

<!-- snippet: modules/01-language/examples/overflow.cpp#widen_first -->
```cpp
int a = 100'000, b = 100'000;
// long long bad = a * b;       // int * int overflows BEFORE the result is widened: UB
long long good = 1LL * a * b;   // 1LL makes the whole product 64-bit from the first step
CHECK_EQ(good, 10'000'000'000LL);
CHECK(good > INT_MAX);          // proof that a * b could never have fit in an int

// int bad_mask = 1 << 40;      // shifting a 32-bit int by 40 bits: UB
long long mask = 1LL << 40;
CHECK_EQ(mask, 1'099'511'627'776LL);
```
<!-- /snippet -->

`long long c = a * b;` is still wrong: the product is computed in `int` and only then widened. Put `1LL *` first (or cast an operand) so every step is 64-bit. The same trap hides in three idioms:

- **Midpoints:** `(lo + hi) / 2` overflows when both are near 2·10⁹. Write `lo + (hi - lo) / 2` (as `templates/binary_search.hpp` does) or C++20 `midpoint(lo, hi)`.
- **Negation and `abs`:** `-INT_MIN`, `abs(INT_MIN)` and `INT_MIN / -1` all overflow.
- **`accumulate`:** the running total has the type of the *initial value*.

<!-- snippet: modules/01-language/examples/overflow.cpp#accumulate -->
```cpp
vector<int> big(3, 1'000'000'000);
// accumulate(big.begin(), big.end(), 0)   // the running sum is an int (the init's type): UB
long long total = accumulate(big.begin(), big.end(), 0LL);
CHECK_EQ(total, 3'000'000'000LL);

vector<double> halves{0.5, 0.5, 0.5};
CHECK_EQ(accumulate(halves.begin(), halves.end(), 0), 0);   // int init: every step truncates
CHECK_NEAR(accumulate(halves.begin(), halves.end(), 0.0), 1.5, 1e-12);
```
<!-- /snippet -->

### Unsigned sizes

<!-- snippet: modules/01-language/examples/overflow.cpp#size_minus_one -->
```cpp
vector<int> empty;
CHECK_EQ(empty.size() - 1, SIZE_MAX);   // size() is unsigned: 0 - 1 wraps to 18446744073709551615
// for (size_t i = 0; i < v.size() - 1; i++)   on an empty v: ~forever, reading garbage
int iterations = 0;
for (int i = 0; i + 1 < (int)empty.size(); i++) iterations++;   // cast once, stay signed
CHECK_EQ(iterations, 0);
```
<!-- /snippet -->

Comparing a negative `int` with a size converts the `int` to unsigned, so `-1 < v.size()` is **false**. `-Wsign-compare` warns; write `i < (int)v.size()` or C++20 `cmp_less(i, v.size())` (`overflow.cpp#signed_unsigned`).

### Characters and integer division

<!-- snippet: modules/01-language/examples/overflow.cpp#char_math -->
```cpp
string word = "banana";
int count[26] = {};             // all zeros; index = letter - 'a'
for (char c : word) count[c - 'a']++;
CHECK_EQ(count['a' - 'a'], 3);
CHECK_EQ(count['n' - 'a'], 2);
CHECK_EQ('7' - '0', 7);         // digit character -> its value
char third = char('a' + 2);     // 'a' + 2 is the int 99: cast back to get a char
CHECK_EQ(third, 'c');
CHECK_EQ(string(1, char('0' + 5)), "5");
```
<!-- /snippet -->

<!-- snippet: modules/01-language/examples/overflow.cpp#division -->
```cpp
CHECK_EQ(7 / 2, 3);
CHECK_EQ(-7 / 2, -3);           // division truncates toward zero (same as C#)
CHECK_EQ(-7 % 3, -1);           // so % can be negative: it takes the dividend's sign
int m = 3;
CHECK_EQ(((-7 % m) + m) % m, 2);   // always lands in [0, m)
int total = 7, per_box = 2;
CHECK_EQ((total + per_box - 1) / per_box, 4);   // ceil(a / b) for positive ints, no floating point
```
<!-- /snippet -->

`((x % m) + m) % m` is the workhorse of modular arithmetic: you need it whenever a subtraction can go negative (Section 4).

### Floating point

<!-- snippet: modules/01-language/examples/overflow.cpp#floating -->
```cpp
double sum = 0.1 + 0.2;
CHECK(sum != 0.3);                    // 0.1 has no exact binary representation
CHECK(fabs(sum - 0.3) < 1e-9);        // compare with a tolerance instead

long long x = 1'000'000'000'000'000'000LL - 1;   // 10^18 - 1: its root is 999'999'999.99...
long long r = (long long)sqrt((double)x);       // but (double)x rounds up to exactly 1e18...
CHECK_EQ(r, 1'000'000'000LL);                    // ...so the "floor of the root" is one too big
CHECK(r * r > x);                                // an exact integer check exposes it
CHECK((double)(1LL << 53) == (double)((1LL << 53) + 1));   // doubles are exact only up to 2^53
```
<!-- /snippet -->

Prefer integers whenever the answer is exact: compare fractions `a/b < c/d` as `1LL * a * d < 1LL * c * b` (for positive denominators), and verify a floating-point result with exact integer arithmetic, as the `r * r > x` check does above. Never use `pow` for integer powers: it returns a `double`.

### Recursion depth

Every call pushes a stack frame. On the ~8 MB default stack, a minimal recursive function in this kit survived depth 150,000 but crashed at 200,000 under the sanitizer build (and at 300,000 with plain `-O2`); frames with more locals die sooner. Depth 10⁴ is always fine, 10⁵ usually is, 10⁶ isn't. When the depth grows with n and n can reach 10⁵–10⁶ (a DFS down a path-shaped graph, recursion along a long list), use an explicit stack (module 10 section 1). ASan reports `stack-overflow`; a judge says "Runtime Error".

**Pitfalls**
- Accidental copies: a `vector` parameter taken by value inside recursion or a loop turns O(n) into O(n²); range-for by value copies every element.
- A reference into a vector dangles once `push_back` reallocates: after `auto& first = v[0]; v.push_back(x);`, `first` may point at freed memory (ASan: heap-use-after-free).
- `vector<bool>` stores packed bits: `auto b = flags[i]` is a proxy that still refers to the bit (the one place `auto x = v[i]` doesn't copy), and `auto& b = flags[i]` doesn't compile.
- `int` overflow in products, sums, midpoints and `accumulate(…, 0)`; `1 << 40` needs `1LL << 40`.
- `v.size() - 1` on an empty vector; `i < v.size()` with a negative `i`.
- Uninitialized locals; `==` on doubles; `pow` for integer powers.

**Recognize it when…**
- `a[i] ≤ 10⁹` (or `10⁵` with n ≥ 10⁵) and you sum or multiply → `long long`. Estimate n × max before you pick a type.
- "Return it modulo 10⁹ + 7" → the true answer is huge: `long long` intermediates, reduced after every operation.
- Counting pairs or subarrays with n ≥ 10⁵ → up to n²/2 ≈ 5·10⁹: the count itself needs `long long`.
- "Reverse the digits", "parse an integer", "the result must fit in 32 bits" → an explicit overflow check *before* the operation that would overflow (7).
- "Multiplying these would overflow, but you only need a property of the result" → track just that property (1822).
- Recursion depth proportional to n, with n ≥ 10⁵ → think about the stack before you code.

## 3. Standard library fluency — collections, sorting, iterators, string builders

### From C# collections to the STL

| C# | C++ | What changes |
|---|---|---|
| `List<T>` | `vector<T>` | `vector<int> v(5)` is **five zeros**; `new List<int>(5)` is empty with capacity 5, which is `v.reserve(5)` |
| `Dictionary<K,V>` | `unordered_map<K,V>` | `m[k]` **inserts** a default value when k is missing; `m.at(k)` throws, like C#'s indexer |
| `SortedDictionary<K,V>` | `map<K,V>` | a balanced BST (red–black tree): `lower_bound`, min = `begin()`, max = `rbegin()` |
| `HashSet<T>` | `unordered_set<T>` | |
| `SortedSet<T>` | `set<T>`, `multiset<T>` | member `lower_bound`/`upper_bound` replace `GetViewBetween`; `multiset` keeps duplicates |
| `PriorityQueue<T,P>` | `priority_queue<T>` | **max-heap** by default (C#'s is a min-heap). Min-heap: `priority_queue<T, vector<T>, greater<T>>`. Neither has decrease-key: push the new entry, skip stale ones as they come out |
| `Queue<T>` / `Stack<T>` | `queue<T>` / `stack<T>` | `push`, `front()`/`top()`, `pop()`. **`pop()` returns void**: read, then pop |
| (none) | `deque<T>` | O(1) at both ends, plus indexing |
| `LinkedList<T>` | `list<T>` | rarely worth it; O(1) erase/splice at an iterator (LRU cache, module 08) |
| `StringBuilder` | `string` with `+=`, or `ostringstream` | `std::string` is mutable, and `+=` is amortized O(1) per character |
| `(int, string)` tuples | `pair`, `tuple` + structured bindings | compare lexicographically: `.first`, then `.second` |
| LINQ `OrderBy(…).ThenBy(…)` | `sort` with a comparator | `stable_sort` keeps ties in input order |
| `Array.BinarySearch` | `lower_bound`, `binary_search` | `lower_bound` returns the insertion point itself, not `~index` |
| `string.Split` | `stringstream` + `getline(ss, part, ',')`, or `>>` | |
| `.Count`, `List.Contains` | `.size()` (unsigned!), `find(…) != v.end()` (O(n)) | |

### What each operation costs

| Container | Index | Find | Insert / erase | Also |
|---|---|---|---|---|
| `vector`, `string` | O(1) | O(n); O(log n) when sorted (`lower_bound`) | O(1)\* at the back, O(n) elsewhere | contiguous: the fastest to scan |
| `deque` | O(1) | O(n) | O(1) at both ends, O(n) in the middle | |
| `list` | none | O(n) | O(1) at an iterator | `size()` is O(1) |
| `map`, `set`, `multiset` | none | O(log n) | O(log n) | sorted iteration, `lower_bound`, min/max |
| `unordered_map`, `unordered_set` | none | O(1) average, O(n) worst | O(1) average | no order; adversarial inputs can force collisions |
| `priority_queue` | top only, O(1) | none | push/pop O(log n) | building one from a whole vector is O(n) |
| `queue`, `stack` | front/top O(1) | none | O(1) | adapters over `deque` |

\* amortized: an occasional `push_back` copies everything (O(n)), but averaged over many pushes each costs O(1). Module 03 section 1 proves it.

<!-- snippet: modules/01-language/examples/stl_tour.cpp#unordered_map -->
```cpp
unordered_map<string, int> freq;                    // Dictionary<string, int>
vector<string> words{"to", "be", "or", "not", "to", "be"};
for (const string& w : words) freq[w]++;            // [] creates a missing key with value 0
CHECK_EQ(freq["to"], 2);
CHECK_EQ(freq.size(), 4);
CHECK(freq.contains("or"));                         // C++20; count(key) == 1 works everywhere
CHECK(freq.find("xyz") == freq.end());              // find() never inserts
int missing = freq["xyz"];                          // [] DOES insert, even when you only read
CHECK_EQ(missing, 0);
CHECK_EQ(freq.size(), 5);                           // "xyz" is a key now
if (auto it = freq.find("not"); it != freq.end()) it->second += 10;   // look up once, update
CHECK_EQ(freq.at("not"), 11);                       // at() throws on a missing key, like C#'s indexer
freq.erase("xyz");
CHECK_EQ(freq.size(), 4);
```
<!-- /snippet -->

`map` has the same interface, iterates in key order, and adds `lower_bound`/`upper_bound` on keys (`stl_tour.cpp#map`). Sets, and the multiset trap:

<!-- snippet: modules/01-language/examples/stl_tour.cpp#sets -->
```cpp
set<int> s{5, 1, 3};                   // SortedSet<int>
s.insert(3);                           // already there: no change
CHECK_EQ(s.size(), 3);
CHECK_EQ(*s.begin(), 1);
CHECK_EQ(*s.lower_bound(2), 3);        // the MEMBER lower_bound: O(log n)
// std::lower_bound(s.begin(), s.end(), 2) compiles too, but is O(n): set iterators can't jump

multiset<int> bag{2, 2, 2, 7};         // a sorted bag: duplicates allowed
bag.erase(bag.find(2));                // erase ONE copy (find() must not be end(): check first)
CHECK_EQ(bag.count(2), 2);
bag.erase(2);                          // erase(value) removes EVERY copy
CHECK_EQ(bag.count(2), 0);

unordered_set<int> seen{4, 8};         // HashSet<int>
CHECK(seen.contains(4));
CHECK(!seen.contains(5));
```
<!-- /snippet -->

<!-- snippet: modules/01-language/examples/stl_tour.cpp#priority_queue -->
```cpp
priority_queue<int> max_heap;                              // MAX-heap by default
for (int x : {3, 1, 4, 1, 5}) max_heap.push(x);            // O(log n) each
CHECK_EQ(max_heap.top(), 5);
max_heap.pop();                                            // returns void: read top() first
CHECK_EQ(max_heap.top(), 4);

priority_queue<int, vector<int>, greater<int>> min_heap;   // min-heap, like C#'s PriorityQueue
for (int x : {3, 1, 4}) min_heap.push(x);
CHECK_EQ(min_heap.top(), 1);

// (distance, node): smallest distance first, ties by node. The Dijkstra shape.
priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
pq.push({5, 0});
pq.push({2, 1});
pq.push({2, 0});
CHECK_EQ(pq.top(), pair<int, int>{2, 0});
```
<!-- /snippet -->

`stl_tour.cpp` also covers constructing vectors, `queue`/`stack`/`deque`, and pairs and tuples.

### The algorithms you'll actually use

The range algorithms take a half-open range `[first, last)` (first included, last excluded), almost always `v.begin(), v.end()`.

| Algorithm | Does | Cost |
|---|---|---|
| `sort`, `stable_sort` | sort, optionally with a comparator; `stable_sort` keeps equal elements in input order | O(n log n) |
| `lower_bound`, `upper_bound`, `equal_range` | on sorted data: first `≥ x`, first `> x`, both | O(log n) |
| `binary_search` | yes/no only | O(log n) |
| `reverse`, `rotate`, `fill`, `iota` | reverse; rotate so a chosen element comes first; fill with a value; fill with 0, 1, 2… | O(n) |
| `accumulate` | sum (mind the initial value's type) | O(n) |
| `min_element`, `max_element` | iterator to the first min / max | O(n) |
| `count`, `find` | occurrences; first match, or `end()` | O(n) |
| `unique` + `erase` | drop *adjacent* duplicates, so sort first | O(n) |
| `next_permutation` | next lexicographic order; returns false after the last | O(n) per call |
| `nth_element` | the k-th smallest at index k, smaller ones before it | O(n) average |
| `partial_sort` | the k smallest, sorted, at the front | O(n log k) |
| `popcount(x)`, `__builtin_popcount(x)` | count of 1 bits: C++20 `<bit>` (unsigned types only), or the GCC/Clang builtin (`…ll` for 64-bit) | O(1) |
| `gcd`, `lcm` | C++17 `<numeric>` | O(log min) |

<!-- snippet: modules/01-language/examples/stl_tour.cpp#sort_search -->
```cpp
vector<int> a{5, 2, 8, 2, 9, 1};
sort(a.begin(), a.end());                                    // O(n log n), not stable
CHECK_EQ(a, vector<int>{1, 2, 2, 5, 8, 9});
CHECK_EQ(lower_bound(a.begin(), a.end(), 2) - a.begin(), 1);   // first >= 2
CHECK_EQ(upper_bound(a.begin(), a.end(), 2) - a.begin(), 3);   // first > 2
CHECK_EQ(lower_bound(a.begin(), a.end(), 7) - a.begin(), 4);   // absent: where 7 would go
auto [first, last] = equal_range(a.begin(), a.end(), 2);      // both bounds at once
CHECK_EQ(last - first, 2);                                     // = how many 2s
CHECK(binary_search(a.begin(), a.end(), 8));                   // yes/no only
sort(a.begin(), a.end(), greater<int>());                      // descending
CHECK_EQ(a, vector<int>{9, 8, 5, 2, 2, 1});
vector<int> b{3, 1, 2};
sort(b.rbegin(), b.rend());                // ALSO descending: sorts the reversed view ascending
CHECK_EQ(b, vector<int>{3, 2, 1});
```
<!-- /snippet -->

The rest of the table is demonstrated, with every result checked, in `stl_tour.cpp#algorithms` and `#permutations_selection`. When the problem itself *is* one of these algorithms, the interviewer wants you to implement it, not call it: ask before you reach for the library.

### Iterators

An iterator is a generalised pointer: `*it` reads the element, `++it` steps, `v.begin()` is the first element and `v.end()` is one **past** the last. For vectors and strings, `it - v.begin()` is the index; `next(it)` and `distance(a, b)` work on every container, `prev(it)` on all but the `unordered_*` ones; on `list`/`set`/`map` they step one element at a time, so jumping k places costs O(k).

C# throws `InvalidOperationException` when a collection changes during `foreach`. C++ doesn't notice: using an invalidated iterator is undefined behaviour.

| Container | What invalidates iterators, pointers and references |
|---|---|
| `vector`, `string` | growth that reallocates (a `push_back` past capacity): **all** of them. `insert`/`erase`: everything at or after the position |
| `deque` | push at either end: all iterators, but pointers and references stay valid; insert or erase in the middle: all; pop at an end: only the erased element's |
| `map`, `set`, `list` | only the erased element's |
| `unordered_*` | a rehash invalidates iterators (not references); erase: only the erased element's |

To delete while walking, use `erase_if(v, pred)` (C++20) or `it = container.erase(it)`, both in `stl_tour.cpp#iterators`. Indices survive a reallocation; iterators and references don't.

### Strings

`std::string` is a mutable `vector<char>` with extras. Splitting, the `string.Split` replacement:

<!-- snippet: modules/01-language/examples/stl_tour.cpp#split -->
```cpp
string csv = "alice,bob,,carol";
vector<string> fields;
stringstream ss(csv);
string field;
while (getline(ss, field, ',')) fields.push_back(field);   // split on ','; keeps empty fields
CHECK_EQ(fields, vector<string>{"alice", "bob", "", "carol"});

stringstream text("  split   on   spaces ");
vector<string> tokens;
string token;
while (text >> token) tokens.push_back(token);             // >> skips any run of whitespace
CHECK_EQ(tokens, vector<string>{"split", "on", "spaces"});
```
<!-- /snippet -->

- `substr(pos, len)` takes a **length** (like C#'s `Substring`) and copies. Inside a loop, that's how O(n) becomes O(n²): compare in place with indices, or use `string_view`.
- `s = s + c` builds a new string (O(n)); `s += c` appends in place (amortized O(1)).
- `find` returns `string::npos` when absent, and is O(n·m) in the worst case (no KMP inside).
- `stoi` throws on non-numbers and on values outside `int`; use `stoll` for 64-bit. The other way: `to_string(x)`.
- To build text from mixed types, `ostringstream out; out << "x=" << 5;` then `out.str()` (like chaining `StringBuilder.Append` in C#).
- Strings are containers: `reverse`, `sort`, `count` and indexing all work, and `<` compares lexicographically. All of this is checked in `stl_tour.cpp#strings`.

### Custom comparators

A comparator answers one question: **must `a` come before `b`?** Decide on the first key that differs, and only then fall through to the next:

<!-- snippet: modules/01-language/examples/comparators.cpp#struct_sort -->
```cpp
struct Player {
    string name;
    int score;
};

// true iff a must come BEFORE b: higher score first, equal scores by name A to Z.
bool ranks_before(const Player& a, const Player& b) {
    if (a.score != b.score) return a.score > b.score;   // decide on the first key that differs...
    return a.name < b.name;                              // ...and only then look at the next one
}
```
<!-- /snippet -->

Pass it as `sort(v.begin(), v.end(), ranks_before)`, or write the same logic inline as a lambda (the worked example below does). For a type with one natural order, let C++20 generate it; `sort`, `set<Point>` and `map<Point, …>` then just work:

<!-- snippet: modules/01-language/examples/comparators.cpp#spaceship -->
```cpp
struct Point {
    int x, y;
    auto operator<=>(const Point&) const = default;   // C++20: compare x, then y; also defines ==
};
```
<!-- /snippet -->

`priority_queue` reads its comparator backwards: it asks "does `a` have *lower* priority than `b`?", so `greater<>` (or a lambda using `>`) puts the smallest on top (`comparators.cpp#heap_set_order`).

**Strict weak ordering.** `sort`, `set`, `map` and `priority_queue` need a comparator that behaves like `<`:
1. `cmp(a, a)` is never true;
2. `cmp(a, b)` and `cmp(b, a)` are never both true;
3. it's transitive: `a` before `b` and `b` before `c` means `a` before `c`;
4. ties (neither comes first) are transitive too.

`<=` breaks rule 1. A multi-key comparator glued together with `||` breaks rule 2. Either one makes `sort` undefined behaviour: it can loop, crash, or read past the end of the array.

<!-- snippet: modules/01-language/examples/comparators.cpp#strict_weak -->
```cpp
auto less_than = [](int a, int b) { return a < b; };
auto less_equal = [](int a, int b) { return a <= b; };      // claims 2 < 2: NOT a valid comparator
vector<int> sample{1, 2, 2, 3};
CHECK(is_strict_weak_order(sample, less_than));
CHECK(!is_strict_weak_order(sample, less_equal));

// The classic multi-key bug: joining the keys with || instead of checking the first key for a tie.
auto sloppy = [](const Player& a, const Player& b) {
    return a.score > b.score || a.name < b.name;   // dan(85) vs bob(70): each "comes before" the other
};
CHECK(sloppy(Player{"dan", 85}, Player{"bob", 70}));
CHECK(sloppy(Player{"bob", 70}, Player{"dan", 85}));
CHECK(is_strict_weak_order(roster, ranks_before));
CHECK(!is_strict_weak_order(roster, sloppy));
// sort(v.begin(), v.end(), less_equal) is undefined behaviour: it may run past the array's end.
```
<!-- /snippet -->

`is_strict_weak_order` (in `comparators.cpp`) checks every triple of a small sample: paste it into a practice file whenever a comparator gets complicated.

**Pitfalls**
- `m[k]` inserts; test membership with `count`, `contains` or `find`.
- `priority_queue` is a max-heap; use `greater<>` for a min-heap.
- `multiset::erase(x)` removes every copy; `erase(find(x))` removes one (check `find(x) != end()` first).
- `std::lower_bound(s.begin(), s.end(), x)` on a `set`/`map` is O(n). Call the member: `s.lower_bound(x)`.
- `unique` without sorting first leaves non-adjacent duplicates.
- `top()`, `front()`, `back()` and `pop()` on an empty container are undefined behaviour, not exceptions.
- `sort` isn't stable; `stable_sort` is.
- A comparator using `<=`, or keys joined with `||`.

**Recognize it when…**
- "How many times…", "frequency", "anagram" → `unordered_map`, or `int cnt[26]` when the alphabet is small.
- "Seen before?", "duplicate", "distinct" → `unordered_set`.
- "Smallest value greater than x", "closest", "in sorted order", "range of keys" → `set`/`map` with `lower_bound`.
- "Repeatedly take the smallest/largest", "top k", "next event" → `priority_queue`.
- "Remove one occurrence from a sorted bag" → `multiset` + `erase(find(x))`.
- "Sort by X, then by Y", "ties broken by…" → a comparator where the first differing key decides.
- "Sort one array by another" → zip them into pairs, or sort an index array with a comparator that looks up the other array.

### Worked example: 1636. Sort Array by Increasing Frequency
[LeetCode 1636](https://leetcode.com/problems/sort-array-by-increasing-frequency/) · Easy

**Problem (paraphrased):** reorder the array so that values occurring fewer times come first; values with the same count go from largest to smallest. n ≤ 100, values in [−100, 100].
**Signals:** "sort by…, and if tied, by…" is a two-key comparator; "frequency" is a hash-map count.
**Brute force, and why it fails:** recount each value inside the comparator: O(n) per comparison, O(n² log n) in total. It passes at n = 100, but it's the version an interviewer asks you to improve.
**Key insight:** count once into a map (O(n)); the comparator then does O(1) lookups. Capture the map by reference, so the lambda neither copies it nor sees stale counts.
**Dry run:** `[2, 3, 1, 3, 2]` → counts {2: 2, 3: 2, 1: 1}.

| value | count | goes | why |
|---|---|---|---|
| 1 | 1 | first | lowest count |
| 3 | 2 | next, twice | ties with 2 on count; 3 > 2, so 3 first |
| 2 | 2 | last, twice | |

Result: `[1, 3, 3, 2, 2]`.

<!-- snippet: modules/01-language/examples/1636-sort-array-by-increasing-frequency.cpp#solution -->
```cpp
class Solution {
public:
    vector<int> frequencySort(vector<int>& nums) {
        unordered_map<int, int> freq;
        for (int x : nums) freq[x]++;
        // [&] captures freq by reference: no copy, and the lambda sees the finished counts.
        sort(nums.begin(), nums.end(), [&](int a, int b) {
            if (freq[a] != freq[b]) return freq[a] < freq[b];   // rarer values first
            return a > b;                                       // same count: larger value first
        });
        return nums;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n log n) time for the sort (each lookup is O(1) on average); O(k) extra space for k distinct values.
**Edge cases:** one element; a single repeated value (nothing to reorder); every count tied (plain descending order); negative values (fine as map keys). The test file also stress-tests 300 random arrays against a brute force that sorts `(count, −value)` pairs.
**Follow-ups:**
- *Linear time?* The values lie in [−100, 100]: count into an array at index `x + 100`, then emit counts 1, 2, … and, within one count, values from high to low. O(n + range).
- *The C# version?* `nums.OrderBy(x => freq[x]).ThenByDescending(x => x)`. LINQ builds the comparator for you; C++ makes you write it.
- *Ties in original order instead?* `stable_sort` with a comparator on the count alone.

## 4. Idioms & fast I/O — the boilerplate you can type from muscle memory

### Fast I/O and the input shapes

<!-- snippet: modules/01-language/examples/fast_io.cpp#main -->
```cpp
ios::sync_with_stdio(false);   // stop syncing with C's stdio: cin/cout get much faster
cin.tie(nullptr);              // stop flushing cout before every cin read
int tests;
cin >> tests;
for (int tc = 1; tc <= tests; tc++) solve(tc);
```
<!-- /snippet -->

- `ios::sync_with_stdio(false)` stops `cin`/`cout` from coordinating with C's `scanf`/`printf` on every operation, which makes them several times faster. Afterwards, **don't mix** the two families: their output can come out in the wrong order.
- `cin.tie(nullptr)` stops `cin` from flushing `cout` before each read.
- End lines with `'\n'`. `endl` also flushes the buffer, and 10⁵ lines with a flush each is how a correct O(n) solution times out.

One test case per `solve()` call, with its own locals, keeps test cases from leaking state into each other:

<!-- snippet: modules/01-language/examples/fast_io.cpp#solve -->
```cpp
void solve(int test_case) {
    int n;
    cin >> n;
    vector<ll> a(n);
    for (auto& x : a) cin >> x;
    ll sum = accumulate(a.begin(), a.end(), 0LL);
    dbg(sum);
    ll biggest = n > 0 ? *max_element(a.begin(), a.end()) : 0;
    cout << "Case #" << test_case << ": sum=" << sum << " max=" << biggest << '\n';   // '\n', not endl
}
```
<!-- /snippet -->

A line containing spaces, after reading numbers with `>>`:

<!-- snippet: modules/01-language/examples/fast_io.cpp#getline -->
```cpp
string line;
getline(cin >> ws, line);      // >> left the last line's '\n' unread; ws skips it (and blank lines)
stringstream words(line);
int count = 0;
string longest;
for (string w; words >> w;) {  // >> splits on any run of spaces
    count++;
    if (w.size() > longest.size()) longest = w;   // strict >: the first longest word wins
}
cout << count << " words, longest: " << longest << '\n';
```
<!-- /snippet -->

`>>` stops *before* the newline, so a bare `getline` right after it returns the empty rest of that line. `cin >> ws` discards the whitespace first.

No count given, so read to the end:

<!-- snippet: modules/01-language/examples/fast_io.cpp#until_eof -->
```cpp
ll value, numbers = 0, total = 0;
while (cin >> value) {         // false at end of input (or at the first token that isn't a number)
    numbers++;
    total += value;
}
cout << "tail: " << numbers << " numbers, sum " << total << '\n';
```
<!-- /snippet -->

Try it: `make run F=modules/01-language/examples/fast_io.cpp IN=modules/01-language/examples/fast_io.in`. `fast_io.2.in` has blank lines, runs of spaces and no final newline; the same code handles it.

The debug macro from `practice/_cp_template.cpp`, which prints the `sum = …` lines you'll see on stderr under `make run`:

<!-- snippet: modules/01-language/examples/fast_io.cpp#dbg -->
```cpp
#ifdef LOCAL   // `make run` passes -DLOCAL: dbg() prints to stderr on your machine, vanishes on the judge
#define dbg(x) cerr << #x << " = " << (x) << '\n'
#else
#define dbg(x)
#endif
```
<!-- /snippet -->

### The LeetCode shape

```c++
class Solution {
public:
    int solve(vector<int>& nums) {      // LeetCode's exact signature: public, same name, same types
        return helper(nums, 0);
    }
private:
    int helper(const vector<int>& nums, int i) { /* ... */ }   // helpers and members are fine
};
```

LeetCode constructs the object and calls the method: you never read input or print. Return containers by value (`vector<int>`), and don't keep state in globals, because they survive between test cases.

### Lambdas

`[captures](params) { body }`. The capture list says which outside variables the body may use: `[]` none, `[&]` all by reference (the usual choice inside a function), `[=]` all **copied when the lambda is created**, `[&x, y]` a mix.

<!-- snippet: modules/01-language/examples/idioms.cpp#lambdas -->
```cpp
auto square = [](int x) { return x * x; };            // [] captures nothing
int calls = 0;
auto counted = [&](int x) { calls++; return x + 1; }; // [&] uses the caller's variables by reference
int base = 10;
auto add_base = [=](int x) { return x + base; };      // [=] COPIES base, right now
base = 100;
CHECK_EQ(square(4), 16);
CHECK_EQ(counted(1) + counted(2), 5);
CHECK_EQ(calls, 2);
CHECK_EQ(add_base(1), 11);          // still the old base. A C# closure would see 100
auto add_base_ref = [&base](int x) { return x + base; };
CHECK_EQ(add_base_ref(1), 101);
```
<!-- /snippet -->

C# closures capture the variable itself, so they see later changes; C++ `[=]` takes a snapshot. And a lambda that captured locals by reference must not outlive them: don't return it or store it past their scope.

A lambda can't call itself by name. Two ways around that:

<!-- snippet: modules/01-language/examples/idioms.cpp#recursive_lambda -->
```cpp
vector<vector<int>> children{{1, 2}, {3}, {}, {}};   // tree: 0 -> 1, 2 and 1 -> 3
// A lambda can't refer to itself by name, so it takes itself as a parameter.
auto count_nodes = [&](auto&& self, int node) -> int {
    int total = 1;
    for (int child : children[node]) total += self(self, child);
    return total;
};
CHECK_EQ(count_nodes(count_nodes, 0), 4);

// std::function can call itself by name, but it's type-erased: every call is an indirect
// call the compiler can't inline, often several times slower in deep, hot recursion.
function<int(int)> count_leaves = [&](int node) -> int {
    if (children[node].empty()) return 1;
    int leaves = 0;
    for (int child : children[node]) leaves += count_leaves(child);
    return leaves;
};
CHECK_EQ(count_leaves(0), 2);        // nodes 2 and 3
```
<!-- /snippet -->

`std::function` is fine for most interview recursion. Prefer the `self` form, or a plain member function, when the recursion runs millions of times.

### Loops, constants, arithmetic

- `for (int& x : v)` to modify, `for (const auto& s : strings)` to read big elements, `for (const auto& [key, value] : m)` over a map (`idioms.cpp#range_for`).
- `emplace_back(a, b)` constructs the element in place from constructor arguments; `push_back({a, b})` builds it, then moves it in. Same complexity: use whichever reads better (`idioms.cpp#emplace`).

<!-- snippet: modules/01-language/examples/idioms.cpp#constants -->
```cpp
using ll = long long;
const int MOD = 1'000'000'007;                  // prime; a residue fits in int, a product of two doesn't
const int INF = 1'000'000'000;                  // 1e9: INF + INF = 2e9 still fits in int (max ≈ 2.1e9)
const ll LINF = 1'000'000'000'000'000'000LL;    // 1e18: LINF + LINF still fits in long long (max ≈ 9.2e18)
```
<!-- /snippet -->

<!-- snippet: modules/01-language/examples/idioms.cpp#mod_arith -->
```cpp
ll a = MOD - 1, b = MOD - 8;                      // two residues: -1 and -8 in disguise
CHECK_EQ((a + b) % MOD, MOD - 9);                 // add, then reduce
CHECK_EQ(a * b % MOD, 8);                         // a * b ≈ 1e18 < LLONG_MAX ≈ 9.2e18: fits in ll
int x = MOD - 1, y = MOD - 8;
CHECK_EQ(1LL * x * y % MOD, 8);                   // ints: widen BEFORE multiplying
CHECK_EQ(((b - a) % MOD + MOD) % MOD, MOD - 7);   // subtract: % can go negative, add MOD back
```
<!-- /snippet -->

**INF:** pick a value that survives the arithmetic you'll do with it. `INT_MAX` doesn't: `dist[u] + w` overflows the first time you relax from an unreached node. `1e9` for `int` (INF + INF still fits) and `1e18` for `long long` do (`idioms.cpp#inf`).

**memset** writes one byte value into every byte, so it only works for values whose bytes are all the same:

<!-- snippet: modules/01-language/examples/idioms.cpp#memset -->
```cpp
int a[5];
memset(a, 0, sizeof a);            // every byte 0x00 -> every int is 0
CHECK_EQ(a[3], 0);
memset(a, -1, sizeof a);           // every byte 0xFF -> every int is -1
CHECK_EQ(a[3], -1);
memset(a, 1, sizeof a);            // every byte 0x01 -> every int is 0x01010101, NOT 1
CHECK_EQ(a[3], 16'843'009);
memset(a, 0x3f, sizeof a);         // 0x3f3f3f3f ≈ 1.06e9: the one "INF" memset can make
CHECK_EQ(a[3], 1'061'109'567);
vector<int> v(5);
fill(v.begin(), v.end(), 7);       // any other value: fill(), or construct vector<int>(n, 7)
CHECK_EQ(v, vector<int>(5, 7));
```
<!-- /snippet -->

**Grids:** `vector<vector<int>> grid(rows, vector<int>(cols, 0));` makes a rows × cols grid of zeros. Walk the four neighbours with direction arrays `DR = {-1, 1, 0, 0}`, `DC = {0, 0, -1, 1}` and an `inside(r, c)` bounds check (`idioms.cpp#grid`); module 14 builds on it.

**Decimals:** OAs often ask for a fixed number of digits.

<!-- snippet: modules/01-language/examples/idioms.cpp#print_decimal -->
```cpp
ostringstream out;                                   // stands in for cout here
out << fixed << setprecision(6) << 2.0 / 3 << '\n';  // fixed: always 6 digits after the point
out << 1e9 + 0.5 << '\n';                            // fixed also stops 1e+09-style output
CHECK_EQ(out.str(), "0.666667\n1000000000.500000\n");
```
<!-- /snippet -->

**Pitfalls**
- `endl` in a loop; forgetting the fast-I/O lines on a 10⁶-number input.
- Mixing `printf` with `cout` after `sync_with_stdio(false)`.
- `getline` straight after `cin >>` returns an empty string.
- Globals not reset between test cases; per-test arrays sized and cleared by the maximum n instead of this test's n (module 03 section 4 shows why that times out).
- `[=]` when you meant to see later updates; a `[&]` lambda outliving the locals it captured.
- `memset(a, 1, …)` expecting ones; `INF = INT_MAX`.
- Printing a `double` without `fixed`: nobody asked for `1e+09`.

**Recognize it when…**
- "The first line contains T, the number of test cases" → `cin >> T;` and one `solve()` per case.
- A line with spaces in it ("a sentence", "a full name") → `getline(cin >> ws, line)`.
- No count, "until the end of input" → `while (cin >> x)`.
- 10⁵ or more numbers in or out → the fast-I/O lines and `'\n'`.
- "Print with 6 digits after the decimal point", "absolute error ≤ 10⁻⁶" → `fixed << setprecision(6)`.
- "Modulo 10⁹ + 7" → `MOD`, `1LL * a * b % MOD`, and `+ MOD` after a subtraction.

## Common mistakes

| Mistake | What you see | Fix |
|---|---|---|
| `int` product or sum past 2·10⁹ | wrong answers on big tests only; UBSan: "signed integer overflow" | `long long`, `1LL * a * b`, `accumulate(…, 0LL)` |
| `for (auto x : v)`, `f(vector<int> v)` | TLE, or edits that don't stick | `const auto&` / `auto&`; pass `const vector<int>&` |
| `v.size() - 1` on an empty vector | the loop runs ~forever; ASan: heap-buffer-overflow | `i + 1 < (int)v.size()` |
| `if (m[key])` as a membership test | the map silently grows | `count`, `contains`, `find` |
| assuming `priority_queue` is a min-heap | the largest comes out first | `priority_queue<T, vector<T>, greater<T>>` |
| `ms.erase(x)` on a multiset | every copy of x disappears | `ms.erase(ms.find(x))` |
| comparator with `<=`, or keys joined by `\|\|` | crashes or a garbage order, often only on big inputs | the first differing key decides; strict `<` / `>` |
| `endl` in an output loop | TLE with 10⁵+ lines | `'\n'` |
| `getline` right after `>>` | an empty line | `getline(cin >> ws, s)` |
| a reference into a vector, then `push_back` | ASan: heap-use-after-free | take the reference after the last `push_back`, or keep an index |
| an uninitialized local | different answers from run to run | initialize everything |
| `pow(10, k)` for integers | off by one after truncation | an integer loop, or a constant |

## Say it out loud

A talk track for [1636. Sort Array by Increasing Frequency](https://leetcode.com/problems/sort-array-by-increasing-frequency/), with the C++ decisions said explicitly:

> "We sort by frequency ascending, ties by value descending. Recounting inside the comparator would cost O(n) per comparison, so I'll count once into an `unordered_map`, O(n). Then one `sort` with a lambda that captures the map by reference, so nothing is copied: compare counts first, values second, with strict comparisons on both so it's a valid ordering. That's O(n log n) time and O(k) extra space for k distinct values. The values are small ints, so there's no overflow risk; if we were summing them I'd use `long long`. Edge cases: a single element, all values equal, all counts tied."

Follow-ups interviewers ask about the language itself:
- *Why `unordered_map` over `map`?* O(1) average versus O(log n), and I don't need key order. Its worst case is O(n) per operation under adversarial collisions; `map` is the safe fallback.
- *Is `std::sort` stable?* No. `stable_sort` is: O(n log n), using extra memory.
- *Why `const vector<int>&`?* Taking it by value copies all n elements on every call.
- *Why `long long` here?* n × the maximum value exceeds 2³¹ − 1, and signed overflow in C++ is undefined behaviour, not a wrap.
- *What if the comparator used `<=`?* It breaks strict weak ordering; `sort` may crash or run off the end of the array.
- *Reference or pointer?* A reference is a non-null alias bound once; a pointer can be null and re-pointed, which is why list and tree nodes use pointers.

## Self-check

1. `vector<int> a{1, 2, 3}; auto b = a; b[0] = 9;` What is `a[0]`? And why is `for (auto row : grid) row[0] = 0;` both slow and wrong?
<details><summary>Answer</summary>

`a[0]` is still 1: `auto b = a` copies every element (with `auto& b = a` it would become 9). In the loop, each `row` is a copy of a whole row, O(cols) per iteration, and the assignment changes that copy, not `grid`. Use `auto&` to modify and `const auto&` to read.

</details>

2. `int a = 100000, b = 100000; long long c = a * b;` What is `c`?
<details><summary>Answer</summary>

Undefined: `a * b` is computed in `int` and overflows before it's widened. Write `1LL * a * b`.

</details>

3. What does `v.size() - 1` evaluate to when `v` is empty, and how do you loop over adjacent pairs safely?
<details><summary>Answer</summary>

`size_t` is unsigned, so it wraps to 18446744073709551615. Write `for (int i = 0; i + 1 < (int)v.size(); i++)`.

</details>

4. `map<string, int> m; if (m["x"] == 0) { … }` What side effect did that have?
<details><summary>Answer</summary>

`operator[]` inserted `"x"` with value 0, so `m.size()` grew by one. Test membership with `m.count("x")`, `m.contains("x")` or `m.find("x") != m.end()`.

</details>

5. Declare a min-heap of `(distance, node)` pairs. What does `pq.pop()` return?
<details><summary>Answer</summary>

`priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;`. `pop()` returns `void`, so read `pq.top()` first.

</details>

6. How do you remove exactly one 5 from a `multiset<int> ms`?
<details><summary>Answer</summary>

`if (auto it = ms.find(5); it != ms.end()) ms.erase(it);`. `ms.erase(5)` would remove every 5.

</details>

7. Why must a comparator never be written with `<=`?
<details><summary>Answer</summary>

`cmp(a, a)` would be true, which breaks strict weak ordering. `sort` depends on that rule and can loop forever or read past the end of the array.

</details>

8. What do `ios::sync_with_stdio(false)` and `cin.tie(nullptr)` do, and what must you avoid afterwards?
<details><summary>Answer</summary>

The first stops the C++ streams synchronizing with C's stdio (much faster I/O); the second stops `cin` from flushing `cout` before every read. Afterwards, don't mix `scanf`/`printf` with `cin`/`cout`.

</details>

9. After `cin >> n;`, `getline(cin, name);` leaves `name` empty. Why, and what's the fix?
<details><summary>Answer</summary>

`>>` left the newline after `n` in the input; `getline` reads up to it and returns the empty remainder of that line. Write `getline(cin >> ws, name)`.

</details>
