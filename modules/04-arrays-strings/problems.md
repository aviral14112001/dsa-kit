# 04 · Arrays & Strings
> The substrate every other structure is built on.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. In-place manipulation — rotation, reversal, partitioning, Dutch national flag

- [ ] **Read** · Notes section 1 · 45m
- [ ] [283. Move Zeroes](https://leetcode.com/problems/move-zeroes/) · Easy · `stable partition` — added · move the kept elements forward, fill the rest
- [ ] [189. Rotate Array](https://leetcode.com/problems/rotate-array/) · Medium · 📖 · `triple reverse` — added · rotate by reversing three times: O(1) space, and provably correct
- [ ] [75. Sort Colors](https://leetcode.com/problems/sort-colors/) · Medium · 📖 · `Dutch national flag` — sheet: Sort an array of 0's 1's and 2's · three regions, three pointers, one pass · [video](https://youtu.be/tp8JIuCXBaU)
- [ ] [31. Next Permutation](https://leetcode.com/problems/next-permutation/) · Medium · `next permutation` — find the pivot from the right, swap, reverse the suffix · [video](https://youtu.be/JDOXKqF60RQ)

### 2. Matrix problems — spiral order, transpose, rotation, row-column sweeps

- [ ] **Read** · Notes section 2 · 45m
- [ ] [54. Spiral Matrix](https://leetcode.com/problems/spiral-matrix/) · Medium · 📖 · `boundary shrink` — added · four boundaries closing in; the loop-condition bugs are the real lesson
- [ ] [48. Rotate Image](https://leetcode.com/problems/rotate-image/) · Medium · 📖 · `transpose + reverse` — added · rotate 90° in place as two simple passes
- [ ] [73. Set Matrix Zeroes](https://leetcode.com/problems/set-matrix-zeroes/) · Medium · `marker rows` — added · O(1) space by reusing row 0 and column 0 as flags

### 3. String mechanics — immutability, builders, character counts, palindromes

- [ ] **Read** · Notes section 3 · 45m
- [ ] [680. Valid Palindrome II](https://leetcode.com/problems/valid-palindrome-ii/) · Easy · `two pointers + one skip` — added · the palindrome check with a single allowed deletion
- [ ] [151. Reverse Words in a String](https://leetcode.com/problems/reverse-words-in-a-string/) · Medium · `in-place reverse` — added · reverse everything, then each word: C++ strings are mutable, so this is O(1) extra
- [ ] [5. Longest Palindromic Substring](https://leetcode.com/problems/longest-palindromic-substring/) · Medium · 📖 · `expand around center` — added · 2n−1 centers, expand each; O(n²) time, O(1) space

### 4. Subarrays & substrings — enumerating them, and why brute force dies at scale

- [ ] **Read** · Notes section 4: counting subarrays, Kadane · 45m
- [ ] [53. Maximum Subarray](https://leetcode.com/problems/maximum-subarray/) · Medium · 📖 · `Kadane` — sheet: Kadane's Algorithm · O(n³) → O(n²) → O(n): the canonical "why brute force dies" walk · [video](https://youtu.be/AHZpyENo7k4)
- [ ] [152. Maximum Product Subarray](https://leetcode.com/problems/maximum-product-subarray/) · Medium · `track max and min` — a negative number swaps the running max and min
