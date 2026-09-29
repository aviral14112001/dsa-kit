# 17 · Advanced Data Structures + Range Queries + Project
> For when queries outnumber updates — or the other way round.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Sparse tables — immutable range min / max in O(1) after preprocessing

- [ ] **Read** · Notes section 1 + templates/sparse_table.hpp · 45m

### 2. Fenwick tree (BIT) — point update, prefix query, inversion counting

- [ ] **Read** · Notes section 2 + templates/fenwick.hpp · 45m
- [ ] [307. Range Sum Query - Mutable](https://leetcode.com/problems/range-sum-query-mutable/) · Medium · 📖 · `Fenwick tree` — added · i & −i walks up for updates, down for queries
- [ ] [315. Count of Smaller Numbers After Self](https://leetcode.com/problems/count-of-smaller-numbers-after-self/) · Hard · 📖 · `BIT + coordinate compression` — added · scan from the right, query the count of smaller values seen

### 3. Segment trees — range sum / min, lazy propagation, range assignment

- [ ] **Read** · Notes section 3 + templates/segment_tree.hpp · 60m
- [ ] [732. My Calendar III](https://leetcode.com/problems/my-calendar-iii/) · Hard · `sweep map / dynamic segment tree` — added · max overlap after each booking
- [ ] [699. Falling Squares](https://leetcode.com/problems/falling-squares/) · Hard · `coordinate compression + range max` — added · each square lands on the max height inside its range, then raises that range: a range-assign / range-max segment tree

### 4. Project — a range-query engine over a live data stream, benchmarked against brute force

- [ ] **Read** · projects/17-range-query-engine/README.md: spec + milestones · 30m
- [ ] **Project** · Milestone 1: brute-force baseline + differential tester, then Fenwick + sparse table behind the engine interface · 90m
- [ ] **Project** · Milestone 2: lazy segment tree (add + assign, sum + min), stress-tested against brute force · 90m
- [ ] **Project** · Milestone 3: benchmark n = 10³…10⁶; write up where each structure wins in the README · 45m
