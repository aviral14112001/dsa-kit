# 12 · Trees + Binary Trees + BST + Project
> Hierarchy, recursion and ordering — then build something real with them.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Traversals — pre / in / post order, level order, iterative and Morris

- [ ] **Read** · Notes section 1: recursive, iterative (explicit stack), Morris, level order · 60m
- [ ] [94. Binary Tree Inorder Traversal](https://leetcode.com/problems/binary-tree-inorder-traversal/) · Easy · 📖 · `inorder three ways` — sheet: Morris Inorder Traversal · recursive, iterative stack, and Morris (O(1) space) · [video](https://youtu.be/80Zug6D1_r4)
- [ ] [144. Binary Tree Preorder Traversal](https://leetcode.com/problems/binary-tree-preorder-traversal/) · Easy · `iterative preorder` — added · stack: push right, then left
- [ ] [102. Binary Tree Level Order Traversal](https://leetcode.com/problems/binary-tree-level-order-traversal/) · Medium · `level-order BFS` — snapshot the queue size for each level · [video](https://youtu.be/EoAsWbO7sqg)
- [ ] [199. Binary Tree Right Side View](https://leetcode.com/problems/binary-tree-right-side-view/) · Medium · `level-order view` — sheet: Right/Left View of BT · the last node of each level · [video](https://youtu.be/KV4mRzTjlAk)
- [ ] [662. Maximum Width of Binary Tree](https://leetcode.com/problems/maximum-width-of-binary-tree/) · Medium · `indexed BFS` — sheet: Maximum Width of BT · position indices (normalize per level to avoid overflow) · [video](https://youtu.be/ZbybYvcVLks)
- [ ] [105. Construct Binary Tree from Preorder and Inorder Traversal](https://leetcode.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/) · Medium · 📖 · `build from traversals` — sheet: Construct a BT from Preorder and Inorder · the preorder root splits the inorder range (hash map for O(1) lookups) · [video](https://youtu.be/aZNaLrVebKQ)
- [ ] [545. Boundary of Binary Tree](https://leetcode.com/problems/boundary-of-binary-tree/) · Medium · 🔒 · `three traversals` — sheet: Boundary Traversal · left boundary top-down, leaves left to right, right boundary bottom-up, with no node counted twice · [video](https://youtu.be/0ca1nvR0be4)
- [ ] [987. Vertical Order Traversal of a Binary Tree](https://leetcode.com/problems/vertical-order-traversal-of-a-binary-tree/) · Hard · `sorted vertical order` — collect (col, row, val), then sort · [video](https://youtu.be/q_a6lpbKJdw)
- [ ] [297. Serialize and Deserialize Binary Tree](https://leetcode.com/problems/serialize-and-deserialize-binary-tree/) · Hard · `serialize a tree` — sheet: Serialize and De-serialize BT · preorder with null markers, parsed back recursively · [video](https://youtu.be/-YbXySKJsX8)

### 2. Tree recursion — height, diameter, path sums, lowest common ancestor

- [ ] **Read** · Notes section 2: return a value vs update a global, post-order combining · 60m
- [ ] [104. Maximum Depth of Binary Tree](https://leetcode.com/problems/maximum-depth-of-binary-tree/) · Easy · `height` — added · 1 + max(left, right)
- [ ] [543. Diameter of Binary Tree](https://leetcode.com/problems/diameter-of-binary-tree/) · Easy · 📖 · `return height, update a global` — the diameter passes through some node: left + right · [video](https://youtu.be/Rezetez59Nk)
- [ ] [257. Binary Tree Paths](https://leetcode.com/problems/binary-tree-paths/) · Easy · `path strings` — added for the sheet's “Print root to leaf path in BT” (no LeetCode link) · backtracking with a string
- [ ] [236. Lowest Common Ancestor of a Binary Tree](https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-tree/) · Medium · 📖 · `LCA by return values` — sheet: LCA in BT · whichever side returns non-null; both sides → this node · [video](https://youtu.be/_-QHfMDde90)
- [ ] [863. All Nodes Distance K in Binary Tree](https://leetcode.com/problems/all-nodes-distance-k-in-binary-tree/) · Medium · `parent map + BFS` — sheet: Print all nodes at a distance of K in BT · treat the tree as a graph · [video](https://youtu.be/i9ORlEy6EsI)
- [ ] [2385. Amount of Time for Binary Tree to Be Infected](https://leetcode.com/problems/amount-of-time-for-binary-tree-to-be-infected/) · Medium · `tree to graph + BFS` — added for the sheet's “Minimum time taken to burn the BT from a given Node” (no LeetCode link) · infection spreads in every direction
- [ ] [124. Binary Tree Maximum Path Sum](https://leetcode.com/problems/binary-tree-maximum-path-sum/) · Hard · `gain vs answer` — return the best one-sided gain, update the answer with both sides · [video](https://youtu.be/WszrfSwMz58)

### 3. BSTs — insert, delete, validate, k-th smallest, balancing intuition

- [ ] **Read** · Notes section 3 · 45m
- [ ] [701. Insert into a Binary Search Tree](https://leetcode.com/problems/insert-into-a-binary-search-tree/) · Medium · `BST insert` — sheet: Insert a given node in BST · walk to a null slot · [video](https://youtu.be/FiFiNvM29ps)
- [ ] [450. Delete Node in a BST](https://leetcode.com/problems/delete-node-in-a-bst/) · Medium · 📖 · `BST delete` — sheet: Delete a node in BST · the three cases; the two-child case uses the in-order successor · [video](https://youtu.be/kouxiP_H5WE)
- [ ] [98. Validate Binary Search Tree](https://leetcode.com/problems/validate-binary-search-tree/) · Medium · `bounds propagation` — sheet: Check if a tree is a BST or not · pass (low, high); watch INT_MIN / INT_MAX values · [video](https://youtu.be/f-sj7I5oXEI)
- [ ] [230. Kth Smallest Element in a BST](https://leetcode.com/problems/kth-smallest-element-in-a-bst/) · Medium · `in-order k-th` — sheet: Kth Smallest and Largest element in BST · stop early; follow-up: subtree sizes for repeated queries · [video](https://youtu.be/9TJYWh0adfk)
- [ ] [235. Lowest Common Ancestor of a Binary Search Tree](https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-search-tree/) · Medium · `BST LCA` — sheet: LCA in BST · split point of p and q · [video](https://youtu.be/cX_kPV_foZc)
- [ ] [285. Inorder Successor in BST](https://leetcode.com/problems/inorder-successor-in-bst/) · Medium · 🔒 · `BST walk with a candidate` — sheet: Inorder successor and predecessor in BST · going left, remember the node as the best successor so far: O(h) · [video](https://youtu.be/SXKAD2svfmI)
- [ ] [1008. Construct Binary Search Tree from Preorder Traversal](https://leetcode.com/problems/construct-binary-search-tree-from-preorder-traversal/) · Medium · `BST from preorder` — sheet: Construct a BST from a preorder traversal · bounds recursion in O(n) · [video](https://youtu.be/UmJT3j26t1I)
- [ ] [653. Two Sum IV - Input is a BST](https://leetcode.com/problems/two-sum-iv-input-is-a-bst/) · Easy · `two sum on a BST` — sheet: Two sum in BST · set, or two BST iterators · [video](https://youtu.be/ssL3sHwPeb4)
- [ ] [99. Recover Binary Search Tree](https://leetcode.com/problems/recover-binary-search-tree/) · Medium · `recover a BST` — sheet: Correct BST with two nodes swapped · find the two inverted pairs during the in-order walk · [video](https://youtu.be/ZWGW7FminDM)
- [ ] [333. Largest BST Subtree](https://leetcode.com/problems/largest-bst-subtree/) · Medium · 🔒 · `post-order returning a summary` — sheet: Largest BST in Binary Tree · each subtree reports (is BST, size, min, max) so its parent decides in O(1) · [video](https://youtu.be/X0oXMdtUDwo)

### 4. Project — a tree-backed autocomplete / file-index, with tests you write first

- [ ] **Read** · projects/12-file-index/README.md: spec + milestones · 30m
- [ ] **Project** · Milestone 1: write the tests for the BST index first (checklist in the README), then make them pass · 120m
- [ ] **Project** · Milestone 2: prefix autocomplete + rank / k-th / range queries using subtree sizes · 90m
- [ ] **Project** · Milestone 3: the CLI over a real directory, then a code review with Claude (`/review`) · 45m
