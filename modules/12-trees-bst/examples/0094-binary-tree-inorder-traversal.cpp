// 94. Binary Tree Inorder Traversal: https://leetcode.com/problems/binary-tree-inorder-traversal/
// Pattern: inorder three ways: recursive, explicit stack, Morris threading (O(1) space). Module 12 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

namespace recursive {
// [snippet:recursive]
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
// [/snippet]
}  // namespace recursive

namespace iterative {
// [snippet:iterative]
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
// [/snippet]
}  // namespace iterative

namespace morris {
// [snippet:morris]
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
// [/snippet]
}  // namespace morris

// Random tree: each new node takes a random free child slot of a random existing node.
TreeNode* random_tree(int n) {
    if (n == 0) return nullptr;
    vector<TreeNode*> nodes{new TreeNode((int)t::rand_int(-100, 100))};
    while ((int)nodes.size() < n) {
        TreeNode* parent = nodes[t::rand_int(0, (long long)nodes.size() - 1)];
        TreeNode*& slot = t::rand_int(0, 1) ? parent->left : parent->right;
        if (!slot) {
            slot = new TreeNode((int)t::rand_int(-100, 100));
            nodes.push_back(slot);
        }
    }
    return nodes[0];
}

int main() {
    recursive::Solution rec;
    iterative::Solution it;
    morris::Solution mor;
    auto all_three = [&](const string& tree, const vector<int>& expected) {
        CHECK_EQ(rec.inorderTraversal(make_tree(tree)), expected);
        CHECK_EQ(it.inorderTraversal(make_tree(tree)), expected);
        TreeNode* root = make_tree(tree);
        CHECK_EQ(mor.inorderTraversal(root), expected);
        CHECK_EQ(tree_to_string(root), tree_to_string(make_tree(tree)));   // Morris restored every pointer
    };
    // official examples
    all_three("[1,null,2,3]", {1, 3, 2});
    all_three("[1,2,3,4,5,null,8,null,null,6,7,9]", {4, 2, 6, 5, 7, 1, 3, 9, 8});
    all_three("[]", {});
    all_three("[1]", {1});
    // edge cases: left chain, right chain, full tree
    all_three("[3,2,null,1]", {1, 2, 3});
    all_three("[1,null,2,null,3]", {1, 2, 3});
    all_three("[4,2,6,1,3,5,7]", {1, 2, 3, 4, 5, 6, 7});

    // a 2000-node left chain (root 1, its left child 2, ...): the iterative and Morris versions
    // don't recurse at all, so depth never matters to them
    TreeNode* chain = nullptr;
    for (int v = 2000; v >= 1; v--) {
        TreeNode* node = new TreeNode(v);
        node->left = chain;
        chain = node;
    }
    vector<int> deepest_first(2000);
    iota(deepest_first.rbegin(), deepest_first.rend(), 1);   // 2000, 1999, ..., 1
    CHECK_EQ(it.inorderTraversal(chain), deepest_first);
    CHECK_EQ(mor.inorderTraversal(chain), deepest_first);

    // stress: the three versions agree, and Morris leaves the tree exactly as it found it
    for (int iter = 0; iter < 300; iter++) {
        TreeNode* root = random_tree((int)t::rand_int(0, 40));
        string before = tree_to_string(root);
        vector<int> expected = rec.inorderTraversal(root);
        CHECK_EQ(it.inorderTraversal(root), expected);
        CHECK_EQ(mor.inorderTraversal(root), expected);
        CHECK_EQ(tree_to_string(root), before);
    }
    return t::summary("0094-binary-tree-inorder-traversal");
}
