// 142. Linked List Cycle II: https://leetcode.com/problems/linked-list-cycle-ii/
// Pattern: Floyd's cycle detection, then the entry-point reset. Module 08 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    ListNode *detectCycle(ListNode *head) {
        ListNode *slow = head, *fast = head;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
            if (slow == fast) {            // phase 1: they met somewhere inside the cycle
                ListNode* entry = head;    // phase 2: head and the meeting point are equally
                while (entry != slow) {    // far from the entry (mod the cycle length)
                    entry = entry->next;
                    slow = slow->next;
                }
                return entry;
            }
        }
        return nullptr;                    // fast reached the end: no cycle
    }
};
// [/snippet]

// Brute force: remember every node; the first one seen twice is the entry. O(n) extra space.
ListNode* brute(ListNode* head) {
    unordered_set<ListNode*> seen;
    for (ListNode* cur = head; cur; cur = cur->next)
        if (!seen.insert(cur).second) return cur;
    return nullptr;
}

ListNode* node_at(ListNode* head, int index) {
    while (index-- > 0) head = head->next;
    return head;
}

int main() {
    Solution sol;
    // official examples: the tail links back to index pos (-1 = no cycle)
    ListNode* ex1 = make_cycle_list({3, 2, 0, -4}, 1);
    CHECK(sol.detectCycle(ex1) == node_at(ex1, 1));
    ListNode* ex2 = make_cycle_list({1, 2}, 0);
    CHECK(sol.detectCycle(ex2) == node_at(ex2, 0));
    CHECK(sol.detectCycle(make_cycle_list({1}, -1)) == nullptr);

    // edge cases
    CHECK(sol.detectCycle(nullptr) == nullptr);
    ListNode* self = make_cycle_list({7}, 0);            // one node pointing at itself
    CHECK(sol.detectCycle(self) == self);
    ListNode* tail_loop = make_cycle_list({1, 2, 3, 4}, 3);  // the tail points at itself
    CHECK(sol.detectCycle(tail_loop) == node_at(tail_loop, 3));
    CHECK(sol.detectCycle(make_list({1, 2, 3})) == nullptr);
    // equal values are different nodes: compare pointers, never ->val
    ListNode* dup = make_cycle_list({5, 5, 5, 5}, 2);
    CHECK(sol.detectCycle(dup) == node_at(dup, 2));

    // The list must be left intact: still the same cycle afterwards.
    CHECK_EQ(to_vector(ex1, 7), vector<int>{3, 2, 0, -4, 2, 0, -4});

    // stress: random lengths and entry points (including "no cycle") vs the hash-set brute force
    for (int iter = 0; iter < 400; iter++) {
        int n = (int)t::rand_int(0, 40);
        int pos = n == 0 ? -1 : (int)t::rand_int(-1, n - 1);
        ListNode* head = make_cycle_list(t::rand_vec(n, 0, 3), pos);
        ListNode* expected = pos < 0 ? nullptr : node_at(head, pos);
        CHECK(brute(head) == expected);
        CHECK(sol.detectCycle(head) == expected);
    }
    return t::summary("0142-linked-list-cycle-ii");
}
