# 08 · Linked Lists
> Pointer discipline — where careless code becomes a null-pointer bug.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Singly & doubly lists — insert, delete, reverse, the dummy-head technique

- [ ] **Read** · Notes section 1 · 45m
- [ ] [206. Reverse Linked List](https://leetcode.com/problems/reverse-linked-list/) · Easy · 📖 · `reverse (iterative + recursive)` — sheet: Reverse a LL · three pointers: prev, cur, next · [video](https://youtu.be/D2vI2DNJGd8?si=RCaLSx01qR21IBdh)
- [ ] [2. Add Two Numbers](https://leetcode.com/problems/add-two-numbers/) · Medium · `dummy + carry` — build the result while walking both lists · [video](https://www.youtube.com/watch?v=LBVsXSMOIk4&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=32)
- [ ] [61. Rotate List](https://leetcode.com/problems/rotate-list/) · Medium · `ring then cut` — sheet: Rotate a LL · close the list into a ring, cut at n − k · [video](https://youtu.be/uT7YI7XbTY8?si=ZaChW3a68c_v54Is)
- [ ] [25. Reverse Nodes in k-Group](https://leetcode.com/problems/reverse-nodes-in-k-group/) · Hard · `reverse in k-groups` — sheet: Reverse LL in group of given size K · reverse one k-node block at a time (sublist reversal, Notes section 1); check k nodes remain first · [video](https://youtu.be/lIar1skcQYI?si=_jFghHKX4eaK36a1)

### 2. Fast & slow pointers — cycle detection, middle node, k-th from the end

- [ ] **Read** · Notes section 2 · 45m
- [ ] [876. Middle of the Linked List](https://leetcode.com/problems/middle-of-the-linked-list/) · Easy · `fast & slow pointers` — sheet: Find Middle of Linked List · when fast reaches the end, slow is at the middle · [video](https://youtu.be/7LjQ57RqgEc?si=ir_rRDio38rhamU_)
- [ ] [141. Linked List Cycle](https://leetcode.com/problems/linked-list-cycle/) · Easy · `Floyd cycle detection` — sheet: Detect a loop in LL · fast meets slow iff there is a cycle · [video](https://youtu.be/wiOo4DC5GGA?si=zagt6O6tFXc4_3cx)
- [ ] [142. Linked List Cycle II](https://leetcode.com/problems/linked-list-cycle-ii/) · Medium · 📖 · `Floyd cycle entry` — sheet: Find the starting point in LL · the meeting-point proof tells you where the cycle starts · [video](https://youtu.be/2Kd0KKmmHFc?si=7UreDPRjRvapeVB0)
- [ ] [160. Intersection of Two Linked Lists](https://leetcode.com/problems/intersection-of-two-linked-lists/) · Easy · `pointer switching` — sheet: Find the intersection point of Y LL · walk A then B vs B then A; they meet at the intersection · [video](https://youtu.be/0DYoPz2Tpt4?si=L-uJs5yXUxj4VJM2)
- [ ] [234. Palindrome Linked List](https://leetcode.com/problems/palindrome-linked-list/) · Easy · `middle + reverse half` — sheet: Check if LL is palindrome or not · O(1) space palindrome check · [video](https://youtu.be/lRY_G-u_8jk?si=BpM8hRYvXSYyjl-G)
- [ ] [19. Remove Nth Node From End of List](https://leetcode.com/problems/remove-nth-node-from-end-of-list/) · Medium · `gap pointers` — sheet: Remove Nth node from the back of the LL · lead by n, then move both; the dummy head handles removing the head · [video](https://youtu.be/3kMKYQ2wNIU?si=DtFDnPU7z9HMz_GM)

### 3. Merging & sorting — merge two lists, merge k lists, merge sort in place

- [ ] **Read** · Notes section 3 · 30m
- [ ] [21. Merge Two Sorted Lists](https://leetcode.com/problems/merge-two-sorted-lists/) · Easy · `merge with dummy` — splice nodes; don't allocate new ones · [video](https://www.youtube.com/watch?v=Xb4slcp1U38&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=29)
- [ ] [148. Sort List](https://leetcode.com/problems/sort-list/) · Medium · `merge sort on a list` — sheet: Sort LL · top-down with fast/slow split; bottom-up for O(1) space · [video](https://youtu.be/8ocB7a_c-Cc?si=Gv-Y8q8-WyARoV35)
- [ ] [23. Merge k Sorted Lists](https://leetcode.com/problems/merge-k-sorted-lists/) · Hard · 📖 · `k-way merge` — sheet: Flattening of LL · a min-heap of heads vs pairwise divide & conquer; both O(N log k) · [video](https://youtu.be/ykelywHJWLg?si=InMg9MmTHzY22NSR)

### 4. Composite structures — LRU cache, skip lists, list-backed queues

- [ ] **Read** · Notes section 4: `std::list` + `unordered_map<key, iterator>`, splice, skip lists · 45m
- [ ] [146. LRU Cache](https://leetcode.com/problems/lru-cache/) · Medium · 📖 · `list + hash map` — O(1) get/put: move to front with `splice`, evict from the back
- [ ] [138. Copy List with Random Pointer](https://leetcode.com/problems/copy-list-with-random-pointer/) · Medium · `copy with extra pointers` — sheet: Clone a LL with random and next pointer · an old → new map, or interleave the copies for O(1) space · [video](https://youtu.be/q570bKdrnlw?si=epZtpWvtNwuTf23o)
- [ ] [460. LFU Cache](https://leetcode.com/problems/lfu-cache/) · Hard · `frequency buckets of lists` — an O(1) LFU with a min-frequency pointer · [video](https://www.youtube.com/watch?v=0PSB9y8ehbk&list=PLgUwDviBIf0p4ozDR_kJJkONnb1wdx2Ma&index=79)
