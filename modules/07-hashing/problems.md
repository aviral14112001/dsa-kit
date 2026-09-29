# 07 · Hashing
> O(1) lookup — and the mistakes that silently make it O(n).

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Hash maps & sets — frequency counts, grouping, de-duplication

- [ ] **Read** · Notes section 1 · 45m
- [ ] [1. Two Sum](https://leetcode.com/problems/two-sum/) · Easy · 📖 · `complement lookup` — store what you've seen and look up target − x: the move every hashing problem builds on · [video](https://youtu.be/UXDSeD9mN-k)
- [ ] [169. Majority Element](https://leetcode.com/problems/majority-element/) · Easy · `count map → Boyer–Moore` — count with a hash map first, then the O(1)-space voting trick as the follow-up · [video](https://youtu.be/nP_ns3uSh80)
- [ ] [229. Majority Element II](https://leetcode.com/problems/majority-element-ii/) · Medium · `Boyer–Moore, two candidates` — at most two values can exceed n/3: keep two candidates, then verify both · [video](https://youtu.be/vwZj1K0e9U8)
- [ ] [49. Group Anagrams](https://leetcode.com/problems/group-anagrams/) · Medium · 📖 · `group by signature` — added · the sorted string (or a count vector) as the key
- [ ] [128. Longest Consecutive Sequence](https://leetcode.com/problems/longest-consecutive-sequence/) · Medium · `set + sequence starts` — only start counting at x when x−1 is absent: O(n) · [video](https://youtu.be/oO5uLE7EUlM)

### 2. Hash functions & collisions — chaining, open addressing, load factor

- [ ] **Read** · Notes section 2 + examples/hashmap_chaining.cpp · 45m
- [ ] [706. Design HashMap](https://leetcode.com/problems/design-hashmap/) · Easy · 📖 · `build your own hash map` — added · chaining first, then open addressing with tombstones

### 3. Key design — tuples, sorted signatures, custom hashable objects

- [ ] **Read** · Notes section 3: pair keys, custom hash, coordinate encoding, splitmix64 · 45m
- [ ] [36. Valid Sudoku](https://leetcode.com/problems/valid-sudoku/) · Medium · `composite keys` — added · the box index (r/3)*3 + c/3 as part of the key

### 4. Prefix sum + hash map — subarray-sum-equals-K and its whole family

- [ ] **Read** · Notes section 4 · 45m
- [ ] [560. Subarray Sum Equals K](https://leetcode.com/problems/subarray-sum-equals-k/) · Medium · 📖 · `prefix sum + count map` — sheet: Count subarrays with given sum · count earlier prefixes equal to prefix − k (seed the map with {0: 1}) · [video](https://www.youtube.com/watch?v=xvNwoz-ufXA&list=PLgUwDviBIf0oF6QL8m22w1hIDC1vJ_BHz&index=32)
- [ ] [325. Maximum Size Subarray Sum Equals k](https://leetcode.com/problems/maximum-size-subarray-sum-equals-k/) · Medium · 🔒 · `prefix sum + first index` — sheet: Longest subarray with sum K · store the first index of each prefix sum to get the longest span with sum k · [video](https://youtu.be/frf7qxiN2qU)
- [ ] [525. Contiguous Array](https://leetcode.com/problems/contiguous-array/) · Medium · `prefix balance + first index` — added · map 0 → −1, 1 → +1; longest zero-sum span
- [ ] [974. Subarray Sums Divisible by K](https://leetcode.com/problems/subarray-sums-divisible-by-k/) · Medium · `prefix mod + count map` — added · C++ `%` can be negative: normalize with ((x % k) + k) % k
