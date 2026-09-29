# 12 · Trees + Binary Trees + BST + Project
> Hierarchy, recursion and ordering — then build something real with them.

**Time:** ~23 h of Core work ([problems](problems.md)) · **Prereqs:** modules 09 (stacks, queues), 10 (recursion) · **You're done when:** (1) from a blank file you can write inorder three ways (recursive, stack, Morris) and level order, and trace Morris on a 7-node tree; (2) before coding any tree recursion you can say in one sentence what the function returns for a subtree, and whether the answer travels up by return value or into a separate variable; (3) you can delete from a BST (all three cases), explain the INT_MIN/INT_MAX validation trap, and your project milestone 1 tests pass.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Traversals | Where you record the node (before / between / after the children) fixes the order; a stack replaces recursion; BFS by levels needs a size snapshot | [traversals.cpp](examples/traversals.cpp), [0094](examples/0094-binary-tree-inorder-traversal.cpp) | 94, 144, 102, 199, 662, 105, 545, 987, 297 |
| 2 | Tree recursion | Decide what f(subtree) returns; combine the children post-order; answers the parent doesn't need go into a separate variable | [tree_recursion.cpp](examples/tree_recursion.cpp) | 104, 543, 257, 236, 863, 2385, 124 |
| 3 | BSTs | left < node < right for whole subtrees: O(h) walks, inorder = sorted, subtree sizes give k-th in O(h) | [bst_basics.cpp](examples/bst_basics.cpp), [0450](examples/0450-delete-node-in-a-bst.cpp) | 701, 450, 98, 230, 235, 285, 1008, 653, 99, 333 |
| 4 | Project | A BST-backed file index built test-first; subtree sizes power rank and k-th | [README](../../projects/12-file-index/README.md) | milestones 1–3 |

## 1. Traversals — pre / in / post order, level order, iterative and Morris

### Concept

A binary tree is either empty or a node holding a value and two subtrees, left and right. That recursive definition is why most tree code is recursive: solve both subtrees, combine. Vocabulary: the **root** has no parent; a **leaf** has no children (both null); the **depth** of a node is its number of edges from the root (the root is at depth 0, which is also its BFS level).

LeetCode's `TreeNode` (in `include/leetcode.hpp`) has `val`, `left` and `right`, and an empty subtree is `nullptr`. A `TreeNode*` behaves like a C# class reference: copying the pointer shares the node, while copying `*node` by value copies only its three fields, not the subtree. In tests, build trees from LeetCode's level-order strings and print them back:

```
make_tree("[3,9,20,null,null,15,7]")         3           depth 0
tree_to_string(root) gives the same string  / \
                                           9   20        depth 1
                                              /  \
                                             15   7      depth 2
```

| Order | Sequence | On the tree above | Use it when |
|---|---|---|---|
| Preorder | node, left, right | 3 9 20 15 7 | copying or serializing (the root comes first, so a reader can rebuild top-down); root-to-leaf paths |
| Inorder | left, node, right | 9 3 15 20 7 | BSTs: inorder is sorted order (Section 3) |
| Postorder | left, right, node | 9 15 7 20 3 | anything that needs the children's answers first: height, size, freeing a tree |
| Level order | by depth, left to right | 3 9 20 15 7 | per-level questions; shortest distance from the root |

Each order visits every node once: O(n) time. The recursive depth-first orders use O(h) stack, where h is the height: about log2 n when balanced, n for a chain. Level order uses O(w) queue space, w = the widest level (up to about n/2).

**Recursive.** The three depth-first orders differ in one line:

<!-- snippet: modules/12-trees-bst/examples/traversals.cpp#recursive -->
```cpp
// The three orders differ only in WHERE a node is recorded relative to its two recursive calls.
void dfs(TreeNode* node, vector<int>& pre, vector<int>& in, vector<int>& post) {
    if (!node) return;                  // the empty tree: the base case for every tree recursion
    pre.push_back(node->val);           // preorder: before the children
    dfs(node->left, pre, in, post);
    in.push_back(node->val);            // inorder: between them
    dfs(node->right, pre, in, post);
    post.push_back(node->val);          // postorder: after both
}
```
<!-- /snippet -->

**Iterative, with an explicit stack.** You need this for three reasons: recursion depth (a 10^5-node chain means 10^5 nested calls, which can overflow the stack), the classic follow-up "now without recursion", and traversals you must pause (a BST iterator keeps its stack between `next()` calls). The call stack was storing "where do I resume?"; store it yourself.

- **Inorder:** push the whole left spine; pop a node, visit it, repeat from its right child. That's 94's worked example below.
- **Preorder (144):** pop, visit, push right then left, so the left child comes off first. Write it yourself.
- **Postorder:** harder, because a node is visited after *both* subtrees. The template below simulates the recursion directly: each node is pushed twice, once to be expanded and once to be visited. (Two other classic routes: produce node-right-left with the preorder loop and reverse the result, or keep one stack plus a pointer to the last node visited, which tells you whether you're coming back up from the right child.)

<!-- snippet: modules/12-trees-bst/examples/traversals.cpp#postorder_iterative -->
```cpp
// Postorder without recursion, by simulating the call stack. Each node is pushed twice: first
// to be expanded (push its parts), later to be visited. A stack is LIFO, so the parts go on in
// the REVERSE of the order they must come off: the node itself (visited last), right, left.
vector<int> postorder_iterative(TreeNode* root) {
    vector<int> out;
    stack<pair<TreeNode*, bool>> st;    // (node, ready to visit?)
    if (root) st.push({root, false});
    while (!st.empty()) {
        auto [node, ready] = st.top();
        st.pop();
        if (ready) {                    // both subtrees are finished
            out.push_back(node->val);
            continue;
        }
        st.push({node, true});                              // comes off third
        if (node->right) st.push({node->right, false});     // comes off second
        if (node->left) st.push({node->left, false});       // comes off first
    }
    return out;
}
```
<!-- /snippet -->

Why push in reverse: a stack is LIFO, so to take *left, right, node* off in that order you put them on as node, right, left. This push-twice recipe turns any recursive tree algorithm into a loop, not only traversals.

**Morris inorder: O(1) extra space.** No stack and no recursion: Morris borrows the tree's own null right pointers to remember where to go back to.
- For a node `cur` with a left subtree, the node visited just before `cur` is its inorder *predecessor*: the rightmost node of the left subtree. Being rightmost, its right pointer is null.
- **First arrival at `cur`:** point the predecessor's right pointer at `cur` (a **thread**), then descend left. When the walk finishes the left subtree, it follows that thread straight back up to `cur`.
- **Second arrival** (the predecessor's right pointer already equals `cur`): the left subtree is done. Remove the thread, visit `cur`, go right.

```
first arrival at 4: its predecessor        later, after visiting 1 2 3:
is 3 (rightmost in 4's left subtree)       3.right leads back up to 4

        4   <- cur                                4 <--------.
       / \                                       / \         |
      2   6                                     2   6        | thread
     / \ / \                                   / \ / \       |
    1  3 5  7                                 1  3 5  7      |
       ^ pred: right is null                     '-----------'
set 3.right = 4, then cur = 2              remove the thread, visit 4, go right
```

It's O(n) time: the predecessor searches walk each left subtree's right spine twice (once to set the thread, once to find it again), and no edge lies on two different spines. The catch: the tree is modified while the walk runs, so it's unsafe for concurrent readers, and stopping early leaves threads behind.

**Level order, with the queue-size snapshot.** A queue hands out nodes in order of depth. To process one level at a time, record the queue's size at the start of each round: at that moment the queue holds exactly one full level. (Induction: it starts as {root}; processing the `level_size` nodes of depth d removes all of them and appends exactly their children, which is depth d + 1.)

<!-- snippet: modules/12-trees-bst/examples/traversals.cpp#level_order -->
```cpp
// How many nodes sit on each level, top to bottom. The loop shape is the point: swap the
// body for whatever your problem needs from each level.
vector<int> nodes_per_level(TreeNode* root) {
    vector<int> counts;
    queue<TreeNode*> q;
    if (root) q.push(root);
    while (!q.empty()) {
        int level_size = (int)q.size();         // snapshot: the queue holds exactly one full level now
        for (int i = 0; i < level_size; i++) {
            TreeNode* node = q.front();
            q.pop();
            if (node->left) q.push(node->left);     // children join the back: they are the next level
            if (node->right) q.push(node->right);
        }
        counts.push_back(level_size);
    }
    return counts;
}
```
<!-- /snippet -->

Swap the loop body for what the problem wants from each level: 102 wants the values, 199 wants the last node, and 662 also needs positions, numbered like a heap array (module 13) and normalized per level so they don't overflow. A DFS works too: pass `depth` down and append to `levels[depth]`; preorder meets each level's nodes left to right.

**Building a tree from two traversals.** Preorder (or postorder) tells you which value is the root; inorder tells you which values lie left of it. See 105 below.

### Pitfalls

- A tree function without `if (!node) return …;` first dereferences nullptr at the first leaf's children. ASan reports a SEGV at an address near 0.
- Passing the output by value (`vector<int> out`) copies it on every call: O(n²), and the results vanish. Pass `vector<int>& out`.
- Level order that re-reads the size while children are being pushed never finishes a level:

```c++
while (!q.empty())
    for (int i = 0; i < (int)q.size(); i++) {   // WRONG: q.size() grows as children are pushed
        TreeNode* node = q.front(); q.pop();
        if (node->left) q.push(node->left);      // snapshot the size in a variable before the loop
    }
```

- Pushing null children and dereferencing them after the pop. Check before pushing (as the templates do) or right after popping; pick one and be consistent.
- Recursion depth: up to 10^5 nodes and no promise of balance means a possible 10^5-deep recursion. Reach for the stack version.
- Morris with an early exit ("stop at the k-th value") leaves threads in the tree. Use the stack version when you want to stop early.
- In 105, searching inorder linearly for every root is O(n²); copying subarrays for every call is O(n²) time and memory. Use index ranges and a hash map.

### Recognize it when…

- "level", "row", "each depth", "right side view", "zigzag", "average of levels", "minimum depth": level order with the snapshot.
- "BST" together with "sorted", "k-th smallest", "validate", "successor", "iterator": inorder.
- "serialize", "copy", "clone", "construct", "path from the root": preorder. Serializing adds null markers so the shape survives (297).
- "vertical order", "columns": record (column, row) for every node during one traversal, then sort (987).
- "boundary": three separate walks, left edge, leaves, right edge, without counting a node twice (545).
- "width of a level", counting the missing nodes in between: position indices per level (662).
- "height", "balanced", "size", "delete the tree", anything computed from the children: postorder.
- "without recursion", "O(h) memory", "`next()` and `hasNext()`": an explicit stack. "O(1) extra space": Morris.
- "given preorder (or postorder) and inorder, build the tree": root from one, split the other with a value → index map.

### Worked example: 94. Binary Tree Inorder Traversal
[LeetCode 94](https://leetcode.com/problems/binary-tree-inorder-traversal/) · Easy

**Problem (paraphrased):** return a binary tree's values in inorder (left subtree, node, right subtree). At most 100 nodes; the follow-up asks for an iterative version.
**Signals:** "inorder" by name; the follow-up is the real exercise.
**Brute force, and why it fails:** the recursive version is already optimal in time, O(n), and uses O(h) stack. It doesn't fail here; the other two versions exist for control (no recursion) and space (O(1)).
**Key insight:** a node is visited when you come back from its left subtree, so the iterative version keeps "the ancestors whose left subtree I'm still inside" on a stack. Morris stores the same "where do I go back to" in the tree's unused null right pointers.
**Dry run:** Morris on the 7-node tree from the diagram, [4,2,6,1,3,5,7]:

| step | cur | action | output so far |
|---|---|---|---|
| 1 | 4 | pred = 3, its right is null: thread 3.right = 4, go left | |
| 2 | 2 | pred = 1, its right is null: thread 1.right = 2, go left | |
| 3 | 1 | no left child: visit, follow 1.right (the thread) | 1 |
| 4 | 2 | pred = 1, 1.right == 2: remove the thread, visit, go right | 1 2 |
| 5 | 3 | no left child: visit, follow 3.right (the thread) | 1 2 3 |
| 6 | 4 | pred = 3, 3.right == 4: remove the thread, visit, go right | 1 2 3 4 |
| 7 | 6 | pred = 5: thread 5.right = 6, go left | 1 2 3 4 |
| 8 | 5 | visit, follow the thread | 1 2 3 4 5 |
| 9 | 6 | remove the thread, visit, go right | 1 2 3 4 5 6 |
| 10 | 7 | visit; 7.right is null: done | 1 2 3 4 5 6 7 |

<!-- snippet: modules/12-trees-bst/examples/0094-binary-tree-inorder-traversal.cpp#recursive -->
```cpp
class Solution {
public:
    vector<int> inorderTraversal(TreeNode* root) {
        vector<int> out;
        visit(root, out);
        return out;
    }

private:
    void visit(TreeNode* node, vector<int>& out) {   // out by reference: one vector, not a copy per call
        if (!node) return;
        visit(node->left, out);
        out.push_back(node->val);                      // inorder: between the two recursive calls
        visit(node->right, out);
    }
};
```
<!-- /snippet -->

<!-- snippet: modules/12-trees-bst/examples/0094-binary-tree-inorder-traversal.cpp#iterative -->
```cpp
class Solution {
public:
    vector<int> inorderTraversal(TreeNode* root) {
        vector<int> out;
        stack<TreeNode*> st;            // ancestors whose left subtree we are inside: each is visited
        TreeNode* cur = root;           // as soon as that left subtree is finished
        while (cur || !st.empty()) {
            while (cur) {               // walk down the left spine, remembering the path
                st.push(cur);
                cur = cur->left;
            }
            cur = st.top();             // the deepest pending node: its left subtree is done
            st.pop();
            out.push_back(cur->val);
            cur = cur->right;           // traverse its right subtree the same way
        }
        return out;
    }
};
```
<!-- /snippet -->

<!-- snippet: modules/12-trees-bst/examples/0094-binary-tree-inorder-traversal.cpp#morris -->
```cpp
class Solution {
public:
    vector<int> inorderTraversal(TreeNode* root) {
        vector<int> out;
        TreeNode* cur = root;
        while (cur) {
            if (!cur->left) {                   // nothing on the left: visit, then go right
                out.push_back(cur->val);
                cur = cur->right;               // this may be a thread, leading back up to an ancestor
                continue;
            }
            TreeNode* pred = cur->left;         // inorder predecessor: rightmost node of the left subtree
            while (pred->right && pred->right != cur) pred = pred->right;
            if (!pred->right) {                 // first arrival: leave a thread back to cur, go left
                pred->right = cur;
                cur = cur->left;
            } else {                            // second arrival, via the thread: the left side is done
                pred->right = nullptr;          // remove the thread, restoring the tree
                out.push_back(cur->val);
                cur = cur->right;
            }
        }
        return out;
    }
};
```
<!-- /snippet -->

**Complexity:** all three are O(n) time. Space: recursive and stack are O(h), which is O(n) for a chain; Morris is O(1) apart from the output (just two pointers), and still O(n) time because every edge is walked a constant number of times.
**Edge cases:**
- Empty tree: `{}`. No version special-cases `root == nullptr`; the loops just don't run.
- Chains: the tests include a 2,000-node left chain; the stack version holds 2,000 pointers, Morris none.
- Morris must restore the tree: the tests compare `tree_to_string` before and after.

**Follow-ups:**
- *Preorder without recursion?* That's 144: the bullet in the concept. *Postorder?* The (node, ready) template above.
- *The k-th smallest in a BST without visiting everything?* Stop the stack version at the k-th pop (230).
- *An iterator with `next()` and `hasNext()`?* Keep the stack as a member and run one round of the loop per `next()`: O(h) memory, O(1) amortized per call.

### Worked example: 105. Construct Binary Tree from Preorder and Inorder Traversal
[LeetCode 105](https://leetcode.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/) · Medium

**Problem (paraphrased):** given the preorder and inorder sequences of a binary tree whose values are all distinct, rebuild the tree. Up to 3,000 nodes.
**Signals:** two traversals as input; "values are unique", so a value pins down its position.
**Brute force, and why it fails:** take `preorder[0]` as the root, scan inorder for it, copy the left and right parts into new vectors, recurse. Correct, but the scan and the copies cost O(n) per node: O(n²) time and memory, worst on a chain.
**Key insight:** preorder lists the root first; its position `mid` in inorder splits the values into the left subtree (`mid - in_lo` values) and the right subtree. That size also locates both subtrees inside preorder, so recurse on index ranges, and find `mid` through a hash map from value to inorder index in O(1).

```
preorder: [ 3 | 9 | 20 15 7 ]    root, then the left subtree (1 value), then the right subtree
inorder:  [ 9 | 3 | 15 20 7 ]    the root at mid = 1, so left_size = mid - in_lo = 1
```

**Dry run:** preorder = [3,9,20,15,7], inorder = [9,3,15,20,7]:

| call (pre_lo, in_lo..in_hi) | root | mid | left_size | left call | right call |
|---|---|---|---|---|---|
| (0, 0..4) | 3 | 1 | 1 | (1, 0..0) | (2, 2..4) |
| (1, 0..0) | 9 | 0 | 0 | (2, 0..-1): null | (2, 1..0): null |
| (2, 2..4) | 20 | 3 | 1 | (3, 2..2): node 15 | (4, 4..4): node 7 |

<!-- snippet: modules/12-trees-bst/examples/0105-construct-binary-tree-from-preorder-and-inorder-traversal.cpp#solution -->
```cpp
class Solution {
public:
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        inorder_index.clear();
        for (int i = 0; i < (int)inorder.size(); i++) inorder_index[inorder[i]] = i;   // values are unique
        return build(preorder, 0, 0, (int)inorder.size() - 1);
    }

private:
    unordered_map<int, int> inorder_index;     // value -> its position in inorder

    // Builds the subtree whose values are inorder[in_lo..in_hi]; its preorder starts at pre_lo.
    //   preorder: [ root | left subtree (left_size values) | right subtree ]
    //   inorder:  [ left subtree (left_size values) | root | right subtree ]
    TreeNode* build(const vector<int>& preorder, int pre_lo, int in_lo, int in_hi) {
        if (in_lo > in_hi) return nullptr;                  // empty range: empty subtree
        TreeNode* root = new TreeNode(preorder[pre_lo]);    // preorder lists the root first
        int mid = inorder_index[root->val];                 // the root splits the inorder range
        int left_size = mid - in_lo;
        root->left = build(preorder, pre_lo + 1, in_lo, mid - 1);
        root->right = build(preorder, pre_lo + 1 + left_size, mid + 1, in_hi);
        return root;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time: every node is created once, with an O(1) average map lookup. O(n) space for the map, plus O(h) recursion.
**Edge cases:**
- One node gives a one-node tree; empty input gives nullptr (the `in_lo > in_hi` base case).
- Sorted input (preorder == inorder) is a 3,000-node right chain: recursion depth 3,000, which is fine (tested).
- The map is a member, so `buildTree` clears it first in case the object is reused.

**Follow-ups:**
- *Inorder + postorder instead?* The root is the *last* postorder value. If you consume postorder from the back, build the right subtree before the left.
- *Preorder + postorder?* Not unique in general: a root with a single child could have it on either side (preorder [1,2], postorder [2,1] fits both). It is unique for full binary trees, where every node has 0 or 2 children.
- *How did the tests check this without a brute force?* A round trip: random tree → (preorder, inorder) → `buildTree` → must equal the original tree. Use that trick whenever an operation has an easy inverse.

## 2. Tree recursion — height, diameter, path sums, lowest common ancestor

### Concept

Every tree recursion starts with one question: **what does `f(node)` return for the subtree rooted at `node`?** Write the answer as a comment before writing any code. Then:

1. **Base case:** the empty tree (`nullptr`), not the leaf. Leaves then need no special code, unless the problem defines something *at* leaves (root-to-leaf paths).
2. **Combine:** express `f(node)` using `f(node->left)`, `f(node->right)` and `node->val`. Both calls run before the combine: post-order.
3. **Cost:** O(1) work per node outside the calls means O(n) time and O(h) stack.

Information moves in three ways:

| Direction | Mechanism | Examples | Use it when |
|---|---|---|---|
| Up (bottom-up) | the return value | height, size, "is it balanced" | the parent's answer is built from its children's answers |
| Down (top-down) | parameters | depth, path sum so far, allowed (low, high) range | the answer at a node depends on its ancestors |
| Sideways | a member, global or reference accumulator | diameter, maximum path sum, counts | what the *parent* needs differs from the final answer |

The third row is the classic trap. In 543 the parent needs its child's *height*, but the answer is a *path length*. Return what the parent needs; record the answer separately (or return both in a pair or struct).

When the parent needs several things, return a struct and fill it in one pass:

<!-- snippet: modules/12-trees-bst/examples/tree_recursion.cpp#bottom_up -->
```cpp
// Everything a parent might need from a subtree, computed in ONE post-order pass.
struct Summary {
    int size = 0;                     // number of nodes
    int height = 0;                   // nodes on the longest downward path: empty tree 0, leaf 1
    long long sum = 0;                // long long: n values of up to 1e9 overflow int
    int leaves = 0;                   // nodes with no children
};

Summary summarize(TreeNode* node) {
    if (!node) return {};                             // the empty tree's answer: all zeros
    Summary l = summarize(node->left), r = summarize(node->right);
    Summary s;                                        // combine: children's answers + this node
    s.size = l.size + r.size + 1;
    s.height = max(l.height, r.height) + 1;
    s.sum = l.sum + r.sum + node->val;
    s.leaves = (!node->left && !node->right) ? 1 : l.leaves + r.leaves;
    return s;
}
```
<!-- /snippet -->

If a summary holds a minimum or maximum, give the empty tree the identity value (INT_MAX for a min, INT_MIN for a max) so it never wins a comparison.

When the answer depends on the path from the root, carry it down:

<!-- snippet: modules/12-trees-bst/examples/tree_recursion.cpp#top_down -->
```cpp
// Largest root-to-leaf sum: carry the path so far DOWN as a parameter, record answers at the
// leaves into a reference. A leaf has no children at all: a node with one child is NOT a leaf,
// so a path may not stop there.
void best_root_to_leaf(TreeNode* node, long long path_sum, long long& best) {
    if (!node) return;
    path_sum += node->val;                            // path_sum is a copy: siblings never see it
    if (!node->left && !node->right) best = max(best, path_sum);
    best_root_to_leaf(node->left, path_sum, best);
    best_root_to_leaf(node->right, path_sum, best);
}
// Call: long long best = LLONG_MIN; best_root_to_leaf(root, 0, best);   (root != nullptr)
```
<!-- /snippet -->

Both are O(n) time and O(h) stack.

**Height and depth: pick a convention and say it out loud.**

| Term | In these notes | Empty tree | One node |
|---|---|---|---|
| height of a subtree | **nodes** on the longest downward path (LeetCode 104's "maximum depth") | 0 | 1 |
| depth of a node | **edges** from the root, i.e. its BFS level | – | root: 0 |
| length of a path (diameter) | **edges** | 0 | 0 |

Textbooks and AVL papers often count height in edges (empty −1, leaf 0). Neither is wrong; mixing them inside one solution is. With node-count heights, the longest path that turns at a node has exactly `height(left) + height(right)` edges.

**Diameter.** The longest path between any two nodes, in edges. Every path has one highest node, where it turns; there it is (the deepest descent on the left) + (the deepest descent on the right). So compute heights bottom-up and, at each node, update `best = max(best, lh + rh)`. Worked below (543).

**Path sums: which paths?**

| Paths allowed | Technique | Problem |
|---|---|---|
| root to leaf | carry the running state down; test at real leaves; to list every path, backtrack (push, recurse, pop) | 257 |
| any downward path (ancestor to descendant) | prefix sums along the current root-to-node path + a hash map | module 07's 560, walked down a path |
| any path, possibly turning at a node | return the best one-sided downward gain; update a global with both sides at each node | 124 |

The last row has the same shape as 543's diameter. Use `long long` sums when values reach 10^9.

**LCA by return values.** The lowest common ancestor of p and q is the deepest node with both in its subtree; a node counts as its own ancestor. In 236 each call reports what its subtree contains: nothing (nullptr), one target (that node), or both (their LCA). Proof in the worked example.

**When distances start at an arbitrary node.** Recursion only walks down, but "distance from this node" also goes up through parents. Record every node's parent in one traversal, then treat the tree as an undirected graph and BFS from the start node (BFS proper is module 14). That's the pattern behind 863 and 2385.

**Binary lifting, in one paragraph.** For many LCA or "k-th ancestor" queries on a fixed tree, first record every node's parent and depth (one DFS or BFS), then precompute `up[j][v]`, the 2^j-th ancestor of v, via `up[j][v] = up[j-1][up[j-1][v]]`: O(n log n) time and memory. A k-th ancestor query jumps along the set bits of k in O(log n). For LCA(u, v): lift the deeper node by the depth difference; if the two are now equal, that's the answer; otherwise, for j from high to low, jump both whenever `up[j][u] != up[j][v]`, and finish at the parent of u. That's O(log n) per query instead of O(n).

### Pitfalls

- Calling `height()` at every node of another recursion: each call costs O(subtree size), so the total is O(n log n) on a balanced tree and O(n²) on a chain (543's brute force). Return the height and the verdict together (for a "balanced?" check, return −1 to mean "unbalanced").
- Treating a missing child as the end of a path: in a min-depth or root-to-leaf recursion, `if (!node) return 0;` lets a node with one child act as a leaf. The tree [1,2] has minimum depth 2, not 1.
- Stale accumulators: a global or `static` can keep its value from the previous test case, and a member keeps its value between calls on the same object. Reset it at the top of the public method, as 543 does.
- Overflow: path sums over up to 10^4 nodes with values up to 10^9 need `long long`, and so do prefix sums along a path.
- Comparing nodes by value in LCA: compare pointers. Values may repeat in general; pointers can't.
- The recursive LCA assumes both targets are in the tree. If one is missing it returns the other, which looks plausible and is wrong; count the targets you actually met.

### Recognize it when…

- "height", "depth", "balanced", "count the nodes", "is the subtree…": a bottom-up return value.
- "longest path between any two nodes", "maximum path sum" with free endpoints: return one side, record both sides in a separate variable.
- "root-to-leaf": carry state down and check real leaves; "list all such paths": backtracking (push, recurse, pop), as in 257.
- "any downward path summing to k": prefix sums along the path (module 07's 560 idea).
- "all nodes at distance k from a target", "time for something to spread from a node": parent map + BFS (863, 2385).
- "lowest common ancestor", "distance between two nodes" (= depth(p) + depth(q) − 2·depth(LCA)), "directions from one node to another": LCA.
- 10^5 nodes and 10^5 ancestor or LCA queries: binary lifting.

### Worked example: 543. Diameter of Binary Tree
[LeetCode 543](https://leetcode.com/problems/diameter-of-binary-tree/) · Easy

**Problem (paraphrased):** return the number of edges on the longest path between any two nodes of a binary tree. Up to 10^4 nodes.
**Signals:** "any two nodes", "may or may not pass through the root", "longest".
**Brute force, and why it fails:** at every node, compute the heights of both subtrees with a separate `height()` call: O(size of the subtree) per node, O(n²) on a chain. BFS from every node is O(n²) as well. With 10^4 nodes that's up to ~10^8 steps: borderline at best, and interviewers expect the one-pass version.
**Key insight:** the longest path turns at its highest node, where its length is lh + rh. One post-order pass computes every height; update the best at each node on the way up. The parent needs the height and the answer is a path: return one, record the other.
**Dry run:** [1,2,3,4,5]:

```
        1
       / \
      2   3
     / \
    4   5
```

| node | lh | rh | best = max(best, lh + rh) | returns 1 + max(lh, rh) |
|---|---|---|---|---|
| 4 | 0 | 0 | 0 | 1 |
| 5 | 0 | 0 | 0 | 1 |
| 2 | 1 | 1 | 2 | 2 |
| 3 | 0 | 0 | 2 | 1 |
| 1 | 2 | 1 | 3 | 3 |

The answer is 3: the path 4-2-1-3 (or 5-2-1-3).

<!-- snippet: modules/12-trees-bst/examples/0543-diameter-of-binary-tree.cpp#solution -->
```cpp
class Solution {
public:
    int diameterOfBinaryTree(TreeNode* root) {
        best = 0;                       // reset: never trust state left over from a previous call
        height(root);
        return best;
    }

private:
    int best = 0;                       // longest path found so far, in edges

    // Returns the height in nodes (empty 0, leaf 1): what the PARENT needs.
    // Records the longest path that turns at this node: what the ANSWER needs.
    // With node-count heights, that path has exactly lh + rh edges.
    int height(TreeNode* node) {
        if (!node) return 0;
        int lh = height(node->left);
        int rh = height(node->right);
        best = max(best, lh + rh);
        return 1 + max(lh, rh);
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time, each node visited once; O(h) stack.
**Edge cases:**
- One node: 0. Two nodes: 1.
- The best path may avoid the root: in the tests, [1,2,null,3,4,5,null,null,6,7,null,null,8] has diameter 6, turning at node 2.
- A chain of n nodes: n − 1, with recursion depth n.

**Follow-ups:**
- *Without a member variable?* Return a pair {height, best path inside this subtree} and combine with `max({best_left, best_right, lh + rh})`.
- *Nodes have weights and you want the largest path sum (124)?* Same shape: return a one-sided value, record a two-sided one. Think about what a subtree with a negative total should contribute.
- *A general tree, not binary?* Keep the two largest child heights at each node. For an unweighted tree given as a graph: BFS from any node to the farthest node x, then BFS from x; the farthest distance from x is the diameter.

### Worked example: 236. Lowest Common Ancestor of a Binary Tree
[LeetCode 236](https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-tree/) · Medium

**Problem (paraphrased):** given a binary tree (not a BST) and two different nodes p and q that are both in it, return their lowest common ancestor; a node may be its own ancestor. Up to 10^5 nodes, unique values.
**Signals:** "lowest common ancestor", the targets given as node pointers, an arbitrary binary tree.
**Brute force, and why it fails:** record the root-to-p and root-to-q paths with a backtracking DFS, then walk both from the root until they differ: O(n) time and O(n) extra memory. It works; it's just two passes plus path bookkeeping. Checking, for every node, whether its subtree holds both targets is O(n²).
**Key insight:** let each call report what it found below it. The first node that hears "found" from both sides is the LCA. A node that *is* p or q can stop without looking further: the answer is either that node, or an ancestor will see it as "found one".

Why the contract in the code holds (induction on the subtree):
- **`root` is p or q:** returning it is right either way. If the other target lies below it, root is the LCA; if not, root is "the one target here".
- **Otherwise** root's targets are exactly its children's targets. If both calls report something, p and q are on different sides, so root is the lowest node holding both.
- **Only one side reports:** that side holds every target in root's subtree, so its report is root's report too.

**Dry run:** [3,5,1,6,2,0,8,null,null,7,4], p = 7, q = 4:

```
          3
        /   \
       5     1
      / \   / \
     6   2 0   8
        / \
       7   4
```

| call | left returns | right returns | returns |
|---|---|---|---|
| 6 | null | null | null |
| 7 | (it is p: stop) | | 7 |
| 4 | (it is q: stop) | | 4 |
| 2 | 7 | 4 | 2: one target on each side |
| 5 | null (from 6) | 2 | 2 |
| 1 | null | null | null |
| 3 | 2 | null | 2 |

<!-- snippet: modules/12-trees-bst/examples/0236-lowest-common-ancestor-of-a-binary-tree.cpp#solution -->
```cpp
class Solution {
public:
    // Contract for the subtree rooted at `root`:
    //   contains both p and q  -> returns their LCA
    //   contains exactly one   -> returns that node
    //   contains neither       -> returns nullptr
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        if (!root || root == p || root == q) return root;   // compare pointers, not values
        TreeNode* left = lowestCommonAncestor(root->left, p, q);
        TreeNode* right = lowestCommonAncestor(root->right, p, q);
        if (left && right) return root;     // one target on each side: they meet here
        return left ? left : right;         // pass up whatever the non-empty side found
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time, every node at most once; O(h) stack, which can be 10^5 frames on a chain.
**Edge cases:**
- p is an ancestor of q: the walk returns p as soon as it reaches it (official example: p = 5, q = 4 gives 5).
- p or q is the root: returns the root at once.
- p == q is outside the constraints, but the code returns p, which is the right answer anyway.

**Follow-ups:**
- *It's a BST (235)?* Use the ordering instead of searching both sides: look for the node where p and q split. O(h).
- *Nodes have parent pointers but you get no root?* Two pointers climb, each restarting from the other's starting node when it passes the top: the same trick as intersecting two linked lists (module 08's 160).
- *Many queries on one tree?* Binary lifting: O(n log n) preprocessing, O(log n) per query.
- *p or q might be missing?* Count the targets met during the same traversal and return nullptr unless both were seen.

## 3. BSTs — insert, delete, validate, k-th smallest, balancing intuition

### Concept

**The invariant:** at every node, *every* key in the left subtree is smaller and *every* key in the right subtree is larger (strictly, when keys are unique). It is a statement about whole subtrees, not about the two children.

Three consequences:

1. Search, insert, delete, minimum, maximum, floor and ceiling each walk a single root-to-leaf path, because at every node the invariant rules out one side. O(h).
2. Inorder visits the keys in sorted order. (Induction: inorder(left) lists the smaller keys in order, then the node, then inorder(right) lists the larger keys in order.)
3. h depends on insertion order: random order gives O(log n) expected height (about 3·log2 n), sorted order gives a chain with h = n.

**The walk.** Search, floor, ceiling, successor and predecessor are one pattern: go left or right by comparison, and when you want the closest key rather than an exact match, remember the best candidate so far (285). The same invariant prunes range queries: a node below the range rules out its whole left subtree, a node above it rules out its whole right subtree.

<!-- snippet: modules/12-trees-bst/examples/bst_basics.cpp#range_sum -->
```cpp
// Sum of the keys in [lo, hi]. O(h + m) for m keys in the range: the BST invariant rules out
// a whole subtree at every node that falls outside the range.
long long range_sum(TreeNode* node, int lo, int hi) {
    if (!node) return 0;
    if (node->val < lo) return range_sum(node->right, lo, hi);  // node and its left subtree: all too small
    if (node->val > hi) return range_sum(node->left, lo, hi);   // node and its right subtree: all too big
    return node->val + range_sum(node->left, lo, hi) + range_sum(node->right, lo, hi);
}
```
<!-- /snippet -->

It touches the m keys in the range plus at most two root-to-leaf boundary paths: O(h + m), not O(n). The tests count it: a 10-key range in a balanced 32,767-key BST touches at most 2h + 10 nodes.

**Insert (701)** is the same walk: when you fall off the tree, the null slot you fell into is exactly where the new key belongs. Hang a new node there (for an empty tree, the new node is the root). Write it yourself.

**Delete (450):** find the node, then one of three cases.

```
case 1: a leaf. Remove it.          case 2: one child. The child takes its place.
     5                  5                5                       5
    / \     delete 7   /                / \      delete 3       / \
   3   7    ------->  3                3   8     ------->      1   8
                                      /
                                     1

case 3: two children. Copy the in-order successor up, then delete the successor.
      5                       6                          6
    /   \     copy succ     /   \     delete the old   /   \
   3     8    6 up         3     8    6 from the      3     8
        / \   ------->          / \   right subtree        / \
       6   9                   6   9  (case 2)            7   9
        \                       \     ------->
         7                       7
```

The successor is the smallest key larger than the deleted one, so it fits in the deleted node's place: larger than everything on the left, smaller than everything else on the right. It is the leftmost node of the right subtree, so it has no left child, and deleting it is always case 1 or 2. (The in-order predecessor, the rightmost node of the left subtree, works symmetrically.) Worked below.

**Validation with bounds (98).** Every node inherits an allowed open interval from its ancestors. The root may be anything; stepping left caps the interval at the parent's key, stepping right floors it at the parent's key. Check every node against its interval. Comparing each node only with its children is not enough:

```
      5
     / \
    4   6          every parent-child pair is in order,
       / \         but 3 sits in 5's right subtree:
      3   7        only the inherited interval (5, 6) rejects it
```

The **INT_MIN / INT_MAX trap:** starting the interval at (INT_MIN, INT_MAX) and testing `low < val < high` rejects valid trees that contain INT_MIN or INT_MAX themselves, and LeetCode's tests include such trees. The fixes: `long long` bounds (LLONG_MIN and LLONG_MAX lie outside int's range), `TreeNode*` bounds where nullptr means "no bound", or checking that the inorder sequence is strictly increasing (keep a pointer to the previous node).

```c++
bool check(TreeNode* node, int low, int high);   // int bounds...
check(root, INT_MIN, INT_MAX);                   // ...reject the valid one-node tree [2147483647]
```

**k-th smallest (230).**
- **By inorder:** run the stack version of inorder and stop at the k-th pop. O(h + k) time, O(h) space.
- **By subtree sizes:** if each node knows its subtree's size, exactly `size(left)` keys are smaller than the node. Descend: k ≤ size(left) → go left; k = size(left) + 1 → this node; otherwise subtract size(left) + 1 and go right. That's O(h) per query, whatever k is. The sizes must be kept current: +1 on every node along an insert's path, −1 along a delete's path, recomputed from the children after a rotation. This is the tool for many k-th queries on a changing tree, and the engine of the section 4 project.

```
sizes in brackets                   k = 4: at 8, size(left) = 3 and 4 = 3 + 1: it's 8
          8 [6]                     k = 5: 5 > 4, so k -= 4 (now 1), go right:
        /     \                            at 10, size(left) = 0 and 1 = 0 + 1: it's 10
     3 [3]    10 [2]                k = 2: 2 <= 3, go left:
     /   \        \                        at 3, size(left) = 1 and 2 = 1 + 1: it's 3
  1 [1]  6 [1]    14 [1]
```

Each query is O(h). **Rank** (how many keys are smaller than x) is the mirror image: walk down as in a search, and every time you go right, add size(left) + 1. You'll implement both, with the size bookkeeping, in the section 4 project.

**Balancing intuition.**
- **Degenerate chains:** insert 1, 2, …, n into a plain BST and each node hangs to the right of the previous one: h = n, every operation O(n), n inserts O(n²). Sorted input (IDs, timestamps) is common, so a plain BST is a liability.
- **Rotations** rewire three pointers locally. They change heights but keep the inorder order, so the BST property survives:

```
        y                          x
       / \     rotate right       / \
      x   C    ----------->      A   y
     / \       <-----------         / \
    A   B      rotate left         B   C

    inorder on both sides: A x B y C   (subtree B just changes parent)
```

- **AVL trees** rotate to keep |height(left) − height(right)| ≤ 1 at every node, so the height stays below about 1.44·log2 n. **Red-black trees** colour nodes red or black, forbid a red node with a red child, and require the same number of black nodes on every root-to-null path: height ≤ 2·log2(n + 1), and any insert or delete needs at most 3 rotations.
- **std::set, std::map, std::multiset, std::multimap:** the standard only demands O(log n) operations, and both libstdc++ (GCC) and libc++ (clang) implement them as red-black trees. In C#, `SortedSet<T>` and `SortedDictionary<TKey, TValue>` are red-black trees too.
- **Treaps** are a BST by key and a heap by a random priority stored in each node. The random priorities give the shape of a random BST, so the expected height is O(log n) whatever the insertion order. With split and merge, a treap is the easiest balanced BST to write from scratch, and easy to augment with sizes or sums.
- **When std::set is enough:** ordered unique keys with insert, erase, find, `lower_bound` / `upper_bound`, minimum (`begin()`), maximum (`rbegin()`), predecessor and successor (`prev` / `next` on an iterator), all O(log n). It is **not** enough when you need rank or the k-th element: `std::distance` between set iterators is O(n). Then use an order-statistics tree (the GCC-only `__gnu_pbds::tree`, which Apple clang doesn't ship, so it's not in this kit), a Fenwick tree over compressed values (module 17), or your own size-augmented tree (the section 4 project).

### Pitfalls

- Validating against the children only, or with `int` sentinel bounds (above).
- `std::lower_bound(s.begin(), s.end(), x)` on a `std::set` compiles but is O(n): set iterators can't jump. Use the member function `s.lower_bound(x)`.
- `ms.erase(x)` on a `multiset` erases *every* copy of x. To erase one: `auto it = ms.find(x); if (it != ms.end()) ms.erase(it);`.
- `m[key]` on a `map` inserts a default value when the key is missing. Test membership with `find`, `count` or C++20's `contains`.
- Dropping the returned subtree: `deleteNode(root->left, key);` instead of `root->left = deleteNode(root->left, key);` never unlinks anything.
- Copying the successor's *value* is fine for int keys. When nodes carry payloads, or outside code holds pointers to nodes, relink the successor node instead.
- Sorted or nearly sorted input turns a plain BST into a chain: O(n) per operation and recursion depth n.
- Duplicates: choose a rule (equal keys go right, or a count per node) and apply it in insert, search and delete alike.

### Recognize it when…

- The statement says BST: expect O(h) by discarding a subtree per step. If your solution visits every node, ask whether the ordering could prune it.
- "k-th smallest", "rank", "median of a changing set", "how many values in [lo, hi]": inorder with an early stop, or subtree sizes for repeated queries.
- "closest value", "floor", "ceiling", "next larger", "successor": the walk with a best-so-far candidate.
- "sorted data to a height-balanced BST": the middle element is the root; recurse on the halves.
- "iterator", "next smallest in O(1) amortized with O(h) memory": a stack of left spines (653 can use two).
- "LCA" in a BST: the split point of p and q (235). "Successor / predecessor": the walk with a candidate (285).
- "build a BST from its preorder": each value must fit the bounds inherited from its ancestors (1008).
- "two nodes were swapped": the inorder sequence stops being sorted in one or two places (99).
- "largest BST inside a binary tree": a post-order summary per subtree (333).
- A problem that needs an ordered, changing set (a sliding window with order, a booking calendar, "the closest earlier timestamp"): `std::set` / `std::map`, not a hand-written BST.

### Worked example: 450. Delete Node in a BST
[LeetCode 450](https://leetcode.com/problems/delete-node-in-a-bst/) · Medium

**Problem (paraphrased):** given the root of a BST and a key, delete the node holding that key, if there is one, and return the (possibly new) root. Up to 10^4 nodes, unique values; the follow-up asks for O(height).
**Signals:** "BST", "delete", "return the root", which hints the root itself may go.
**Brute force, and why it fails:** collect the values with an inorder walk, drop the key, rebuild a balanced BST from the sorted list: O(n) time and memory. It ignores the structure and misses the O(h) follow-up.
**Key insight:** find the node with the BST walk; then it's one of the three cases. If every call returns the root of its subtree after the deletion and the caller stores that into its child pointer, splicing a node out is one `return`, and deleting the root needs no special case.
**Dry run:** [5,3,6,2,4,null,7], key = 3:

| call | comparison | action |
|---|---|---|
| deleteNode(5, 3) | 3 < 5 | 5.left = deleteNode(3, 3) |
| deleteNode(3, 3) | found; two children | successor = leftmost of 3's right subtree = 4; copy 4 into the node; node.right = deleteNode(4, 4) |
| deleteNode(4, 4) | found; no left child | return 4.right, which is nullptr |

The result is [5,4,6,2,null,null,7].

<!-- snippet: modules/12-trees-bst/examples/0450-delete-node-in-a-bst.cpp#solution -->
```cpp
class Solution {
public:
    // Returns the root of the subtree after deleting key from it. The caller stores the result
    // back into its child pointer, which is how a deleted node gets spliced out.
    TreeNode* deleteNode(TreeNode* root, int key) {
        if (!root) return nullptr;                              // fell off the tree: key not present
        if (key < root->val) {
            root->left = deleteNode(root->left, key);
        } else if (key > root->val) {
            root->right = deleteNode(root->right, key);
        } else {
            // Cases 1 and 2: at most one child. That child (or nullptr) takes this node's place.
            // (LeetCode never frees nodes; in your own code, `delete` the unlinked node.)
            if (!root->left) return root->right;
            if (!root->right) return root->left;
            // Case 3: two children. The in-order successor (leftmost node of the right subtree)
            // is the next larger value: it fits here, above everything left and below everything
            // else on the right. Copy it up, then delete it from the right subtree. It has no
            // left child, so that second delete is always case 1 or 2.
            TreeNode* successor = root->right;
            while (successor->left) successor = successor->left;
            root->val = successor->val;
            root->right = deleteNode(root->right, successor->val);
        }
        return root;
    }
};
```
<!-- /snippet -->

**Complexity:** O(h) time: one walk down to the key, one down to its successor, and the second delete retraces that same path. O(h) recursion stack; an iterative version with a parent pointer needs O(1). h is O(log n) when balanced and O(n) for a chain.
**Edge cases:**
- The key is absent: the walk falls off the tree and stores nullptr back into a child that was already nullptr. Nothing changes.
- Deleting the root: the returned value is the new root; [1] becomes [].
- The successor has a right child: that child takes the successor's place ([8,3,12,null,null,10,15,null,11] with key 8, in the tests).

**Follow-ups:**
- *Without copying values?* Unlink the successor from its parent (its right child takes its place), give it the deleted node's two children, and return it.
- *Iteratively?* Walk down keeping a parent pointer, then relink `parent->left` or `parent->right`. Same three cases, O(1) extra space.
- *Keep it balanced under many deletions?* That's what AVL and red-black rotations are for. In C++, reach for `std::set::erase`.

## 4. Project — a tree-backed autocomplete / file-index, with tests you write first

LeetCode proves you can write one function. Product interviews also probe whether you can shape a small data structure, test it and use it in a program. This project does exactly that with section 3: a BST keyed by file names, augmented with subtree sizes, built test-first.

- Read [projects/12-file-index/README.md](../../projects/12-file-index/README.md) for the spec, the milestones and the test checklist.
- **Milestone 1 is test-first:** write the tests from the README's checklist before any implementation, watch them fail, then make them pass. Use `CHECK_EQ` and a brute-force oracle (a sorted `vector<string>`), exactly like the stress tests in this module's examples.
- **Milestone 2:** prefix autocomplete plus rank, k-th and range queries. The subtree sizes from section 3 give rank and k-th in O(h). All names with a given prefix form one contiguous run in sorted order that starts at the smallest name ≥ the prefix (the walk with a best-so-far candidate from section 3), and a range count is a difference of two ranks.
- **Milestone 3:** the command-line tool over a real directory, then a code review with Claude (`/review`).

It's your code: ask Claude for reviews and hints, not for the implementation.

## Common mistakes

- No `nullptr` base case, so the first leaf's children crash the program.
- Treating a node with one child as a leaf (minimum depth, root-to-leaf sums).
- Mixing height conventions (nodes vs edges) inside one solution: off-by-one diameters and depths.
- Returning the wrong quantity when what the parent needs differs from the final answer (543, 124). Decide which travels up and which goes into a separate variable.
- An accumulator that isn't reset between test cases or calls.
- Level order without the size snapshot.
- BST validation against the children only, or with `int` sentinel bounds.
- Forgetting `root->left = …` when a recursive call returns the new subtree root (delete, insert, trim).
- Accidental O(n²): `height()` inside a recursion, a linear search for the root in 105, vectors copied into every call.
- Recursion depth on 10^4–10^5-node chains: have the iterative version ready.

## Say it out loud

A talk track for 236:

1. **Restate:** "It's a binary tree, not a BST, and both nodes are in it. I return the deepest node that has both of them below it, counting a node as its own descendant."
2. **Brute force:** "I could record the root-to-p and root-to-q paths and compare them: O(n) time, O(n) extra memory, two passes."
3. **Insight:** "One post-order pass instead: each call returns null, the target it found, or the answer. The first node that gets something from both children is the LCA."
4. **Complexity:** "O(n) time, O(h) stack. h can be n on a skewed tree, so at 10^5 nodes I'd offer the iterative parent-map version."
5. **Edge cases:** "p is an ancestor of q; p or q is the root; a skewed tree."

Follow-ups you'll hear in tree rounds:
- "Now do it without recursion." An explicit stack; for postorder, a reversed preorder or a last-visited pointer.
- "O(1) extra space?" Morris, for inorder.
- "What if it's a BST?" Use the ordering: O(h) walks, the split point for LCA (235), inorder is sorted.
- "What if the tree is very deep?" Recursion depth: switch to the iterative version.
- "What if there are many queries?" Precompute: subtree sizes, binary lifting.
- "Can you do it in one pass?" Return a struct or pair instead of calling a helper at every node.

## Self-check

1. What's the extra space of recursive inorder on n nodes, and when does it bite?
<details><summary>Answer</summary>O(h) for the call stack: about log2 n when balanced, n for a chain. At 10^5 nodes in a skewed tree that's 10^5 nested calls, which can overflow the stack; use the explicit-stack version or Morris.</details>

2. In level order, why must `q.size()` be saved in a variable before the inner loop?
<details><summary>Answer</summary>The loop pushes the children of the current level, so `q.size()` keeps growing and the loop would run into the next level. At the start of a round the queue holds exactly one level; the snapshot freezes that count.</details>

3. In Morris traversal, how does the algorithm know whether it is arriving at `cur` for the first or the second time?
<details><summary>Answer</summary>It finds `cur`'s inorder predecessor (rightmost node of the left subtree). If the predecessor's right pointer is null, it's the first arrival: set the thread and go left. If it already points to `cur`, the left subtree is done: remove the thread, visit `cur`, go right.</details>

4. Why do preorder + inorder determine a tree with distinct values, while preorder + postorder may not?
<details><summary>Answer</summary>Preorder names the root, and inorder splits the remaining values into left and right, recursively. With preorder + postorder, a node with a single child can't say which side the child is on: preorder [1,2] and postorder [2,1] fit both "2 is the left child" and "2 is the right child".</details>

5. With height counted in nodes (empty = 0, leaf = 1), what is the length in edges of the longest path turning at a node, and why does 543 need a separate `best` variable?
<details><summary>Answer</summary>height(left) + height(right). The parent needs the height to compute its own values, but the answer is a path length, so the recursion returns the height and records the path length separately (or returns both as a pair).</details>

6. State what `lowestCommonAncestor(root, p, q)` returns for a subtree that contains exactly one of p and q, and for one that contains neither.
<details><summary>Answer</summary>Exactly one: that node (p or q). Neither: nullptr. Both: their LCA. A node receiving non-null from both children is therefore the LCA.</details>

7. Draw a tree that passes "every node is larger than its left child and smaller than its right child" but is not a BST. What goes wrong with `int` bounds starting at INT_MIN and INT_MAX?
<details><summary>Answer</summary>Root 5, left 4, right 6, and 6's left child 3: every parent-child pair is in order, but 3 is in 5's right subtree. With `int` bounds and strict comparisons, a node whose value is INT_MIN or INT_MAX fails `low < val < high` even though the tree is valid; use `long long` bounds, pointer bounds, or an inorder check.</details>

8. In BST deletion with two children, why the in-order successor, and why can't deleting the successor hit the two-children case again?
<details><summary>Answer</summary>The successor is the next larger key, so it's larger than everything in the left subtree and smaller than everything else in the right subtree: it can take the deleted node's place. It's the leftmost node of the right subtree, so it has no left child, and removing it is case 1 or 2.</details>

9. How do you find the k-th smallest key using subtree sizes, and what must insert and delete do to keep that working?
<details><summary>Answer</summary>At each node let s = size(left). If k ≤ s go left; if k = s + 1 return the node; else k -= s + 1 and go right. O(h) per query. Insert adds 1 to the size of every node on its path, delete subtracts 1 along its path, and a rotation recomputes the sizes of the rotated nodes from their children.</details>

10. When is `std::set` the right tool, and when isn't it?
<details><summary>Answer</summary>Right: ordered unique keys with insert / erase / find / lower_bound / upper_bound, minimum and maximum, predecessor and successor, all O(log n). Not right: rank or k-th queries, because `std::distance` on set iterators is O(n); use an order-statistics structure (Fenwick tree over compressed values, a size-augmented tree, or GCC's pbds).</details>
