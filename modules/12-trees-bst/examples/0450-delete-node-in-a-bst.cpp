// 450. Delete Node in a BST: https://leetcode.com/problems/delete-node-in-a-bst/
// Pattern: BST delete: the three cases; the two-child case uses the in-order successor. Module 12 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:solution]
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
// [/snippet]

void inorder_of(TreeNode* node, vector<int>& out) {
    if (!node) return;
    inorder_of(node->left, out);
    out.push_back(node->val);
    inorder_of(node->right, out);
}

int height(TreeNode* node) { return node ? 1 + max(height(node->left), height(node->right)) : 0; }

// Random BST over sorted distinct values: pick a random root, recurse on each side.
TreeNode* random_bst(const vector<int>& sorted_vals, int lo, int hi) {
    if (lo > hi) return nullptr;
    int mid = (int)t::rand_int(lo, hi);
    TreeNode* node = new TreeNode(sorted_vals[mid]);
    node->left = random_bst(sorted_vals, lo, mid - 1);
    node->right = random_bst(sorted_vals, mid + 1, hi);
    return node;
}

int main() {
    Solution sol;
    auto del = [&](const string& tree, int key) { return tree_to_string(sol.deleteNode(make_tree(tree), key)); };
    // official examples ([5,2,6,null,4,null,7] would also be accepted for the first one)
    CHECK_EQ(del("[5,3,6,2,4,null,7]", 3), "[5,4,6,2,null,null,7]");
    CHECK_EQ(del("[5,3,6,2,4,null,7]", 0), "[5,3,6,2,4,null,7]");     // key absent: unchanged
    CHECK_EQ(del("[]", 0), "[]");
    // each case on its own
    CHECK_EQ(del("[5,3,6,2,4,null,7]", 2), "[5,3,6,null,4,null,7]");  // case 1: a leaf
    CHECK_EQ(del("[5,3,6,2,4,null,7]", 6), "[5,3,7,2,4]");            // case 2: one child moves up
    CHECK_EQ(del("[5,3,6,2,4,null,7]", 5), "[6,3,7,2,4]");            // case 3 at the root
    CHECK_EQ(del("[1]", 1), "[]");                                    // the only node
    CHECK_EQ(del("[8,3,12,null,null,10,15,null,11]", 8), "[10,3,12,null,null,11,15]");  // successor has a right child

    // stress: after deleting, the in-order walk must equal the sorted values minus the key.
    // (In-order sorted <=> valid BST, so this checks the BST property and the contents at once.)
    for (int iter = 0; iter < 400; iter++) {
        set<int> distinct;
        int n = (int)t::rand_int(0, 30);
        while ((int)distinct.size() < n) distinct.insert((int)t::rand_int(-50, 50));
        vector<int> vals(distinct.begin(), distinct.end());
        TreeNode* root = random_bst(vals, 0, n - 1);
        int key = (n > 0 && t::rand_int(0, 3) > 0) ? vals[t::rand_int(0, n - 1)] : (int)t::rand_int(-55, 55);
        int height_before = height(root);
        root = sol.deleteNode(root, key);
        distinct.erase(key);
        vector<int> got;
        inorder_of(root, got);
        CHECK_EQ(got, vector<int>(distinct.begin(), distinct.end()));
        CHECK(height(root) <= height_before);                       // deleting never makes it taller
    }
    return t::summary("0450-delete-node-in-a-bst");
}
