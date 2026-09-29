# 09 · Stacks + Queues + Deque
> Order of processing, chosen deliberately.

**Time:** ~13 h of Core work ([problems](problems.md)) · **Prereqs:** modules 03 (amortized analysis), 06 (sliding windows), 08 (linked lists and list-backed queues) · **You're done when:** (1) for each of the four monotonic-stack variants you can write the loop from memory and say which comparison it uses and why; (2) you can explain the amortized O(n) argument, and the one-side-strict tie-breaking rule on the array [2, 2]; (3) every box in problems.md is ticked, and you've re-solved the 📖 ones (20, 84, 622, 239) from a blank file, each in under 20 minutes.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Stack applications | LIFO: the most recent unfinished item is the one the next input deals with | [0020](examples/0020-valid-parentheses.cpp), [undo_redo.cpp](examples/undo_redo.cpp) | 20, 150, 227, 155, 735, 277 |
| 2 | Monotonic stacks | Keep only indices that can still be an answer; each is pushed and popped once, so O(n) | [monotonic.hpp](../../templates/monotonic.hpp) | 496, 503, 901, 907, 2104, 402, 84, 85 |
| 3 | Queues & circular buffers | FIFO gives arrival order and BFS distance order; a ring buffer is an array + head + count | [0622](examples/0622-design-circular-queue.cpp), [rate_limiter.cpp](examples/rate_limiter.cpp), [blocking_queue.cpp](examples/blocking_queue.cpp) | 622, 933 |
| 4 | Deque tricks | Two open ends: expire old items at the front, drop dominated ones at the back | [monotonic.hpp](../../templates/monotonic.hpp) (`window_best`), [zero_one_bfs.cpp](examples/zero_one_bfs.cpp) | 239 |

## 1. Stack applications — balanced parentheses, expression evaluation, undo semantics

### Concept

**Stacks in C++, coming from C#:**

| C# `Stack<T>` | C++ `std::stack<T>` | a `vector` or `string` as a stack |
|---|---|---|
| `Push(x)` | `push(x)` | `push_back(x)` |
| `Peek()` | `top()` | `back()` |
| `Pop()` returns the item | `pop()` returns **void** | `pop_back()` |
| throws when empty | **undefined behavior** when empty | same |
| `Count` | `size()`, `empty()` | same |

`std::stack` is an adapter over `std::deque` by default. A plain `vector` works just as well and also lets you index into or iterate over the stack.

Use a stack when the newest unresolved item is the one the next input interacts with: nesting (brackets, tags, calls), reversing, undo, and any recursion you want to make explicit (modules 10 and 12).

**Balanced parentheses.** The invariant: after reading a prefix, the stack holds exactly the openers not yet closed, innermost on top. A closer is valid only if it matches the innermost unclosed opener, which is the top. At the end, the string is balanced if and only if nothing is left. Counting per bracket type isn't enough: `([)]` has balanced counts but crossed nesting. A single counter works only for one bracket type, and it must never go negative.

**Evaluating postfix (RPN).** In `2 1 + 3 *`, meaning (2 + 1) × 3, operands go on a stack. An operator pops two values, applies itself and pushes the result; the final answer is the one value left. Postfix needs no parentheses or precedence rules because the token order already encodes them.

| token | 2 | 1 | + | 3 | * |
|---|---|---|---|---|---|
| stack after | 2 | 2 1 | 3 | 3 3 | 9 |

The first pop is the *right* operand: for `a b -`, pop b, then a, and compute a − b. C++ integer division truncates toward zero, as in C#: −7 / 2 == −3.

**Evaluating infix** (`2 * (3 + 4) - 5`) needs precedence and parentheses. The classic method, Dijkstra's shunting-yard, keeps two stacks: operands and pending operators. In general terms:
1. A number goes on the operand stack.
2. An operator first applies every pending operator of higher *or equal* precedence (equal, because − and / are left-associative), stopping at a `(`. Each application pops two operands and pushes the result. Then the new operator waits on its stack.
3. `(` waits on the operator stack. `)` applies pending operators back to the matching `(`, then drops it.
4. At the end, apply everything still pending.

| token | 2 | * | ( | 3 | + | 4 | ) | − | 5 | end |
|---|---|---|---|---|---|---|---|---|---|---|
| operands | 2 | 2 | 2 | 2 3 | 2 3 | 2 3 4 | 2 7 | 14 | 14 5 | 9 |
| operators | | * | * ( | * ( | * ( + | * ( + | * | − | − | |

At `+` the top is `(`, so nothing is applied: parentheses shield what's inside them. At `)`, the `+` is applied (7) and the `(` dropped. At `−`, the pending `*` ranks at least as high, so it goes first (14). Unary minus and parentheses are what make a full calculator hard; without parentheses a simpler single pass works (227's nudge below).

**Undo/redo with two stacks.** Each edit is a command that stores what it needs to reverse itself (the Command pattern).
- New edit: apply it, push it on `undos`, and **clear `redos`**. The undone future no longer fits the new present; this is the rule people forget.
- Undo: pop from `undos`, reverse the command, push it on `redos`.
- Redo: pop from `redos`, apply it again, push it on `undos`.

```
type "a", "b", "c":   undos [a b c]   redos []        text "abc"
undo, undo:           undos [a]       redos [c b]     text "a"
type "x":             undos [a x]     redos []        text "ax": b and c are gone for good
```

### Template

The two-stack editor ([undo_redo.cpp](examples/undo_redo.cpp), stress-tested against a model that snapshots every version of the text). The bracket matcher is worked example 20 below.

<!-- snippet: modules/09-stacks-queues-deque/examples/undo_redo.cpp#undo_redo -->
```cpp
// A text buffer with two edit commands. Each command stores what it needs to reverse itself.
//   new edit: apply it, push it on `undos`, and CLEAR `redos` (the undone future is gone)
//   undo:     pop from `undos`, reverse it, push it on `redos`
//   redo:     pop from `redos`, apply it again, push it on `undos`
class Editor {
public:
    const string& text() const { return doc; }

    void type(const string& s) { run({Cmd::Type, s}); }
    void backspace(int k) {  // delete the last k characters (all of them if there are fewer)
        k = min(k, (int)doc.size());
        run({Cmd::Erase, doc.substr(doc.size() - k)});  // remember the erased text for undo
    }
    bool undo() { return move_top(undos, redos, /*forward=*/false); }
    bool redo() { return move_top(redos, undos, /*forward=*/true); }

private:
    struct Cmd {
        enum Kind { Type, Erase } kind;
        string text;  // typed text, or the text that was erased
    };
    string doc;
    stack<Cmd> undos, redos;

    void apply(const Cmd& c, bool forward) {
        bool adds = (c.kind == Cmd::Type) == forward;  // typing forward, or undoing an erase
        if (adds) doc += c.text;
        else doc.erase(doc.size() - c.text.size());
    }
    void run(const Cmd& c) {
        apply(c, true);
        undos.push(c);
        redos = {};                                    // std::stack has no clear()
    }
    bool move_top(stack<Cmd>& from, stack<Cmd>& to, bool forward) {
        if (from.empty()) return false;                // nothing to undo / redo
        Cmd c = from.top();                            // top() then pop(): pop() returns void
        from.pop();
        apply(c, forward);
        to.push(c);
        return true;
    }
};
```
<!-- /snippet -->

Undo and redo are O(1) stack operations plus the O(length) string edit. Memory grows with the history; real editors cap it.

### Pitfalls

- **`top()` or `pop()` on an empty `std::stack`** is undefined behavior (C# throws). Check `empty()` first; in 20, a leading closer hits exactly this.
- **`pop()` returns void.** `int x = st.pop();` doesn't compile; read `top()`, then `pop()`.
- **Operand order.** In RPN the first pop is the right operand; `a b -` is a − b.
- **`std::stack` has no `clear()` and no iteration.** Assign `st = {}`, or use a `vector`.
- **Forgetting the leftovers.** A string of only openers passes every closer check; 20 ends with `return open.empty()`.
- **Recursion is an implicit stack.** Input nested 10^5 deep can overflow the call stack; an explicit stack can't.
- **Overflow and division by zero** in expression evaluation. LeetCode 150 promises neither happens; real code checks.

### Recognize it when…

- "Valid", "balanced", "well-formed", "matching", "nested" brackets or tags.
- "Evaluate" an expression, "reverse Polish notation", "calculator".
- Nested structure to decode or evaluate (`k[...]`, paths with `..`), or "backspace" characters.
- "Undo", "go back", "cancel the last".
- Items that collide with the previous survivor (735, asteroids).
- Each comparison eliminates one candidate, and one survivor remains to verify (277, the celebrity).
- In general: each new element interacts only with the most recent unresolved one.

Core nudges: [150. Evaluate Reverse Polish Notation](https://leetcode.com/problems/evaluate-reverse-polish-notation/): the operand stack traced above; pop b, then a. [227. Basic Calculator II](https://leetcode.com/problems/basic-calculator-ii/): no parentheses, so skip the operator stack: apply * and / right away, and defer + and −. [155. Min Stack](https://leetcode.com/problems/min-stack/): store (value, minimum so far) in every entry. [735. Asteroid Collision](https://leetcode.com/problems/asteroid-collision/): a stack of survivors; resolve each new asteroid against the top. [277. Find the Celebrity](https://leetcode.com/problems/find-the-celebrity/) (Premium): each knows(a, b) call rules out one of the two, so n − 1 calls leave one suspect; then verify it.

### Worked example: 20. Valid Parentheses
[LeetCode 20](https://leetcode.com/problems/valid-parentheses/) · Easy

**Problem (paraphrased):** A string holds only the characters `()[]{}`. Decide whether every opener is closed by the same type in the right order, and every closer has an opener.
**Signals:** "valid", several bracket types, "correct order".
**Brute force, and why it fails:** delete adjacent pairs `()`, `[]`, `{}` until none remain; the string is valid if it ends up empty. Correct, and the test file uses it as its oracle, but O(n²) on 10^4 characters.
**Key insight:** a closer must match the most recent unclosed opener, and "most recent" means a stack.
**Dry run:**

| input | char | stack after | |
|---|---|---|---|
| `{[]}(` | `{` | `{` | |
| | `[` | `{ [` | |
| | `]` | `{` | matches `[` |
| | `}` | | matches `{` |
| | `(` | `(` | |
| | end | `(` | not empty: false |
| `([)]` | `(` | `(` | |
| | `[` | `( [` | |
| | `)` | | top is `[`, needed `(`: false |

<!-- snippet: modules/09-stacks-queues-deque/examples/0020-valid-parentheses.cpp#solution -->
```cpp
class Solution {
public:
    bool isValid(string s) {
        stack<char> open;                 // unmatched openers, innermost on top
        for (char c : s) {
            if (c == '(' || c == '[' || c == '{') {
                open.push(c);
                continue;
            }
            char want = c == ')' ? '(' : c == ']' ? '[' : '{';
            if (open.empty() || open.top() != want) return false;  // nothing to close, or crossed
            open.pop();                   // std::stack::pop() returns void: read top() first
        }
        return open.empty();              // leftover openers were never closed
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time; O(n) space in the worst case (all openers).
**Edge cases:**
- A closer first (`)`): the stack is empty.
- Only openers (`((`): leftovers at the end.
- Crossed nesting (`([)]`): the top doesn't match.
- Odd length: always invalid, so you can return early.

**Follow-ups:**
- *Only one bracket type?* A counter that never goes negative and ends at 0: O(1) space.
- *Fix the string with the fewest removals?* Keep the *indices* of unmatched brackets on the stack; whatever is left unmatched gets removed.
- *Longest valid substring?* Also a stack of indices instead of characters.

## 2. Monotonic stacks — next greater element, largest rectangle, stock span

### Concept

**The question.** For every index i, find the nearest index to its right (or left) holding a greater (or smaller) value. The brute force scans outward from each i: O(n²), too slow for n = 10^5.

**The insight.** Scan left to right for "next greater". An index whose answer isn't known yet is *waiting*; keep the waiting indices on a stack.
- When a[j] arrives, it is the answer for every waiting index with a smaller value.
- Those are exactly the ones on top. (If a waiting index had a smaller value than one pushed after it, that later one would have answered it already.) So the waiting values never increase from bottom to top.
- So a[j] pops and answers from the top until it meets a value that isn't smaller, then starts waiting itself.

```
next greater, a = [2, 1, 5, 3, 4]        (shown as values; the stack stores indices)

j=0  a=2                                   push   stack: 2
j=1  a=1  not > 2                          push   stack: 2 1
j=2  a=5  pops 1 and 2: answer is index 2  push   stack: 5
j=3  a=3  not > 5                          push   stack: 5 3
j=4  a=4  pops 3: answer is index 4        push   stack: 5 4
end       5 and 4 never answered: they keep "none" (n = 5)
answers (indices): [2, 2, 5, 4, 5]
```

**Why it's O(n): the amortized argument.** The inner `while` can pop many indices in one step, so a single iteration can cost O(n). But every index is pushed exactly once and popped at most once, so the whole scan does at most 2n stack operations: O(n) total, with an O(n) stack. (Accounting view: charge each pop to the push that put the index there.)

**The four variants.** The same loop with a different comparison. The template's conventions:

| For each i, the nearest… | Call | Stack values, bottom → top | None |
|---|---|---|---|
| j > i with a[j] > a[i] | `next_greater(a)` | never increase | n |
| j > i with a[j] < a[i] | `next_smaller(a)` | never decrease | n |
| j < i with a[j] > a[i] | `prev_greater(a)` | strictly decrease | −1 |
| j < i with a[j] < a[i] | `prev_smaller(a)` | strictly increase | −1 |

A "next" answer is written when an index is popped. A "previous" answer is whatever sits on top after popping. One left-to-right pass can produce both: when j pops p, j is p's next element, and the index just beneath p is p's previous element under the opposite strictness (it was the top when p was pushed). Worked example 84 uses exactly that.

**Strict vs non-strict: duplicates.** When values repeat, does an equal value count as "greater"? The template's default is strict (it doesn't); `strict = false` accepts it. For d = [2, 2, 1, 2, 3]:

| Call | Result | |
|---|---|---|
| `next_greater(d)` | [4, 4, 3, 4, 5] | both 2s wait for the 3 |
| `next_greater(d, false)` | [1, 3, 3, 4, 5] | an equal 2 counts |
| `prev_smaller(d)` | [−1, −1, −1, 2, 3] | |
| `prev_smaller(d, false)` | [−1, 0, −1, 2, 3] | |

When the problem defines the relation, its wording picks one: 739's "warmer" means strictly warmer. When you only need a max or min over candidates (84's largest rectangle), either choice gives the right answer. When you *count or sum over subarrays*, the choice decides correctness.

**The contribution technique.** "Sum of the minimum over all subarrays" has O(n²) subarrays. Flip the sum: add up, for each index i, a[i] × (the number of subarrays whose minimum is a[i]). With L = previous smaller and R = next smaller, a[i] is the minimum of every [l, r] with L < l ≤ i ≤ r < R, and there are (i − L) × (R − i) of them.

With duplicates, a subarray has several minimal elements, and exactly one must get the credit. **Make exactly one side strict.** With `L = prev_smaller(a)` (strict) and `R = next_smaller(a, false)` (non-strict), index i gets exactly the subarrays where it is the *rightmost* minimum. Check it on [2, 2], which has 3 subarrays:

| Boundaries | Index 0 | Index 1 | Total | |
|---|---|---|---|---|
| both strict | 1 × 2 = 2 | 2 × 1 = 2 | 4 | [2, 2] is counted twice |
| both non-strict | 1 × 1 = 1 | 1 × 1 = 1 | 2 | [2, 2] is counted by neither |
| strict left, non-strict right | 1 × 1 = 1 | 2 × 1 = 2 | 3 | exact |

`templates/tests/monotonic_test.cpp` checks this identity on random arrays full of duplicates. 907 is this plus a sum and a modulus; 2104 applies it to both the max and the min.

### Template

The two loops and the four named variants ([monotonic.hpp](../../templates/monotonic.hpp), stress-tested against O(n²) definitions with heavy duplicates in `templates/tests/monotonic_test.cpp`):

<!-- snippet: templates/monotonic.hpp#core -->
```cpp
// next_index(a, beats)[i] = smallest j > i with beats(a[j], a[i]), or n if there is none.
// prev_index(a, beats)[i] = largest  j < i with beats(a[j], a[i]), or -1 if there is none.
// beats is one of greater<T>(), greater_equal<T>(), less<T>(), less_equal<T>().
template <class T, class Beats>
vector<int> next_index(const vector<T>& a, Beats beats) {
    int n = (int)a.size();
    vector<int> res(n, n);
    vector<int> waiting;  // indices still waiting for their answer, oldest at the bottom
    for (int j = 0; j < n; j++) {
        // No waiting value beats the one below it, so the indices a[j] beats sit together on
        // top: pop and answer them, and stop at the first one a[j] doesn't beat.
        while (!waiting.empty() && beats(a[j], a[waiting.back()])) {
            res[waiting.back()] = j;
            waiting.pop_back();
        }
        waiting.push_back(j);
    }
    return res;  // indices left waiting keep the default n: nothing to their right beats them
}

template <class T, class Beats>
vector<int> prev_index(const vector<T>& a, Beats beats) {
    int n = (int)a.size();
    vector<int> res(n, -1);
    vector<int> candidates;  // indices that can still be the answer for a later index
    for (int i = 0; i < n; i++) {
        // A candidate that doesn't beat a[i] is useless from now on: i is closer to every later
        // index, and beats whatever that candidate would have beaten.
        while (!candidates.empty() && !beats(a[candidates.back()], a[i])) candidates.pop_back();
        if (!candidates.empty()) res[i] = candidates.back();
        candidates.push_back(i);
    }
    return res;
}
```
<!-- /snippet -->

<!-- snippet: templates/monotonic.hpp#four -->
```cpp
// The four variants. strict = true (default): strictly greater / smaller.
// strict = false: greater-or-equal / smaller-or-equal (an equal value counts).
template <class T>
vector<int> next_greater(const vector<T>& a, bool strict = true) {
    return strict ? next_index(a, greater<T>()) : next_index(a, greater_equal<T>());
}
template <class T>
vector<int> next_smaller(const vector<T>& a, bool strict = true) {
    return strict ? next_index(a, less<T>()) : next_index(a, less_equal<T>());
}
template <class T>
vector<int> prev_greater(const vector<T>& a, bool strict = true) {
    return strict ? prev_index(a, greater<T>()) : prev_index(a, greater_equal<T>());
}
template <class T>
vector<int> prev_smaller(const vector<T>& a, bool strict = true) {
    return strict ? prev_index(a, less<T>()) : prev_index(a, less_equal<T>());
}
```
<!-- /snippet -->

Each call is O(n) time and O(n) space, by the amortized argument above.

### Pitfalls

- **Values instead of indices** on the stack: then you can't compute distances or widths. Push indices and read values through them.
- **The wrong "none" sentinel.** The template uses n and −1 so that widths and counts need no special case; 739 wants 0. Know what your formula expects.
- **Strictness when counting.** Both sides strict double-counts ties; both non-strict misses them.
- **Leftovers.** Indices still on the stack at the end never found an answer. They keep the default, or, in 84, need a final flush.
- **Overflow.** At LeetCode 84's limits, `height * width` is at most 10^9 and fits an `int`. 907's sum doesn't: use `long long` and the modulus.
- **Circular arrays** (503): loop i from 0 to 2n − 1 and use i % n.

### Recognize it when…

- "Next greater / warmer / taller", "how many days until", "the first element to the right that…".
- "Previous smaller", "nearest smaller to the left", "how many consecutive days back", "stock span", "visible people or buildings".
- "Largest rectangle" in a histogram; "maximal rectangle" in a matrix (a histogram per row).
- "Sum or count, over all subarrays, of the min or max" → contribution technique.
- "Remove k digits to make the smallest number" → greedy pops from a monotonic stack.
- n up to 10^5, with a natural O(n²) brute force that scans until it hits a bigger or smaller value.

Core nudges: [496. Next Greater Element I](https://leetcode.com/problems/next-greater-element-i/): next greater over nums2 (739's loop), then a value → answer map. [503. Next Greater Element II](https://leetcode.com/problems/next-greater-element-ii/): circular, see the pitfall above. [901. Online Stock Span](https://leetcode.com/problems/online-stock-span/): the span reaches back to the previous strictly greater price; online, store (price, span) pairs and absorb the spans you pop. [907. Sum of Subarray Minimums](https://leetcode.com/problems/sum-of-subarray-minimums/): the contribution technique above, in `long long`, mod 10^9 + 7. [2104. Sum of Subarray Ranges](https://leetcode.com/problems/sum-of-subarray-ranges/): the contribution technique twice, sum of maxima minus sum of minima. [402. Remove K Digits](https://leetcode.com/problems/remove-k-digits/): a larger digit followed by a smaller one should go while removals remain; watch out for leading zeros. [85. Maximal Rectangle](https://leetcode.com/problems/maximal-rectangle/): see 84's first follow-up below.

### Worked example: 739. Daily Temperatures
[LeetCode 739](https://leetcode.com/problems/daily-temperatures/) · Medium

**Problem (paraphrased):** For each day's temperature, report how many days you'd wait for a strictly warmer day, or 0 if none comes.
**Signals:** "how many days until", "warmer" (strict), up to 10^5 days.
**Brute force, and why it fails:** scan forward from each day to the first warmer one. When temperatures never rise (all equal, say), that's about n²/2 = 5·10^9 steps.
**Key insight:** this is "next strictly greater element", and the answer is j − i. Days waiting for a warmer day form a stack whose temperatures never increase, and each new day resolves every colder waiting day at the top.
**Dry run:** [73, 74, 75, 71, 69, 72, 76, 73], with the stack shown as day:temperature:

| day | temp | pops (answer) | stack after |
|---|---|---|---|
| 0 | 73 | | 0:73 |
| 1 | 74 | 0 (1) | 1:74 |
| 2 | 75 | 1 (1) | 2:75 |
| 3 | 71 | | 2:75 3:71 |
| 4 | 69 | | 2:75 3:71 4:69 |
| 5 | 72 | 4 (1), 3 (2) | 2:75 5:72 |
| 6 | 76 | 5 (1), 2 (4) | 6:76 |
| 7 | 73 | | 6:76 7:73 |

Days 6 and 7 are never popped and keep 0: the answer is [1, 1, 4, 2, 1, 1, 0, 0].

<!-- snippet: modules/09-stacks-queues-deque/examples/0739-daily-temperatures.cpp#solution -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** O(n) time, since every day is pushed and popped at most once; O(n) space for the stack.
**Edge cases:**
- Equal temperatures don't answer each other (strict): [50, 50, 50] gives all 0.
- Temperatures that never rise: the stack grows to n, and one hot day can empty it.
- A single day: [0].

**Follow-ups:**
- *With the template?* `next_greater(t)`, then j − i, or 0 when j == n. The test file checks that both agree.
- *Scan right to left instead?* Keep a stack of candidate warmer days; pop everything no warmer than today; the top is the answer. Still O(n).
- *The array is circular?* Loop over 2n indices mod n (503).

### Worked example: 84. Largest Rectangle in Histogram
[LeetCode 84](https://leetcode.com/problems/largest-rectangle-in-histogram/) · Hard

**Problem (paraphrased):** Bars of width 1 stand side by side. Find the area of the largest rectangle that fits inside the histogram.
**Signals:** "largest rectangle", "histogram", up to 10^5 bars of height up to 10^4.
**Brute force, and why it fails:** for every range [l, r], track its minimum height: O(n²), about 5·10^9 steps at n = 10^5.
**Key insight:** the best rectangle is as tall as the shortest bar inside it. So for each bar i, take the widest rectangle of exactly its height: it stretches left to the previous smaller bar and right to the next smaller bar, so its width is R − L − 1. The answer is the max over i of heights[i] × (R[i] − L[i] − 1). In the one-pass version, the moment a bar is popped you know both boundaries: the bar that pops it is its next smaller, and the bar beneath it on the stack is its previous smaller.
**Dry run:** [2, 1, 5, 6, 2, 3], one pass; the stack is shown as index:height, and i = 6 is the height-0 flush.

| i | h | popped: height × (i − left − 1) | stack after |
|---|---|---|---|
| 0 | 2 | | 0:2 |
| 1 | 1 | 0:2 → 2 × (1 − (−1) − 1) = 2 | 1:1 |
| 2 | 5 | | 1:1 2:5 |
| 3 | 6 | | 1:1 2:5 3:6 |
| 4 | 2 | 3:6 → 6 × (4 − 2 − 1) = 6; 2:5 → 5 × (4 − 1 − 1) = **10** | 1:1 4:2 |
| 5 | 3 | | 1:1 4:2 5:3 |
| 6 | 0 | 5:3 → 3 × 1 = 3; 4:2 → 2 × 4 = 8; 1:1 → 1 × 6 = 6 | 6:0 |

<!-- snippet: modules/09-stacks-queues-deque/examples/0084-largest-rectangle-in-histogram.cpp#solution -->
```cpp
class Solution {
public:
    int largestRectangleArea(vector<int>& heights) {
        int n = (int)heights.size(), best = 0;
        vector<int> st;  // indices of bars with strictly increasing heights
        for (int i = 0; i <= n; i++) {
            int h = i == n ? 0 : heights[i];  // a height-0 bar at the end flushes the stack
            while (!st.empty() && heights[st.back()] >= h) {
                // Bar `top` can't extend right past i. Its left limit is the bar below it on the
                // stack (the previous smaller one), so its widest rectangle spans (left, i).
                int top = st.back();
                st.pop_back();
                int left = st.empty() ? -1 : st.back();
                best = max(best, heights[top] * (i - left - 1));
            }
            st.push_back(i);
        }
        return best;
    }
};
```
<!-- /snippet -->

The two-pass version on top of the template reads almost like the key insight:

<!-- snippet: modules/09-stacks-queues-deque/examples/0084-largest-rectangle-in-histogram.cpp#boundaries -->
```cpp
// The same answer from templates/monotonic.hpp: bar i's widest rectangle of height heights[i]
// spans the open interval (previous smaller, next smaller).
int largestRectangleBoundaries(const vector<int>& heights) {
    vector<int> left = prev_smaller(heights), right = next_smaller(heights);
    int best = 0;
    for (int i = 0; i < (int)heights.size(); i++)
        best = max(best, heights[i] * (right[i] - left[i] - 1));
    return best;
}
```
<!-- /snippet -->

**Complexity:** O(n) time and O(n) space, for both versions.
**Edge cases:**
- Equal heights: popping on `>=` lets the *last* bar of an equal run compute the full width. The earlier ones compute truncated widths, which is harmless for a max.
- Strictly increasing heights: nothing pops until the flush.
- Zero heights; a single bar.
- The largest case, 10^5 bars of height 10^4: the area is 10^9, which still fits an `int`.

**Follow-ups:**
- *The largest rectangle of 1s in a binary matrix?* Build histogram heights row by row and run this on each row: O(rows × cols) (85).
- *Why is popping on equal heights safe?* For a max, only the widest rectangle of each height matters, and the last bar of the equal run finds it.
- *Without the height-0 sentinel?* After the loop, pop the remaining bars with right boundary n. The sentinel just folds that into the loop.

## 3. Queues & circular buffers — BFS frontiers, rate limiting, producer-consumer

### Concept

**Queues in C++.** `std::queue<T>` adapts `std::deque`: `push`, `front`, `back`, `pop` (void again), `empty`, `size`. C#'s `Queue<T>` (`Enqueue`, `Dequeue`, `Peek`) is internally a ring buffer, the structure described below.

**The queue as a BFS frontier.** In an unweighted graph, BFS pushes the start node, then repeatedly pops a node and pushes its unvisited neighbours. The invariant: the queue holds nodes at distance d followed by nodes at distance d + 1, never a third distance and never out of order. Popping a d node pushes only d + 1 nodes at the back; once the d nodes run out, the d + 1 nodes are at the front and push d + 2 behind them. So nodes leave in nondecreasing distance, and the first time a node is reached is along a shortest path. A stack (DFS) has no such order, which is why DFS doesn't give shortest paths.

```
  0 ─ 1 ─ 2        queue over time, [distance]:
  │       │        0[0]
  3 ───── 4        1[1] 3[1]
                   3[1] 2[2]        at most two distance levels at any moment
                   2[2] 4[2]
                   4[2]
```

Mark a node when you *push* it, not when you pop it, or a node reachable from several frontier nodes is queued several times. Module 14 builds on this (grids, multi-source BFS, levels).

**Ring buffers.** A fixed array of capacity k, plus a `head` index and a `count`. Push writes at (head + count) mod k; pop advances head; the rear is at (head + count − 1) mod k. Every operation is O(1), and memory never grows.

```
capacity 5: push 1..5, pop twice, push 6 and 7:

index:    0     1     2     3     4
value:  [ 6 ] [ 7 ] [ 3 ] [ 4 ] [ 5 ]        head = 2, count = 5 (full)
                ▲     ▲
              rear   front
```

Why keep `count`? With only head and tail indices, "empty" and "full" both look like head == tail. The fixes: keep a count (as below), or leave one slot always empty. Real ring buffers often use a power-of-two capacity so that `% k` becomes `& (k − 1)`. They're everywhere: keyboard, network and audio buffers, "keep the last N log lines", and bounded producer-consumer queues. Moving head *backwards* (a circular deque) needs (head − 1 + k) % k, because in C++ `-1 % k` is −1, not k − 1.

**Rate limiting.** "At most `limit` requests per `window`" shows up in system-design rounds and in 933.

| Strategy | State | Accuracy | Notes |
|---|---|---|---|
| Fixed window counter | one counter per window | up to 2 × limit get through across a window boundary | O(1) memory, simplest |
| Sliding log | a queue of accepted timestamps | exact | O(limit) memory per client |
| Sliding window counter | current and previous window counts, weighted by overlap | approximate | O(1) memory, smooths the boundary burst |
| Token bucket | a token count and the last refill time | exact for its own rule | allows bursts up to the bucket size, at an average rate r |

Token bucket in one line: on each request, `tokens = min(capacity, tokens + (now − last) × r)` and `last = now`; allow the request (spending one token) if `tokens ≥ 1`. Keep tokens in integer thousandths to avoid floating-point drift. The sliding log is a pure queue problem: expire timestamps at the front, append accepted ones at the back.

**Producer–consumer.** Producers create work items and consumers process them, at different speeds. A bounded queue between them absorbs bursts and applies back-pressure: producers block when it's full, consumers block when it's empty. In C++ that's a `std::mutex` guarding the queue plus two `std::condition_variable`s, `not_full` and `not_empty`.
- **Wait with a predicate:** `cv.wait(lock, [&] { return condition; })` loops internally. Condition variables can wake up spuriously, and another thread can take the slot between the notify and your wakeup, so the condition must be re-checked after every wakeup.
- **Change shared state under the mutex, then notify.** Otherwise a wakeup can be lost: the waiter checks the condition, the other thread changes the state and notifies, and only then does the waiter go to sleep, forever.
- **Shut down explicitly:** a `closed` flag plus `notify_all`, so blocked threads wake up and exit. The alternative is a "poison pill" item for each consumer.
- **C# equivalents:** `lock` with `Monitor.Wait`/`Monitor.PulseAll`; in practice you'd use `BlockingCollection<T>` (bounded capacity, `CompleteAdding()`) or `System.Threading.Channels`.

### Template

BFS with the queue as the frontier ([bfs_frontier.cpp](examples/bfs_frontier.cpp), stress-tested against Bellman-Ford):

<!-- snippet: modules/09-stacks-queues-deque/examples/bfs_frontier.cpp#bfs -->
```cpp
// Fewest edges from src to every node (-1 = unreachable). adj[u] lists u's neighbours.
// Invariant: the queue holds nodes at distance d, then nodes at distance d + 1, never anything
// else, so nodes leave in order of distance and the first time you reach a node is the shortest.
vector<int> bfs(const vector<vector<int>>& adj, int src) {
    vector<int> dist(adj.size(), -1);
    queue<int> frontier;
    dist[src] = 0;                  // mark a node when you PUSH it, so it's queued only once
    frontier.push(src);
    while (!frontier.empty()) {
        int u = frontier.front();
        frontier.pop();
        for (int v : adj[u]) {
            if (dist[v] != -1) continue;
            dist[v] = dist[u] + 1;
            frontier.push(v);
        }
    }
    return dist;
}
```
<!-- /snippet -->

O(V + E) time, O(V) space. The ring buffer is worked example 622 below.

The sliding-log limiter ([rate_limiter.cpp](examples/rate_limiter.cpp), stress-tested against a brute force that recounts every allowed request). The same file has a fixed-window counter, plus a test that sends both of them requests at t = 7, 8, 9, 10, 11, 12 with a limit of 3 per 10: the sliding log lets 3 through, and the fixed window lets all 6 through, because they straddle the boundary at 10.

<!-- snippet: modules/09-stacks-queues-deque/examples/rate_limiter.cpp#sliding_log -->
```cpp
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
```
<!-- /snippet -->

The sliding log is amortized O(1) per request (every timestamp is pushed and popped once) with O(limit) memory; the fixed window is O(1) time and memory.

A bounded blocking queue ([blocking_queue.cpp](examples/blocking_queue.cpp)). The test passes 10 000 items from a producer thread to a consumer thread through a queue of capacity 16 and checks the count, the sum and the FIFO order. It also runs 2 producers against 2 consumers at capacity 1.

<!-- snippet: modules/09-stacks-queues-deque/examples/blocking_queue.cpp#blocking_queue -->
```cpp
// push waits while the queue is full; pop waits while it's empty.
// close() wakes every waiter: later pushes fail, and pop drains what's left, then returns nullopt.
template <class T>
class BlockingQueue {
public:
    explicit BlockingQueue(size_t capacity) : cap(capacity) {}

    bool push(T item) {
        unique_lock<mutex> lock(m);
        // wait() unlocks while asleep, relocks on wakeup, and re-checks the condition, so a
        // spurious wakeup or a stolen slot just sends it back to sleep
        not_full.wait(lock, [&] { return q.size() < cap || closed; });
        if (closed) return false;
        q.push(std::move(item));
        not_empty.notify_one();  // wake one consumer, if any is waiting
        return true;
    }

    optional<T> pop() {
        unique_lock<mutex> lock(m);
        not_empty.wait(lock, [&] { return !q.empty() || closed; });
        if (q.empty()) return nullopt;  // closed and fully drained
        T item = std::move(q.front());
        q.pop();
        not_full.notify_one();          // wake one producer, if any is waiting
        return item;
    }

    void close() {
        {
            lock_guard<mutex> lock(m);  // change shared state only under the lock
            closed = true;
        }
        not_full.notify_all();
        not_empty.notify_all();
    }

private:
    size_t cap;
    queue<T> q;
    bool closed = false;
    mutex m;                            // guards q and closed
    condition_variable not_full, not_empty;
};
```
<!-- /snippet -->

Each operation is O(1) plus waiting. Notifying while still holding the lock is correct. Notifying just after unlocking is slightly faster: the woken thread doesn't wake up only to block on a mutex you still hold.

### Pitfalls

- **`front()` or `pop()` on an empty queue** is undefined behavior.
- **BFS: mark on push, not on pop,** or nodes get queued several times.
- **Ring buffer: empty vs full** look alike by indices alone; and `(head - 1) % k` is negative when head is 0.
- **Rate limiter boundaries.** Decide half-open (t − w, t] or closed [t − w, t] (933 counts [t − 3000, t]); the off-by-one bugs live there. Timestamps must not decrease, or expiring from the front breaks.
- **Condition variables:** `cv.wait(lock)` without a predicate breaks on spurious wakeups; changing state outside the lock loses wakeups; no `notify_all` on shutdown leaves threads blocked and hangs `join()`.
- **Slow work inside the lock** (processing an item while holding the mutex) serializes every thread.
- **Data races.** Every access to the shared queue holds the mutex, reads like `size()` included. The example passes ThreadSanitizer (`-fsanitize=thread`).

### Recognize it when…

- "In the order they arrive", "first come, first served", "level by level", "minimum number of steps" on an unweighted graph or grid → queue / BFS.
- "Last N", "fixed-size buffer", "overwrite the oldest", "circular" → ring buffer.
- "Requests in the past X ms", "at most K per window", "hits in the last 5 minutes" → a queue of timestamps.
- "Multithreaded", "producer", "consumer", "blocking", "thread-safe queue" → mutex + condition variables.
- "Implement a queue using stacks" → the two-stack transfer (amortized O(1)).

Core nudge: [933. Number of Recent Calls](https://leetcode.com/problems/number-of-recent-calls/): the sliding log without a limit; mind the inclusive boundary.

### Worked example: 622. Design Circular Queue
[LeetCode 622](https://leetcode.com/problems/design-circular-queue/) · Medium

**Problem (paraphrased):** Implement a fixed-capacity FIFO queue on a circular buffer, without the language's built-in queue: `enQueue`/`deQueue` report success, `Front`/`Rear` return −1 when empty, plus `isEmpty` and `isFull`.
**Signals:** "circular", "fixed size k", "reuse the space freed at the front".
**Brute force, and why it fails:** a vector whose `deQueue` erases the front element: O(k) per call. A linked-list queue works, but it allocates per element and misses what's being tested.
**Key insight:** never move elements, move indices. The front is `buf[head]`, the next free slot is `(head + count) % k`, and `count` tells "empty" from "full".
**Dry run:** the official example, capacity 3.

| call | buf | head | count | returns |
|---|---|---|---|---|
| enQueue(1) | [1 _ _] | 0 | 1 | true |
| enQueue(2) | [1 2 _] | 0 | 2 | true |
| enQueue(3) | [1 2 3] | 0 | 3 | true |
| enQueue(4) | [1 2 3] | 0 | 3 | false: full |
| Rear() | | | | 3 |
| isFull() | | | | true |
| deQueue() | [1 2 3] | 1 | 2 | true; slot 0 is free (the stale 1 is simply ignored) |
| enQueue(4) | [4 2 3] | 1 | 3 | true, written at (1 + 2) % 3 = 0 |
| Rear() | | | | buf[(1 + 3 − 1) % 3] = buf[0] = 4 |

<!-- snippet: modules/09-stacks-queues-deque/examples/0622-design-circular-queue.cpp#solution -->
```cpp
class MyCircularQueue {
public:
    MyCircularQueue(int k) : buf(k), cap(k) {}

    bool enQueue(int value) {
        if (isFull()) return false;
        buf[(head + count) % cap] = value;  // the slot just past the rear, wrapping around
        count++;
        return true;
    }
    bool deQueue() {
        if (isEmpty()) return false;
        head = (head + 1) % cap;            // abandon the front slot; enQueue will reuse it
        count--;
        return true;
    }
    int Front() { return isEmpty() ? -1 : buf[head]; }
    int Rear() { return isEmpty() ? -1 : buf[(head + count - 1) % cap]; }
    bool isEmpty() { return count == 0; }
    bool isFull() { return count == cap; }

private:
    vector<int> buf;
    int cap;
    int head = 0;   // index of the front element
    int count = 0;  // number of elements: tells "empty" from "full", which look alike by indices
};
```
<!-- /snippet -->

**Complexity:** O(1) per operation; O(k) space.
**Edge cases:**
- Capacity 1: head wraps to 0 on every dequeue.
- `Front`/`Rear` on an empty queue: −1.
- `enQueue` when full, `deQueue` when empty: return false and change nothing.

**Follow-ups:**
- *Head and tail instead of count?* Then full and empty both have head == tail. Waste one slot (allocate k + 1) or keep a flag.
- *Thread-safe and blocking?* Add a mutex and two condition variables: the blocking queue above.
- *Double-ended?* Also push and pop at the front, with head = (head − 1 + k) % k.

## 4. Deque tricks — sliding-window maximum, palindrome checks, min-stack

### Concept

**`std::deque`.** O(1) push and pop at both ends, and O(1) indexing (`dq[i]`). It stores fixed-size blocks plus an index of block pointers, so growing never moves existing elements. Even so, `push_front`/`push_back` invalidate all *iterators* (references to elements stay valid), and inserting in the middle invalidates everything. `System.Collections.Generic` has no deque type; `LinkedList<T>` is the usual stand-in.

**Sliding-window maximum with a monotonic deque.** The window [i − k + 1, i] slides right. Keep a deque of the window's indices whose values strictly decrease from front to back:
- **Back:** before pushing i, pop every index whose value is ≤ a[i]. That element is older and not larger, so a[i] outlives it and it can never be a window maximum again.
- **Front:** the current maximum. When its index leaves the window (it equals i − k), pop it.

Each index enters and leaves once, so it's O(n) total: the same amortized argument as the monotonic stack. The deque is a monotonic stack at the back that also expires old entries at the front.

```
a = [1, 3, -1, -3, 5, 3, 6, 7], k = 3       (shown as values; the deque stores indices)

i=0   1                                  deque: 1
i=1   3    pop 1 (1 ≤ 3)                 deque: 3
i=2  -1                                  deque: 3 -1         max 3
i=3  -3                                  deque: 3 -1 -3      max 3
i=4   5    pop -3, -1, 3                 deque: 5            max 5
i=5   3                                  deque: 5 3          max 5
i=6   6    pop 3, 5                      deque: 6            max 6
i=7   7    pop 6                         deque: 7            max 7
```

At i = 4 the old 3 (index 1) is also out of the window; the back loop happens to remove it first. The template generalizes this with a comparator (`window_best`): `greater` gives the max, `less` the min. Two deques over the same window, one for the max and one for the min, handle "longest window with max − min ≤ limit".

**The min-stack idea (155).** A stack that also reports its minimum in O(1): each entry stores, next to its value, the minimum of itself and everything beneath it. Push computes min(new value, min below); pop discards the pair, and the previous minimum is back on top automatically. It works because a stack only ever removes its newest element, and the minimum of the older elements never changes. A queue removes its oldest element, which is why a sliding-window minimum needs the deque trick instead (or a queue built from two min-stacks, for amortized O(1)).

```
push 5, 3, 7, 2:   (value, min), top first:  (2,2) (7,3) (3,3) (5,5)      min 2
pop:                                         (7,3) (3,3) (5,5)            min 3 again
```

**Palindrome checks with a deque.** Load the characters, then compare and drop both ends until at most one is left. Two pointers on the string do the same in O(1) extra space (125, in module 02). The deque version fits when characters arrive as a stream, or when a simulation consumes both ends anyway.

**0-1 BFS (a preview; module 15 teaches it).** When every edge weighs 0 or 1, you don't need Dijkstra's priority queue, just a deque. A 0-weight edge gives v the same distance as u, so v joins the current frontier at the *front*; a 1-weight edge gives distance + 1, so v goes to the *back*. The deque keeps BFS's two-level invariant (distances d…d, then d + 1…d + 1), so nodes leave in nondecreasing distance: O(V + E) instead of O((V + E) log V).

### Template

Window max and min on top of one comparator-driven loop ([monotonic.hpp](../../templates/monotonic.hpp)):

<!-- snippet: templates/monotonic.hpp#window -->
```cpp
// window_best(a, k, better)[i] = the best value of a[i .. i+k-1], for i = 0 .. n-k.
// better = greater<T>() gives the max, less<T>() the min. Returns {} unless 1 <= k <= n.
template <class T, class Better>
vector<T> window_best(const vector<T>& a, int k, Better better) {
    int n = (int)a.size();
    if (k < 1 || k > n) return {};
    vector<T> res;
    res.reserve(n - k + 1);
    deque<int> dq;  // indices in the window, oldest at the front; each value strictly better
                    // than every value behind it, so the front is the window's best
    for (int i = 0; i < n; i++) {
        // An older index that isn't better than a[i] can never be the best again: a[i] is at
        // least as good and stays in the window longer. (Ties: the newer index wins.)
        while (!dq.empty() && !better(a[dq.back()], a[i])) dq.pop_back();
        dq.push_back(i);
        if (dq.front() == i - k) dq.pop_front();  // exactly one index (i - k) leaves per step
        if (i >= k - 1) res.push_back(a[dq.front()]);
    }
    return res;
}
template <class T>
vector<T> sliding_window_max(const vector<T>& a, int k) { return window_best(a, k, greater<T>()); }
template <class T>
vector<T> sliding_window_min(const vector<T>& a, int k) { return window_best(a, k, less<T>()); }
```
<!-- /snippet -->

O(n) time, O(k) space for the deque. The 0-1 BFS preview ([zero_one_bfs.cpp](examples/zero_one_bfs.cpp), stress-tested against Bellman-Ford):

<!-- snippet: modules/09-stacks-queues-deque/examples/zero_one_bfs.cpp#zero_one_bfs -->
```cpp
// Shortest paths when every edge weighs 0 or 1. adj[u] holds (v, w) pairs with w in {0, 1}.
// A 0-edge keeps the distance, so v goes to the FRONT; a 1-edge adds one, so v goes to the BACK.
// The deque then holds distances d..d, d+1..d+1, like the BFS frontier, and pops in order.
vector<int> zero_one_bfs(const vector<vector<pair<int, int>>>& adj, int src) {
    vector<int> dist(adj.size(), INT_MAX);
    deque<int> dq;
    dist[src] = 0;
    dq.push_back(src);
    while (!dq.empty()) {
        int u = dq.front();
        dq.pop_front();
        for (auto [v, w] : adj[u]) {
            if (dist[u] + w >= dist[v]) continue;  // no improvement
            dist[v] = dist[u] + w;                 // a node may be queued twice; the stale copy
            if (w == 0) dq.push_front(v);          // relaxes nothing when it's popped later
            else dq.push_back(v);
        }
    }
    return dist;  // INT_MAX = unreachable
}
```
<!-- /snippet -->

0-1 BFS runs in O(V + E) time.

### Pitfalls

- **Values instead of indices in the deque:** then you can't tell when the front leaves the window. Store indices.
- **Off-by-one windows.** The index leaving at step i is i − k, and a full window exists only once i ≥ k − 1.
- **Popping the back on `<` vs `≤`.** Both give correct maximum *values*; `≤` keeps the deque strictly decreasing and shorter. If you need the *index* of the max, decide which of two equal values should win.
- **k = 0 or k > n.** The template returns `{}`; LeetCode guarantees 1 ≤ k ≤ n.
- **Deque iterator invalidation:** pushing at either end invalidates iterators, though not references.
- **`isalnum`/`tolower` on a plain `char`:** negative values (non-ASCII bytes) are undefined behavior. Cast to `unsigned char` first.
- **0-1 BFS with other weights** gives wrong answers. Weights beyond 0 and 1 need Dijkstra (module 15).

### Recognize it when…

- "Maximum/minimum of every window of size k", "sliding window maximum".
- "Longest subarray where max − min ≤ limit" → two monotonic deques.
- "The best of the last k values" inside a DP transition → a window maximum over the dp array.
- "Shortest subarray with sum ≥ K" when negatives are allowed → a monotonic deque over prefix sums.
- "A grid where some moves are free and others cost 1", "the fewest cells to change" → 0-1 BFS.
- "Get the min or max in O(1)": on a stack → min-stack; on a queue or window → monotonic deque, or two stacks.

This section's only list problem is 239, worked below; the template's `sliding_window_min` is the same loop with the comparison flipped.

### Worked example: 239. Sliding Window Maximum
[LeetCode 239](https://leetcode.com/problems/sliding-window-maximum/) · Hard

**Problem (paraphrased):** Given an array and a window size k, return the maximum of each length-k window as it slides from left to right.
**Signals:** "window of size k", "maximum"; up to 10^5 elements, and k up to n.
**Brute force, and why it fails:** take the max of each window directly: O(nk), about 2.5·10^9 steps when k ≈ n/2. A `multiset` of the window (insert the new value, erase the old, read the largest) is O(n log k): acceptable, and a good intermediate answer to mention.
**Key insight:** an element that is older and not larger than a newer one can never be a maximum again, so drop it. What's left is decreasing, so the maximum is at the front, and the front is also the oldest element, the only one that can expire.
**Dry run:** the trace above is official example 1; the result is [3, 3, 5, 5, 6, 7].

<!-- snippet: modules/09-stacks-queues-deque/examples/0239-sliding-window-maximum.cpp#solution -->
```cpp
class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        vector<int> result;
        deque<int> dq;  // indices in the window, values strictly decreasing front to back
        for (int i = 0; i < (int)nums.size(); i++) {
            // nums[i] is at least as big and leaves later: smaller-or-equal older values are dead
            while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
            dq.push_back(i);
            if (dq.front() == i - k) dq.pop_front();  // the front index just slid out on the left
            if (i >= k - 1) result.push_back(nums[dq.front()]);  // a full window: front is its max
        }
        return result;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time, since each index is pushed and popped at most once; O(k) space, since the deque holds only indices inside the window.
**Edge cases:**
- k = 1: the output is the input. k = n: a single window.
- Duplicates: [4, 4, 4, 4] with k = 2 gives [4, 4, 4].
- Strictly decreasing input: the front expires on every step.
- Negative values.

**Follow-ups:**
- *Window minimum?* Flip the comparison (`sliding_window_min`).
- *Max − min of each window?* Two deques at once.
- *A stream of unknown length?* The loop already works online: one push, at most one expiry, one read per element.
- *Without a deque?* A queue made of two max-stacks is amortized O(1) too; a sparse table answers any window in O(1) after O(n log n) preprocessing (module 17).

## Common mistakes

| Mistake | Symptom | Fix |
|---|---|---|
| `top()`/`front()`/`pop()` on an empty container | Crash, or garbage that passes locally | Check `empty()` first; `make run` catches it under ASan |
| `int x = st.pop();` | Compile error: `pop()` returns void | `int x = st.top(); st.pop();` |
| Values on a monotonic stack or deque | Can't compute widths or expire the window | Store indices |
| Both sides strict (or both non-strict) when counting | Wrong totals whenever values repeat | One side strict, the other not |
| Forgetting the elements left on the stack | Missing answers, e.g. the rectangles at the end of 84 | Default values, or a sentinel flush |
| Marking BFS nodes on pop | The same node queued many times | Mark on push |
| Ring buffer with head and tail only | Can't tell full from empty | Keep a count |
| `(head - 1) % k` | Negative index | `(head - 1 + k) % k` |
| `cv.wait(lock)` without a predicate | Rare hangs or wrong results under load | `cv.wait(lock, [&] { return condition; })` |
| An O(nk) window scan | Time limit exceeded | A monotonic deque |

## Say it out loud

A talk track, using 739:

1. **Restate:** "For each day, how many days until a strictly warmer one; 0 if none comes."
2. **Brute force:** "Scan forward from every day: O(n²), about 5·10^9 steps on 10^5 days that never get warmer. Too slow."
3. **Insight:** "It's next greater element. I scan once and keep a stack of days still waiting for a warmer day. Their temperatures never increase from bottom to top, so today resolves every colder day at the top."
4. **Complexity:** "Every index is pushed and popped at most once, so O(n) time and O(n) space, even with the nested loop."
5. **Edge cases:** "Equal temperatures don't count, so the comparison is strict. A sequence that never rises fills the stack. Days left on the stack keep 0."

Follow-ups interviewers commonly ask in this module:
- *"There's a loop inside a loop. Why is it O(n)?"* Each index is pushed once and popped at most once: at most 2n operations in total.
- *"What about duplicates?"* The problem's wording picks strict or non-strict; when counting subarrays, make exactly one side strict.
- *"Make it circular."* Loop over 2n indices mod n.
- *"Make the queue thread-safe, and blocking."* A mutex plus two condition variables, waiting with predicates.
- *"Implement a queue with stacks."* Two stacks, amortized O(1).
- *"Now the window minimum too."* A second deque with the comparison flipped.

## Self-check

1. `std::stack::pop()` returns void. How do you get the popped value, and what happens when you call `top()` on an empty stack?
<details><summary>Answer</summary>Read <code>top()</code> first, then call <code>pop()</code>. On an empty stack both are undefined behavior (C# would throw), so check <code>empty()</code> first. The void return is for exception safety: if <code>pop()</code> returned the element by value and that copy threw, the element would already be gone.</details>

2. Evaluating the RPN tokens `a b -`, which popped value is the left operand?
<details><summary>Answer</summary>The second one popped. The first pop returns b (the right operand), the second returns a, and you compute a − b. The same holds for division.</details>

3. In "next greater", why do the waiting values never increase from bottom to top?
<details><summary>Answer</summary>When an index is pushed, it has just popped every waiting index with a smaller value. So nothing beneath it is smaller than it, and every later push keeps that property.</details>

4. The monotonic stack has a nested `while`. Why is it still O(n)?
<details><summary>Answer</summary>Each index is pushed exactly once and popped at most once, so the total number of pops across the whole scan is at most n. The work is O(n) amortized, even though one step can pop many indices.</details>

5. Why must exactly one side be strict in the contribution technique? Use [2, 2].
<details><summary>Answer</summary>Each subarray must be credited to exactly one index. Both strict: each 2 extends over the other, so [2, 2] is counted twice (total 4 instead of 3). Both non-strict: each 2 stops at the other, so [2, 2] is counted by neither (total 2). Strict on the left and non-strict on the right credits each subarray to its rightmost minimum: exactly 3.</details>

6. In 84's one-pass version, when bar `top` is popped at index i, what are the two boundaries of its widest rectangle?
<details><summary>Answer</summary>The right boundary is i, the first bar to its right that is not taller. The left boundary is the index now on top of the stack (or −1 if the stack is empty), its previous smaller bar. The width is i − left − 1.</details>

7. In a ring buffer of capacity k, where does the next element go, and why keep a count?
<details><summary>Answer</summary>At (head + count) % k; the rear element sits at (head + count − 1) % k. Without the count, a full buffer and an empty one both have head == tail, so you can't tell them apart (the other fix is to waste one slot).</details>

8. Why must a condition-variable wait re-check its condition, and why change the state under the mutex?
<details><summary>Answer</summary>A wait can return spuriously, and another thread may grab the item or slot between the notify and the wakeup, so the condition must be re-checked in a loop, which the predicate overload does. Changing the state without the mutex can lose a wakeup: the notify can land between the waiter's check and its sleep.</details>

9. In the sliding-window-maximum deque, why pop the back on ≤, and when do you pop the front?
<details><summary>Answer</summary>A back element whose value is ≤ a[i] is older and not larger than a[i], so it can never be a window maximum again: a[i] outlives it. Pop the front when its index equals i − k, meaning it just left the window.</details>

10. In 0-1 BFS, why does a node reached by a 0-weight edge go to the front of the deque?
<details><summary>Answer</summary>It has the same distance as the node being processed, so it belongs to the current frontier level. With 0-edges at the front and 1-edges at the back, the deque always holds distances d…d, then d + 1…d + 1, so nodes come out in nondecreasing distance, just as in plain BFS.</details>
