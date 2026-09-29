# 18 · Tries at Scale + Bit Manipulation + Number Theory + String Algorithms
> The specialist toolkit that turns a hard problem into a short one.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Tries at scale — bitwise tries, maximum XOR pair, prefix aggregation

- [ ] **Read** · Notes section 1 + templates/xor_trie.hpp · 30m
- [ ] [421. Maximum XOR of Two Numbers in an Array](https://leetcode.com/problems/maximum-xor-of-two-numbers-in-an-array/) · Medium · 📖 · `bitwise trie` — greedily take the opposite bit from the top down · [video](https://www.youtube.com/watch?v=EIhAwfHubE8&list=PLgUwDviBIf0pcIDCZnxhv0LkHf5KzG9zp&index=6)
- [ ] [1707. Maximum XOR With an Element From Array](https://leetcode.com/problems/maximum-xor-with-an-element-from-array/) · Hard · `offline XOR trie` — sheet: Maximum Xor with an element from an array · sort the queries by limit, insert numbers as you go · [video](https://www.youtube.com/watch?v=Q8LhG9Pi5KM&list=PLgUwDviBIf0pcIDCZnxhv0LkHf5KzG9zp&index=7)

### 2. Bit manipulation — masks, set-bit counting, subset enumeration, XOR tricks

- [ ] **Read** · Notes section 2 · 45m
- [ ] [136. Single Number](https://leetcode.com/problems/single-number/) · Easy · `XOR cancels pairs` — added · a ^ a = 0, a ^ 0 = a
- [ ] [191. Number of 1 Bits](https://leetcode.com/problems/number-of-1-bits/) · Easy · `n & (n − 1)` — added · each step clears the lowest set bit
- [ ] [645. Set Mismatch](https://leetcode.com/problems/set-mismatch/) · Easy · `XOR / sum equations` — added for the sheet's “Find the repeating and missing number” (no LeetCode link) · two equations (sum and sum of squares), or split by a set bit of the XOR
- [ ] [137. Single Number II](https://leetcode.com/problems/single-number-ii/) · Medium · 📖 · `bit counts mod 3` — count each bit position mod 3 (then the ones/twos state machine)
- [ ] [260. Single Number III](https://leetcode.com/problems/single-number-iii/) · Medium · `split by lowest set bit` — xor & −xor separates the two numbers · [video](https://youtu.be/UA5JnV1J2sI?si=VFBRJyb3boZvx_r1)

### 3. Number theory — sieve, GCD / LCM, modular arithmetic, fast exponentiation

- [ ] **Read** · Notes section 3 + templates/number_theory.hpp · 45m
- [ ] [204. Count Primes](https://leetcode.com/problems/count-primes/) · Medium · 📖 · `sieve of Eratosthenes` — added for the sheet's “Print all primes till N” (no LeetCode link) · start crossing out at i·i; O(n log log n)
- [ ] [1922. Count Good Numbers](https://leetcode.com/problems/count-good-numbers/) · Medium · `fast modular exponentiation` — added · 5^even · 4^odd mod 1e9+7

### 4. String algorithms — KMP, Z-function, Rabin-Karp, Manacher, suffix arrays

- [ ] **Read** · Notes section 4 + templates/strings.hpp · 75m
- [ ] [28. Find the Index of the First Occurrence in a String](https://leetcode.com/problems/find-the-index-of-the-first-occurrence-in-a-string/) · Easy · 📖 · `naive → KMP` — sheet: KMP Algorithm or LPS array · the prefix function makes matching O(n + m)
- [ ] [1392. Longest Happy Prefix](https://leetcode.com/problems/longest-happy-prefix/) · Hard · `prefix function or Z-function` — added for the sheet's “Z function” (no LeetCode link) · the answer is π of the last character
- [ ] [686. Repeated String Match](https://leetcode.com/problems/repeated-string-match/) · Medium · `repeat + search` — sheet: Rabin Karp Algorithm · at most ⌈m/n⌉ + 1 repeats
- [ ] [647. Palindromic Substrings](https://leetcode.com/problems/palindromic-substrings/) · Medium · `Manacher (or expand)` — added · all palindromic substrings in O(n)
- [ ] [1044. Longest Duplicate Substring](https://leetcode.com/problems/longest-duplicate-substring/) · Hard · 📖 · `binary search + Rabin–Karp` — added · the length is monotone; hash every window of that length
- [ ] [214. Shortest Palindrome](https://leetcode.com/problems/shortest-palindrome/) · Hard · `KMP on s + '#' + reverse(s)` — the longest palindromic prefix
