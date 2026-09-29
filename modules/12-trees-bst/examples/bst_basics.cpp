// BST template: let the ordering discard whole subtrees. A range query visits only the nodes in
// the range plus two root-to-leaf boundary paths. Module 12 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:range_sum]
// Sum of the keys in [lo, hi]. O(h + m) for m keys in the range: the BST invariant rules out
// a whole subtree at every node that falls outside the range.
long long range_sum(TreeNode* node, int lo, int hi) {
    if (!node) return 0;
    if (node->val < lo) return range_sum(node->right, lo, hi);  // node and its left subtree: all too small
    if (node->val > hi) return range_sum(node->left, lo, hi);   // node and its right subtree: all too big
    return node->val + range_sum(node->left, lo, hi) + range_sum(node->right, lo, hi);
}
// [/snippet]

// Same walk, counting the nodes it touches: to check the O(h + m) claim, not to show in the notes.
long long touched = 0;
long long range_sum_counted(TreeNode* node, int lo, int hi) {
    if (!node) return 0;
    touched++;
    if (node->val < lo) return range_sum_counted(node->right, lo, hi);
    if (node->val > hi) return range_sum_counted(node->left, lo, hi);
    return node->val + range_sum_counted(node->left, lo, hi) + range_sum_counted(node->right, lo, hi);
}

// Random BST over sorted distinct values: pick a random root, recurse on each side. Every shape,
// from balanced to a chain, can come out, and the result is a valid BST by construction.
TreeNode* random_bst(const vector<int>& sorted_vals, int lo, int hi) {
    if (lo > hi) return nullptr;
    int mid = (int)t::rand_int(lo, hi);
    TreeNode* node = new TreeNode(sorted_vals[mid]);
    node->left = random_bst(sorted_vals, lo, mid - 1);
    node->right = random_bst(sorted_vals, mid + 1, hi);
    return node;
}

// Perfectly balanced BST over sorted_vals[lo..hi]: the middle element is the root.
TreeNode* balanced_bst(const vector<int>& sorted_vals, int lo, int hi) {
    if (lo > hi) return nullptr;
    int mid = lo + (hi - lo) / 2;
    return new TreeNode(sorted_vals[mid], balanced_bst(sorted_vals, lo, mid - 1), balanced_bst(sorted_vals, mid + 1, hi));
}

int height(TreeNode* node) { return node ? 1 + max(height(node->left), height(node->right)) : 0; }

int main() {
    /*        8
            /   \
           3     10
          / \      \
         1   6      14      */
    TreeNode* root = make_tree("[8,3,10,1,6,null,14]");
    CHECK_EQ(range_sum(root, 4, 10), 24LL);            // 6 + 8 + 10
    CHECK_EQ(range_sum(root, 1, 14), 42LL);            // everything
    CHECK_EQ(range_sum(root, 7, 7), 0LL);              // an empty range between keys
    CHECK_EQ(range_sum(root, 15, 20), 0LL);
    CHECK_EQ(range_sum(nullptr, 0, 5), 0LL);
    CHECK_EQ(range_sum(make_tree("[2147483647]"), INT_MAX, INT_MAX), (long long)INT_MAX);

    // pruning: on a balanced BST of 2^15 - 1 keys, a narrow range touches O(h + m) nodes, not n
    vector<int> keys((1 << 15) - 1);
    iota(keys.begin(), keys.end(), 0);
    TreeNode* big = balanced_bst(keys, 0, (int)keys.size() - 1);
    touched = 0;
    CHECK_EQ(range_sum_counted(big, 1000, 1009), 10045LL);   // 1000 + ... + 1009
    CHECK(touched <= 2 * height(big) + 10);

    // stress against the brute force: sum every key in range
    for (int iter = 0; iter < 300; iter++) {
        set<int> distinct;
        int n = (int)t::rand_int(0, 40);
        while ((int)distinct.size() < n) distinct.insert((int)t::rand_int(-100, 100));
        vector<int> vals(distinct.begin(), distinct.end());
        TreeNode* r = random_bst(vals, 0, n - 1);
        for (int probe = 0; probe < 10; probe++) {
            int lo = (int)t::rand_int(-110, 110), hi = (int)t::rand_int(lo, 110);
            long long expected = 0;
            for (int v : vals)
                if (lo <= v && v <= hi) expected += v;
            CHECK_EQ(range_sum(r, lo, hi), expected);
        }
    }
    return t::summary("bst_basics");
}
