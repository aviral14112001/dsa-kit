// 206. Reverse Linked List: https://leetcode.com/problems/reverse-linked-list/
// Pattern: pointer reversal, iterative (prev / cur / next) and recursive. Module 08 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

namespace iterative {
// [snippet:iterative]
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode* prev = nullptr;        // head of the part already reversed
        ListNode* cur = head;            // head of the part not yet touched
        while (cur) {
            ListNode* next = cur->next;  // save it: the next line overwrites cur->next
            cur->next = prev;            // flip one arrow
            prev = cur;                  // the reversed part grows by one node
            cur = next;
        }
        return prev;                     // cur fell off the end; prev is the old tail
    }
};
// [/snippet]
}  // namespace iterative

namespace recursive {
// [snippet:recursive]
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        if (!head || !head->next) return head;        // 0 or 1 node: already reversed
        ListNode* newHead = reverseList(head->next);  // reverse everything after head
        head->next->next = head;  // head's old successor is now the tail of that part: hang head after it
        head->next = nullptr;     // head is the new tail; without this, the last two nodes form a cycle
        return newHead;
    }
};
// [/snippet]
}  // namespace recursive

vector<ListNode*> nodes_of(ListNode* head) {
    vector<ListNode*> out;
    for (; head; head = head->next) out.push_back(head);
    return out;
}

int main() {
    iterative::Solution it;
    recursive::Solution rec;

    // official examples
    CHECK_EQ(to_vector(it.reverseList(make_list({1, 2, 3, 4, 5}))), vector<int>{5, 4, 3, 2, 1});
    CHECK_EQ(to_vector(it.reverseList(make_list({1, 2}))), vector<int>{2, 1});
    CHECK(it.reverseList(nullptr) == nullptr);
    CHECK_EQ(to_vector(rec.reverseList(make_list({1, 2, 3, 4, 5}))), vector<int>{5, 4, 3, 2, 1});
    CHECK_EQ(to_vector(rec.reverseList(make_list({1, 2}))), vector<int>{2, 1});
    CHECK(rec.reverseList(nullptr) == nullptr);

    // one node, and duplicates
    CHECK_EQ(to_vector(it.reverseList(make_list({7}))), vector<int>{7});
    CHECK_EQ(to_vector(rec.reverseList(make_list({7}))), vector<int>{7});
    CHECK_EQ(to_vector(rec.reverseList(make_list({1, 1, 2}))), vector<int>{2, 1, 1});

    // It relinks the SAME nodes (no allocation): the node addresses come back in reverse order.
    ListNode* head = make_list({1, 2, 3, 4});
    vector<ListNode*> before = nodes_of(head);
    vector<ListNode*> after = nodes_of(it.reverseList(head));
    reverse(before.begin(), before.end());
    CHECK(after == before);

    // LeetCode's limit is 5000 nodes: fine for recursion. The iterative version doesn't care.
    vector<int> big(5000);
    iota(big.begin(), big.end(), 0);
    vector<int> big_rev(big.rbegin(), big.rend());
    CHECK_EQ(to_vector(rec.reverseList(make_list(big))), big_rev);
    vector<int> huge(200000);
    iota(huge.begin(), huge.end(), 0);
    CHECK_EQ(to_vector(it.reverseList(make_list(huge))).front(), 199999);

    // stress: both versions vs std::reverse on a vector
    for (int iter = 0; iter < 300; iter++) {
        vector<int> v = t::rand_vec((int)t::rand_int(0, 20), -50, 50);
        vector<int> expected(v.rbegin(), v.rend());
        CHECK_EQ(to_vector(it.reverseList(make_list(v))), expected);
        CHECK_EQ(to_vector(rec.reverseList(make_list(v))), expected);
    }
    return t::summary("0206-reverse-linked-list");
}
