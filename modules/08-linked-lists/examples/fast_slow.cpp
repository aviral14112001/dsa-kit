// Module 08 section 2: fast & slow pointers to find the first middle node (the one you split at), and a
// fixed gap for the k-th node from the end. (876, in your list, asks for the SECOND middle: that
// one is yours to write.)
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:middle_first]
// Even lengths have two middles: for 1 -> 2 -> 3 -> 4 this returns 2, the FIRST middle.
// Use it to SPLIT a list: the first half ends at the returned node, so 2 nodes split 1 + 1.
// (Splitting after the second middle turns 2 nodes into 2 + 0, and merge sort never shrinks.)
ListNode* middle_first(ListNode* head) {
    if (!head) return nullptr;
    ListNode *slow = head, *fast = head;
    while (fast->next && fast->next->next) {  // fast moves 2 per step, slow moves 1
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}
// [/snippet]

// [snippet:kth_from_end]
// The k-th node from the end (k = 1 is the last node), or nullptr if there are fewer than k.
// Open a gap of k nodes between lead and trail, then move both: when lead falls off the end,
// trail is exactly k nodes behind it.
ListNode* kth_from_end(ListNode* head, int k) {
    ListNode* lead = head;
    for (int i = 0; i < k; i++) {
        if (!lead) return nullptr;       // the list is shorter than k
        lead = lead->next;
    }
    ListNode* trail = head;
    while (lead) {
        lead = lead->next;
        trail = trail->next;
    }
    return trail;
}
// [/snippet]

int main() {
    // first middle: lengths 1..5 by hand
    CHECK_EQ(middle_first(make_list({1}))->val, 1);
    CHECK_EQ(middle_first(make_list({1, 2}))->val, 1);
    CHECK_EQ(middle_first(make_list({1, 2, 3}))->val, 2);
    CHECK_EQ(middle_first(make_list({1, 2, 3, 4}))->val, 2);
    CHECK_EQ(middle_first(make_list({1, 2, 3, 4, 5}))->val, 3);
    CHECK(middle_first(nullptr) == nullptr);

    // k-th from the end
    ListNode* l = make_list({1, 2, 3, 4, 5});
    CHECK_EQ(kth_from_end(l, 1)->val, 5);
    CHECK_EQ(kth_from_end(l, 2)->val, 4);
    CHECK_EQ(kth_from_end(l, 5)->val, 1);        // k = length: the head itself
    CHECK(kth_from_end(l, 6) == nullptr);        // too short
    CHECK(kth_from_end(nullptr, 1) == nullptr);

    // stress vs index arithmetic on a vector (values are the indices, so ->val is the position)
    for (int iter = 0; iter < 300; iter++) {
        int n = (int)t::rand_int(0, 30);
        vector<int> v(n);
        iota(v.begin(), v.end(), 0);
        ListNode* head = make_list(v);
        if (n > 0) CHECK_EQ(middle_first(head)->val, (n - 1) / 2);
        int k = (int)t::rand_int(1, n + 2);
        ListNode* got = kth_from_end(head, k);
        if (k <= n) CHECK(got && got->val == n - k);
        else CHECK(got == nullptr);
    }
    return t::summary("fast_slow");
}
