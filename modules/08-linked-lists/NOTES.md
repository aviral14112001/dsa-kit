# 08 · Linked Lists
> Pointer discipline — where careless code becomes a null-pointer bug.

**Time:** ~11 h of Core work ([problems](problems.md)) · **Prereqs:** modules 01 (pointers, `new`, references), 02 (the fast/slow skeleton), 03 (the cost of recursion on the stack) · **You're done when:** (1) you can write 206 (both versions), 142 and 146 from a blank file, each in under 15 minutes, and each passes on the first submit; (2) you can prove out loud, in under two minutes, why Floyd's reset lands on the cycle entry; (3) every box in problems.md is ticked, with 148 solved both top-down and bottom-up.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Singly & doubly lists | A dummy node before the head turns every edit into "edit after `prev`"; sentinels do the same for doubly linked lists | [list_basics.cpp](examples/list_basics.cpp), [doubly_linked_list.cpp](examples/doubly_linked_list.cpp) | 206, 2, 61, 25 |
| 2 | Fast & slow pointers | Speeds 1 and 2: they meet inside a cycle, and slow stops at the middle; a fixed gap finds the k-th from the end | [fast_slow.cpp](examples/fast_slow.cpp) | 876, 141, 142, 160, 234, 19 |
| 3 | Merging & sorting | Dummy + tail pointer, splice nodes, never allocate; k lists in O(N log k) with a heap or by merging pairs | [0023](examples/0023-merge-k-sorted-lists.cpp) (heap, pairing) | 21, 148, 23 |
| 4 | Composite structures | Hash map to find a node in O(1), doubly linked list to move or remove it in O(1) | [0146](examples/0146-lru-cache.cpp), [list_queue.cpp](examples/list_queue.cpp) | 146, 138, 460 |

## 1. Singly & doubly lists — insert, delete, reverse, the dummy-head technique

### Concept

**The node.** LeetCode's definition, which `include/leetcode.hpp` copies exactly (never redefine it):

```c++
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};
```

A list is a `ListNode*` to its first node, and `nullptr` ends it. Coming from C#: there, `ListNode` would be a class, so every variable of that type already holds a reference. In C++ the pointer `ListNode*` plays that role, and a `ListNode` held by value is a separate node (a copy). Dereferencing `nullptr` is undefined behavior, not a catchable `NullReferenceException`: usually a crash, and under `make run` AddressSanitizer names the line.

```
head
 │
 ▼
┌───┬───┐   ┌───┬───┐   ┌───┬───┐
│ 1 │ ●─┼──►│ 2 │ ●─┼──►│ 3 │ ∅ │
└───┴───┘   └───┴───┘   └───┴───┘
```

| Operation | `vector` | singly linked | doubly linked |
|---|---|---|---|
| read the i-th element | O(1) | O(i) | O(i) |
| insert/erase at the front | O(n) | O(1) | O(1) |
| insert after a node you hold | O(n) | O(1) | O(1) |
| erase a node you hold | O(n) | O(n): you need its predecessor | O(1) |
| memory | contiguous, cache-friendly | +1 pointer per node, scattered | +2 pointers per node |

Every list edit is "change some `next` pointer". The whole module is about doing that without losing the rest of the list and without dereferencing `nullptr`.

**The dummy head.** Inserting or deleting at a position means changing the `next` of the node *before* it. The head has no node before it, so an edit at the head changes the variable `head` instead. That's a special case, and it causes most "fails on a one-node list" and "fails when the first node must be removed" bugs. A dummy node in front of the head gives every real node a predecessor. Every edit becomes "edit after `prev`", and the answer is `dummy.next`.

```
ListNode dummy(0, head);

 dummy       head
┌───┬───┐   ┌───┬───┐   ┌───┬───┐
│ 0 │ ●─┼──►│ 2 │ ●─┼──►│ 4 │ ∅ │     insert 1:  prev = &dummy, link the new node after prev
└───┴───┘   └───┴───┘   └───┴───┘     delete 2:  prev = &dummy, prev->next = prev->next->next
  ▲ prev
```

Put the dummy on the stack (`ListNode dummy;`): no `new`, nothing to free. It dies when the function returns, which is why you return `dummy.next` and never `&dummy`. `make_list` in `include/leetcode.hpp` uses the same trick to append: a dummy plus a `tail` pointer. The delete primitive is one line:

```c++
prev->next = prev->next->next;   // unlink the node after prev (on LeetCode, don't delete it)
```

**The pointer-to-pointer trick.** Instead of tracking the node before the current one, track *the pointer that points at* the current node: a `ListNode**` that starts as `&head` and moves to `&node->next`. Writing through it (`*link = ...`) edits either `head` or some node's `next` with the same line of code, so the special case disappears without a dummy. On a whiteboard the dummy is easier to explain; default to it, and know this one well enough to read it.

**Reversal.** Walk the list once and flip each `next` to point backwards. The invariant: `prev` heads the already-reversed prefix, `cur` heads the untouched suffix, and the two share no nodes. Worked example 206 below traces both the iterative and the recursive version.

**Reversing a sublist** (25 reverses one block of k after another). Only the two ends of the block need reconnecting:

```
reverse positions 2..4:

  dummy → 1 → [2 → 3 → 4] → 5 → ∅
          ▲    ▲       ▲    ▲
       before first   last  after

run the 206 loop for exactly 3 nodes, then reconnect both ends:

  dummy → 1 → [4 → 3 → 2] → 5 → ∅
  before->next = last          first->next = after
```

What to remember: stop `before` at the node in front of the range (a dummy covers ranges that start at the head). `first` becomes the block's tail. When the loop finishes its k flips, `prev` is the block's new head and `cur` is `after`.

**Doubly linked lists and sentinels.** A `prev` pointer makes "erase this node" O(1), because you no longer search for the predecessor, and it lets you walk backwards. The cost: every link or unlink writes four pointers, and forgetting one corrupts the list in a way that shows up much later. Sentinels tame it. A permanent `head` node sits before the first element and a `tail` node after the last. Every real node then always has real neighbours on both sides, so linking and unlinking need no null checks:

```
 head (sentinel)                              tail (sentinel)
┌───┐     ┌───┐     ┌───┐     ┌───┐     ┌───┐
│ H │ ◄─► │ 1 │ ◄─► │ 2 │ ◄─► │ 3 │ ◄─► │ T │        empty list:  H ◄─► T
└───┘     └───┘     └───┘     └───┘     └───┘

unlink(2):  x->prev->next = x->next;   x->next->prev = x->prev;    (1 ◄─► 3)
```

The usual `std::list` implementations do the same with a single sentinel that closes the list into a ring; that sentinel is `end()`.

**Memory on LeetCode.** The judge owns the input nodes and never asks you to free them. The convention is to unlink removed nodes without `delete`, and this kit's test runner turns off ASan's leak check for that reason. Your own data structures (the `DList` below, section 4's queue and hand-rolled LRU) own their nodes: they free them in the destructor, and they delete the copy constructor and copy assignment so a copy can't free the same nodes twice (the Rule of Three; C#'s garbage collector hides all of this). In an interview one sentence covers it: "I'm unlinking without deleting because the caller owns the nodes. In production I'd free them or use smart pointers."

### Template

Sorted insert three ways: without a dummy, with one, and with a pointer-to-pointer. [list_basics.cpp](examples/list_basics.cpp) stress-tests the three against each other.

<!-- snippet: modules/08-linked-lists/examples/list_basics.cpp#no_dummy -->
```cpp
// Without a dummy: inserting before the head is a special case, because it changes `head`
// itself instead of some node's `next`.
ListNode* insert_sorted_no_dummy(ListNode* head, int x) {
    if (!head || x <= head->val) return new ListNode(x, head);  // the special case
    ListNode* prev = head;
    while (prev->next && prev->next->val < x) prev = prev->next;
    prev->next = new ListNode(x, prev->next);
    return head;
}
```
<!-- /snippet -->

<!-- snippet: modules/08-linked-lists/examples/list_basics.cpp#dummy -->
```cpp
// With a dummy node in front of the head, EVERY insertion is "link a node after prev".
ListNode* insert_sorted(ListNode* head, int x) {
    ListNode dummy(0, head);  // lives on the stack: no new, nothing to free
    ListNode* prev = &dummy;
    while (prev->next && prev->next->val < x) prev = prev->next;
    prev->next = new ListNode(x, prev->next);
    return dummy.next;        // not `head`: the head may have changed
}
```
<!-- /snippet -->

<!-- snippet: modules/08-linked-lists/examples/list_basics.cpp#pointer_to_pointer -->
```cpp
// Pointer-to-pointer: `link` points at the pointer that will change: &head at first, then
// &node->next. Both are just "the arrow pointing at the current node", so there's no special case.
ListNode* insert_sorted_pp(ListNode* head, int x) {
    ListNode** link = &head;
    while (*link && (*link)->val < x) link = &(*link)->next;
    *link = new ListNode(x, *link);  // redirect that arrow to the new node
    return head;
}
```
<!-- /snippet -->

Each is O(n) time for the walk and O(1) extra space.

The doubly linked list with sentinels. [doubly_linked_list.cpp](examples/doubly_linked_list.cpp) stress-tests it against `std::deque` and checks both link directions after every operation.

<!-- snippet: modules/08-linked-lists/examples/doubly_linked_list.cpp#dlist -->
```cpp
struct DNode {
    int val = 0;
    DNode* prev = nullptr;
    DNode* next = nullptr;
};

// head.next is the first real node, tail.prev the last; an empty list is head <-> tail.
// Every real node always has a real prev and next, so linking and unlinking never test for
// null and never special-case the first or last node.
class DList {
public:
    DList() { head.next = &tail; tail.prev = &head; }
    ~DList() { while (!empty()) pop_front(); }
    DList(const DList&) = delete;             // it owns raw pointers: a copy would free them twice
    DList& operator=(const DList&) = delete;

    bool empty() const { return head.next == &tail; }
    DNode* first() { return head.next; }      // == end() when empty
    DNode* last() { return tail.prev; }       // == rend() when empty
    DNode* end() { return &tail; }            // one past the last node
    DNode* rend() { return &head; }           // one before the first node

    // Link x in just before pos (pos may be end()). O(1).
    void insert_before(DNode* pos, DNode* x) {
        x->prev = pos->prev;
        x->next = pos;
        pos->prev->next = x;
        pos->prev = x;
    }
    // Unlink a real node without freeing it. O(1): x already knows both neighbours.
    void unlink(DNode* x) {
        x->prev->next = x->next;
        x->next->prev = x->prev;
    }

    void push_front(int v) { insert_before(head.next, new DNode{v}); }
    void push_back(int v) { insert_before(&tail, new DNode{v}); }
    int pop_front() { return erase(head.next); }  // precondition: !empty()
    int pop_back() { return erase(tail.prev); }   // precondition: !empty()
    void move_to_front(DNode* x) {                // the LRU cache's core move (Section 4)
        unlink(x);                                // two statements on purpose (see Pitfalls)
        insert_before(head.next, x);
    }

private:
    DNode head, tail;  // sentinels: they never hold data and are never unlinked
    int erase(DNode* x) {
        unlink(x);
        int v = x->val;
        delete x;
        return v;
    }
};
```
<!-- /snippet -->

Every operation is O(1). `move_to_front` is the move an LRU cache makes on every access (Section 4).

### Pitfalls

- **Losing the rest of the list.** Save `cur->next` before you overwrite it. Draw the boxes and arrows before writing the loop.
- **Returning `head` after the head changed.** With a dummy, return `dummy.next`.
- **Null checks in the wrong order.** `while (cur && cur->val < x)` is safe; `while (cur->val < x && cur)` dereferences first. `&&` evaluates left to right and stops early.
- **An unterminated tail.** The new last node needs `next = nullptr`, or you've built a cycle: recursive reversal's `head->next = nullptr`, cutting a list in half or closing a ring and cutting it open again. The symptom is a time-out or a garbage `to_vector`.
- **A pointer to the stack dummy.** `return &dummy;` hands out a dead object. Return `dummy.next`.
- **`ListNode* dummy = new ListNode();`** works, but it leaks and needs `->` everywhere. Prefer `ListNode dummy;` with `&dummy`.
- **Unspecified argument evaluation order.** With an `unlink` that returned `x`, `insert_before(head.next, unlink(x))` would look fine, but C++ may read `head.next` *before* `unlink(x)` runs. If `x` is the first node, that links `x` next to itself. Keep pointer surgery in separate statements; that's why `move_to_front` has two lines.
- **Values instead of nodes.** When identity matters (cycles, intersections), compare `a == b` (addresses), not `a->val == b->val`.
- **Recursion depth.** Recursive list code uses one stack frame per node. LeetCode's 5000 nodes are fine; a million nodes can overflow a stack of a few MB.

### Recognize it when…

- The input is a `ListNode* head`. There's no random access, so the tools are pointer rewiring, fast/slow pointers, or copying into a vector (O(n) space, usually the brute force).
- "Remove…", "insert…", "partition…", "delete duplicates…", and the head itself might change → dummy head.
- "Reverse", "in groups of k", "palindrome in O(1) space" → the 206 loop on all or part of the list.
- "Rotate by k places" → find the length and the tail first; k can be far larger than the length.
- Numbers stored as linked digits → walk both lists with a carry, building the answer off a dummy.
- "Erase a node given only a handle", "move to front", "O(1) remove" → a doubly linked list with sentinels (plus a hash map, section 4).

Core nudges: [2. Add Two Numbers](https://leetcode.com/problems/add-two-numbers/): dummy plus carry; keep looping while either list or the carry remains. [61. Rotate List](https://leetcode.com/problems/rotate-list/): reduce k modulo the length, then close the list into a ring and cut it at the right node. [25. Reverse Nodes in k-Group](https://leetcode.com/problems/reverse-nodes-in-k-group/): the sublist reversal above, block after block; check that k nodes remain before reversing a block.

### Worked example: 206. Reverse Linked List
[LeetCode 206](https://leetcode.com/problems/reverse-linked-list/) · Easy

**Problem (paraphrased):** Given the head of a singly linked list, return the head of the same nodes in reverse order. The follow-up asks for both an iterative and a recursive version.

**Signals:** "reverse" plus a linked list; the recursive follow-up.

**Brute force, and why it fails:** copy the values into a vector and write them back reversed. O(n) time but O(n) extra space, and it doesn't touch the links, which is the skill being tested.

**Key insight:** flip one arrow per step, and save `next` before flipping, because the flip destroys your only way forward.

**Dry run:** iterative, on 1 → 2 → 3:

| after step | prev | cur | the links |
|---|---|---|---|
| start | ∅ | 1 | 1 → 2 → 3 → ∅ |
| 1 | 1 | 2 | ∅ ← 1 &nbsp;&nbsp; 2 → 3 → ∅ |
| 2 | 2 | 3 | ∅ ← 1 ← 2 &nbsp;&nbsp; 3 → ∅ |
| 3 | 3 | ∅ | ∅ ← 1 ← 2 ← 3, return prev = 3 |

Recursive, in the call for node 1, after `reverseList(2)` has returned 3:

```
before:                    1 → 2 ← 3      (1->next is still 2, and 2->next is ∅)
                                   ▲ newHead
head->next->next = head:   1 ⇄ 2 ← 3      (2 now points back at 1: a two-node cycle, for one line)
head->next = nullptr:      ∅ ← 1 ← 2 ← 3
```

<!-- snippet: modules/08-linked-lists/examples/0206-reverse-linked-list.cpp#iterative -->
```cpp
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
```
<!-- /snippet -->

<!-- snippet: modules/08-linked-lists/examples/0206-reverse-linked-list.cpp#recursive -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** iterative O(n) time, O(1) space. Recursive O(n) time, O(n) stack: one frame per node.

**Edge cases:**
- Empty list: the loop never runs, and the function returns `nullptr`.
- One node: returned unchanged.
- Long lists: use the iterative version (the test file reverses 200 000 nodes with it).

**Follow-ups:**
- *Reverse only positions left..right?* The same loop for exactly right − left + 1 nodes, then reconnect both ends (the sublist diagram above).
- *Reverse in groups of k?* That's 25, in your list.
- *Print the list backwards without modifying it?* Recursion or an explicit stack: O(n) space either way.

## 2. Fast & slow pointers — cycle detection, middle node, k-th from the end

### Concept

**Floyd's cycle detection.** `slow` moves 1 node per step and `fast` moves 2.
- No cycle: `fast` reaches `nullptr` after about n/2 steps.
- A cycle of λ nodes: once both pointers are inside it, let g be how far `fast` must walk forward to reach `slow`. Each step `fast` moves 2 and `slow` moves 1, so g drops by exactly 1. A gap that shrinks by exactly 1 can't jump over 0, so they meet, at most λ steps after `slow` enters the cycle.

```
            μ nodes before the cycle
  head → ○ → ○ → E → ○ → ○
                 ▲       │         E = where the cycle starts (the entry)
                 │       ▼         M = where slow and fast meet
                 ○ ◄─ M ◄─ ○        λ = number of nodes on the cycle
```

**Where does the cycle start?** After they meet, restart one pointer at `head` and advance both one step at a time: they meet at the entry. The proof is in worked example 142.

**The middle node, two conventions.** An even-length list has two middles:

| length | nodes | second middle (876's answer) | first middle (`middle_first`, for splitting) |
|---|---|---|---|
| 1 | 1 | 1 | 1 |
| 2 | 1 2 | 2 | 1 |
| 3 | 1 2 3 | 2 | 2 |
| 4 | 1 2 3 4 | 3 | 2 |
| 5 | 1 2 3 4 5 | 3 | 3 |

The loop condition decides which one you get. `middle_first` (below) stops `slow` at the first middle; that's the one to cut a list at, because the first half ends there and 2 nodes split 1 + 1. For 876, find the condition that lets `slow` take one more step on even lengths, and check it against the table.

**The k-th node from the end, with a gap.** Advance `lead` by k nodes, then move `lead` and `trail` together. The gap stays k, so when `lead` becomes `nullptr`, `trail` is k nodes from the end. One pass, O(1) space. Two passes (count n, then walk n − k) is also O(n) and fine to mention; the gap is the answer to "can you do it in one pass?".

**A palindrome check by reversing half** (234), in O(1) extra space:
1. Find the first middle. The second half starts right after it.
2. Reverse the second half with the 206 loop.
3. Walk both halves together, comparing values. The reversed half is never longer, so stop when it ends.
4. Reverse the second half back, so the caller gets the list unchanged. Say it even if you skip it; interviewers notice.

```
1 → 2 → 3 → 2' → 1'      first middle = 3; the second half is 2' → 1'
1 → 2 → 3 → 2' → ∅       after reversing, 3 still points at 2' (now the half's tail)
1' → 2' → ∅              compare 1 with 1', then 2 with 2': equal, so it's a palindrome
```

**Floyd beyond lists.** Repeatedly applying any function x → f(x) on a finite set must eventually cycle, so Floyd works there too. If every value of an array is a valid index, `i → nums[i]` is a linked list in disguise: a value stored at two indices is a node with two incoming arrows, which is exactly what a cycle entry looks like.

### Template

The first middle and the k-th from the end ([fast_slow.cpp](examples/fast_slow.cpp), stress-tested against index arithmetic):

<!-- snippet: modules/08-linked-lists/examples/fast_slow.cpp#middle_first -->
```cpp
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
```
<!-- /snippet -->

<!-- snippet: modules/08-linked-lists/examples/fast_slow.cpp#kth_from_end -->
```cpp
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
```
<!-- /snippet -->

Both are O(n) time and O(1) space.

### Pitfalls

- **`while (fast->next && fast)`** dereferences before checking. Always test `fast` first.
- **Comparing values** (`slow->val == fast->val`) instead of nodes. Different nodes can hold equal values; 142's tests include a cycle of all-equal values.
- **Starting `fast = head->next`.** That shifts which middle you get and breaks 142's distance argument (the reset no longer lands on the entry). Start both at `head`.
- **Splitting at the second middle.** Two nodes become halves of 2 and 0, and merge sort recurses forever.
- **19 needs the node *before* the target.** Start the gap from a dummy so `trail` stops one node early; then removing the head is not a special case.
- **Side effects.** 234's half-reversal modifies the input. Restore it, or say why you don't.

### Recognize it when…

- "Cycle", "loop", "does it ever repeat", with O(1) memory. A hash set of visited nodes is the O(n)-space brute force.
- "Middle", "split into two halves", "second half" → fast/slow; split at the first middle.
- "n-th from the end", "in one pass" → the gap.
- "Palindrome" + linked list + O(1) space → middle + reverse half.
- Two lists that may join into one tail (a Y shape) → compare nodes, not values; the lists differ only before the junction.

Core nudges: [876. Middle of the Linked List](https://leetcode.com/problems/middle-of-the-linked-list/): fast & slow, returning the second middle (see the table). [141. Linked List Cycle](https://leetcode.com/problems/linked-list-cycle/): phase 1 of the worked example below. [160. Intersection of Two Linked Lists](https://leetcode.com/problems/intersection-of-two-linked-lists/): pointer switching; walk A then B, and B then A, and the two walkers meet at the junction. [234. Palindrome Linked List](https://leetcode.com/problems/palindrome-linked-list/): middle + reverse half, as above. [19. Remove Nth Node From End of List](https://leetcode.com/problems/remove-nth-node-from-end-of-list/): a gap of n, starting from a dummy.

### Worked example: 142. Linked List Cycle II
[LeetCode 142](https://leetcode.com/problems/linked-list-cycle-ii/) · Medium

**Problem (paraphrased):** Return the node where the list's cycle begins, or null if there is no cycle, without modifying the list. The `pos` in the examples only describes how the test was built; your function never sees it.

**Signals:** "where the cycle begins", and a follow-up asking for O(1) memory.

**Brute force, and why it fails:** store visited nodes in a hash set; the first node seen twice is the entry. O(n) time and O(n) space: correct, a fine first answer, but the follow-up rules it out.

**Key insight:** when slow and fast meet, the head is as far from the entry as the meeting point is (up to whole laps). A pointer from each, both moving one step at a time, meets exactly at the entry.

**Proof:** Let μ be the number of nodes before the entry, λ the cycle length, and say they meet x steps past the entry (0 ≤ x < λ).
1. Slow walked s = μ + x steps.
2. Fast walked 2s steps and stands on the same node, so it walked slow's path plus k ≥ 1 whole laps: 2s = s + kλ, hence s = kλ.
3. So μ + x = kλ, that is, μ = kλ − x.
4. A pointer that starts at the meeting point (x past the entry) and walks μ steps ends x + (kλ − x) = kλ steps past the entry: on the entry itself. A pointer that starts at `head` reaches the entry after the same μ steps.
5. They can't meet sooner, because the head pointer is outside the cycle until step μ.

**Dry run:** the first official example, 3 → 2 → 0 → −4 → back to 2. So μ = 1 and λ = 3.

| step | slow | fast | |
|---|---|---|---|
| 0 | 3 | 3 | |
| 1 | 2 | 0 | |
| 2 | 0 | 2 | fast has lapped once |
| 3 | −4 | −4 | meet: x = 2 past the entry, s = 3 = 1·λ |
| reset | entry = 3, slow = −4 | | μ = kλ − x = 3 − 2 = 1 |
| +1 | entry = 2, slow = 2 | | same node: return it |

<!-- snippet: modules/08-linked-lists/examples/0142-linked-list-cycle-ii.cpp#solution -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** O(n) time: phase 1 takes at most μ + λ steps and phase 2 takes μ. O(1) space.

**Edge cases:**
- Empty list, or one node without a loop: the `while` condition fails at once.
- One node pointing at itself: they meet after one step, the entry is `head`, and phase 2's loop doesn't run.
- A cycle that starts at the head (μ = 0): x = kλ − μ ≡ 0, so they meet at the head.
- Equal values on different nodes: compare pointers.

**Follow-ups:**
- *The cycle's length?* After meeting, hold one pointer still and walk the other until it comes back: λ steps.
- *Why speeds 1 and 2, not 1 and 3?* A relative speed of 1 shrinks the gap by exactly 1 per step, so it hits 0. With speed 3 the gap shrinks by 2, and an odd gap on an even-length cycle is skipped forever.
- *Could you mark visited nodes instead?* Only by modifying the list (a flag, or redirecting `next`), which is forbidden here.

## 3. Merging & sorting — merge two lists, merge k lists, merge sort in place

### Concept

**Merging two sorted lists.** Use a dummy and a `tail` pointer, and repeatedly splice the smaller of the two heads onto `tail`. When one list runs out, attach the rest of the other in a single step: it's already sorted and linked. Nothing is allocated: O(n + m) time, O(1) space. Taking from the first list on ties makes the merge *stable* (equal values keep their original order), and that is what makes merge sort stable.

**Merging k sorted lists** (N nodes in total):

| Approach | Time | Extra space | Why |
|---|---|---|---|
| merge each list into an accumulator | O(kN) | O(1) | the accumulator is re-walked on every merge: N/k + 2N/k + … ≈ kN/2 |
| collect all values, sort, rebuild | O(N log N) | O(N) | throws away the sortedness |
| min-heap of the k current heads | O(N log k) | O(k) | N pops and pushes at log k each |
| merge pairs, then pairs of pairs | O(N log k) | O(1) iterative | log k rounds, each touching every node once |

The last two have the same bound. Merging pairs needs no heap and no memory. The heap version works on *streams* you can only read forward, such as k sorted files: that's an external k-way merge.

C++ vs C#: `std::priority_queue` is a **max**-heap by default, while .NET's `PriorityQueue<TElement, TPriority>` is a min-heap, so C# habits bite here. The comparator reads backwards: `cmp(a, b) == true` means "a ranks below b", so a min-heap by value uses `a->val > b->val`. Module 13 covers heaps in depth.

**Merge sort on a linked list** (148). Lists are the one place merge sort needs no auxiliary array, because merging relinks nodes. Heap sort needs random access, which lists don't have; quicksort can partition a list by relinking, but it keeps its O(n²) worst case.
- *Top-down:* a list of 0 or 1 nodes is sorted. Otherwise cut it after `middle_first` (Section 2), sort each half recursively, and merge them. O(n log n) time and O(log n) stack. The cut matters: set the first half's last `next` to `nullptr`, or the "first half" is still the whole list.
- *Bottom-up* (O(1) extra space): no recursion. Pass 1 merges runs of width 1 into sorted runs of 2, pass 2 merges those into runs of 4, and so on until one run covers the list: ⌈log₂ n⌉ passes of O(n) each.

```
width 1:  4 | 2 | 1 | 3 | 5    →   2 4 | 1 3 | 5
width 2:  2 4 | 1 3 | 5        →   1 2 3 4 | 5
width 4:  1 2 3 4 | 5          →   1 2 3 4 5
```

Each pass walks the list: cut off a run of `width` nodes, cut off the next run, merge the two, attach the result to the tail of what's built so far, and move the tail to its end. The helper you'll want is "cut the list after `width` nodes and return what follows"; writing it is the real work of 148. In an interview, write top-down first and offer bottom-up when they push on space (the recursion stack counts).

### Template

The two k-way merges are worked example 23 below. The pairing version calls `mergeTwo(a, b)`, the two-list merge described at the top of this section. That's 21 in your list, so it isn't printed here: write it first (dummy + tail pointer, splice, take from `a` on ties). The example file keeps a tested copy at the very bottom, behind a spoiler comment.

### Pitfalls

- **`priority_queue` is a max-heap.** Without the reversed comparator you merge in descending order.
- **`nullptr` in the heap.** The comparator dereferences it. Skip empty lists when you build the heap.
- **A `>=` comparator.** Comparators must be strict (`>`, never `>=`); anything else is undefined behavior inside standard containers and algorithms.
- **Merging into an accumulator one list at a time.** That's O(kN): about kN/2 = 5·10^7 steps at LeetCode's limits (k = N = 10^4). It may pass, but the interviewer is waiting for log k.
- **Forgetting to cut when splitting** for merge sort: infinite recursion, or a list merged with itself.
- **Allocating nodes** (`new ListNode(a->val)`) when splicing works: O(N) extra memory, and interviewers notice.
- **Sorting a copy of the values** is O(N log N) and a fine brute force to state, but not an answer to "sort this list".

### Recognize it when…

- "k sorted lists / arrays / streams / files", "merge them" → heap of heads, or merge pairs.
- "Sort a linked list", "O(n log n) time and O(1) memory" → bottom-up merge sort.
- "k-th smallest across sorted rows or lists" → heap of heads (module 13).
- Any step that needs sorted output from sorted pieces: merge sort's merge, external sorting.

Core nudges: [21. Merge Two Sorted Lists](https://leetcode.com/problems/merge-two-sorted-lists/): the two-list merge from the top of this section; splice, don't allocate. [148. Sort List](https://leetcode.com/problems/sort-list/): top-down first (cut at `middle_first`, recurse, merge), then bottom-up.

### Worked example: 23. Merge k Sorted Lists
[LeetCode 23](https://leetcode.com/problems/merge-k-sorted-lists/) · Hard

**Problem (paraphrased):** You get an array of k linked lists, each sorted ascending. Merge them into one sorted list and return its head.

**Signals:** "k sorted lists", "merge". Up to 10^4 lists and 10^4 nodes in total.

**Brute force, and why it fails:** collect the values, sort, and build a new list: O(N log N) time and O(N) memory, ignoring that the lists are sorted. Merging lists one at a time into an accumulator is O(kN).

**Key insight:** the next output node is always the smallest of the k current heads. Keep those k candidates in a min-heap (O(log k) per step), or merge in pairs so each node takes part in only log k merges.

**Dry run (heap):** A = 1→4→5, B = 1→3→4, C = 2→6. Equal values can come out in either order (a heap isn't stable), but the output values are the same.

| heap (value + list) | pop | push | output so far |
|---|---|---|---|
| 1A 1B 2C | 1A | 4A | 1 |
| 1B 2C 4A | 1B | 3B | 1 1 |
| 2C 3B 4A | 2C | 6C | 1 1 2 |
| 3B 4A 6C | 3B | 4B | 1 1 2 3 |
| 4A 4B 6C | 4A | 5A | 1 1 2 3 4 |
| 4B 5A 6C | 4B | | 1 1 2 3 4 4 |
| 5A 6C | 5A | | 1 1 2 3 4 4 5 |
| 6C | 6C | | 1 1 2 3 4 4 5 6 |

Merging pairs on the same input: gap 1 merges A and B into slot 0 (C has no partner yet); gap 2 merges slot 0 with C. Two rounds, since ⌈log₂ 3⌉ = 2.

<!-- snippet: modules/08-linked-lists/examples/0023-merge-k-sorted-lists.cpp#heap -->
```cpp
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
```
<!-- /snippet -->

<!-- snippet: modules/08-linked-lists/examples/0023-merge-k-sorted-lists.cpp#divide_conquer -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** both are O(N log k) time. The heap uses O(k) extra space. Pairing uses O(1): it reuses the input vector and doesn't recurse.

**Edge cases:**
- k = 0 (`[]`), or only empty lists (`[[]]`): return `nullptr`.
- Empty lists mixed with non-empty ones: never push `nullptr` into the heap.
- k not a power of two: the `i + gap < k` test leaves the odd list out until a later round.

**Follow-ups:**
- *Which one would you ship?* Pairing when every list is in memory: no heap, no allocation. The heap when the inputs are streams.
- *What if each list is a huge file on disk?* The same heap, reading each file in buffered chunks: an external k-way merge.
- *Without reusing the input vector?* Recursive divide & conquer over index ranges: the same O(N log k), plus O(log k) stack.

## 4. Composite structures — LRU cache, skip lists, list-backed queues

### Concept

**Why combine structures.** A hash map answers "where is key k?" in O(1) but keeps no order. A doubly linked list can move or remove a node in O(1) but can't find one. Map each key to its list node and you get both. That's the LRU cache, the LFU cache, and most "design a data structure with O(1) operations" questions.

**The `std::list` facts that make it work:**
- Iterators (and pointers and references) to elements stay valid through inserts, through erasing *other* elements, and through `splice`. So an `unordered_map<Key, list<...>::iterator>` stays correct. A `vector` would invalidate them on every reallocation or middle erase.
- `items.splice(pos, items, it)` unlinks the node at `it` and relinks it before `pos`: O(1), no copy, no allocation, and `it` still points at the moved node.
- `size()` is O(1) (guaranteed since C++11).
- The C# equivalent is `LinkedList<T>` plus `Dictionary<TKey, LinkedListNode<T>>`; the `LinkedListNode<T>` plays the iterator's role.

```
where (hash map):    key 3 ──┐     key 1 ──┐     key 2 ──┐
                             ▼             ▼             ▼
items (list):   front →  (3, c)  ◄─►   (1, a)  ◄─►   (2, b)  ← back
                         most recent                 least recent: evict here

get(1):          splice (1, a) to the front
put(4, d), full: erase key 2 from where, pop_back, emplace_front (4, d), store its iterator
```

**LFU, the idea** (460, in your list). Evict the least *frequently* used key, breaking ties by least recent. The O(1) design is this section's pattern twice over: group keys into buckets by use count, make each bucket an LRU list, and keep track of the smallest count that has any keys. Working out how that smallest count changes on an access and on an insert is the exercise.

**Skip lists.** A sorted linked list with express lanes. Each node gets a random height: level 0 always, and each further level with probability 1/2 (keep flipping a coin while it lands heads). Level i links the nodes taller than i, so higher levels skip more nodes:

```
L3: H ─────────────────────────────► 9 ──────────────────────► ∅
L2: H ───────────► 4 ──────────────► 9 ───────────► 12 ──────► ∅
L1: H ─► 1 ──────► 4 ─► 6 ─────────► 9 ─► 11 ─────► 12 ──────► ∅
L0: H ─► 1 ─► 3 ─► 4 ─► 6 ─► 7 ────► 9 ─► 11 ─────► 12 ─► 15 ─► ∅

search 7:  L3: 9 is too big, drop.   L2: 4, then 9 is too big, drop.
           L1: 6, then 9 is too big, drop.   L0: 7, found.
```

Search starts at the top level: move right while the next key is smaller than the target, otherwise drop one level. Insert runs the same search, remembers the last node visited on each level, flips coins for the new node's height, and splices it into those levels. There are about log₂ n levels, and you take at most about 2 steps right per level on average, so search, insert and delete are O(log n) *expected*. The expected pointer count is about 2n. The worst case is O(n), but it needs absurdly bad luck.

Where they're used: Redis sorted sets keep a skip list next to a hash table; LevelDB and RocksDB use one for their in-memory write buffer (the memtable); Java ships `ConcurrentSkipListMap`. The draw is simple code, cheap ordered iteration and range scans, and concurrent versions that are far easier to get right than concurrent balanced trees. In a C++ interview, `std::map`/`std::set` (red-black trees; C#'s `SortedDictionary`/`SortedSet`) give the same O(log n) ordered operations.

**List-backed queues and deques.** A singly linked list with head *and* tail pointers is a queue: push at the tail and pop at the head, both O(1). Popping at the tail would need the node before it, which is O(n). A doubly linked list (Section 1's `DList`) is a deque: O(1) at both ends. `std::queue<T, std::list<T>>` works, but the default container, `std::deque`, stores elements in contiguous blocks: no allocation per element and better cache behavior, so it's faster in practice. Module 09 section 3 builds the array-based alternative, the ring buffer.

### Template

A queue on a singly linked list with a tail pointer ([list_queue.cpp](examples/list_queue.cpp), stress-tested against `std::queue`):

<!-- snippet: modules/08-linked-lists/examples/list_queue.cpp#linked_queue -->
```cpp
// Push at the tail, pop at the head: both O(1). (Popping at the tail of a SINGLY linked list
// would be O(n): you'd need the node before the tail.)
class LinkedQueue {
public:
    LinkedQueue() = default;
    LinkedQueue(const LinkedQueue&) = delete;             // a copy would share the nodes, then
    LinkedQueue& operator=(const LinkedQueue&) = delete;  // both destructors would free them
    ~LinkedQueue() { while (!empty()) pop(); }

    bool empty() const { return head == nullptr; }
    int size() const { return count; }
    int front() const { return head->val; }  // precondition: !empty()

    void push(int x) {
        ListNode* node = new ListNode(x);
        if (tail) tail->next = node;  // non-empty: link after the old tail
        else head = node;             // empty: the new node is also the front
        tail = node;
        count++;
    }
    void pop() {                      // precondition: !empty()
        ListNode* old = head;
        head = head->next;
        if (!head) tail = nullptr;    // removed the last node: tail must not dangle
        delete old;
        count--;
    }

private:
    ListNode* head = nullptr;  // front: the next to pop
    ListNode* tail = nullptr;  // back: the last pushed
    int count = 0;
};
```
<!-- /snippet -->

O(1) per operation. The LRU cache, both with `std::list` and hand-rolled, is in the worked example below.

### Pitfalls

- **LRU: `get` also counts as a use.** Forgetting to refresh on `get` is the most common bug.
- **LRU: `put` on an existing key** updates the value *and* moves the node. Check for the key before evicting; evicting first throws out a key for nothing when the cache is full.
- **LRU: storing only values in the list.** Eviction then can't tell which map entry to erase. Store the key in the node.
- **`where[key]` in `get`** inserts a default entry for a missing key. Use `find`.
- **Hand-rolled LRU: read `lru->key` before `delete lru`.** A use-after-free often "works" until it doesn't; ASan catches it.
- **`list.remove(value)` and `std::find` on a list are O(n).** The O(1) operations take an iterator: `erase(it)`, `splice`, `insert(it, v)`.
- **Raw-pointer ownership.** A class that owns raw pointers needs a destructor and deleted (or deep) copy operations, or a copy frees the same nodes twice.
- **`unique_ptr<Node> next` chains destroy recursively.** Freeing a million-node list that way can overflow the stack. Free long lists in a loop.

### Recognize it when…

- "Design a data structure", "O(1) get and put", "capacity", "evict the least recently / least frequently used".
- "Move to front", "most recent", "back and forward history", "remove in O(1) given the element".
- "Ordered set with O(log n) insert, delete and search, without a balanced tree" → skip list.
- "Copy a list whose nodes have an extra pointer" → a map from each old node to its copy.

Core nudges: [138. Copy List with Random Pointer](https://leetcode.com/problems/copy-list-with-random-pointer/): an old → new node map, or interleave the copies (A → A′ → B → B′) for O(1) space. [460. LFU Cache](https://leetcode.com/problems/lfu-cache/): frequency buckets of LRU lists plus a minimum-frequency pointer, as above.

### Worked example: 146. LRU Cache
[LeetCode 146](https://leetcode.com/problems/lru-cache/) · Medium

**Problem (paraphrased):** Build a key-value cache with a fixed capacity: `get(key)` returns the value or −1, and `put(key, value)` inserts or updates. Both count as a use. When an insert pushes the size over capacity, remove the key whose last use is oldest. Both operations must run in O(1) average time.

**Signals:** "least recently used", "capacity", "O(1) average time".

**Brute force, and why it fails:** a vector of (key, value) pairs ordered by recency. Finding a key is O(capacity), and so is moving it to the end. With up to 2·10^5 calls and capacity up to 3000 that's around 6·10^8 steps, and it misses the point of the question.

**Key insight:** a linked list keeps the recency order and moves any node to the front in O(1), provided you can find the node in O(1). The hash map finds it.

**Dry run:** capacity 2, the official example:

| call | list, front → back | returns |
|---|---|---|
| put(1,1) | (1,1) | |
| put(2,2) | (2,2) (1,1) | |
| get(1) | (1,1) (2,2) | 1 |
| put(3,3) | (3,3) (1,1) | key 2 evicted |
| get(2) | (3,3) (1,1) | −1 |
| put(4,4) | (4,4) (3,3) | key 1 evicted |
| get(1) | (4,4) (3,3) | −1 |
| get(3) | (3,3) (4,4) | 3 |
| get(4) | (4,4) (3,3) | 4 |

<!-- snippet: modules/08-linked-lists/examples/0146-lru-cache.cpp#solution -->
```cpp
class LRUCache {
public:
    LRUCache(int capacity) : cap(capacity) {}

    int get(int key) {
        auto it = where.find(key);                     // find, not where[key]: [] would insert
        if (it == where.end()) return -1;
        items.splice(items.begin(), items, it->second);  // move to the front; iterator stays valid
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = where.find(key);
        if (it != where.end()) {                       // existing key: update it, then refresh it
            it->second->second = value;
            items.splice(items.begin(), items, it->second);
            return;
        }
        if ((int)items.size() == cap) {                // full: evict the least recently used
            where.erase(items.back().first);           // the list stores the key for this line
            items.pop_back();
        }
        items.emplace_front(key, value);
        where[key] = items.begin();
    }

private:
    int cap;
    list<pair<int, int>> items;                              // (key, value), most recent first
    unordered_map<int, list<pair<int, int>>::iterator> where;  // key -> its node in items
};
```
<!-- /snippet -->

A common follow-up is "now without `std::list`". It's the same design on section 1's sentinel list, with a hash map to raw node pointers:

<!-- snippet: modules/08-linked-lists/examples/0146-lru-cache.cpp#handrolled -->
```cpp
class LRUCache {
    struct Node {
        int key, val;
        Node* prev = nullptr;
        Node* next = nullptr;
    };
    int cap;
    unordered_map<int, Node*> where;
    Node head{}, tail{};  // sentinels: head.next is the most recent, tail.prev the least recent

    void unlink(Node* x) {
        x->prev->next = x->next;
        x->next->prev = x->prev;
    }
    void push_front(Node* x) {
        x->prev = &head;
        x->next = head.next;
        head.next->prev = x;
        head.next = x;
    }

public:
    LRUCache(int capacity) : cap(capacity) {
        head.next = &tail;
        tail.prev = &head;
    }
    ~LRUCache() {
        for (auto& [key, node] : where) delete node;
    }
    LRUCache(const LRUCache&) = delete;             // owns raw pointers: forbid copies
    LRUCache& operator=(const LRUCache&) = delete;

    int get(int key) {
        auto it = where.find(key);
        if (it == where.end()) return -1;
        Node* x = it->second;
        unlink(x);
        push_front(x);
        return x->val;
    }

    void put(int key, int value) {
        if (auto it = where.find(key); it != where.end()) {
            Node* x = it->second;
            x->val = value;
            unlink(x);
            push_front(x);
            return;
        }
        if ((int)where.size() == cap) {
            Node* lru = tail.prev;
            unlink(lru);
            where.erase(lru->key);   // read the key BEFORE freeing the node
            delete lru;
        }
        Node* x = new Node{key, value};
        push_front(x);
        where[key] = x;
    }
};
```
<!-- /snippet -->

**Complexity:** O(1) average per call (hash map operations plus a constant number of pointer writes); O(capacity) space.

**Edge cases:**
- Capacity 1: every new key evicts the previous one.
- `put` on an existing key while full: update and refresh, evict nothing.
- `get` on a missing key must not insert it.
- `get` on the key that's already most recent: splicing a node to its own position is fine.

**Follow-ups:**
- *Make it thread-safe?* One mutex around both operations; even `get` writes, because it reorders the list. For throughput, shard the cache by key hash, one lock per shard.
- *Evict by frequency instead?* LFU (460), the idea above.
- *Add time-to-live expiry?* Store an expiry time per entry and treat expired entries as misses on `get`; clean up lazily, or with a min-heap ordered by expiry.

## Common mistakes

| Mistake | Symptom | Fix |
|---|---|---|
| Coding before drawing | Twenty minutes of debugging pointers | Draw the boxes, and the before/after of every pointer you assign |
| Special-casing the head | Fails when the head is removed or the list has one node | Dummy node; return `dummy.next` |
| Overwriting `next` before saving it | The rest of the list is lost | `ListNode* next = cur->next;` first |
| New tail not set to `nullptr` | Time-out; output that repeats forever | Terminate every list you cut or build |
| `fast->next` checked before `fast` | Null dereference on even lengths | `while (fast && fast->next)` |
| Comparing `->val` for identity | Wrong cycle entry or intersection | Compare pointers |
| Returning `&dummy` | Dangling pointer, garbage output | Return `dummy.next` |
| Default `priority_queue` | Descending merge | `a->val > b->val` comparator |
| Storing `vector` iterators, or calling `list::remove` | Invalidated iterators, or O(n) per operation | `std::list` iterators + `splice` / `erase(it)` |
| Recursion on huge lists | Stack overflow | The iterative version |

## Say it out loud

A talk track, using 142:

1. **Restate:** "Return the node where the cycle starts, or null. I may not modify the list."
2. **Brute force:** "Store visited nodes in a hash set; the first repeat is the entry. O(n) time, O(n) space."
3. **Insight:** "Floyd: slow and fast must meet inside a cycle, because the gap shrinks by one per step. At the meeting point, the head and the meeting point are the same distance from the entry, modulo the cycle length. So I restart one pointer at head and step both by one."
4. **Complexity:** "O(n) time, O(1) space."
5. **Edge cases:** "Empty list, one node, a node pointing at itself, and a cycle that starts at the head. I compare nodes, not values."
6. **While coding,** narrate the pointer guards: "fast moves two, so I check fast and fast->next."

Follow-ups interviewers commonly ask in this module:
- *"Can you do it in O(1) extra space?"* Hash set → Floyd; copy into a vector → rewire pointers in place.
- *"Iteratively and recursively?"* Know both versions of 206, and the stack cost of the recursive one.
- *"What if the list were doubly linked?"* Erase is O(1); reversal swaps `prev` and `next` in every node.
- *"Implement it without `std::list`."* The hand-rolled LRU with sentinels.
- *"Is your merge stable? Who frees the removed nodes?"* Take from the first list on ties; the caller owns the nodes.

## Self-check

1. Why does a dummy head remove special cases, and what must you return at the end?
<details><summary>Answer</summary>Every insert or delete changes the <code>next</code> of the node before the position; only the head has no such node. A dummy in front gives it one, so all edits are "after <code>prev</code>". Return <code>dummy.next</code>, because the real head may have changed; never return <code>&dummy</code>, since it lives on the stack.</details>

2. In iterative reversal, what happens if you write `cur->next = prev;` before saving `cur->next`?
<details><summary>Answer</summary>You lose the only pointer to the rest of the list. <code>cur</code> can't advance, so the reversal stops after one node, and every node after it becomes unreachable.</details>

3. Why must slow and fast meet if there is a cycle, and why can't fast jump over slow?
<details><summary>Answer</summary>Once both are on the cycle, the distance from fast forward to slow shrinks by exactly 1 per step (fast +2, slow +1). A gap that drops by exactly 1 must hit 0, so they meet within λ steps of slow entering the cycle. Skipping would need the gap to drop by 2 or more at once.</details>

4. Prove that 142's reset meets at the entry.
<details><summary>Answer</summary>With μ nodes before the entry, cycle length λ and meeting point x past the entry: slow walked s = μ + x and fast 2s = s + kλ, so s = kλ and μ = kλ − x. Walking μ steps from the meeting point lands kλ past the entry, which is the entry; walking μ steps from head also lands on the entry, and not earlier, since the head pointer is off the cycle until then.</details>

5. On 1 → 2 → 3 → 4, which middle do you cut at before recursing in merge sort, and what goes wrong with the other one?
<details><summary>Answer</summary>The first middle, 2: the first half ends there, so every split makes both halves smaller (2 nodes split 1 + 1). Cutting after the second middle, 3, turns a 2-node list into halves of 2 and 0, so the recursion never shrinks.</details>

6. Why is merging k lists one at a time into an accumulator O(kN), while the heap and pairing versions are O(N log k)?
<details><summary>Answer</summary>The accumulator is re-walked by every merge, and it grows to N/k, 2N/k, …, N, which sums to about kN/2. The heap does N pops and pushes at O(log k) each. Pairing halves the number of lists per round, so there are log k rounds, and each round touches every node once.</details>

7. What makes it safe to store `std::list` iterators in a hash map, and what exactly does `splice` do?
<details><summary>Answer</summary>List iterators stay valid until their own element is erased: inserting, erasing other elements and splicing don't invalidate them. <code>splice(pos, lst, it)</code> unlinks the node at <code>it</code> and relinks it before <code>pos</code> in O(1), with no copy and no allocation, and <code>it</code> keeps pointing at the moved node.</details>

8. In the LRU cache, why does the list node store the key and not just the value?
<details><summary>Answer</summary>Eviction starts from the list's back, and it must also erase that entry from the hash map. Only the key can find the map entry, so the node carries it.</details>

9. What is a skip list's expected search cost, why, and where is one used in practice?
<details><summary>Answer</summary>O(log n) expected: a node reaches level i with probability 2<sup>−i</sup>, so there are about log₂ n levels, and a search takes at most about 2 steps right per level on average. Redis sorted sets, the LevelDB/RocksDB memtable and Java's <code>ConcurrentSkipListMap</code> all use one.</details>

10. Why does top-down merge sort on a list use O(log n) extra space while bottom-up uses O(1)?
<details><summary>Answer</summary>Top-down recurses on halves, so the call stack is log n frames deep; merging itself relinks nodes and needs no buffer. Bottom-up replaces the recursion with loops over run widths 1, 2, 4, …, keeping only a few pointers (the tail and the two run heads).</details>
