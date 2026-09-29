// 105. Construct Binary Tree from Preorder and Inorder Traversal: https://leetcode.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/
// Pattern: build from traversals: the preorder root splits the inorder range (hash map for O(1) lookups). Module 12 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:solution]
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
// [/snippet]

void preorder_of(TreeNode* node, vector<int>& out) {
    if (!node) return;
    out.push_back(node->val);
    preorder_of(node->left, out);
    preorder_of(node->right, out);
}

void inorder_of(TreeNode* node, vector<int>& out) {
    if (!node) return;
    inorder_of(node->left, out);
    out.push_back(node->val);
    inorder_of(node->right, out);
}

// Random tree with distinct values (as the problem guarantees): shuffled values, random shape.
TreeNode* random_tree(int n) {
    vector<int> vals(6001);
    iota(vals.begin(), vals.end(), -3000);
    shuffle(vals.begin(), vals.end(), t::rng());
    if (n == 0) return nullptr;
    vector<TreeNode*> nodes{new TreeNode(vals[0])};
    while ((int)nodes.size() < n) {
        TreeNode* parent = nodes[t::rand_int(0, (long long)nodes.size() - 1)];
        TreeNode*& slot = t::rand_int(0, 1) ? parent->left : parent->right;
        if (!slot) {
            slot = new TreeNode(vals[nodes.size()]);
            nodes.push_back(slot);
        }
    }
    return nodes[0];
}

int main() {
    Solution sol;
    auto build = [&](vector<int> pre, vector<int> in) { return tree_to_string(sol.buildTree(pre, in)); };
    // official examples
    CHECK_EQ(build({3, 9, 20, 15, 7}, {9, 3, 15, 20, 7}), "[3,9,20,null,null,15,7]");
    CHECK_EQ(build({-1}, {-1}), "[-1]");
    // edge cases: left chain, right chain, empty input
    CHECK_EQ(build({3, 2, 1}, {1, 2, 3}), "[3,2,null,1]");
    CHECK_EQ(build({1, 2, 3}, {1, 2, 3}), "[1,null,2,null,3]");
    CHECK_EQ(build({}, {}), "[]");

    // a 3000-node chain (the maximum size): recursion depth 3000 is fine
    vector<int> up(3000);
    iota(up.begin(), up.end(), 0);
    TreeNode* chain = sol.buildTree(up, up);            // every node is its parent's right child
    int depth = 0;
    for (TreeNode* node = chain; node; node = node->right) depth++;
    CHECK_EQ(depth, 3000);

    // stress, as a round trip: tree -> (preorder, inorder) -> buildTree -> the same tree.
    // No brute force needed: a correct builder must reproduce the tree exactly.
    for (int iter = 0; iter < 300; iter++) {
        TreeNode* original = random_tree((int)t::rand_int(0, 50));
        vector<int> pre, in;
        preorder_of(original, pre);
        inorder_of(original, in);
        CHECK_EQ(tree_to_string(sol.buildTree(pre, in)), tree_to_string(original));
    }
    return t::summary("0105-construct-binary-tree-from-preorder-and-inorder");
}
