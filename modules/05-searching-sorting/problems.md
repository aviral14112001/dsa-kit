# 05 · Searching + Sorting
> The two operations that quietly decide the complexity of everything else.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Comparison sorts — merge, quick, heap: stability, in-place, worst cases

- [ ] **Read** · Notes section 1 + templates/sorting.hpp (merge, quick, heap) · 60m
- [ ] [912. Sort an Array](https://leetcode.com/problems/sort-an-array/) · Medium · 📖 · `implement the sorts` — added · submit merge sort, randomized quicksort and heap sort in turn, and see which ones TLE
- [ ] [88. Merge Sorted Array](https://leetcode.com/problems/merge-sorted-array/) · Easy · `merge from the back` — sheet: Merge two sorted arrays without extra space · the merge step, done in place by filling from the end · [video](https://youtu.be/n7uwj04E0I4)
- [ ] [493. Reverse Pairs](https://leetcode.com/problems/reverse-pairs/) · Hard · `merge-sort counting` — count cross pairs during the merge step (the inversion family) · [video](https://youtu.be/0e4bZaP3MDI)

### 2. Non-comparison sorts — counting, radix, bucket, and when they beat O(n log n)

- [ ] **Read** · Notes section 2 + templates/sorting.hpp (counting, radix) · 45m
- [ ] [1122. Relative Sort Array](https://leetcode.com/problems/relative-sort-array/) · Easy · `counting sort` — added · small value range → count, then emit
- [ ] [451. Sort Characters By Frequency](https://leetcode.com/problems/sort-characters-by-frequency/) · Medium · `bucket by frequency` — added · frequencies are ≤ n, so bucket by count: O(n)

### 3. Custom comparators — multi-key ordering, sorting objects, breaking ties

- [ ] **Read** · Notes section 3 · 30m
- [ ] [179. Largest Number](https://leetcode.com/problems/largest-number/) · Medium · 📖 · `comparator on concatenation` — added · order by a+b > b+a; also why a comparator must be a strict weak ordering
- [ ] [973. K Closest Points to Origin](https://leetcode.com/problems/k-closest-points-to-origin/) · Medium · `partial selection` — added · you only need the k smallest: `nth_element` / `partial_sort`

### 4. Search on sorted data — lower / upper bound, first & last occurrence, rotated arrays

- [ ] **Read** · Notes section 4 + templates/binary_search.hpp · 45m
- [ ] [704. Binary Search](https://leetcode.com/problems/binary-search/) · Easy · `binary search invariants` — added · write it once with a clear invariant, and never again with off-by-ones
- [ ] [34. Find First and Last Position of Element in Sorted Array](https://leetcode.com/problems/find-first-and-last-position-of-element-in-sorted-array/) · Medium · 📖 · `lower_bound + upper_bound` — added · first and last position as two boundary searches
- [ ] [33. Search in Rotated Sorted Array](https://leetcode.com/problems/search-in-rotated-sorted-array/) · Medium · `rotated array` — added · one half is always sorted; decide which half to keep
- [ ] [153. Find Minimum in Rotated Sorted Array](https://leetcode.com/problems/find-minimum-in-rotated-sorted-array/) · Medium · `rotated minimum` — compare with the right end, not the left · [video](https://youtu.be/nhEMDKMB44g)
- [ ] [81. Search in Rotated Sorted Array II](https://leetcode.com/problems/search-in-rotated-sorted-array-ii/) · Medium · `rotated with duplicates` — duplicates break the "which half" test: shrink both ends · [video](https://youtu.be/w2G2W8l__pc)
- [ ] [162. Find Peak Element](https://leetcode.com/problems/find-peak-element/) · Medium · `binary search on slope` — the array isn't sorted; follow the uphill side · [video](https://youtu.be/cXxmbemS6XM)
- [ ] [540. Single Element in a Sorted Array](https://leetcode.com/problems/single-element-in-a-sorted-array/) · Medium · `pairing parity` — sheet: Single element in sorted array · before the single element, pairs start at even indices · [video](https://youtu.be/AZOmHuHadxQ)
- [ ] [240. Search a 2D Matrix II](https://leetcode.com/problems/search-a-2d-matrix-ii/) · Medium · `staircase search` — sheet: Search in 2D matrix - II · start at the top-right corner: every comparison discards a row or a column, O(m + n) · [video](https://youtu.be/9ZbB397jU4k)
- [ ] [1901. Find a Peak Element II](https://leetcode.com/problems/find-a-peak-element-ii/) · Medium · `binary search on columns` — sheet: Find Peak Element - II · binary-search the column; the max of the mid column tells you which side holds a peak · [video](https://youtu.be/nGGp5XBzC4g?si=WCop5C6Azj5gAELH)
- [ ] [4. Median of Two Sorted Arrays](https://leetcode.com/problems/median-of-two-sorted-arrays/) · Hard · `partition binary search` — sheet: Median of 2 sorted arrays · binary-search the cut in the smaller array · [video](https://www.youtube.com/watch?v=NTop3VTjmxk&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=65)
