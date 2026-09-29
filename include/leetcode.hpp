// LeetCode's exact ListNode / TreeNode definitions, plus helpers to build and inspect them in tests.
//
//   ListNode* head = make_list({1, 2, 3});          to_vector(head) -> {1, 2, 3}
//   ListNode* cyc  = make_cycle_list({3, 2, 0, -4}, 1);   // tail links to index 1 (LC 141/142)
//   TreeNode* root = make_tree("[3,9,20,null,null,15,7]"); // LeetCode's level-order format
//   tree_to_string(root) -> "[3,9,20,null,null,15,7]"
//   find_node(root, 15)  -> pointer to the node with value 15 (for LCA-style tests)
//
// Nodes are allocated with `new` and never freed, just like on LeetCode. That's fine for tests.
#pragma once
#include <bits/stdc++.h>

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

inline ListNode* make_list(const std::vector<int>& vals) {
    ListNode dummy;
    ListNode* tail = &dummy;
    for (int v : vals) tail = tail->next = new ListNode(v);
    return dummy.next;
}

// pos = index the tail should point back to; -1 means no cycle.
inline ListNode* make_cycle_list(const std::vector<int>& vals, int pos) {
    ListNode* head = make_list(vals);
    if (pos < 0 || !head) return head;
    ListNode *tail = head, *target = nullptr;
    for (int i = 0; tail; ++i) {
        if (i == pos) target = tail;
        if (!tail->next) break;
        tail = tail->next;
    }
    tail->next = target;
    return head;
}

// Stops after `limit` nodes so an accidental cycle can't hang a test.
inline std::vector<int> to_vector(const ListNode* head, size_t limit = 1'000'000) {
    std::vector<int> out;
    for (; head && out.size() < limit; head = head->next) out.push_back(head->val);
    return out;
}

inline TreeNode* make_tree(const std::string& s) {
    std::vector<std::optional<int>> tokens;
    std::string cur;
    for (char c : s) {
        if (c == '[' || c == ']' || c == ' ') continue;
        if (c == ',') { tokens.push_back(cur == "null" ? std::nullopt : std::optional<int>(std::stoi(cur))); cur.clear(); }
        else cur += c;
    }
    if (!cur.empty()) tokens.push_back(cur == "null" ? std::nullopt : std::optional<int>(std::stoi(cur)));
    if (tokens.empty() || !tokens[0]) return nullptr;

    TreeNode* root = new TreeNode(*tokens[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    size_t i = 1;
    while (!q.empty() && i < tokens.size()) {
        TreeNode* node = q.front(); q.pop();
        if (i < tokens.size() && tokens[i]) q.push(node->left = new TreeNode(*tokens[i]));
        ++i;
        if (i < tokens.size() && tokens[i]) q.push(node->right = new TreeNode(*tokens[i]));
        ++i;
    }
    return root;
}

inline std::string tree_to_string(const TreeNode* root) {
    std::vector<std::string> out;
    std::queue<const TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        const TreeNode* node = q.front(); q.pop();
        if (!node) { out.push_back("null"); continue; }
        out.push_back(std::to_string(node->val));
        q.push(node->left);
        q.push(node->right);
    }
    while (!out.empty() && out.back() == "null") out.pop_back();
    std::string s = "[";
    for (size_t i = 0; i < out.size(); ++i) s += (i ? "," : "") + out[i];
    return s + "]";
}

inline TreeNode* find_node(TreeNode* root, int val) {
    if (!root || root->val == val) return root;
    if (TreeNode* l = find_node(root->left, val)) return l;
    return find_node(root->right, val);
}
