# Schedule

<!-- schedule-args: scripts/schedule.py --start 2026-09-27 --all -->
**Sun 27 Sep 2026 → Thu 28 Jan 2027** · 18 weeks · ~248 h planned · weekdays 2h00, weekends 2h30

Generated from the `## Core` items in `modules/*/problems.md`; ticking boxes there is what counts. Behind or ahead? Run `make replan`: it re-plans only the unchecked items, starting today. `make today` shows today's slice, and anything overdue.

Every Sunday opens with a review hour, and from 31 Oct every Saturday opens with a timed mock. Planning times: Easy 20 min · Medium 40 · Hard 60 · re-solving a 📖 worked example 15/25/35. Hit a timebox? Read a hint, finish, tag the line `#redo`.

## At a glance

| Week | Dates | Modules |
|---|---|---|
| 1 | 27 Sep – 3 Oct | 01 Programming Language Concepts (C++), 02 Common DSA Patterns, 03 Complexity Analysis & Patterns |
| 2 | 4 Oct – 10 Oct | 03 Complexity Analysis & Patterns, 04 Arrays & Strings, 05 Searching + Sorting |
| 3 | 11 Oct – 17 Oct | 05 Searching + Sorting, 06 Two Pointers + Sliding Window + Prefix Sum |
| 4 | 18 Oct – 24 Oct | 06 Two Pointers + Sliding Window + Prefix Sum, 07 Hashing |
| 5 | 25 Oct – 31 Oct | 07 Hashing, 08 Linked Lists |
| 6 | 1 Nov – 7 Nov | 08 Linked Lists, 09 Stacks + Queues + Deque |
| 7 | 8 Nov – 14 Nov | 09 Stacks + Queues + Deque, 10 Recursion + Backtracking |
| 8 | 15 Nov – 21 Nov | 10 Recursion + Backtracking, 11 Binary Search + Greedy + Intervals |
| 9 | 22 Nov – 28 Nov | 11 Binary Search + Greedy + Intervals, 12 Trees + Binary Trees + BST + Project |
| 10 | 29 Nov – 5 Dec | 12 Trees + Binary Trees + BST + Project |
| 11 | 6 Dec – 12 Dec | 12 Trees + Binary Trees + BST + Project, 13 Heaps + Priority Queue + Tries |
| 12 | 13 Dec – 19 Dec | 13 Heaps + Priority Queue + Tries, 14 Graphs + BFS + DFS + Topological Sort + DSU |
| 13 | 20 Dec – 26 Dec | 14 Graphs + BFS + DFS + Topological Sort + DSU, 15 Shortest Path + MST + Advanced Graphs |
| 14 | 27 Dec – 2 Jan | 15 Shortest Path + MST + Advanced Graphs, 16 Dynamic Programming |
| 15 | 3 Jan – 9 Jan | 16 Dynamic Programming |
| 16 | 10 Jan – 16 Jan | 16 Dynamic Programming, 17 Advanced Data Structures + Range Queries + Project |
| 17 | 17 Jan – 23 Jan | 17 Advanced Data Structures + Range Queries + Project, 18 Tries at Scale + Bit Manipulation + Number Theory + String Algorithms |
| 18 | 24 Jan – 28 Jan | 18 Tries at Scale + Bit Manipulation + Number Theory + String Algorithms, 19 Company Specific Questions |

## Week 1 · 27 Sep – 3 Oct 2026

| Day | Plan | Time |
|---|---|---|
| Sun 27 Sep | `01` Read: Notes section 1, then run `make test` and `make run F=practice/_template.cpp` once<br>`01` LC 1929 Concatenation of Array<br>`01` LC 1480 Running Sum of 1d Array<br>`01` Read: Notes section 2 + examples/value_vs_reference.cpp + examples/overflow.cpp | 2h25 |
| Mon 28 Sep | `01` LC 7 Reverse Integer<br>`01` LC 1822 Sign of the Product of an Array<br>`01` Read: Notes section 3 + examples/stl_tour.cpp + examples/comparators.cpp | 2h00 |
| Tue 29 Sep | `01` LC 217 Contains Duplicate<br>`01` LC 242 Valid Anagram<br>`01` LC 1636 Sort Array by Increasing Frequency 📖<br>`01` LC 1768 Merge Strings Alternately<br>`01` Read: Notes section 4 + examples/fast_io.cpp (the stdin template OAs expect) | 2h00 |
| Wed 30 Sep | `01` Drill: Type the LeetCode skeleton and the stdin template from memory until each takes under 2 minutes<br>`02` Read: Notes section 1: the four-question triage + the master pattern table<br>`02` Drill: Classification drill A: the 15 statements in Notes section 1 → pattern + one-line reason (answers are hidden there)<br>`02` Read: Notes section 2: the four skeletons, each with one traced example | 2h15 |
| Thu 1 Oct | `02` LC 125 Valid Palindrome<br>`02` LC 643 Maximum Average Subarray I<br>`02` LC 69 Sqrt(x)<br>`02` Read: Notes section 3: BFS, backtracking and memo → table skeletons<br>`02` LC 70 Climbing Stairs | 2h05 |
| Fri 2 Oct | `02` Read: Notes section 4: constraint → complexity table + signal words<br>`02` Drill: Classification drill B: 15 more statements, deciding from the constraints alone<br>`03` Read: Notes section 1 + examples/amortized_vector.cpp<br>`03` Drill: Analyze snippets 1–8 in Notes section 1 before opening the answers | 2h15 |
| Sat 3 Oct | `03` Read: Notes section 2<br>`03` LC 509 Fibonacci Number 📖<br>`03` Drill: Solve recurrences 1–8 in Notes section 2 (master theorem or recursion tree)<br>`03` Read: Notes section 3<br>`03` Read: Notes section 4: the n → complexity table | 2h30 |

## Week 2 · 4 Oct – 10 Oct 2026

| Day | Plan | Time |
|---|---|---|
| Sun 4 Oct | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`03` LC 2006 Count Number of Pairs With Absolute Dif…<br>`03` LC 2367 Number of Arithmetic Triplets<br>`03` Drill: Ten constraint blocks in Notes section 4: write the target complexity + a candidate technique for each<br>`04` Read: Notes section 1 | 2h45 |
| Mon 5 Oct | `04` LC 283 Move Zeroes<br>`04` LC 189 Rotate Array 📖<br>`04` LC 75 Sort Colors 📖<br>`04` LC 31 Next Permutation | 1h50 |
| Tue 6 Oct | `04` Read: Notes section 2<br>`04` LC 54 Spiral Matrix 📖<br>`04` LC 48 Rotate Image 📖<br>`04` LC 73 Set Matrix Zeroes | 2h15 |
| Wed 7 Oct | `04` Read: Notes section 3<br>`04` LC 680 Valid Palindrome II<br>`04` LC 151 Reverse Words in a String<br>`04` LC 5 Longest Palindromic Substring 📖 | 2h10 |
| Thu 8 Oct | `04` Read: Notes section 4: counting subarrays, Kadane<br>`04` LC 53 Maximum Subarray 📖<br>`04` LC 152 Maximum Product Subarray | 1h50 |
| Fri 9 Oct | `05` Read: Notes section 1 + templates/sorting.hpp (merge, quick, heap)<br>`05` LC 912 Sort an Array 📖<br>`05` LC 88 Merge Sorted Array | 1h45 |
| Sat 10 Oct | `05` LC 493 Reverse Pairs<br>`05` Read: Notes section 2 + templates/sorting.hpp (counting, radix)<br>`05` LC 1122 Relative Sort Array<br>`05` LC 451 Sort Characters By Frequency | 2h45 |

## Week 3 · 11 Oct – 17 Oct 2026

| Day | Plan | Time |
|---|---|---|
| Sun 11 Oct | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`05` Read: Notes section 3<br>`05` LC 179 Largest Number 📖<br>`05` LC 973 K Closest Points to Origin | 2h35 |
| Mon 12 Oct | `05` Read: Notes section 4 + templates/binary_search.hpp<br>`05` LC 704 Binary Search<br>`05` LC 34 Find First and Last Position of Element… 📖<br>`05` LC 33 Search in Rotated Sorted Array | 2h10 |
| Tue 13 Oct | `05` LC 153 Find Minimum in Rotated Sorted Array<br>`05` LC 81 Search in Rotated Sorted Array II<br>`05` LC 162 Find Peak Element | 2h00 |
| Wed 14 Oct | `05` LC 540 Single Element in a Sorted Array<br>`05` LC 240 Search a 2D Matrix II<br>`05` LC 1901 Find a Peak Element II | 2h00 |
| Thu 15 Oct | `05` LC 4 Median of Two Sorted Arrays<br>`06` Read: Notes section 1<br>`06` LC 167 Two Sum II - Input Array Is Sorted 📖 | 2h10 |
| Fri 16 Oct | `06` LC 11 Container With Most Water<br>`06` LC 15 3Sum 📖<br>`06` LC 18 4Sum | 1h45 |
| Sat 17 Oct | `06` LC 42 Trapping Rain Water<br>`06` Read: Notes section 2: fixed, longest, shortest, and count-subarrays windows<br>`06` LC 3 Longest Substring Without Repeating Cha… 📖 | 2h25 |

## Week 4 · 18 Oct – 24 Oct 2026

| Day | Plan | Time |
|---|---|---|
| Sun 18 Oct | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`06` LC 1423 Maximum Points You Can Obtain from Cards<br>`06` LC 1004 Max Consecutive Ones III | 2h20 |
| Mon 19 Oct | `06` LC 424 Longest Repeating Character Replacement<br>`06` LC 340 Longest Substring with At Most K Distin…<br>`06` LC 1358 Number of Substrings Containing All Thr… | 2h00 |
| Tue 20 Oct | `06` LC 930 Binary Subarrays With Sum<br>`06` LC 1248 Count Number of Nice Subarrays<br>`06` LC 76 Minimum Window Substring 📖 | 1h55 |
| Wed 21 Oct | `06` Read: Notes section 3<br>`06` LC 303 Range Sum Query - Immutable<br>`06` LC 238 Product of Array Except Self 📖<br>`06` LC 304 Range Sum Query 2D - Immutable | 2h10 |
| Thu 22 Oct | `06` Read: Notes section 4<br>`06` LC 1109 Corporate Flight Bookings 📖<br>`06` LC 1094 Car Pooling | 1h50 |
| Fri 23 Oct | `07` Read: Notes section 1<br>`07` LC 1 Two Sum 📖<br>`07` LC 169 Majority Element<br>`07` LC 229 Majority Element II | 2h00 |
| Sat 24 Oct | `07` LC 49 Group Anagrams 📖<br>`07` LC 128 Longest Consecutive Sequence<br>`07` Read: Notes section 2 + examples/hashmap_chaining.cpp<br>`07` LC 706 Design HashMap 📖 | 2h05 |

## Week 5 · 25 Oct – 31 Oct 2026

| Day | Plan | Time |
|---|---|---|
| Sun 25 Oct | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`07` Read: Notes section 3: pair keys, custom hash, coordinate encoding, splitmix64<br>`07` LC 36 Valid Sudoku | 2h25 |
| Mon 26 Oct | `07` Read: Notes section 4<br>`07` LC 560 Subarray Sum Equals K 📖<br>`07` LC 325 Maximum Size Subarray Sum Equals k | 1h50 |
| Tue 27 Oct | `07` LC 525 Contiguous Array<br>`07` LC 974 Subarray Sums Divisible by K<br>`08` Read: Notes section 1 | 2h05 |
| Wed 28 Oct | `08` LC 206 Reverse Linked List 📖<br>`08` LC 2 Add Two Numbers<br>`08` LC 61 Rotate List | 1h35 |
| Thu 29 Oct | `08` LC 25 Reverse Nodes in k-Group<br>`08` Read: Notes section 2<br>`08` LC 876 Middle of the Linked List | 2h05 |
| Fri 30 Oct | `08` LC 141 Linked List Cycle<br>`08` LC 142 Linked List Cycle II 📖<br>`08` LC 160 Intersection of Two Linked Lists<br>`08` LC 234 Palindrome Linked List<br>`08` LC 19 Remove Nth Node From End of List | 2h05 |
| Sat 31 Oct | Timed mock round (`/mock`) + post-mortem<br>`08` Read: Notes section 3<br>`08` LC 21 Merge Two Sorted Lists<br>`08` LC 148 Sort List | 2h30 |

## Week 6 · 1 Nov – 7 Nov 2026

| Day | Plan | Time |
|---|---|---|
| Sun 1 Nov | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`08` LC 23 Merge k Sorted Lists 📖<br>`08` Read: Notes section 4: `std::list` + `unordered_map<key, iterator>`, splice, skip lists<br>`08` LC 146 LRU Cache 📖 | 2h45 |
| Mon 2 Nov | `08` LC 138 Copy List with Random Pointer<br>`08` LC 460 LFU Cache | 1h40 |
| Tue 3 Nov | `09` Read: Notes section 1<br>`09` LC 20 Valid Parentheses 📖<br>`09` LC 150 Evaluate Reverse Polish Notation | 1h40 |
| Wed 4 Nov | `09` LC 227 Basic Calculator II<br>`09` LC 155 Min Stack<br>`09` LC 735 Asteroid Collision | 2h00 |
| Thu 5 Nov | `09` LC 277 Find the Celebrity<br>`09` Read: Notes section 2<br>`09` LC 739 Daily Temperatures 📖 | 2h05 |
| Fri 6 Nov | `09` LC 496 Next Greater Element I<br>`09` LC 503 Next Greater Element II<br>`09` LC 901 Online Stock Span | 1h40 |
| Sat 7 Nov | Timed mock round (`/mock`) + post-mortem<br>`09` LC 907 Sum of Subarray Minimums<br>`09` LC 2104 Sum of Subarray Ranges | 2h20 |

## Week 7 · 8 Nov – 14 Nov 2026

| Day | Plan | Time |
|---|---|---|
| Sun 8 Nov | Rest day | 0h00 |
| Mon 9 Nov | `09` LC 402 Remove K Digits<br>`09` LC 84 Largest Rectangle in Histogram 📖<br>`09` LC 85 Maximal Rectangle | 2h15 |
| Tue 10 Nov | `09` Read: Notes section 3: ring buffers, rate limiters, a blocking queue with mutex + condition_variable<br>`09` LC 622 Design Circular Queue 📖<br>`09` LC 933 Number of Recent Calls<br>`09` Read: Notes section 4 | 2h15 |
| Wed 11 Nov | `09` LC 239 Sliding Window Maximum 📖<br>`10` Read: Notes section 1<br>`10` LC 50 Pow(x, n) 📖 | 1h45 |
| Thu 12 Nov | `10` Read: Notes section 2: the subsets, combinations and permutations templates; handling duplicates<br>`10` LC 78 Subsets 📖<br>`10` LC 90 Subsets II | 2h05 |
| Fri 13 Nov | `10` LC 46 Permutations 📖<br>`10` LC 22 Generate Parentheses<br>`10` LC 17 Letter Combinations of a Phone Number | 1h45 |
| Sat 14 Nov | Timed mock round (`/mock`) + post-mortem<br>`10` LC 39 Combination Sum<br>`10` LC 40 Combination Sum II | 2h20 |

## Week 8 · 15 Nov – 21 Nov 2026

| Day | Plan | Time |
|---|---|---|
| Sun 15 Nov | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`10` LC 131 Palindrome Partitioning<br>`10` Read: Notes section 3 | 2h25 |
| Mon 16 Nov | `10` LC 79 Word Search<br>`10` LC 1219 Path with Maximum Gold<br>`10` LC 51 N-Queens 📖 | 1h55 |
| Tue 17 Nov | `10` LC 37 Sudoku Solver<br>`10` Read: Notes section 4<br>`10` LC 698 Partition to K Equal Sum Subsets 📖 | 1h55 |
| Wed 18 Nov | `11` Read: Notes section 1 (uses templates/binary_search.hpp)<br>`11` LC 875 Koko Eating Bananas 📖<br>`11` LC 1552 Magnetic Force Between Two Balls | 1h50 |
| Thu 19 Nov | `11` LC 410 Split Array Largest Sum<br>`11` LC 2387 Median of a Row Wise Sorted Matrix | 1h40 |
| Fri 20 Nov | `11` LC 378 Kth Smallest Element in a Sorted Matrix<br>`11` Read: Notes section 2: exchange argument, stays-ahead, and counterexamples where greedy fails | 1h40 |
| Sat 21 Nov | Timed mock round (`/mock`) + post-mortem<br>`11` LC 55 Jump Game<br>`11` LC 678 Valid Parenthesis String | 2h20 |

## Week 9 · 22 Nov – 28 Nov 2026

| Day | Plan | Time |
|---|---|---|
| Sun 22 Nov | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`11` LC 135 Candy<br>`11` Read: Notes section 3 | 2h45 |
| Mon 23 Nov | `11` LC 56 Merge Intervals 📖<br>`11` LC 57 Insert Interval<br>`11` LC 435 Non-overlapping Intervals<br>`11` LC 2406 Divide Intervals Into Minimum Number of… 📖 | 2h10 |
| Tue 24 Nov | `11` Read: Notes section 4<br>`11` LC 1011 Capacity To Ship Packages Within D Days<br>`11` LC 1353 Maximum Number of Events That Can Be At… | 1h50 |
| Wed 25 Nov | `12` Read: Notes section 1: recursive, iterative (explicit stack), Morris, level order<br>`12` LC 94 Binary Tree Inorder Traversal 📖<br>`12` LC 144 Binary Tree Preorder Traversal<br>`12` LC 102 Binary Tree Level Order Traversal | 2h15 |
| Thu 26 Nov | `12` LC 199 Binary Tree Right Side View<br>`12` LC 662 Maximum Width of Binary Tree<br>`12` LC 105 Construct Binary Tree from Preorder and… 📖 | 1h45 |
| Fri 27 Nov | `12` LC 545 Boundary of Binary Tree<br>`12` LC 987 Vertical Order Traversal of a Binary Tr… | 1h40 |
| Sat 28 Nov | Timed mock round (`/mock`) + post-mortem<br>`12` LC 297 Serialize and Deserialize Binary Tree | 2h00 |

## Week 10 · 29 Nov – 5 Dec 2026

| Day | Plan | Time |
|---|---|---|
| Sun 29 Nov | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`12` Read: Notes section 2: return a value vs update a global, post-order combining<br>`12` LC 104 Maximum Depth of Binary Tree<br>`12` LC 543 Diameter of Binary Tree 📖 | 2h35 |
| Mon 30 Nov | `12` LC 257 Binary Tree Paths<br>`12` LC 236 Lowest Common Ancestor of a Binary Tree 📖<br>`12` LC 863 All Nodes Distance K in Binary Tree<br>`12` LC 2385 Amount of Time for Binary Tree to Be In… | 2h05 |
| Tue 1 Dec | `12` LC 124 Binary Tree Maximum Path Sum<br>`12` Read: Notes section 3 | 1h45 |
| Wed 2 Dec | `12` LC 701 Insert into a Binary Search Tree<br>`12` LC 450 Delete Node in a BST 📖<br>`12` LC 98 Validate Binary Search Tree | 1h45 |
| Thu 3 Dec | `12` LC 230 Kth Smallest Element in a BST<br>`12` LC 235 Lowest Common Ancestor of a Binary Sear…<br>`12` LC 285 Inorder Successor in BST | 2h00 |
| Fri 4 Dec | `12` LC 1008 Construct Binary Search Tree from Preor…<br>`12` LC 653 Two Sum IV - Input is a BST<br>`12` LC 99 Recover Binary Search Tree | 1h40 |
| Sat 5 Dec | Timed mock round (`/mock`) + post-mortem<br>`12` LC 333 Largest BST Subtree<br>`12` Read: projects/12-file-index/README.md: spec + milestones | 2h10 |

## Week 11 · 6 Dec – 12 Dec 2026

| Day | Plan | Time |
|---|---|---|
| Sun 6 Dec | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`12` Project: Milestone 1: write the tests for the BST index first (checklist in the README), then make them pass (1/2) | 2h00 |
| Mon 7 Dec | `12` Project: Milestone 1: write the tests for the BST index first (checklist in the README), then make them pass (2/2) | 1h00 |
| Tue 8 Dec | `12` Project: Milestone 2: prefix autocomplete + rank / k-th / range queries using subtree sizes<br>`12` Project: Milestone 3: the CLI over a real directory, then a code review with Claude (`/review`) | 2h15 |
| Wed 9 Dec | `13` Read: Notes section 1 + templates/heap.hpp (a binary heap you can read end to end)<br>`13` LC 703 Kth Largest Element in a Stream 📖<br>`13` LC 373 Find K Pairs with Smallest Sums | 1h40 |
| Thu 10 Dec | `13` Read: Notes section 2<br>`13` LC 215 Kth Largest Element in an Array<br>`13` LC 1834 Single-Threaded CPU | 2h05 |
| Fri 11 Dec | `13` LC 295 Find Median from Data Stream 📖<br>`13` Read: Notes section 3 + templates/trie.hpp<br>`13` LC 208 Implement Trie (Prefix Tree) 📖 | 1h45 |
| Sat 12 Dec | Timed mock round (`/mock`) + post-mortem<br>`13` LC 211 Design Add and Search Words Data Struct…<br>`13` LC 1804 Implement Trie II (Prefix Tree) | 2h20 |

## Week 12 · 13 Dec – 19 Dec 2026

| Day | Plan | Time |
|---|---|---|
| Sun 13 Dec | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`13` LC 1858 Longest Word With All Prefixes<br>`13` Read: Notes section 4<br>`13` LC 1268 Search Suggestions System 📖 | 2h35 |
| Mon 14 Dec | `13` LC 212 Word Search II<br>`13` LC 1698 Number of Distinct Substrings in a Stri… | 1h40 |
| Tue 15 Dec | `14` Read: Notes section 1<br>`14` LC 1971 Find if Path Exists in Graph<br>`14` LC 547 Number of Provinces | 1h45 |
| Wed 16 Dec | `14` Read: Notes section 2: grids, multi-source BFS, colour-based cycle detection, bipartite<br>`14` LC 733 Flood Fill<br>`14` LC 200 Number of Islands 📖<br>`14` LC 994 Rotting Oranges 📖 | 2h10 |
| Thu 17 Dec | `14` LC 542 01 Matrix<br>`14` LC 130 Surrounded Regions<br>`14` LC 694 Number of Distinct Islands | 2h00 |
| Fri 18 Dec | `14` LC 785 Is Graph Bipartite?<br>`14` LC 127 Word Ladder | 1h40 |
| Sat 19 Dec | Timed mock round (`/mock`) + post-mortem<br>`14` LC 126 Word Ladder II<br>`14` Read: Notes section 3 | 2h45 |

## Week 13 · 20 Dec – 26 Dec 2026

| Day | Plan | Time |
|---|---|---|
| Sun 20 Dec | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`14` LC 207 Course Schedule 📖<br>`14` LC 210 Course Schedule II | 2h05 |
| Mon 21 Dec | `14` LC 269 Alien Dictionary<br>`14` Read: Notes section 4 + templates/dsu.hpp<br>`14` LC 684 Redundant Connection 📖 | 2h10 |
| Tue 22 Dec | `14` LC 1319 Number of Operations to Make Network Co…<br>`14` LC 827 Making A Large Island | 1h40 |
| Wed 23 Dec | `14` LC 305 Number of Islands II<br>`15` Read: Notes section 1 + templates/graph.hpp (dijkstra, zero_one_bfs) | 2h00 |
| Thu 24 Dec | `15` LC 743 Network Delay Time 📖<br>`15` LC 1631 Path With Minimum Effort<br>`15` LC 2290 Minimum Obstacle Removal to Reach Corner 📖 | 1h40 |
| Fri 25 Dec | Rest day | 0h00 |
| Sat 26 Dec | Timed mock round (`/mock`) + post-mortem<br>`15` Read: Notes section 2<br>`15` LC 787 Cheapest Flights Within K Stops 📖 | 2h10 |

## Week 14 · 27 Dec – 2 Jan 2027

| Day | Plan | Time |
|---|---|---|
| Sun 27 Dec | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`15` LC 1334 Find the City With the Smallest Number …<br>`15` Read: Notes section 3 | 2h25 |
| Mon 28 Dec | `15` LC 1584 Min Cost to Connect All Points 📖<br>`15` LC 1135 Connecting Cities With Minimum Cost<br>`15` Read: Notes section 4: Tarjan low-link, Kosaraju, Edmonds–Karp / Dinic, min cut | 2h05 |
| Tue 29 Dec | `15` LC 1192 Critical Connections in a Network 📖<br>`15` LC 1568 Minimum Number of Days to Disconnect Is… | 1h35 |
| Wed 30 Dec | `15` LC 1520 Maximum Number of Non-Overlapping Subst…<br>`16` Read: Notes section 1: the recipe (state, transition, base case, order, answer) | 2h00 |
| Thu 31 Dec | `16` LC 746 Min Cost Climbing Stairs<br>`16` LC 198 House Robber 📖<br>`16` LC 139 Word Break<br>`16` Read: Notes section 2 | 2h10 |
| Fri 1 Jan | Rest day | 0h00 |
| Sat 2 Jan | Timed mock round (`/mock`) + post-mortem<br>`16` LC 62 Unique Paths 📖<br>`16` LC 63 Unique Paths II | 2h05 |

## Week 15 · 3 Jan – 9 Jan 2027

| Day | Plan | Time |
|---|---|---|
| Sun 3 Jan | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`16` Read: Notes section 3: 0/1 vs unbounded loops, LIS in O(n log n), LCS / edit distance tables | 2h30 |
| Mon 4 Jan | `16` LC 416 Partition Equal Subset Sum 📖<br>`16` LC 494 Target Sum<br>`16` LC 1049 Last Stone Weight II | 1h45 |
| Tue 5 Jan | `16` LC 518 Coin Change II<br>`16` LC 322 Coin Change 📖<br>`16` LC 300 Longest Increasing Subsequence 📖<br>`16` LC 673 Number of Longest Increasing Subsequence | 2h10 |
| Wed 6 Jan | `16` LC 1143 Longest Common Subsequence 📖<br>`16` LC 718 Maximum Length of Repeated Subarray<br>`16` LC 516 Longest Palindromic Subsequence<br>`16` LC 72 Edit Distance 📖 | 2h10 |
| Thu 7 Jan | `16` LC 44 Wildcard Matching<br>`16` LC 714 Best Time to Buy and Sell Stock with Tr… | 1h40 |
| Fri 8 Jan | `16` LC 188 Best Time to Buy and Sell Stock IV | 1h00 |
| Sat 9 Jan | Timed mock round (`/mock`) + post-mortem<br>`16` Read: Notes section 4 | 2h30 |

## Week 16 · 10 Jan – 16 Jan 2027

| Day | Plan | Time |
|---|---|---|
| Sun 10 Jan | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`16` LC 1039 Minimum Score Triangulation of Polygon<br>`16` LC 312 Burst Balloons 📖 | 2h15 |
| Mon 11 Jan | `16` LC 132 Palindrome Partitioning II<br>`16` LC 337 House Robber III 📖<br>`16` LC 1986 Minimum Number of Work Sessions to Fini… 📖 | 1h50 |
| Tue 12 Jan | `16` LC 2376 Count Special Integers 📖<br>`17` Read: Notes section 1 + templates/sparse_table.hpp<br>`17` Read: Notes section 2 + templates/fenwick.hpp | 2h05 |
| Wed 13 Jan | `17` LC 307 Range Sum Query - Mutable 📖<br>`17` LC 315 Count of Smaller Numbers After Self 📖<br>`17` Read: Notes section 3 + templates/segment_tree.hpp | 2h00 |
| Thu 14 Jan | `17` LC 732 My Calendar III<br>`17` LC 699 Falling Squares | 2h00 |
| Fri 15 Jan | `17` Read: projects/17-range-query-engine/README.md: spec + milestones<br>`17` Project: Milestone 1: brute-force baseline + differential tester, then Fenwick + sparse table behind the engine interface | 2h00 |
| Sat 16 Jan | Timed mock round (`/mock`) + post-mortem<br>`17` Project: Milestone 2: lazy segment tree (add + assign, sum + min), stress-tested against brute force | 2h30 |

## Week 17 · 17 Jan – 23 Jan 2027

| Day | Plan | Time |
|---|---|---|
| Sun 17 Jan | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`17` Project: Milestone 3: benchmark n = 10³…10⁶; write up where each structure wins in the README<br>`18` Read: Notes section 1 + templates/xor_trie.hpp<br>`18` LC 421 Maximum XOR of Two Numbers in an Array 📖 | 2h40 |
| Mon 18 Jan | `18` LC 1707 Maximum XOR With an Element From Array<br>`18` Read: Notes section 2<br>`18` LC 136 Single Number | 2h05 |
| Tue 19 Jan | `18` LC 191 Number of 1 Bits<br>`18` LC 645 Set Mismatch<br>`18` LC 137 Single Number II 📖<br>`18` LC 260 Single Number III | 1h45 |
| Wed 20 Jan | `18` Read: Notes section 3 + templates/number_theory.hpp<br>`18` LC 204 Count Primes 📖<br>`18` LC 1922 Count Good Numbers | 1h50 |
| Thu 21 Jan | `18` Read: Notes section 4 + templates/strings.hpp<br>`18` LC 28 Find the Index of the First Occurrence … 📖 | 1h30 |
| Fri 22 Jan | `18` LC 1392 Longest Happy Prefix<br>`18` LC 686 Repeated String Match | 1h40 |
| Sat 23 Jan | Timed mock round (`/mock`) + post-mortem<br>`18` LC 647 Palindromic Substrings<br>`18` LC 1044 Longest Duplicate Substring 📖 | 2h15 |

## Week 18 · 24 Jan – 28 Jan 2027

| Day | Plan | Time |
|---|---|---|
| Sun 24 Jan | Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill<br>`18` LC 214 Shortest Palindrome<br>`19` Read: Notes section 1: finding company sets, what each kind of company tests, the Indian OA landscape | 2h45 |
| Mon 25 Jan | `19` Research: Target list: 8–10 companies; for each, the loop format and the topics that recur (Glassdoor / LeetCode Discuss)<br>`19` Practice: Company set 1: 4 recent problems for your #1 target, timed, stdin/stdout if their OA uses it | 2h15 |
| Tue 26 Jan | `19` Practice: Company set 2: 4 recent problems for your #2 and #3 targets, timed<br>`19` Read: Notes section 2: the 45-minute script and what interviewers score | 2h00 |
| Wed 27 Jan | `19` Mock: Full mock A: `/mock` with an unseen Medium + the follow-up turn<br>`19` Mock: Full mock B: `/mock hard` with an unseen Hard | 2h00 |
| Thu 28 Jan | `19` Read: Notes section 3 + mocks/_postmortem-template.md<br>`19` Review: Re-read every post-mortem in mocks/; turn the 3 most frequent misses into #redo items and flashcards<br>`19` Read: Notes section 4: the follow-up bank, by pattern<br>`19` Drill: Follow-up drill: 6 solved problems, answer their follow-ups out loud from the Notes section 4 bank | 2h05 |
