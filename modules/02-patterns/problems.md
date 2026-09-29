# 02 · Common DSA Patterns
> The dozen shapes almost every problem collapses into, once you can see them.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Pattern recognition — mapping a problem statement onto a known template

- [ ] **Read** · Notes section 1: the four-question triage + the master pattern table · 30m
- [ ] **Drill** · Classification drill A: the 15 statements in Notes section 1 → pattern + one-line reason (answers are hidden there) · 30m

### 2. The core set — two pointers, sliding window, fast-slow, binary search on answer

- [ ] **Read** · Notes section 2: the four skeletons, each with one traced example · 45m
- [ ] [125. Valid Palindrome](https://leetcode.com/problems/valid-palindrome/) · Easy · `two pointers (converging)` — added · the converging-pointers skeleton on its easiest input
- [ ] [643. Maximum Average Subarray I](https://leetcode.com/problems/maximum-average-subarray-i/) · Easy · `fixed sliding window` — added · the fixed-window skeleton: add one element, drop one, never re-sum
- [ ] [69. Sqrt(x)](https://leetcode.com/problems/sqrtx/) · Easy · `binary search on answer` — added for the sheet's “Find Nth root of a number” (no LeetCode link) · "largest x with x·x ≤ n": search the answer, not the array

### 3. Traversal & state patterns — BFS levels, DFS with backtracking, memoise-then-tabulate

- [ ] **Read** · Notes section 3: BFS, backtracking and memo → table skeletons · 45m
- [ ] [70. Climbing Stairs](https://leetcode.com/problems/climbing-stairs/) · Easy · `memoise → tabulate` — added · recursion → memo → table → two variables, all on one problem

### 4. Choosing between them — the signals in the constraints that pick your approach

- [ ] **Read** · Notes section 4: constraint → complexity table + signal words · 30m
- [ ] **Drill** · Classification drill B: 15 more statements, deciding from the constraints alone · 30m
