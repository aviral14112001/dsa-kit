// 337. House Robber III: https://leetcode.com/problems/house-robber-iii/
// Pattern: tree DP, post-order returning a tuple {rob this node, skip this node}. Module 16 section 4.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:solution]
class Solution {
    // For the subtree rooted at node: {best if node is robbed, best if node is skipped}.
    pair<int, int> dfs(TreeNode* node) {
        if (!node) return {0, 0};
        auto [robLeft, skipLeft] = dfs(node->left);       // post-order: children first
        auto [robRight, skipRight] = dfs(node->right);
        int robThis = node->val + skipLeft + skipRight;   // robbing node rules out both children
        int skipThis = max(robLeft, skipLeft) + max(robRight, skipRight);   // children choose freely
        return {robThis, skipThis};
    }

public:
    int rob(TreeNode* root) {
        auto [robRoot, skipRoot] = dfs(root);
        return max(robRoot, skipRoot);
    }
};
// [/snippet]

// Random binary tree with n nodes: each new node hangs off a random free child slot.
TreeNode* randomTree(int n, vector<TreeNode*>& nodes, vector<int>& parent) {
    nodes.clear();
    parent.clear();
    nodes.push_back(new TreeNode((int)t::rand_int(0, 20)));
    parent.push_back(-1);
    while ((int)nodes.size() < n) {
        int p = (int)t::rand_int(0, (int)nodes.size() - 1);
        TreeNode*& slot = t::rand_int(0, 1) ? nodes[p]->left : nodes[p]->right;
        if (slot) continue;                                // taken: pick again
        slot = new TreeNode((int)t::rand_int(0, 20));
        nodes.push_back(slot);
        parent.push_back(p);
    }
    return nodes[0];
}

// Brute force: every subset of nodes in which no node is chosen together with its parent. n <= ~12.
int brute(const vector<TreeNode*>& nodes, const vector<int>& parent) {
    int n = nodes.size(), best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        int total = 0;
        bool ok = true;
        for (int i = 0; i < n && ok; i++) {
            if (!(mask >> i & 1)) continue;
            if (parent[i] != -1 && (mask >> parent[i] & 1)) ok = false;
            total += nodes[i]->val;
        }
        if (ok) best = max(best, total);
    }
    return best;
}

int main() {
    Solution sol;
    CHECK_EQ(sol.rob(make_tree("[3,2,3,null,3,null,1]")), 7);   // official examples
    CHECK_EQ(sol.rob(make_tree("[3,4,5,1,3,null,1]")), 9);

    CHECK_EQ(sol.rob(make_tree("[5]")), 5);                     // single node
    CHECK_EQ(sol.rob(make_tree("[0,0,0]")), 0);
    CHECK_EQ(sol.rob(make_tree("[4,1,null,2,null,3]")), 7);     // a chain is 198 on 4,1,2,3
    CHECK_EQ(sol.rob(make_tree("[1,10,1,1,1,10,10]")), 30);     // one child + the other child's children
                                                                // beats "every other level" (1 + 1+1+10+10)
    TreeNode* chain = nullptr;                                  // 10^4 nodes deep: recursion depth check
    for (int i = 0; i < 10000; i++) chain = new TreeNode(1, chain, nullptr);
    CHECK_EQ(sol.rob(chain), 5000);

    vector<TreeNode*> nodes;
    vector<int> parent;
    for (int iter = 0; iter < 300; iter++) {                    // stress test vs every valid subset
        TreeNode* root = randomTree((int)t::rand_int(1, 12), nodes, parent);
        CHECK_EQ(sol.rob(root), brute(nodes, parent));
    }
    return t::summary("0337-house-robber-iii");
}
