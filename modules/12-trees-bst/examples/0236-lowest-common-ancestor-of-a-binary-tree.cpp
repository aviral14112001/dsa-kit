// 236. Lowest Common Ancestor of a Binary Tree: https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-tree/
// Pattern: LCA by return values: whichever side returns non-null; both sides -> this node. Module 12 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:solution]
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
// [/snippet]

// Brute force: parent pointers, mark every ancestor of p (p included), climb from q to the first mark.
TreeNode* brute(TreeNode* root, TreeNode* p, TreeNode* q) {
    unordered_map<TreeNode*, TreeNode*> parent{{root, nullptr}};
    vector<TreeNode*> todo{root};
    while (!todo.empty()) {
        TreeNode* v = todo.back();
        todo.pop_back();
        for (TreeNode* c : {v->left, v->right})
            if (c) parent[c] = v, todo.push_back(c);
    }
    unordered_set<TreeNode*> ancestors_of_p;
    for (TreeNode* v = p; v; v = parent[v]) ancestors_of_p.insert(v);
    TreeNode* v = q;
    while (!ancestors_of_p.count(v)) v = parent[v];
    return v;
}

// Random tree with distinct values; returns every node too, so tests can pick p and q.
TreeNode* random_tree(int n, vector<TreeNode*>& nodes) {
    nodes = {new TreeNode(0)};
    while ((int)nodes.size() < n) {
        TreeNode* parent = nodes[t::rand_int(0, (long long)nodes.size() - 1)];
        TreeNode*& slot = t::rand_int(0, 1) ? parent->left : parent->right;
        if (!slot) {
            slot = new TreeNode((int)nodes.size());
            nodes.push_back(slot);
        }
    }
    return nodes[0];
}

int main() {
    Solution sol;
    // official examples
    TreeNode* root = make_tree("[3,5,1,6,2,0,8,null,null,7,4]");
    auto lca = [&](TreeNode* r, int p, int q) {
        return sol.lowestCommonAncestor(r, find_node(r, p), find_node(r, q))->val;
    };
    CHECK_EQ(lca(root, 5, 1), 3);
    CHECK_EQ(lca(root, 5, 4), 5);          // a node is its own ancestor: 5 is above 4
    TreeNode* small = make_tree("[1,2]");
    CHECK_EQ(lca(small, 1, 2), 1);
    // edge cases: deep in one subtree, order of p and q doesn't matter, p == q
    CHECK_EQ(lca(root, 7, 4), 2);
    CHECK_EQ(lca(root, 4, 7), 2);
    CHECK_EQ(lca(root, 6, 4), 5);
    CHECK_EQ(lca(root, 7, 8), 3);
    CHECK_EQ(lca(root, 0, 0), 0);
    // stress against the brute force
    for (int iter = 0; iter < 300; iter++) {
        vector<TreeNode*> nodes;
        TreeNode* r = random_tree((int)t::rand_int(2, 60), nodes);
        TreeNode* p = nodes[t::rand_int(0, (long long)nodes.size() - 1)];
        TreeNode* q = nodes[t::rand_int(0, (long long)nodes.size() - 1)];
        CHECK_EQ(sol.lowestCommonAncestor(r, p, q), brute(r, p, q));
    }
    return t::summary("0236-lowest-common-ancestor-of-a-binary-tree");
}
