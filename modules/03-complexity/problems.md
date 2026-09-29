# 03 · Complexity Analysis & Patterns
> Reason about cost before you write a line — and defend the number out loud.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Big-O, Big-Θ, Big-Ω — worst, average and amortized analysis

- [ ] **Read** · Notes section 1 + examples/amortized_vector.cpp · 45m
- [ ] **Drill** · Analyze snippets 1–8 in Notes section 1 before opening the answers · 30m

### 2. Recurrences — recursion trees, the master theorem, divide-and-conquer costs

- [ ] **Read** · Notes section 2 · 45m
- [ ] [509. Fibonacci Number](https://leetcode.com/problems/fibonacci-number/) · Easy · 📖 · `recursion tree` — added · naive O(φⁿ) → memo O(n) → two variables O(1) space: count the calls to see it
- [ ] **Drill** · Solve recurrences 1–8 in Notes section 2 (master theorem or recursion tree) · 30m

### 3. Space complexity — auxiliary vs total, recursion stack, in-place transforms

- [ ] **Read** · Notes section 3 · 30m

### 4. Reading constraints — deriving the target complexity from n before you code

- [ ] **Read** · Notes section 4: the n → complexity table · 30m
- [ ] [2006. Count Number of Pairs With Absolute Difference K](https://leetcode.com/problems/count-number-of-pairs-with-absolute-difference-k/) · Easy · `constraints allow O(n²)` — added · n ≤ 200: the brute force IS the answer; say why out loud
- [ ] [2367. Number of Arithmetic Triplets](https://leetcode.com/problems/number-of-arithmetic-triplets/) · Easy · `O(n³) → O(n)` — added · brute force passes, then improve it with a set, and compare the two
- [ ] **Drill** · Ten constraint blocks in Notes section 4: write the target complexity + a candidate technique for each · 20m
