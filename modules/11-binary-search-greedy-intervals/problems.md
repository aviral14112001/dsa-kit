# 11 · Binary Search + Greedy + Intervals
> Three techniques that turn “too slow” into “fast enough”.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Binary search on the answer — minimum feasible value, predicate monotonicity

- [ ] **Read** · Notes section 1 (uses templates/binary_search.hpp) · 45m
- [ ] [875. Koko Eating Bananas](https://leetcode.com/problems/koko-eating-bananas/) · Medium · 📖 · `BS on answer (min feasible)` — speed → hours is monotone: find the first speed that works · [video](https://youtu.be/qyfekrNni90)
- [ ] [1552. Magnetic Force Between Two Balls](https://leetcode.com/problems/magnetic-force-between-two-balls/) · Medium · `maximize the minimum` — added for the sheet's “Aggressive Cows” (no LeetCode link) · greedy placement as the feasibility test
- [ ] [410. Split Array Largest Sum](https://leetcode.com/problems/split-array-largest-sum/) · Hard · `minimize the maximum` — sheet: Book Allocation Problem · a greedy split as the feasibility test · [video](https://www.youtube.com/watch?v=gYmWHvRHu-s&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=69)
- [ ] [2387. Median of a Row Wise Sorted Matrix](https://leetcode.com/problems/median-of-a-row-wise-sorted-matrix/) · Medium · 🔒 · `BS on value + counting` — sheet: Matrix Median · binary-search the answer value; count elements ≤ mid with upper_bound per row · [video](https://youtu.be/Q9wXgdxJq48?si=ScI_0uzJh7yg8nrX)
- [ ] [378. Kth Smallest Element in a Sorted Matrix](https://leetcode.com/problems/kth-smallest-element-in-a-sorted-matrix/) · Medium · `BS on value + counting` — added (free twin of 2387) · count elements ≤ mid with a staircase walk

### 2. Greedy proofs — the exchange argument, and when local optimum is actually safe

- [ ] **Read** · Notes section 2: exchange argument, stays-ahead, and counterexamples where greedy fails · 60m
- [ ] [55. Jump Game](https://leetcode.com/problems/jump-game/) · Medium · `farthest reach` — track the max reachable index · [video](https://youtu.be/tZAa_jJ3SwQ?si=voKd7n9VTLDRRNzJ)
- [ ] [678. Valid Parenthesis String](https://leetcode.com/problems/valid-parenthesis-string/) · Medium · `range of open counts` — sheet: Valid Paranthesis Checker · track the min and max possible open brackets · [video](https://youtu.be/cHT6sG_hUZI?si=XRHeyh7jOaLaTy3g)
- [ ] [135. Candy](https://leetcode.com/problems/candy/) · Hard · `two passes` — left-to-right, then right-to-left · [video](https://youtu.be/IIqVFvKE6RY?si=EjmuXZJNLQLUkEd7)

### 3. Interval problems — merging, insertion, minimum meeting rooms

- [ ] **Read** · Notes section 3 · 45m
- [ ] [56. Merge Intervals](https://leetcode.com/problems/merge-intervals/) · Medium · 📖 · `sort by start + merge` — added · the base template for every interval problem
- [ ] [57. Insert Interval](https://leetcode.com/problems/insert-interval/) · Medium · `three phases` — before, overlapping, after · [video](https://youtu.be/xxRE-46OCC8?si=a7aPuIw16zDx2lAa)
- [ ] [435. Non-overlapping Intervals](https://leetcode.com/problems/non-overlapping-intervals/) · Medium · `sort by end (activity selection)` — keep the interval that ends first · [video](https://youtu.be/HDHQ8lAWakY?si=JVtLqboGdpUTOVjf)
- [ ] [2406. Divide Intervals Into Minimum Number of Groups](https://leetcode.com/problems/divide-intervals-into-minimum-number-of-groups/) · Medium · 📖 · `min rooms (heap or sweep)` — sheet: Minimum number of platforms required for a railway · the free twin of Meeting Rooms II: max overlap at any point · [video](https://youtu.be/AsGzwR_FWok?si=165acXU_dtqOHuo9)

### 4. Scheduling & allocation — activity selection, deadlines, ship-within-D-days

- [ ] **Read** · Notes section 4 · 30m
- [ ] [1011. Capacity To Ship Packages Within D Days](https://leetcode.com/problems/capacity-to-ship-packages-within-d-days/) · Medium · `BS on capacity` — added · ship within D days: greedy day filling as the feasibility test
- [ ] [1353. Maximum Number of Events That Can Be Attended](https://leetcode.com/problems/maximum-number-of-events-that-can-be-attended/) · Medium · `deadlines + min-heap` — added for the sheet's “Job sequencing Problem” (no LeetCode link) · each day, attend the open event that ends soonest
