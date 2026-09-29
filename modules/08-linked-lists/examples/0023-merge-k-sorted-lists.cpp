// 23. Merge k Sorted Lists: https://leetcode.com/problems/merge-k-sorted-lists/
// Pattern: k-way merge, with a min-heap of list heads or by merging pairs (divide & conquer).
// Both are O(N log k) for N nodes in total. Module 08 section 3.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// mergeTwo(a, b): merge two sorted lists by relinking their nodes. It is problem 21 in your list,
// so it lives at the very bottom of this file: write your own before you scroll down there.
ListNode* mergeTwo(ListNode* a, ListNode* b);

namespace heap_version {
// [snippet:heap]
class Solution {
public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        // priority_queue puts the "largest" on top, where cmp(a, b) == true means a < b.
        // Declaring a < b whenever a->val > b->val puts the SMALLEST value on top: a min-heap.
        auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
        priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> heap(cmp);
        for (ListNode* head : lists)
            if (head) heap.push(head);   // skip empty lists: cmp would dereference nullptr
        ListNode dummy;
        ListNode* tail = &dummy;
        while (!heap.empty()) {
            ListNode* node = heap.top();  // the smallest node not yet placed
            heap.pop();
            tail->next = node;
            tail = node;
            if (node->next) heap.push(node->next);  // its successor is now that list's candidate
        }
        return dummy.next;  // the last node placed had no successor, so the list ends cleanly
    }
};
// [/snippet]
}  // namespace heap_version

namespace dc_version {
// [snippet:divide_conquer]
// mergeTwo(a, b) merges two sorted lists by splicing: that's problem 21 in your list. Paste your
// own 21 above this class to submit it.
class Solution {
public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        int k = (int)lists.size();
        if (k == 0) return nullptr;
        // Round with gap g merges lists[i] and lists[i + g] into lists[i], for i = 0, 2g, 4g...
        // Every round halves the number of lists and touches each node once: log k rounds of O(N).
        for (int gap = 1; gap < k; gap *= 2)
            for (int i = 0; i + gap < k; i += 2 * gap)
                lists[i] = mergeTwo(lists[i], lists[i + gap]);
        return lists[0];
    }
};
// [/snippet]
}  // namespace dc_version

// Brute force: collect every value and sort. O(N log N) time, O(N) space; allocates new nodes.
vector<int> brute(const vector<vector<int>>& lists) {
    vector<int> all;
    for (const auto& l : lists) all.insert(all.end(), l.begin(), l.end());
    sort(all.begin(), all.end());
    return all;
}

vector<ListNode*> build(const vector<vector<int>>& lists) {
    vector<ListNode*> heads;
    for (const auto& l : lists) heads.push_back(make_list(l));
    return heads;
}

set<ListNode*> node_set(const vector<ListNode*>& heads) {
    set<ListNode*> s;
    for (ListNode* h : heads)
        for (; h; h = h->next) s.insert(h);
    return s;
}

int main() {
    heap_version::Solution heap_sol;
    dc_version::Solution dc_sol;
    auto run_both = [&](const vector<vector<int>>& lists, const vector<int>& expected) {
        vector<ListNode*> a = build(lists), b = build(lists);  // separate copies: merging relinks nodes
        CHECK_EQ(to_vector(heap_sol.mergeKLists(a)), expected);
        CHECK_EQ(to_vector(dc_sol.mergeKLists(b)), expected);
    };

    // official examples
    run_both({{1, 4, 5}, {1, 3, 4}, {2, 6}}, {1, 1, 2, 3, 4, 4, 5, 6});
    run_both({}, {});
    run_both({{}}, {});
    // edge cases: empty lists mixed in, one list, negatives and duplicates, k not a power of two
    run_both({{}, {1}, {}}, {1});
    run_both({{-3, 0, 0, 7}}, {-3, 0, 0, 7});
    run_both({{5}, {4}, {3}, {2}, {1}}, {1, 2, 3, 4, 5});
    CHECK_EQ(to_vector(mergeTwo(make_list({1, 2, 4}), make_list({1, 3, 4}))), vector<int>{1, 1, 2, 3, 4, 4});
    CHECK(mergeTwo(nullptr, nullptr) == nullptr);

    // mergeTwo is stable: on equal values the node from the first list comes first
    ListNode* first = make_list({2});
    ListNode* second = make_list({2});
    CHECK(mergeTwo(first, second) == first);

    // stress vs sort-everything; also check the output is made of exactly the input nodes
    for (int iter = 0; iter < 300; iter++) {
        int k = (int)t::rand_int(0, 9);
        vector<vector<int>> lists(k);
        for (auto& l : lists) {
            l = t::rand_vec((int)t::rand_int(0, 6), -10, 10);
            sort(l.begin(), l.end());
        }
        vector<int> expected = brute(lists);
        vector<ListNode*> a = build(lists), b = build(lists);
        set<ListNode*> nodes_a = node_set(a), nodes_b = node_set(b);
        ListNode* merged_a = heap_sol.mergeKLists(a);
        ListNode* merged_b = dc_sol.mergeKLists(b);
        CHECK_EQ(to_vector(merged_a), expected);
        CHECK_EQ(to_vector(merged_b), expected);
        CHECK(node_set({merged_a}) == nodes_a);
        CHECK(node_set({merged_b}) == nodes_b);
    }
    return t::summary("0023-merge-k-sorted-lists");
}

// ---------------------------------------------------------------------------------------------
// Spoiler for problem 21 (Merge Two Sorted Lists), used by the divide & conquer version above.
// ---------------------------------------------------------------------------------------------
// Relinks the nodes (nothing allocated). O(n + m) time, O(1) space.
ListNode* mergeTwo(ListNode* a, ListNode* b) {
    ListNode dummy;                // the merged list grows off dummy.next
    ListNode* tail = &dummy;       // last node of the merged list so far
    while (a && b) {
        if (b->val < a->val) { tail->next = b; b = b->next; }
        else { tail->next = a; a = a->next; }  // ties take from a: the merge is stable
        tail = tail->next;
    }
    tail->next = a ? a : b;        // one list ran out: attach the rest of the other in one step
    return dummy.next;
}
