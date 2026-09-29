// Tree traversal templates: the three depth-first orders (recursive), postorder with an explicit
// stack (simulating the recursion), and level order with the queue-size snapshot. Module 12 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:recursive]
// The three orders differ only in WHERE a node is recorded relative to its two recursive calls.
void dfs(TreeNode* node, vector<int>& pre, vector<int>& in, vector<int>& post) {
    if (!node) return;                  // the empty tree: the base case for every tree recursion
    pre.push_back(node->val);           // preorder: before the children
    dfs(node->left, pre, in, post);
    in.push_back(node->val);            // inorder: between them
    dfs(node->right, pre, in, post);
    post.push_back(node->val);          // postorder: after both
}
// [/snippet]

// [snippet:postorder_iterative]
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
// [/snippet]

// [snippet:level_order]
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
// [/snippet]

// Random tree: each new node takes a random free child slot of a random existing node.
TreeNode* random_tree(int n) {
    if (n == 0) return nullptr;
    vector<TreeNode*> nodes{new TreeNode((int)t::rand_int(0, 99))};
    while ((int)nodes.size() < n) {
        TreeNode* parent = nodes[t::rand_int(0, (long long)nodes.size() - 1)];
        TreeNode*& slot = t::rand_int(0, 1) ? parent->left : parent->right;
        if (!slot) {
            slot = new TreeNode((int)t::rand_int(0, 99));
            nodes.push_back(slot);
        }
    }
    return nodes[0];
}

// Brute force for nodes_per_level: record every node's depth with a plain DFS, then count.
void count_depths(TreeNode* node, int depth, vector<int>& counts) {
    if (!node) return;
    if (depth == (int)counts.size()) counts.push_back(0);
    counts[depth]++;
    count_depths(node->left, depth + 1, counts);
    count_depths(node->right, depth + 1, counts);
}

int main() {
    /*        1
            /   \
           2     3
          / \     \
         4   5     6        */
    TreeNode* root = make_tree("[1,2,3,4,5,null,6]");
    vector<int> pre, in, post;
    dfs(root, pre, in, post);
    CHECK_EQ(pre, vector<int>{1, 2, 4, 5, 3, 6});
    CHECK_EQ(in, vector<int>{4, 2, 5, 1, 3, 6});
    CHECK_EQ(post, vector<int>{4, 5, 2, 6, 3, 1});
    CHECK_EQ(postorder_iterative(root), post);
    CHECK_EQ(postorder_iterative(nullptr), vector<int>{});
    CHECK_EQ(postorder_iterative(make_tree("[1,2,null,3]")), vector<int>{3, 2, 1});      // left chain
    CHECK_EQ(postorder_iterative(make_tree("[1,null,2,null,3]")), vector<int>{3, 2, 1}); // right chain

    CHECK_EQ(nodes_per_level(make_tree("[3,9,20,null,null,15,7]")), vector<int>{1, 2, 2});
    CHECK_EQ(nodes_per_level(root), vector<int>{1, 2, 3});
    CHECK_EQ(nodes_per_level(nullptr), vector<int>{});
    CHECK_EQ(nodes_per_level(make_tree("[1,null,2,null,3]")), vector<int>{1, 1, 1});

    // stress: the iterative postorder matches the recursive one; level counts match a DFS count
    for (int iter = 0; iter < 300; iter++) {
        TreeNode* r = random_tree((int)t::rand_int(0, 40));
        vector<int> p, i, q;
        dfs(r, p, i, q);
        CHECK_EQ(postorder_iterative(r), q);
        vector<int> counts;
        count_depths(r, 0, counts);
        CHECK_EQ(nodes_per_level(r), counts);
    }
    return t::summary("traversals");
}
