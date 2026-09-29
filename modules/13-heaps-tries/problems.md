# 13 · Heaps + Priority Queue + Tries
> Always-ready extremes, and prefix lookups that scale.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Binary heaps — heapify, push / pop cost, heap sort, k-way merge

- [ ] **Read** · Notes section 1 + templates/heap.hpp (a binary heap you can read end to end) · 45m
- [ ] [703. Kth Largest Element in a Stream](https://leetcode.com/problems/kth-largest-element-in-a-stream/) · Easy · 📖 · `size-k min-heap` — added for the sheet's “Implement Min Heap” (no LeetCode link) · the top is the k-th largest
- [ ] [373. Find K Pairs with Smallest Sums](https://leetcode.com/problems/find-k-pairs-with-smallest-sums/) · Medium · `k-way merge frontier` — added · push (i, 0) pairs, expand on pop

### 2. Priority queues — top-K, running median with two heaps, task scheduling

- [ ] **Read** · Notes section 2 · 45m
- [ ] [215. Kth Largest Element in an Array](https://leetcode.com/problems/kth-largest-element-in-an-array/) · Medium · `quickselect / heap` — average O(n) quickselect vs O(n log k) heap vs `nth_element`
- [ ] [1834. Single-Threaded CPU](https://leetcode.com/problems/single-threaded-cpu/) · Medium · `event simulation` — added · sort by arrival, heap by (duration, index)
- [ ] [295. Find Median from Data Stream](https://leetcode.com/problems/find-median-from-data-stream/) · Hard · 📖 · `two heaps` — max-heap low half, min-heap high half, keep the sizes balanced

### 3. Tries — insert, search, prefix counting, memory trade-offs

- [ ] **Read** · Notes section 3 + templates/trie.hpp · 45m
- [ ] [208. Implement Trie (Prefix Tree)](https://leetcode.com/problems/implement-trie-prefix-tree/) · Medium · 📖 · `implement a trie` — added · children[26] + end flag; pointer vs pool allocation
- [ ] [211. Design Add and Search Words Data Structure](https://leetcode.com/problems/design-add-and-search-words-data-structure/) · Medium · `trie + wildcard DFS` — added · '.' branches into every child
- [ ] [1804. Implement Trie II (Prefix Tree)](https://leetcode.com/problems/implement-trie-ii-prefix-tree/) · Medium · 🔒 · `trie with counts` — sheet: Trie Implementation and Advanced Operations · count words ending at each node and words passing through it; erase decrements both
- [ ] [1858. Longest Word With All Prefixes](https://leetcode.com/problems/longest-word-with-all-prefixes/) · Medium · 🔒 · `trie + DFS through word ends` — a word qualifies only if every prefix is also a word; tie-break lexicographically · [video](https://www.youtube.com/watch?v=AWnBa91lThI&list=PLgUwDviBIf0pcIDCZnxhv0LkHf5KzG9zp&index=3)

### 4. Applications — autocomplete, spell check, streaming statistics

- [ ] **Read** · Notes section 4 · 30m
- [ ] [1268. Search Suggestions System](https://leetcode.com/problems/search-suggestions-system/) · Medium · 📖 · `autocomplete` — added · a trie with top-3 lists per node, vs sort + lower_bound
- [ ] [212. Word Search II](https://leetcode.com/problems/word-search-ii/) · Hard · `trie + backtracking` — added · one DFS over the board for all the words; prune found words
- [ ] [1698. Number of Distinct Substrings in a String](https://leetcode.com/problems/number-of-distinct-substrings-in-a-string/) · Medium · 🔒 · `trie of suffixes` — insert every suffix: each new trie node is a new distinct substring (a suffix array does it faster) · [video](https://www.youtube.com/watch?v=RV0QeTyHZxo&list=PLgUwDviBIf0pcIDCZnxhv0LkHf5KzG9zp&index=4)
