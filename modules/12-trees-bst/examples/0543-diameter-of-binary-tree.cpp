// 543. Diameter of Binary Tree: https://leetcode.com/problems/diameter-of-binary-tree/
// Pattern: return height, update a global: the longest path turns at some node, left + right. Module 12 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:solution]
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
// [/snippet]

// Brute force: the distance between two nodes follows from their root-to-node paths (both
// lengths minus twice the shared prefix). Try every pair. O(n^2 h), independent of the height idea.
int brute(TreeNode* root) {
    vector<vector<TreeNode*>> paths;        // the root-to-node path of every node
    vector<TreeNode*> path;
    function<void(TreeNode*)> collect = [&](TreeNode* v) {
        if (!v) return;
        path.push_back(v);
        paths.push_back(path);
        collect(v->left);
        collect(v->right);
        path.pop_back();
    };
    collect(root);
    int best = 0;
    for (const auto& a : paths)
        for (const auto& b : paths) {
            size_t common = 0;
            while (common < a.size() && common < b.size() && a[common] == b[common]) common++;
            best = max(best, (int)(a.size() + b.size() - 2 * common));
        }
    return best;
}

// Random tree: each new node takes a random free child slot of a random existing node.
TreeNode* random_tree(int n) {
    if (n == 0) return nullptr;
    vector<TreeNode*> nodes{new TreeNode(0)};
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
    CHECK_EQ(sol.diameterOfBinaryTree(make_tree("[1,2,3,4,5]")), 3);   // 4-2-1-3 or 5-2-1-3
    CHECK_EQ(sol.diameterOfBinaryTree(make_tree("[1,2]")), 1);
    // edge cases
    CHECK_EQ(sol.diameterOfBinaryTree(make_tree("[1]")), 0);           // one node: a path of 0 edges
    CHECK_EQ(sol.diameterOfBinaryTree(make_tree("[1,2,null,3,null,4]")), 3);   // a chain
    // the longest path doesn't pass through the root: it turns at node 2
    CHECK_EQ(sol.diameterOfBinaryTree(make_tree("[1,2,null,3,4,5,null,null,6,7,null,null,8]")), 6);
    // stress against the brute force
    for (int iter = 0; iter < 300; iter++) {
        TreeNode* root = random_tree((int)t::rand_int(1, 40));
        CHECK_EQ(sol.diameterOfBinaryTree(root), brute(root));
    }
    return t::summary("0543-diameter-of-binary-tree");
}
