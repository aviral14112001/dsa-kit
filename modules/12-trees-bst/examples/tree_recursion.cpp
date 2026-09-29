// Tree recursion templates: design the return value (bottom-up, post-order combine) vs carry state
// down and accumulate into a reference (top-down). Module 12 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:bottom_up]
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
// [/snippet]

// [snippet:top_down]
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
// [/snippet]

// Random tree: each new node takes a random free child slot of a random existing node.
TreeNode* random_tree(int n, int lo, int hi) {
    if (n == 0) return nullptr;
    vector<TreeNode*> nodes{new TreeNode((int)t::rand_int(lo, hi))};
    while ((int)nodes.size() < n) {
        TreeNode* parent = nodes[t::rand_int(0, (long long)nodes.size() - 1)];
        TreeNode*& slot = t::rand_int(0, 1) ? parent->left : parent->right;
        if (!slot) {
            slot = new TreeNode((int)t::rand_int(lo, hi));
            nodes.push_back(slot);
        }
    }
    return nodes[0];
}

// Brute force for both: list the nodes with a parent map (BFS), then answer each question directly.
struct Flat {
    vector<TreeNode*> nodes;                          // in BFS order
    unordered_map<TreeNode*, TreeNode*> parent;
    int levels = 0;
    explicit Flat(TreeNode* root) {
        vector<TreeNode*> level;
        if (root) level.push_back(root), parent[root] = nullptr;
        while (!level.empty()) {
            levels++;
            vector<TreeNode*> next;
            for (TreeNode* v : level) {
                nodes.push_back(v);
                for (TreeNode* c : {v->left, v->right})
                    if (c) parent[c] = v, next.push_back(c);
            }
            level = next;
        }
    }
};

int main() {
    TreeNode* root = make_tree("[5,4,8,11,null,13,4,7,2,null,null,null,1]");
    Summary s = summarize(root);
    CHECK_EQ(s.size, 9);
    CHECK_EQ(s.height, 4);
    CHECK_EQ(s.sum, 55LL);
    CHECK_EQ(s.leaves, 4);                            // 7, 2, 13, 1
    CHECK_EQ(summarize(nullptr).size, 0);
    CHECK_EQ(summarize(nullptr).height, 0);
    CHECK_EQ(summarize(make_tree("[7]")).height, 1);
    CHECK_EQ(summarize(make_tree("[7]")).leaves, 1);
    CHECK_EQ(summarize(make_tree("[1,2]")).leaves, 1);   // node 1 has a child, so only 2 is a leaf
    CHECK_EQ(summarize(make_tree("[1000000000,1000000000,1000000000]")).sum, 3000000000LL);

    long long best = LLONG_MIN;
    best_root_to_leaf(root, 0, best);                 // leaves 7, 2, 13, 1: best path 5+4+11+7 = 27
    CHECK_EQ(best, 27LL);
    best = LLONG_MIN;
    best_root_to_leaf(make_tree("[1,2]"), 0, best);   // node 1 has one child, so it's not a leaf
    CHECK_EQ(best, 3LL);
    best = LLONG_MIN;
    best_root_to_leaf(make_tree("[-5,-3,-8]"), 0, best);
    CHECK_EQ(best, -8LL);

    // stress against the brute force
    for (int iter = 0; iter < 300; iter++) {
        TreeNode* r = random_tree((int)t::rand_int(1, 40), -50, 50);
        Flat flat(r);
        Summary got = summarize(r);
        CHECK_EQ(got.size, (int)flat.nodes.size());
        CHECK_EQ(got.height, flat.levels);
        long long sum = 0;
        int leaves = 0;
        long long best_leaf_path = LLONG_MIN;
        for (TreeNode* v : flat.nodes) {
            sum += v->val;
            if (!v->left && !v->right) {              // walk each leaf up to the root
                leaves++;
                long long path = 0;
                for (TreeNode* u = v; u; u = flat.parent[u]) path += u->val;
                best_leaf_path = max(best_leaf_path, path);
            }
        }
        CHECK_EQ(got.sum, sum);
        CHECK_EQ(got.leaves, leaves);
        long long top_down = LLONG_MIN;
        best_root_to_leaf(r, 0, top_down);
        CHECK_EQ(top_down, best_leaf_path);
    }
    return t::summary("tree_recursion");
}
