// Module 08 section 1: the dummy-head technique and the pointer-to-pointer trick, shown on one operation
// (insert into a sorted list) written three ways. All three must build the same list.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:no_dummy]
// Without a dummy: inserting before the head is a special case, because it changes `head`
// itself instead of some node's `next`.
ListNode* insert_sorted_no_dummy(ListNode* head, int x) {
    if (!head || x <= head->val) return new ListNode(x, head);  // the special case
    ListNode* prev = head;
    while (prev->next && prev->next->val < x) prev = prev->next;
    prev->next = new ListNode(x, prev->next);
    return head;
}
// [/snippet]

// [snippet:dummy]
// With a dummy node in front of the head, EVERY insertion is "link a node after prev".
ListNode* insert_sorted(ListNode* head, int x) {
    ListNode dummy(0, head);  // lives on the stack: no new, nothing to free
    ListNode* prev = &dummy;
    while (prev->next && prev->next->val < x) prev = prev->next;
    prev->next = new ListNode(x, prev->next);
    return dummy.next;        // not `head`: the head may have changed
}
// [/snippet]

// [snippet:pointer_to_pointer]
// Pointer-to-pointer: `link` points at the pointer that will change: &head at first, then
// &node->next. Both are just "the arrow pointing at the current node", so there's no special case.
ListNode* insert_sorted_pp(ListNode* head, int x) {
    ListNode** link = &head;
    while (*link && (*link)->val < x) link = &(*link)->next;
    *link = new ListNode(x, *link);  // redirect that arrow to the new node
    return head;
}
// [/snippet]

int main() {
    // hand-checked: front, middle, back, duplicate, empty list
    CHECK_EQ(to_vector(insert_sorted(make_list({2, 4}), 1)), vector<int>{1, 2, 4});
    CHECK_EQ(to_vector(insert_sorted(make_list({2, 4}), 3)), vector<int>{2, 3, 4});
    CHECK_EQ(to_vector(insert_sorted(make_list({2, 4}), 5)), vector<int>{2, 4, 5});
    CHECK_EQ(to_vector(insert_sorted(make_list({2, 4}), 2)), vector<int>{2, 2, 4});
    CHECK_EQ(to_vector(insert_sorted(nullptr, 9)), vector<int>{9});
    CHECK_EQ(to_vector(insert_sorted_pp(nullptr, 9)), vector<int>{9});
    CHECK_EQ(to_vector(insert_sorted_pp(make_list({2, 4}), 1)), vector<int>{1, 2, 4});
    CHECK_EQ(to_vector(insert_sorted_no_dummy(make_list({2, 4}), 1)), vector<int>{1, 2, 4});

    // An equal value goes BEFORE the existing equal nodes in all three versions: check it by address.
    ListNode* old = make_list({5});
    ListNode* head = insert_sorted(old, 5);
    CHECK(head != old && head->next == old);
    head = insert_sorted_pp(old, 5);
    CHECK(head != old && head->next == old);

    // stress: build lists by repeated insertion; all three must match a sorted vector
    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(0, 15);
        ListNode *a = nullptr, *b = nullptr, *c = nullptr;
        vector<int> expected;
        for (int i = 0; i < n; i++) {
            int x = (int)t::rand_int(-5, 5);
            a = insert_sorted_no_dummy(a, x);
            b = insert_sorted(b, x);
            c = insert_sorted_pp(c, x);
            expected.insert(lower_bound(expected.begin(), expected.end(), x), x);
        }
        CHECK_EQ(to_vector(a), expected);
        CHECK_EQ(to_vector(b), expected);
        CHECK_EQ(to_vector(c), expected);
    }
    return t::summary("list_basics");
}
