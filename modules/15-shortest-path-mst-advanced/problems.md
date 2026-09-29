# 15 · Shortest Path + MST + Advanced Graphs
> Weighted graphs, where the naive answer is usually the wrong one.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Dijkstra & 0-1 BFS — non-negative weights, priority-queue relaxation

- [ ] **Read** · Notes section 1 + templates/graph.hpp (dijkstra, zero_one_bfs) · 60m
- [ ] [743. Network Delay Time](https://leetcode.com/problems/network-delay-time/) · Medium · 📖 · `Dijkstra` — added for the sheet's “Dijkstra's algorithm” (no LeetCode link) · min-heap of (dist, node); skip stale entries
- [ ] [1631. Path With Minimum Effort](https://leetcode.com/problems/path-with-minimum-effort/) · Medium · `minimax Dijkstra` — the path cost is the max edge, not the sum · [video](https://youtu.be/0ytpZyiZFhA)
- [ ] [2290. Minimum Obstacle Removal to Reach Corner](https://leetcode.com/problems/minimum-obstacle-removal-to-reach-corner/) · Hard · 📖 · `0-1 BFS` — added · a deque: 0-weight edges to the front, 1-weight to the back

### 2. Bellman-Ford & Floyd-Warshall — negative edges, all-pairs distances

- [ ] **Read** · Notes section 2 · 45m
- [ ] [787. Cheapest Flights Within K Stops](https://leetcode.com/problems/cheapest-flights-within-k-stops/) · Medium · 📖 · `Bellman-Ford with k rounds` — sheet: Cheapest flight within K stops · relax from a copy of last round's array · [video](https://youtu.be/9XybHVqTHcQ)
- [ ] [1334. Find the City With the Smallest Number of Neighbors at a Threshold Distance](https://leetcode.com/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/) · Medium · `Floyd-Warshall` — added for the sheet's “Floyd warshall algorithm” (no LeetCode link) · all pairs in O(n³) when n ≤ 100

### 3. Minimum spanning trees — Kruskal, Prim, and where MSTs actually show up

- [ ] **Read** · Notes section 3 · 45m
- [ ] [1584. Min Cost to Connect All Points](https://leetcode.com/problems/min-cost-to-connect-all-points/) · Medium · 📖 · `MST on a dense graph` — added · O(n²) Prim without a heap vs Kruskal on n² edges
- [ ] [1135. Connecting Cities With Minimum Cost](https://leetcode.com/problems/connecting-cities-with-minimum-cost/) · Medium · 🔒 · `Kruskal / Prim` — sheet: Find the MST weight · classic MST on an edge list; return −1 if the graph stays disconnected · [video](https://youtu.be/mJcZjjKzeqk)

### 4. Advanced — strongly connected components, bridges, articulation points, max-flow basics

- [ ] **Read** · Notes section 4: Tarjan low-link, Kosaraju, Edmonds–Karp / Dinic, min cut · 60m
- [ ] [1192. Critical Connections in a Network](https://leetcode.com/problems/critical-connections-in-a-network/) · Hard · 📖 · `bridges (Tarjan)` — sheet: Bridges in graph · low[v] > tin[u] ⇒ edge (u, v) is a bridge · [video](https://youtu.be/qrAub5z8FeA)
- [ ] [1568. Minimum Number of Days to Disconnect Island](https://leetcode.com/problems/minimum-number-of-days-to-disconnect-island/) · Hard · `articulation points` — added for the sheet's “Articulation point in graph” (no LeetCode link) · the answer is always 0, 1 or 2
- [ ] [1520. Maximum Number of Non-Overlapping Substrings](https://leetcode.com/problems/maximum-number-of-non-overlapping-substrings/) · Hard · `SCC / interval closure` — sheet: Kosaraju's algorithm · a letter's span must grow to cover every letter inside it; take the smallest closed spans greedily by end · [video](https://www.youtube.com/watch?v=V8qIqJxCioo&list=PLgUwDviBIf0rGEWe64KWas0Nryn7SCRWw&index=27)
