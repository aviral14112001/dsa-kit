# 16 · Dynamic Programming
> The topic that decides most offers — taught as a recipe, not a flash of insight.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. State design — defining dp[i], the transition, and proving nothing is missed

- [ ] **Read** · Notes section 1: the recipe (state, transition, base case, order, answer) · 60m
- [ ] [746. Min Cost Climbing Stairs](https://leetcode.com/problems/min-cost-climbing-stairs/) · Easy · `dp[i] = min cost to reach i` — added for the sheet's “Frog jump with K distances” (no LeetCode link) · the recipe on the smallest possible state
- [ ] [198. House Robber](https://leetcode.com/problems/house-robber/) · Medium · 📖 · `take / skip` — at each house: take it or skip it; what does each choice leave behind? · [video](https://www.youtube.com/watch?v=3WaxQMELSkw)
- [ ] [139. Word Break](https://leetcode.com/problems/word-break/) · Medium · `prefix reachability` — added · can the prefix of length i be segmented? think about where its last word starts

### 2. Memoisation → tabulation — recursion first, then bottom-up, then space-optimised

- [ ] **Read** · Notes section 2 · 45m
- [ ] [62. Unique Paths](https://leetcode.com/problems/unique-paths/) · Medium · 📖 · `grid paths` — added · memo → table → one row: the three stages side by side
- [ ] [63. Unique Paths II](https://leetcode.com/problems/unique-paths-ii/) · Medium · `grid paths with obstacles` — zero out the blocked cells · [video](https://www.youtube.com/watch?v=TmhpgXScLyY)

### 3. Classic families — knapsack, LIS, LCS, edit distance, coin change, partitions

- [ ] **Read** · Notes section 3: 0/1 vs unbounded loops, LIS in O(n log n), LCS / edit distance tables · 90m
- [ ] [416. Partition Equal Subset Sum](https://leetcode.com/problems/partition-equal-subset-sum/) · Medium · 📖 · `0/1 knapsack (subset sum)` — added for the sheet's “0 and 1 Knapsack” (no LeetCode link) · iterate capacity downward so each item is used once
- [ ] [494. Target Sum](https://leetcode.com/problems/target-sum/) · Medium · `subset-sum count` — turn ± signs into a subset with sum (total + target) / 2 · [video](https://www.youtube.com/watch?v=b3GD8263-PQ)
- [ ] [1049. Last Stone Weight II](https://leetcode.com/problems/last-stone-weight-ii/) · Medium · `0/1 knapsack` — sheet: Partition a set into two subsets with minimum absolute sum difference · split the stones into two piles as evenly as possible · [video](https://www.youtube.com/watch?v=GS_OqZb2CWc)
- [ ] [518. Coin Change II](https://leetcode.com/problems/coin-change-ii/) · Medium · `unbounded knapsack (count)` — coins in the outer loop → combinations, not permutations · [video](https://www.youtube.com/watch?v=HgyouUi11zk)
- [ ] [322. Coin Change](https://leetcode.com/problems/coin-change/) · Medium · 📖 · `unbounded knapsack (min)` — added · coins can repeat: iterate capacity upward
- [ ] [300. Longest Increasing Subsequence](https://leetcode.com/problems/longest-increasing-subsequence/) · Medium · 📖 · `LIS` — O(n²) DP, then the O(n log n) tails array with `lower_bound` · [video](https://youtu.be/on2hvxBXJH4)
- [ ] [673. Number of Longest Increasing Subsequence](https://leetcode.com/problems/number-of-longest-increasing-subsequence/) · Medium · `LIS with counts` — track the length and the count per index · [video](https://youtu.be/cKVl1TFdNXg)
- [ ] [1143. Longest Common Subsequence](https://leetcode.com/problems/longest-common-subsequence/) · Medium · 📖 · `LCS table` — match → diagonal + 1, else max(up, left) · [video](https://youtu.be/-zI4mrF2Pb4)
- [ ] [718. Maximum Length of Repeated Subarray](https://leetcode.com/problems/maximum-length-of-repeated-subarray/) · Medium · `LCS-style table, contiguous` — added for the sheet's “Longest common substring” (no LeetCode link) · like LCS, but a mismatch breaks the run: what should the cell hold then?
- [ ] [516. Longest Palindromic Subsequence](https://leetcode.com/problems/longest-palindromic-subsequence/) · Medium · `LCS with the reverse` — relate it to a problem you've already solved on two strings, or think in intervals [l, r] · [video](https://youtu.be/6i_T5kkfv4A)
- [ ] [72. Edit Distance](https://leetcode.com/problems/edit-distance/) · Medium · 📖 · `edit distance` — insert / delete / replace as three neighbours · [video](https://youtu.be/fJaKO8FbDdo)
- [ ] [44. Wildcard Matching](https://leetcode.com/problems/wildcard-matching/) · Hard · `pattern DP` — '*' matches any sequence · [video](https://youtu.be/ZmlQ3vgAOMo)
- [ ] [714. Best Time to Buy and Sell Stock with Transaction Fee](https://leetcode.com/problems/best-time-to-buy-and-sell-stock-with-transaction-fee/) · Medium · `state machine + fee` — hold / free states · [video](https://youtu.be/k4eK-vEmnKg)
- [ ] [188. Best Time to Buy and Sell Stock IV](https://leetcode.com/problems/best-time-to-buy-and-sell-stock-iv/) · Hard · `state machine, k transactions` — dp over (transactions, holding) · [video](https://youtu.be/IV1dHbk5CDc)

### 4. Harder shapes — interval DP, DP on trees, bitmask DP, digit DP

- [ ] **Read** · Notes section 4 · 90m
- [ ] [1039. Minimum Score Triangulation of Polygon](https://leetcode.com/problems/minimum-score-triangulation-of-polygon/) · Medium · `interval DP` — added for the sheet's “Matrix chain multiplication” (no LeetCode link) · choose the triangle on edge (i, j)
- [ ] [312. Burst Balloons](https://leetcode.com/problems/burst-balloons/) · Hard · 📖 · `interval DP (last to burst)` — added · pick the LAST balloon in (l, r) so the subproblems stay independent
- [ ] [132. Palindrome Partitioning II](https://leetcode.com/problems/palindrome-partitioning-ii/) · Hard · `palindrome table + cuts` — an O(n²) palindrome precompute · [video](https://youtu.be/_H8V5hJUGd0)
- [ ] [337. House Robber III](https://leetcode.com/problems/house-robber-iii/) · Medium · 📖 · `tree DP` — added · return (rob this node, skip this node) from each child
- [ ] [1986. Minimum Number of Work Sessions to Finish the Tasks](https://leetcode.com/problems/minimum-number-of-work-sessions-to-finish-the-tasks/) · Medium · 📖 · `bitmask DP` — added · dp[mask] = (sessions, time used in the last one)
- [ ] [2376. Count Special Integers](https://leetcode.com/problems/count-special-integers/) · Hard · 📖 · `digit DP` — added · position, tight flag, used-digit mask
