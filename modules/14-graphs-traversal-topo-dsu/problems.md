# 14 · Graphs + BFS + DFS + Topological Sort + DSU
> Model the problem as nodes and edges and half the work is already done.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Representations — adjacency list vs matrix, weighted and directed variants

- [ ] **Read** · Notes section 1 · 45m
- [ ] [1971. Find if Path Exists in Graph](https://leetcode.com/problems/find-if-path-exists-in-graph/) · Easy · `build an adjacency list` — added · edge list → `vector<vector<int>>`, then BFS / DFS
- [ ] [547. Number of Provinces](https://leetcode.com/problems/number-of-provinces/) · Medium · `matrix components` — added · adjacency matrix → DFS per unvisited node

### 2. BFS & DFS — shortest hops, components, cycle detection, bipartite check

- [ ] **Read** · Notes section 2: grids, multi-source BFS, colour-based cycle detection, bipartite · 60m
- [ ] [733. Flood Fill](https://leetcode.com/problems/flood-fill/) · Easy · `grid DFS / BFS` — the flood-fill skeleton that every "islands" problem reuses
- [ ] [200. Number of Islands](https://leetcode.com/problems/number-of-islands/) · Medium · 📖 · `grid components` — DFS/BFS flood fill counting; direction arrays · [video](https://www.youtube.com/watch?v=muncqlKJrH0&list=PLgUwDviBIf0oE3gA41TKO2H5bHpPd7fzn&index=8)
- [ ] [994. Rotting Oranges](https://leetcode.com/problems/rotting-oranges/) · Medium · 📖 · `multi-source BFS` — sheet: Rotten Oranges · all rotten oranges start in the queue at time 0 · [video](https://www.youtube.com/watch?v=yf3oUhkvqA0)
- [ ] [542. 01 Matrix](https://leetcode.com/problems/01-matrix/) · Medium · `multi-source distances` — sheet: Distance of nearest cell having one · BFS from every 0 at once · [video](https://youtu.be/edXdVwkYHF8)
- [ ] [130. Surrounded Regions](https://leetcode.com/problems/surrounded-regions/) · Medium · `border DFS` — mark what is connected to the border, flip the rest · [video](https://youtu.be/BtdgAys4yMk)
- [ ] [694. Number of Distinct Islands](https://leetcode.com/problems/number-of-distinct-islands/) · Medium · 🔒 · `DFS + shape signature` — record each island's DFS path (or relative coordinates) and count distinct signatures
- [ ] [785. Is Graph Bipartite?](https://leetcode.com/problems/is-graph-bipartite/) · Medium · `two-colouring` — sheet: Bipartite graph · BFS/DFS colouring over every component · [video](https://youtu.be/KG5YFfR0j8A)
- [ ] [127. Word Ladder](https://leetcode.com/problems/word-ladder/) · Hard · `implicit graph BFS` — neighbours by changing one letter; BFS gives the shortest chain · [video](https://youtu.be/tRPda0rcf8E)
- [ ] [126. Word Ladder II](https://leetcode.com/problems/word-ladder-ii/) · Hard · `BFS layers + path backtracking` — BFS builds the shortest-path DAG level by level; DFS from the end lists every path · [video](https://youtu.be/AD4SFl7tu7I?si=EpcJQTWm2YeURvEG)

### 3. Topological sort — Kahn's algorithm, DFS ordering, dependency resolution

- [ ] **Read** · Notes section 3 · 45m
- [ ] [207. Course Schedule](https://leetcode.com/problems/course-schedule/) · Medium · 📖 · `cycle check via topo` — sheet: Detect a cycle in a directed graph · Kahn's in-degree queue; a cycle leaves nodes unprocessed · [video](https://youtu.be/zQ3zgFypzX4)
- [ ] [210. Course Schedule II](https://leetcode.com/problems/course-schedule-ii/) · Medium · `topo order` — output Kahn's pop order · [video](https://youtu.be/WAOfKpxYHR8)
- [ ] [269. Alien Dictionary](https://leetcode.com/problems/alien-dictionary/) · Hard · 🔒 · `topo from comparisons` — build edges from adjacent words · [video](https://youtu.be/U3N_je7tWAs)

### 4. Disjoint set union — union by rank, path compression, connectivity queries

- [ ] **Read** · Notes section 4 + templates/dsu.hpp · 45m
- [ ] [684. Redundant Connection](https://leetcode.com/problems/redundant-connection/) · Medium · 📖 · `DSU cycle detection` — added for the sheet's “Disjoint Set” (no LeetCode link) · the first edge whose endpoints are already joined
- [ ] [1319. Number of Operations to Make Network Connected](https://leetcode.com/problems/number-of-operations-to-make-network-connected/) · Medium · `DSU components` — components − 1 moves, if there are enough cables · [video](https://youtu.be/FYrl7iz9_ZU)
- [ ] [827. Making A Large Island](https://leetcode.com/problems/making-a-large-island/) · Hard · `label components + try each zero` — size every island once, then for each 0 add up the distinct neighbouring islands · [video](https://youtu.be/lgiz0Oup6gM)
- [ ] [305. Number of Islands II](https://leetcode.com/problems/number-of-islands-ii/) · Hard · 🔒 · `online DSU` — land appears one cell at a time: union with land neighbours and report the component count each time · [video](https://youtu.be/Rn6B-Q4SNyA)
