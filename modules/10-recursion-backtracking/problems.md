# 10 · Recursion + Backtracking
> Search the whole space — then learn where to stop searching.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Recursion mechanics — base cases, the call stack, converting to iteration

- [ ] **Read** · Notes section 1 · 45m
- [ ] [50. Pow(x, n)](https://leetcode.com/problems/powx-n/) · Medium · 📖 · `halving recursion` — added · x^n = (x^(n/2))², O(log n); the n = INT_MIN trap; then the iterative version

### 2. Subsets, permutations & combinations — the choose / don't-choose tree

- [ ] **Read** · Notes section 2: the subsets, combinations and permutations templates; handling duplicates · 60m
- [ ] [78. Subsets](https://leetcode.com/problems/subsets/) · Medium · 📖 · `subsets` — sheet: Power Set (and "Power Set Bit Manipulation": redo it with masks) · the choose / skip tree: 2ⁿ leaves · [video](https://www.youtube.com/watch?v=b7AYbpM5YrE&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=67)
- [ ] [90. Subsets II](https://leetcode.com/problems/subsets-ii/) · Medium · `subsets with duplicates` — sort, then skip equal values at the same depth · [video](https://www.youtube.com/watch?v=RIn3gOkbhQE&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=53)
- [ ] [46. Permutations](https://leetcode.com/problems/permutations/) · Medium · 📖 · `permutations` — added · a used[] array (or swapping); n! leaves
- [ ] [22. Generate Parentheses](https://leetcode.com/problems/generate-parentheses/) · Medium · `constrained generation` — open < n → add '('; close < open → add ')'
- [ ] [17. Letter Combinations of a Phone Number](https://leetcode.com/problems/letter-combinations-of-a-phone-number/) · Medium · `cartesian product` — one digit per level
- [ ] [39. Combination Sum](https://leetcode.com/problems/combination-sum/) · Medium · `combinations, reuse allowed` — recurse from i, not i+1; sort to break early · [video](https://www.youtube.com/watch?v=OyZFFqQtu98&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=49)
- [ ] [40. Combination Sum II](https://leetcode.com/problems/combination-sum-ii/) · Medium · `combinations, no reuse, duplicates` — 39 + skip duplicates at the same depth · [video](https://www.youtube.com/watch?v=G1fRTGRxXU8&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=50)
- [ ] [131. Palindrome Partitioning](https://leetcode.com/problems/palindrome-partitioning/) · Medium · `partition backtracking` — cut at every palindromic prefix · [video](https://youtu.be/_H8V5hJUGd0)

### 3. Constraint problems — N-Queens, Sudoku, word search, rat in a maze

- [ ] **Read** · Notes section 3 · 45m
- [ ] [79. Word Search](https://leetcode.com/problems/word-search/) · Medium · `grid DFS with marking` — mark visited in place, then restore it on return
- [ ] [1219. Path with Maximum Gold](https://leetcode.com/problems/path-with-maximum-gold/) · Medium · `grid DFS with restore` — added · maximize the gold on a path
- [ ] [51. N-Queens](https://leetcode.com/problems/n-queens/) · Hard · 📖 · `constraint sets` — column and both diagonals as O(1) lookups · [video](https://www.youtube.com/watch?v=i05Ju7AftcM&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=57)
- [ ] [37. Sudoku Solver](https://leetcode.com/problems/sudoku-solver/) · Hard · `constraint propagation` — row/col/box bitmasks; try the cell with the fewest options · [video](https://www.youtube.com/watch?v=FWAIf_EVUKE&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=58)

### 4. Pruning — feasibility checks and orderings that cut the tree early

- [ ] **Read** · Notes section 4 · 30m
- [ ] [698. Partition to K Equal Sum Subsets](https://leetcode.com/problems/partition-to-k-equal-sum-subsets/) · Medium · 📖 · `ordering + symmetry pruning` — added · sort descending, skip empty buckets after the first
