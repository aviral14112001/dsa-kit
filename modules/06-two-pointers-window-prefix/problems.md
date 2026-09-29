# 06 · Two Pointers + Sliding Window + Prefix Sum
> Turn nested loops into a single pass — the most reused trick in the book.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Two pointers — opposite ends, same direction, pair and triplet sums

- [ ] **Read** · Notes section 1 · 45m
- [ ] [167. Two Sum II - Input Array Is Sorted](https://leetcode.com/problems/two-sum-ii-input-array-is-sorted/) · Medium · 📖 · `converging pointers` — added · sorted + target sum: move the pointer that can fix the sum
- [ ] [11. Container With Most Water](https://leetcode.com/problems/container-with-most-water/) · Medium · `converging pointers + proof` — added · moving the taller wall can never help; say why
- [ ] [15. 3Sum](https://leetcode.com/problems/3sum/) · Medium · 📖 · `sort + two pointers + dedupe` — fix one element, two-pointer the rest, skip duplicates at both levels · [video](https://youtu.be/DhFh8Kw7ymk)
- [ ] [18. 4Sum](https://leetcode.com/problems/4sum/) · Medium · `two pointers, one more level` — 3Sum plus an outer loop; watch the overflow · [video](https://youtu.be/eD95WRfh81c)
- [ ] [42. Trapping Rain Water](https://leetcode.com/problems/trapping-rain-water/) · Hard · `two pointers with running maxes` — the lower side's water is already decided · [video](https://youtu.be/1_5VuquLbXg?si=NFG6df318_6OtGvg)

### 2. Fixed & variable windows — longest / shortest window under a constraint

- [ ] **Read** · Notes section 2: fixed, longest, shortest, and count-subarrays windows · 60m
- [ ] [3. Longest Substring Without Repeating Characters](https://leetcode.com/problems/longest-substring-without-repeating-characters/) · Medium · 📖 · `variable window (longest)` — grow right, shrink left until valid, record the max · [video](https://youtu.be/-zSxTJkcdAo?si=I2zfR-vlDMg0zU9z)
- [ ] [1423. Maximum Points You Can Obtain from Cards](https://leetcode.com/problems/maximum-points-you-can-obtain-from-cards/) · Medium · `window on the complement` — taking k cards from the ends leaves a contiguous window of n − k: minimize its sum · [video](https://youtu.be/pBWCOCS636U?si=-X64rY67noxvOwrG)
- [ ] [1004. Max Consecutive Ones III](https://leetcode.com/problems/max-consecutive-ones-iii/) · Medium · `longest window with budget` — at most k zeros inside the window · [video](https://youtu.be/3E4JBHSLpYk?si=SoOW64pP6otEKxBw)
- [ ] [424. Longest Repeating Character Replacement](https://leetcode.com/problems/longest-repeating-character-replacement/) · Medium · `longest window + max-frequency trick` — why the max frequency never needs to decrease · [video](https://youtu.be/_eNhaDCr6P0?si=pBWcEjozF5poom0p)
- [ ] [340. Longest Substring with At Most K Distinct Characters](https://leetcode.com/problems/longest-substring-with-at-most-k-distinct-characters/) · Medium · 🔒 · `variable window + count map` — shrink while the window holds more than k distinct characters · [video](https://youtu.be/teM9ZsVRQyc?si=Kh0_u6aCkkBU3Q33)
- [ ] [1358. Number of Substrings Containing All Three Characters](https://leetcode.com/problems/number-of-substrings-containing-all-three-characters/) · Medium · `count windows` — shortest valid window ending at each r · [video](https://youtu.be/xtqN4qlgr8s?si=kuaLHVOLXhh5Z2tW)
- [ ] [930. Binary Subarrays With Sum](https://leetcode.com/problems/binary-subarrays-with-sum/) · Medium · `exactly K = atMost(K) − atMost(K−1)` — the trick that turns "exactly" into two windows · [video](https://youtu.be/XnMdNUkX6VM?si=Nyt8EveeLUg8lmty)
- [ ] [1248. Count Number of Nice Subarrays](https://leetcode.com/problems/count-number-of-nice-subarrays/) · Medium · `count windows` — exactly k odd numbers · [video](https://youtu.be/j_QOv9OT9Og?si=Oq5-5hyFkzVSOZpP)
- [ ] [76. Minimum Window Substring](https://leetcode.com/problems/minimum-window-substring/) · Hard · 📖 · `shortest window with counts` — "formed" counter + shrink loop: the hardest window template · [video](https://youtu.be/WJaij9ffOIY?si=-xnsWIH84zWU0ICd)

### 3. Prefix & suffix sums — range queries, equilibrium points, 2D prefix grids

- [ ] **Read** · Notes section 3 · 45m
- [ ] [303. Range Sum Query - Immutable](https://leetcode.com/problems/range-sum-query-immutable/) · Easy · `prefix sums` — added · O(1) range sum after O(n) preprocessing
- [ ] [238. Product of Array Except Self](https://leetcode.com/problems/product-of-array-except-self/) · Medium · 📖 · `prefix × suffix products` — added · no division, O(1) extra space besides the output
- [ ] [304. Range Sum Query 2D - Immutable](https://leetcode.com/problems/range-sum-query-2d-immutable/) · Medium · `2D prefix sums` — added · inclusion–exclusion over four corners

### 4. Difference arrays — range updates in O(1), sweep-line counting

- [ ] **Read** · Notes section 4 · 45m
- [ ] [1109. Corporate Flight Bookings](https://leetcode.com/problems/corporate-flight-bookings/) · Medium · 📖 · `difference array` — added · +v at l, −v at r+1, then a prefix sum rebuilds every value
- [ ] [1094. Car Pooling](https://leetcode.com/problems/car-pooling/) · Medium · `difference array over time` — added · capacity check at every stop
