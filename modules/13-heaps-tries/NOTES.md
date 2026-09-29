# 13 · Heaps + Priority Queue + Tries
> Always-ready extremes, and prefix lookups that scale.

**Time:** ~10 h of Core work ([problems](problems.md)) · **Prereqs:** modules 01 (STL containers, comparators), 05 (sorting), 12 (trees) · **You're done when:** (1) from memory you can write sift-up, sift-down and heapify on an array and explain in two sentences why heapify is O(n); (2) you can declare a min-heap, a max-heap and a custom-order `priority_queue` without looking anything up, and pick the right heap for top-K and running-median problems; (3) you can write a trie with prefix counts and erase from a blank file and state its memory cost per node.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Binary heaps | A complete tree in an array; sift-up / sift-down repair one path in O(log n); bottom-up heapify is O(n) | [heap.hpp](../../templates/heap.hpp), [priority_queue_basics.cpp](examples/priority_queue_basics.cpp) | 703, 373 |
| 2 | Priority queues | A size-k heap keeps the top K; two heaps split a stream at its median; lazy deletion stands in for erase and decrease-key | [0295](examples/0295-find-median-from-data-stream.cpp), [event_simulation.cpp](examples/event_simulation.cpp), [lazy_deletion.cpp](examples/lazy_deletion.cpp) | 215, 1834, 295 |
| 3 | Tries | One node per prefix: O(L) per operation whatever the number of words; pass / end counts give prefix counts and deletion | [trie.hpp](../../templates/trie.hpp), [0208](examples/0208-implement-trie-prefix-tree.cpp) | 208, 211, 1804, 1858 |
| 4 | Applications | Autocomplete = reach the prefix, then read a precomputed or on-demand top-k; a trie steers and prunes searches | [1268](examples/1268-search-suggestions-system.cpp) | 1268, 212, 1698 |

## 1. Binary heaps — heapify, push / pop cost, heap sort, k-way merge

### Concept

A heap is for one job: you need the current extreme (largest or smallest) again and again while elements keep arriving and leaving. A sorted array gives the extreme in O(1) but inserts in O(n); a balanced BST (`std::set`) does everything in O(log n) with a node allocation per element; a binary heap gives top in O(1) and push / pop in O(log n) inside one plain array, and nothing else: no search, no ordered iteration, no arbitrary erase.

**The array layout.** A heap is a *complete* binary tree (every level full except possibly the last, which fills from the left), stored level by level. Completeness means no gaps: n elements occupy exactly indices 0..n−1, so a push appends at index n and a pop removes index n−1.

```
index:  0  1  2  3  4  5  6  7  8
value: [9, 7, 8, 3, 5, 6, 4, 1, 2]

              9 (0)                  children of i: 2i + 1 and 2i + 2
           /        \                parent of i:   (i - 1) / 2   (integer division)
       7 (1)          8 (2)          height: floor(log2 n), 3 here
       /    \        /    \          1-based arrays: children 2i and 2i + 1, parent i / 2
    3 (3)  5 (4)  6 (5)  4 (6)
    /   \
 1 (7)  2 (8)
```

**The heap property** (max-heap): every node is ≥ its children, so by induction the root is the maximum. Siblings and cousins are unordered. A heap is much weaker than "sorted", and that's exactly why it's cheaper to maintain.

Below, "a outranks b" means "a belongs above b": in a max-heap, a > b.

**Sift-up** repairs a push. Append the new element as the last leaf; while it outranks its parent, swap them. Only the edge between the moving element and its parent can be out of order, and each swap moves that edge up a level, so it's at most `height` swaps: O(log n).

**Sift-down** repairs a pop. Move the last leaf into the root and shrink the array; while a child outranks the moving element, swap it with the *higher-ranked* child (that child becomes the parent of its former sibling, so it must outrank it). At most `height` swaps: O(log n).

```
push 10 into the heap above: append at index 9, then sift up (10 > 5, 10 > 7, 10 > 9)

        9                               10
      /   \                            /    \
     7     8          ----->          9      8
    / \   / \                        / \    / \
   3   5 6   4                      3   7  6   4
  / \  /                           / \  /
 1   2 10                         1   2 5

pop from the original 9-element heap: move the last leaf (2) to the root, then sift down (2 < 8, 2 < 6)

        2                                8
      /   \                            /   \
     7     8          ----->          7     6
    / \   / \                        / \   / \
   3   5 6   4                      3   5 2   4
  /                                /
 1                                1
```

Here is the whole thing, readable end to end in `templates/heap.hpp`. It uses `priority_queue`'s comparator convention, so the default `less<T>` gives a max-heap:

<!-- snippet: templates/heap.hpp#sift -->
```cpp
template <class T, class Compare = less<T>>
struct BinaryHeap {
    // A complete binary tree stored level by level, with no gaps and no pointers:
    // a[0] is the root (the top); node i has children 2i+1 and 2i+2, and parent (i-1)/2.
    // Heap property: no node outranks its parent, so a[0] outranks everything.
    vector<T> a;
    Compare cmp;

    // Does a[i] belong above a[j]? (With the default less<T>: is a[i] > a[j]?)
    bool outranks(int i, int j) const { return cmp(a[j], a[i]); }

    // a[i] may outrank its parent (it was just appended as the last leaf).
    // Invariant: the only possible violation is between i and its parent. Each swap moves
    // that violation one level up, so this stops after at most height = floor(log2 n) swaps.
    void sift_up(int i) {
        while (i > 0 && outranks(i, (i - 1) / 2)) {
            swap(a[i], a[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }

    // a[i] may rank below a child (it was just moved into the root).
    // Invariant: the only possible violations are between i and its children. Swap with the
    // HIGHER-ranked child: it becomes the parent of the other child, so it must outrank it.
    void sift_down(int i) {
        int n = (int)a.size();
        while (true) {
            int best = i, left = 2 * i + 1, right = 2 * i + 2;
            if (left < n && outranks(left, best)) best = left;
            if (right < n && outranks(right, best)) best = right;
            if (best == i) return;              // a[i] outranks both children: the heap is valid
            swap(a[i], a[best]);
            i = best;
        }
    }
```
<!-- /snippet -->

**Heapify: building a heap from n items in O(n).** Pushing them one by one costs O(n log n). Instead, sift down every internal node, from the last one (index n/2 − 1) back to the root. When you reach node i, its two subtrees are already heaps (leaves trivially are), so one `sift_down(i)` makes subtree i a heap.

<!-- snippet: templates/heap.hpp#heapify -->
```cpp
explicit BinaryHeap(Compare c = Compare()) : cmp(c) {}

// Heapify: build from n items in O(n). Sift down every internal node, from the last one
// back to the root. Nodes n/2 .. n-1 are leaves, and a leaf is already a one-node heap;
// when we reach node i, both of its subtrees are heaps, so one sift_down(i) fixes subtree i.
explicit BinaryHeap(vector<T> items, Compare c = Compare()) : a(std::move(items)), cmp(c) {
    for (int i = (int)a.size() / 2 - 1; i >= 0; i--) sift_down(i);
}
```
<!-- /snippet -->

Why O(n), in words: a sift-down from node i travels at most i's *height* (the levels below it). Half the nodes are leaves and cost nothing, a quarter cost at most one swap, an eighth at most two; only the root can travel the full height. Pushing one by one is the opposite: most nodes sit near the bottom, and each push may climb all the way up.

As a sum: about n / 2^(h+1) nodes have height h, so the total is about n · Σ h / 2^(h+1) = n, because Σ h / 2^h = 2. (Exactly: the heights of a complete tree add up to less than n.)

[heapify_vs_pushes.cpp](examples/heapify_vs_pushes.cpp) checks the claim with a comparator that counts its own calls (the quickest way to test any complexity claim). For n = 131,072 ascending values, the worst case for pushes into a max-heap because every new value climbs to the root, heapify makes 262,110 comparisons (under 2n) and n pushes make 1,966,099 (about n log2 n). On random input the pushes are far cheaper, about 2.3 comparisons each in the demo, because an average push climbs only a level or two. So heapify's O(n) is a worst-case guarantee; n pushes are O(n) only on random input.

The rest of the struct:

<!-- snippet: templates/heap.hpp#ops -->
```cpp
    bool empty() const { return a.empty(); }
    int size() const { return (int)a.size(); }
    const T& top() const { assert(!a.empty()); return a[0]; }

    void push(T x) {                    // O(log n): add as the last leaf, then sift it up
        a.push_back(std::move(x));      // std::move, not move: clang flags the unqualified call
        sift_up((int)a.size() - 1);
    }

    void pop() {                        // O(log n): move the last leaf into the root, then sift it down
        assert(!a.empty());
        swap(a[0], a.back());
        a.pop_back();
        if (!a.empty()) sift_down(0);
    }
};
```
<!-- /snippet -->

| Operation | Cost |
|---|---|
| `top` | O(1) |
| `push`, `pop` | O(log n) |
| heapify n items | O(n) |
| find or erase an arbitrary element | O(n); `priority_queue` doesn't offer it at all |

**Heap sort, briefly.** Heapify the array as a max-heap, then n times swap the root (the maximum) into the last slot of the heap region, shrink the region by one and sift down. The array ends up ascending: O(n log n) worst case, O(1) extra space, not stable, and in practice slower than a good quicksort because it jumps around memory. Module 05 builds it from scratch (912 and `templates/sorting.hpp`); with the `<algorithm>` heap functions it's three lines:

<!-- snippet: modules/13-heaps-tries/examples/priority_queue_basics.cpp#heap_sort -->
```cpp
// Heap sort with the <algorithm> heap functions, which work on any random-access range.
void heap_sort(vector<int>& v) {
    make_heap(v.begin(), v.end());                  // O(n) heapify: max at v[0]
    for (auto end = v.end(); end != v.begin(); --end)
        pop_heap(v.begin(), end);                   // swap the max to end-1, sift down in [begin, end-1)
}   // v is now ascending (std::sort_heap is exactly this loop). O(n log n), in place, not stable.
```
<!-- /snippet -->

**`std::priority_queue` in practice.**

<!-- snippet: modules/13-heaps-tries/examples/priority_queue_basics.cpp#declare -->
```cpp
priority_queue<int> max_heap;                               // default: the LARGEST element on top
priority_queue<int, vector<int>, greater<int>> min_heap;    // greater<int> flips it: smallest on top

// pairs and tuples compare field by field (lexicographically), so with greater<> a heap of
// (distance, node) pops the smallest distance first, ties broken by the smaller node.
priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> by_distance;

// A custom order. The comparator answers "does a rank BELOW b?"; the top is the element that
// ranks below nothing. Here a later deadline ranks below, so the earliest deadline is on top.
auto later_deadline = [](const Task& a, const Task& b) { return a.deadline > b.deadline; };
priority_queue<Task, vector<Task>, decltype(later_deadline)> by_deadline(later_deadline);
```
<!-- /snippet -->

- The default is a **max-heap**. C#'s `PriorityQueue<TElement, TPriority>` (.NET 6+) dequeues the *smallest* priority first: the opposite default.
- The comparator answers "does a rank below b?", so the same `less` that sorts ascending puts the largest on top, and `greater` gives a min-heap. For a custom order, return true when a should come out *later*.
- `pop()` returns nothing: read `top()` first. There's no iteration in priority order, no find, no erase and no decrease-key (Section 2 shows the workaround).
- Pairs and tuples compare field by field, which gives tie-breaking for free: `(distance, node)` breaks distance ties by node.
- Build from existing data in O(n) with the range constructor, `priority_queue<int> pq(v.begin(), v.end());`, which heapifies instead of pushing n times.
- In C++20, `priority_queue<T, vector<T>, decltype(cmp)> pq;` compiles without passing `cmp`, because captureless lambdas are default-constructible. Passing it, as above, works in every standard and is required when the lambda captures something.
- The negation trick (push −x into a max-heap to get a min-heap) works, except that −INT_MIN overflows.

**k-way merge.** To merge k sorted lists, keep one candidate per list in a min-heap: the smallest remaining element overall is always one of the k candidates. Pop it, output it, push its successor from the same list.

<!-- snippet: modules/13-heaps-tries/examples/priority_queue_basics.cpp#k_way_merge -->
```cpp
// Merges k sorted lists into one sorted list in O(N log k), N = total number of elements.
// The heap holds at most one candidate per list: (value, which list, index in that list).
vector<int> merge_k_sorted(const vector<vector<int>>& lists) {
    using Entry = tuple<int, int, int>;
    priority_queue<Entry, vector<Entry>, greater<Entry>> heap;     // smallest value on top
    for (int i = 0; i < (int)lists.size(); i++)
        if (!lists[i].empty()) heap.push({lists[i][0], i, 0});
    vector<int> out;
    while (!heap.empty()) {
        auto [value, from, idx] = heap.top();       // the smallest remaining value overall
        heap.pop();
        out.push_back(value);
        if (idx + 1 < (int)lists[from].size())      // its list's next element replaces it
            heap.push({lists[from][idx + 1], from, idx + 1});
    }
    return out;
}
```
<!-- /snippet -->

Each of the N elements is pushed and popped once: O(N log k) time, O(k) heap memory. Merging the lists one after another costs O(N·k). Module 08's 23 does this on linked lists. The same frontier idea works when the sorted lists are only *implicit*, such as the rows of a sorted matrix or the sums nums1[i] + nums2[j] as j grows: that's 373.

### Pitfalls

- `priority_queue<int>` is a max-heap, the reverse of C#'s `PriorityQueue`.
- Comparator direction: `sort(v.begin(), v.end(), greater<>())` sorts descending, but `priority_queue<int, vector<int>, greater<>>` puts the *smallest* on top. Read the comparator as "who sinks".
- A comparator must be a *strict weak ordering* (in plain words: behave like `<`, so `cmp(a, a)` is false): `<=` breaks it, and that's undefined behaviour (a heap may silently come out in the wrong order; `std::sort` can read past the end and crash). Never compare by subtraction (`a - b` overflows).
- `top()` or `pop()` on an empty `priority_queue` is undefined behaviour: check `empty()` first.
- Heaps aren't stable: equal-priority elements come out in any order. Add a tiebreaker (an index or a timestamp) when the order matters.
- The underlying container isn't sorted: iterating over a copy of the heap doesn't give priority order.
- Big objects in the heap get copied on every swap: store indices or small tuples.
- Inside templates, write `std::move(x)`: clang flags an unqualified `move` (`-Wunqualified-std-cast-call`) even with `using namespace std;`, and this kit's `-Werror` turns that into a build failure.

### Recognize it when…

- "k-th largest / smallest", "top k", "k closest", especially when k ≪ n or the data arrives as a stream: a size-k heap of the *opposite* type (Section 2).
- "repeatedly take the largest (or smallest), change it, put it back": a heap simulation.
- "merge k sorted …", "k smallest pairs / sums", "k-th smallest in a sorted matrix": a k-way merge frontier.
- "median", "balance two halves", "running statistics of a stream": two heaps (Section 2).
- "tasks", "CPU", "servers", "meeting rooms", "earliest available": an event simulation (Section 2).
- 10^5 operations mixing inserts with "remove the current best": a heap. Re-sorting after each insert is O(n log n) per operation.

### Worked example: 703. Kth Largest Element in a Stream
[LeetCode 703](https://leetcode.com/problems/kth-largest-element-in-a-stream/) · Easy

**Problem (paraphrased):** a class is built from k and an initial list of numbers; each `add(val)` inserts a number and returns the k-th largest number seen so far. Up to 10^4 initial numbers and 10^4 calls; whenever `add` returns, at least k numbers exist.
**Signals:** "k-th largest" plus "stream": keep answering after every insert.
**Brute force, and why it fails:** keep all numbers and re-sort on every `add`: O(n log n) per call, about 3·10^9 comparisons over 10^4 calls on 2·10^4 numbers. A sorted vector with insertion is O(n) per call: better, but the memory still grows with the stream, and it keeps numbers that can never matter again.
**Key insight:** numbers below the current k-th largest can never become the answer again. Keep only the k largest seen so far, in a **min**-heap: its top is the smallest of them, which is exactly the k-th largest. A new number either beats the top (push it, then pop the top) or can never matter.
**Dry run:** k = 3, nums = [4,5,8,2]. The constructor pushes 4, 5, 8, then 2, and pops 2: the heap is {4, 5, 8}.

| add | heap after push | size > 3, so pop | heap | returns top |
|---|---|---|---|---|
| 3 | {3, 4, 5, 8} | 3 | {4, 5, 8} | 4 |
| 5 | {4, 5, 5, 8} | 4 | {5, 5, 8} | 5 |
| 10 | {5, 5, 8, 10} | 5 | {5, 8, 10} | 5 |
| 9 | {5, 8, 9, 10} | 5 | {8, 9, 10} | 8 |
| 4 | {4, 8, 9, 10} | 4 | {8, 9, 10} | 8 |

<!-- snippet: modules/13-heaps-tries/examples/0703-kth-largest-element-in-a-stream.cpp#solution -->
```cpp
class KthLargest {
public:
    KthLargest(int k, vector<int>& nums) : k(k) {
        for (int x : nums) add(x);
    }

    int add(int val) {
        top_k.push(val);
        if ((int)top_k.size() > k) top_k.pop();   // evict the smallest: it can't be among the k largest
        return top_k.top();                        // the smallest of the k largest = the k-th largest
    }

private:
    int k;
    priority_queue<int, vector<int>, greater<int>> top_k;   // min-heap holding the k largest values so far
};
```
<!-- /snippet -->

**Complexity:** O(log k) per `add`, since the heap never holds more than k + 1 numbers; O(n log k) to construct; O(k) memory however long the stream runs.
**Edge cases:**
- Fewer than k initial numbers (k = nums.size() + 1 is allowed): the heap is simply smaller until enough arrive; the code only pops above size k.
- k = 1: a running maximum.
- Duplicates are separate elements: the second official example holds four 7s.

**Follow-ups:**
- *Why not a max-heap of everything?* O(n) memory, and digging out the k-th means k pops per query.
- *Numbers can also be removed?* The size-k trick breaks (an evicted number might be needed again). Keep all numbers in an order-statistics structure, or a `multiset` with an iterator parked at the k-th largest.
- *k close to n?* For a one-shot array, mirror it: the k-th largest is the (n − k + 1)-th smallest, so keep the n − k + 1 smallest in a max-heap, whichever side is smaller. (In a stream n keeps growing, so the mirror needs a second heap, as in section 2's median.)

## 2. Priority queues — top-K, running median with two heaps, task scheduling

### Concept

**Top-K with a size-k heap.** To keep the k largest elements of a stream, hold them in a min-heap of size k. Its top is the weakest of the current winners: the gatekeeper. For each new element, push it, and if the size exceeds k, pop the top. Invariant: the heap holds the k largest elements seen so far; a new element either enters and evicts the weakest winner, or is smaller than all k winners and can never become one. O(n log k) time, O(k) memory. Always choose the heap type *opposite* to the goal: the k largest live in a min-heap, the k smallest in a max-heap.

| Approach for "the k largest" | Time | Extra memory | Use it when |
|---|---|---|---|
| sort, take the last k | O(n log n) | O(1) in place | one-shot, simplest |
| size-k min-heap | O(n log k) | O(k) | streams; k ≪ n |
| heapify everything, pop k times | O(n + k log n) | O(n) | one-shot, k small |
| quickselect / `std::nth_element` | O(n) on average | O(1) | one-shot, only the k-th element (or an unordered top k) |
| counting / buckets | O(n + range of values) | O(range) | small value ranges, frequencies |

215 is the one-shot version of this table: try the heap and quickselect, and compare with `nth_element`.

**Two heaps: the running median.** Split everything seen so far at the middle: the smaller half in a max-heap `low`, the larger half in a min-heap `high`. Keep two invariants: (1) every value in `low` ≤ every value in `high`; (2) `low` has the same size as `high`, or one more. Then the median is `low.top()` when the count is odd and the average of the two tops when it's even.

```
      low (max-heap)             high (min-heap)
  [ 1  2  3  (5) ]     |     [ (6)  8  9 ]       7 values: the median is low's top, 5
     smaller half    top | top   larger half     8 values: the average of the two tops
```

Inserting keeps both invariants in three moves: push into `low`; move `low`'s maximum into `high` (the ordering holds again); if `high` is now bigger, move its minimum back. O(log n) per insert, O(1) per median. The same "two heaps with a boundary between them" shape gives any fixed percentile (keep a size ratio instead of equal sizes). Worked below (295).

**Event simulations.** CPU schedulers, servers, meeting rooms and printers are all one loop over time:
1. Sort the arrivals by time.
2. Keep a heap of whatever gets served next: resources keyed by when they become free, or waiting jobs keyed by priority.
3. Move the clock from event to event, never tick by tick: when nothing is ready, jump straight to the next arrival.
4. Break ties explicitly by putting an index in the tuple.

<!-- snippet: modules/13-heaps-tries/examples/event_simulation.cpp#simulation -->
```cpp
// k identical printers serve jobs first come, first served. jobs[i] = {arrival, duration}, sorted
// by arrival. Returns each job's finish time. O(n log k).
vector<long long> finish_times(const vector<pair<int, int>>& jobs, int k) {
    // One entry per printer: the time it becomes free. Min-heap: the soonest-free printer on top.
    priority_queue<long long, vector<long long>, greater<long long>> free_at;
    for (int i = 0; i < k; i++) free_at.push(0);
    vector<long long> finish;
    for (auto [arrival, duration] : jobs) {
        // Start when the job has arrived AND a printer is free. If a printer is already idle, the
        // max() jumps straight to the arrival: the clock moves event to event, never tick by tick.
        long long start = max<long long>(arrival, free_at.top());
        free_at.pop();
        long long done = start + duration;     // long long: n durations of up to 1e9 overflow int
        finish.push_back(done);
        free_at.push(done);
    }
    return finish;
}
```
<!-- /snippet -->

O(n log k) for n jobs and k printers. 1834 has a single CPU, so no heap of free times; instead it keeps a heap of *waiting tasks* keyed by (duration, index): write that one yourself. Module 11's 2406 and 1353 use the same heap of end times.

**Lazy deletion.** `std::priority_queue` can't erase an arbitrary element or change a priority (there's no decrease-key). Don't fight it: leave dead entries in the heap and throw them away when they reach the top.
- **Erase by value:** count pending deletions in a hash map; before reading the top, pop while the top is marked; keep your own live size, because `heap.size()` includes the dead entries.
- **Update by id** (the decrease-key substitute): push (new priority, id) and record `current[id]`. When an entry surfaces whose priority no longer matches `current[id]`, it's stale: pop it and move on. Dijkstra (module 15's 743) works exactly like this, with `dist[]` as `current`.

<!-- snippet: modules/13-heaps-tries/examples/lazy_deletion.cpp#by_value -->
```cpp
// A min-heap with erase(x). Erased values stay inside the heap until they reach the top, where
// they're thrown away. Each pushed copy is thrown away at most once, so the cleanup is amortized
// into the pushes: every operation is still O(log n) amortized.
struct ErasableMinHeap {
    priority_queue<int, vector<int>, greater<int>> heap;
    unordered_map<int, int> doomed;     // value -> copies erased but still inside `heap`
    int live = 0;                       // heap.size() counts the doomed copies too; this doesn't

    void push(int x) { heap.push(x); live++; }
    void erase(int x) { doomed[x]++; live--; }        // precondition: a live copy of x is stored
    int top() { discard_doomed(); return heap.top(); }
    void pop() { discard_doomed(); heap.pop(); live--; }
    int size() const { return live; }

    void discard_doomed() {             // invariant afterwards: heap.top() (if any) is live
        while (!heap.empty()) {
            auto it = doomed.find(heap.top());
            if (it == doomed.end()) return;
            heap.pop();
            if (--it->second == 0) doomed.erase(it);
        }
    }
};
```
<!-- /snippet -->

<!-- snippet: modules/13-heaps-tries/examples/lazy_deletion.cpp#by_version -->
```cpp
// "Change the priority of item id" without decrease-key: push a fresh entry and remember the
// current priority. An entry that disagrees with `current` is stale and is skipped when it
// surfaces. (Dijkstra does exactly this with its dist[] array; see module 15.)
struct UpdatableMaxHeap {
    priority_queue<pair<int, int>> heap;    // (priority, id): highest priority on top
    unordered_map<int, int> current;        // id -> its live priority; absent = not stored

    void set(int id, int priority) {        // insert or update: O(log n)
        current[id] = priority;
        heap.push({priority, id});
    }
    void remove(int id) { current.erase(id); }
    bool empty() { discard_stale(); return heap.empty(); }
    pair<int, int> top() { discard_stale(); return heap.top(); }    // (priority, id)
    void pop() {
        discard_stale();
        current.erase(heap.top().second);
        heap.pop();
    }

    void discard_stale() {
        while (!heap.empty()) {
            auto [priority, id] = heap.top();
            auto it = current.find(id);
            if (it != current.end() && it->second == priority) return;   // live entry
            heap.pop();                     // stale: updated or removed since it was pushed
        }
    }
};
```
<!-- /snippet -->

Cost: every pushed entry is popped at most once, so the cleanup is paid for by the pushes: O(log P) amortized per operation, where P is the number of pushes (dead entries make the heap bigger than the live set). If dead entries could pile up, or you need the minimum and the maximum at once, use a `std::multiset` instead: erase is a real O(log n) operation there, at a bigger constant.

### Pitfalls

- Averaging two ints: `(a + b) / 2` truncates and can overflow. Convert first: `(a + (double)b) / 2`.
- Two heaps: pushing into the wrong heap first, or skipping the rebalance step, silently breaks the size invariant; the median is then wrong only on some inputs.
- Lazy deletion: using `heap.size()` as the size; reading `top()` without discarding dead entries first; in two-heap designs, rebalancing by raw sizes instead of live sizes, or cleaning only one of the heaps.
- Event simulations: ticking the clock one unit at a time (times up to 10^9 means TLE); `int` finish times (a sum of durations overflows); no tie-breaker.
- Top-K: a heap of all n elements when k ≪ n wastes memory; the wrong heap type (a max-heap pops the largest, the very elements you want to keep).

### Recognize it when…

- "k-th largest", "top k", "k closest", "k most frequent": a size-k heap, or quickselect when it's one-shot.
- "median of a stream", "balance", "split into two halves as numbers arrive": two heaps.
- "tasks with arrival and processing times", "servers", "rooms", "CPU": an event simulation.
- "remove an element from the heap", "priorities change", "outdated entries": lazy deletion, or `std::set`.
- Values arrive online and every query asks about the current state: update a structure incrementally instead of recomputing.

### Worked example: 295. Find Median from Data Stream
[LeetCode 295](https://leetcode.com/problems/find-median-from-data-stream/) · Hard

**Problem (paraphrased):** support adding integers one at a time and asking, at any point, for the median of everything added so far (the average of the two middle values when the count is even). Up to 5·10^4 calls.
**Signals:** "median", "data stream", adds and queries interleaved.
**Brute force, and why it fails:** keep a sorted vector: each insert shifts O(n) elements, so 5·10^4 inserts cost O(n²), about 1.25·10^9 moves in the worst case. Re-sorting per query is worse.
**Key insight:** the median depends only on the two values next to the boundary between the smaller and the larger half. A max-heap of the smaller half and a min-heap of the larger half expose exactly those two values, and an insert only ever moves one element across the boundary.
**Dry run:** add 1, 2, 3:

| add | 1. push into low | 2. low's max → high | 3. rebalance | low / high | median |
|---|---|---|---|---|---|
| 1 | low {1} | low {}, high {1} | high is bigger: 1 goes back | {1} / {} | 1 |
| 2 | low {1, 2} | 2 → high: low {1}, high {2} | sizes equal | {1} / {2} | 1.5 |
| 3 | low {1, 3} | 3 → high: low {1}, high {2, 3} | high is bigger: 2 goes back | {1, 2} / {3} | 2 |

<!-- snippet: modules/13-heaps-tries/examples/0295-find-median-from-data-stream.cpp#solution -->
```cpp
class MedianFinder {
public:
    MedianFinder() {}

    void addNum(int num) {
        low.push(num);                      // 1. put it in the low half...
        high.push(low.top());               // 2. ...then hand the low half's max to the high half:
        low.pop();                          //    now every low value <= every high value again
        if (high.size() > low.size()) {     // 3. restore the size rule
            low.push(high.top());
            high.pop();
        }
    }

    double findMedian() {
        if (low.size() > high.size()) return low.top();    // odd count: the middle is low's max
        return (low.top() + (double)high.top()) / 2;       // even: convert BEFORE adding (no int overflow)
    }

private:
    // Invariants: every value in low <= every value in high, and
    //             low.size() == high.size() or low.size() == high.size() + 1.
    priority_queue<int> low;                                // max-heap: the smaller half
    priority_queue<int, vector<int>, greater<int>> high;    // min-heap: the larger half
};
```
<!-- /snippet -->

**Complexity:** `addNum` O(log n) (at most five pushes and pops), `findMedian` O(1), O(n) memory.
**Edge cases:**
- One value: its own median.
- Duplicates can sit on both sides; the invariant only needs ≤.
- Values near INT_MAX: the tests average INT_MAX and INT_MAX − 2, which overflows if you add them as ints.
- Input order doesn't matter: the "move back" step runs on exactly the inserts that make the count odd.

**Follow-ups:**
- *All values in [0, 100]?* 101 counters: O(1) per add, and a scan of 101 buckets per median.
- *99% of values in [0, 100]?* Counters for the range, plus a count (or small heaps) for the values below and above it.
- *Only the last k values count (a sliding window)?* Values must also leave: two heaps with lazy deletion, or two `multiset`s.

## 3. Tries — insert, search, prefix counting, memory trade-offs

### Concept

A trie stores strings by their characters: each node stands for a prefix, each edge appends one character, and the root is the empty prefix. A word is a path from the root whose last node carries a word-end mark. Words that share a prefix share its nodes.

```
words: car, cart, cat, dog         number = pass: stored words running through the node
                                   *      = a stored word ends here (end = 1)
            (root) 4
           /        \
         c 3        d 1
          |          |
         a 3        o 1
        /   \        |
     r 2*   t 1*    g 1*
      |
     t 1*
```

Every operation walks one path, so it costs O(L) for a string of length L, however many strings are stored:

| Operation | Trie | Hash set of strings | Sorted vector |
|---|---|---|---|
| insert | O(L) | O(L) average | O(n) shifting |
| exact lookup | O(L) | O(L) average | O(L log n) |
| does any word start with p / how many | O(\|p\|) with counts | no (scan all: O(n·L)) | O(\|p\| log n): two binary searches |
| list the words starting with p, in order | O(\|p\| + subtree) | no | O(\|p\| log n + output) |

So for exact lookups only, a hash set is simpler and as fast. A trie earns its place on prefix questions, especially when you walk it one character at a time (autocomplete as the user types, word search on a grid), because each step continues from the previous node in O(1).

**Node designs.** Measured on this machine (Apple clang, `-O2`, 64-bit) with 200,000 random words of 3 to 10 letters, about 687,000 nodes; treat the numbers as rough:

| Node design | Bytes per node | Insert all | 200k lookups | Notes |
|---|---|---|---|---|
| `array<Node*, 26>` + flag, one `new` per node | 216, plus malloc overhead | ~33 ms | ~6 ms | simplest; fast; wasteful when most nodes have one child; one allocation per node |
| `unordered_map<char, Node*>` | ~100 on this data | ~53 ms | ~18 ms | any alphabet; about 3 allocations per node; ~3× slower lookups; smaller only when nodes have few children |
| pool: `vector<Node>`, `array<int, 26>` | 112, plus unused vector capacity (up to 2×) | ~33 ms | ~5 ms | half the size of pointers; one growing buffer; indices survive reallocation; reset with one assign |

A `map<char, Node*>` keeps children sorted (handy for a lexicographic DFS) at a higher cost. For a small fixed alphabet, the array is almost always the right call. C# bridge: `new Node()` under a garbage collector is cheap and freeing is automatic; in C++ every `new` is an allocator call and nothing is freed unless you arrange it (owning `unique_ptr` children, as in 208 below, or a pool that dies with its vector).

**Insert, search, startsWith.** Walk from the root: insert creates missing children as it goes; search needs the whole path AND a word-end mark on the last node; startsWith only needs the path. Worked below with pointers (208); the template does it with a pool.

**Prefix counting.** Give every node `pass`, the number of stored words running through it. Then "how many words start with p" is the `pass` of p's node: O(|p|). Make `end` a count rather than a flag and duplicates work too.

<!-- snippet: templates/trie.hpp#trie -->
```cpp
struct Trie {
    struct Node {
        array<int, 26> child;   // child[c] = index in `nodes` of the node for (this prefix + c), or -1
        int pass = 0;           // stored words whose path runs through here = words with this prefix
        int end = 0;            // stored words that end exactly here (a count, so duplicates work)
        Node() { child.fill(-1); }
    };
    vector<Node> nodes = {Node()};   // nodes[0] is the root: the empty prefix, passed by every word

    void insert(const string& word) {                 // O(L)
        int cur = 0;
        nodes[cur].pass++;
        for (char ch : word) {
            int c = ch - 'a';
            if (nodes[cur].child[c] == -1) {
                nodes[cur].child[c] = (int)nodes.size();   // the index the new node is about to get
                nodes.emplace_back();                      // may reallocate: never hold a Node& across this
            }
            cur = nodes[cur].child[c];
            nodes[cur].pass++;
        }
        nodes[cur].end++;
    }

    // Index of the node spelling s, or -1 if the path leaves the trie. After erases the node may
    // be dead (pass == 0), so the queries below look at the counts, not just at the index.
    int find(const string& s) const {                 // O(L)
        int cur = 0;
        for (char ch : s) {
            cur = nodes[cur].child[ch - 'a'];
            if (cur == -1) return -1;
        }
        return cur;
    }

    bool contains(const string& word) const {
        int v = find(word);
        return v != -1 && nodes[v].end > 0;           // the path exists AND a word ends there
    }

    int count_prefix(const string& prefix) const {    // stored words (with repeats) starting with prefix
        int v = find(prefix);
        return v == -1 ? 0 : nodes[v].pass;
    }

    bool starts_with(const string& prefix) const { return count_prefix(prefix) > 0; }
```
<!-- /snippet -->

**Deletion.** Two ways:
1. **By counts (lazy):** check the word is stored, then undo its insert's increments along the path. Nodes stay allocated; a node with `pass == 0` is dead and reads as absent, and a later insert through it brings it back. That's the template's `erase`, O(L).
2. **Physically:** recurse down the word, and on the way back delete every child that has no children left and no word ending at it. Needed when memory must actually be returned (a long-running program with a pointer trie).

<!-- snippet: templates/trie.hpp#erase -->
```cpp
    // Removes one copy of word: undo exactly the increments its insert made. Nodes are not freed:
    // a node whose pass drops to 0 is dead, and a later insert through it brings it back.
    bool erase(const string& word) {                  // O(L)
        if (!contains(word)) return false;            // a missing word must not touch any count
        int cur = 0;
        nodes[cur].pass--;
        for (char ch : word) {
            cur = nodes[cur].child[ch - 'a'];
            nodes[cur].pass--;
        }
        nodes[cur].end--;
        return true;
    }
};
```
<!-- /snippet -->

### Pitfalls

- `ch - 'a'` assumes lowercase a–z. An uppercase letter, digit or space indexes out of bounds (ASan catches it here; a judge may not). Map a bigger alphabet explicitly.
- A reference into a vector-backed pool dangles as soon as the vector grows:

```c++
Node& node = nodes[cur];
nodes.emplace_back();              // may move every node to a new buffer...
node.child[c] = new_index;         // ...so `node` points into freed memory: heap-use-after-free
```

  Hold indices, not references, across any push or emplace. (The template writes the new index into the parent *before* growing the pool.)
- search vs startsWith: search must check the word-end mark. "app" isn't a word just because "apple" is.
- Erasing a word that isn't stored must not touch any count: check first.
- Memory: 10^5 words of 10 letters can mean 10^6 nodes, 216 MB with pointer arrays and 112 MB with a pool. Estimate before you pick a design.
- After lazy deletion, "the node exists" no longer means "some word has this prefix": test `pass > 0`.
- Recursion over a trie (DFS, physical delete) goes as deep as the longest word: fine at 2,000, not at 10^5-character strings.

### Recognize it when…

- "prefix", "starts with", "autocomplete", "search suggestions", "a dictionary of words" plus many queries: a trie.
- A search pattern with wildcards ('.' matches any letter): a DFS over the trie that branches at the wildcard (211).
- "count the words with this prefix", "erase a word": counts on the nodes (1804: `templates/trie.hpp` shows the counters, so write it from memory before you peek).
- "the longest word all of whose prefixes are words": a DFS that only steps onto word-end nodes (1858).
- "the number of distinct substrings": every suffix inserted into a trie, then count what you built (1698, section 4).
- Many words searched in one grid or text at once: a trie of the words, walked during the search (212, section 4).
- Bits instead of letters (maximum XOR pairs): a binary trie, module 18's 421.

### Worked example: 208. Implement Trie (Prefix Tree)
[LeetCode 208](https://leetcode.com/problems/implement-trie-prefix-tree/) · Medium

**Problem (paraphrased):** implement `insert(word)`, `search(word)` (is this exact word stored?) and `startsWith(prefix)` (does any stored word start with it?). Lowercase letters, lengths up to 2,000, up to 3·10^4 calls.
**Signals:** "prefix", and the insert / search / startsWith trio: the canonical trie API.
**Brute force, and why it fails:** a vector of words makes search and startsWith O(n·L); a hash set fixes search but not startsWith; a sorted vector answers startsWith with `lower_bound` in O(L log n) but inserts in O(n).
**Key insight:** make the prefixes themselves the nodes. Every operation becomes one walk of at most L steps, whatever n is, and the only difference between search and startsWith is whether the last node must carry the word-end mark.
**Dry run:** insert("apple") creates a → p → p → l → e and marks e. search("app") walks a → p → p, which exists but isn't marked: false. startsWith("app"): the path exists, true. insert("app") marks the second p, so search("app") is now true.

<!-- snippet: modules/13-heaps-tries/examples/0208-implement-trie-prefix-tree.cpp#solution -->
```cpp
class Trie {
public:
    Trie() {}

    void insert(string word) {
        Node* cur = &root;
        for (char ch : word) {
            unique_ptr<Node>& next = cur->child[ch - 'a'];
            if (!next) next = make_unique<Node>();      // create the edge the first time it's needed
            cur = next.get();
        }
        cur->is_word = true;
    }

    bool search(string word) {
        const Node* node = walk(word);
        return node && node->is_word;       // the path must exist AND a word must end there
    }

    bool startsWith(string prefix) {
        return walk(prefix) != nullptr;     // no deletions, so every node lies on some word's path
    }

private:
    struct Node {
        array<unique_ptr<Node>, 26> child;  // owning pointers: destroying the root frees the whole trie
        bool is_word = false;               // a stored word ends here (not just passes through)
    };
    Node root;                              // the empty prefix

    // The node reached by spelling s from the root, or nullptr if the path breaks off.
    const Node* walk(const string& s) const {
        const Node* cur = &root;
        for (char ch : s) {
            cur = cur->child[ch - 'a'].get();
            if (!cur) return nullptr;
        }
        return cur;
    }
};
```
<!-- /snippet -->

**Complexity:** O(L) time per call. Insert allocates at most L nodes, so memory is O(total characters inserted) nodes of about 216 bytes each.
**Edge cases:**
- One word is a prefix of another ("app" and "apple"): the word-end mark tells them apart.
- A query longer than every stored word: the walk falls off the trie and returns nullptr.
- Inserting the same word twice changes nothing.
- A 2,000-letter word: destroying the trie frees 2,000 nested nodes recursively, which is fine at that depth (tested).

**Follow-ups:**
- *Why a pool instead?* `templates/trie.hpp`: `vector<Node>` with int indices, half the bytes per node, one buffer, cheap to reset between test cases.
- *A big or Unicode alphabet?* Children in an `unordered_map`, or a small sorted vector of (character, child) pairs per node.
- *Memory too high?* Merge chains of single-child nodes into one edge labelled with a string: a radix (Patricia) tree.

## 4. Applications — autocomplete, spell check, streaming statistics

### Concept

**Autocomplete: three designs.**

| Design | Build | Query for prefix p (up to k results) | Memory | Pick it when |
|---|---|---|---|---|
| sort + `lower_bound` | O(n log n) string comparisons | O(\|p\| log n) to find where the matches start, then read k; as the user types, the start only moves right | just the words | a fixed list; the interview default (1268) |
| trie + DFS on demand | O(total characters) | walk to p's node, DFS the children in a–z order, stop after k words: O(\|p\| + nodes explored), which can be large for a big subtree with few words | the nodes | the list changes often; queries are rare |
| trie + top-k list per node | O(total characters · k) | walk to p's node and read its list: O(\|p\| + k) | nodes + k indices per node | many queries on a stable list; ranking by popularity (refresh the lists along a word's path when its score changes) |

Store indices in per-node lists, never copies of the strings.

**Spell check (trie + edit distance), in one paragraph.** To find every dictionary word within edit distance d of a query, don't fill a full edit-distance table (module 16's 72) against each word. Walk the trie instead: each node's DP row ("this prefix vs the query") is computed from its parent's row in O(|query|), so words that share a prefix share the work. When the smallest value in a node's row exceeds d, no word below that node can get back within d, so skip the whole subtree. Most of the dictionary is never visited.

**Many words in one grid (212): trie + backtracking.** Searching word by word repeats the grid DFS once per word. Put all the words in a trie instead and run one backtracking DFS per starting cell that may only follow trie edges, so a path that is no word's prefix dies immediately. The pruning that makes it fast: once a word is found, stop looking for it, and stop entering trie branches that no longer lead to any unfound word. The grid mechanics (mark a cell visited, restore it on the way back) are module 10's 79.

**Distinct substrings (1698).** A trie holding every suffix of s contains every substring of s exactly once, as a path from the root. That's O(n²) nodes, fine for n in the low thousands; module 18's suffix structures do it faster.

**Streaming statistics.** Pick the structure by the statistic:

| Statistic over a stream | Structure | Cost per update / query |
|---|---|---|
| running sum or mean | two variables | O(1) |
| sliding-window sum or mean | queue + running sum | O(1) |
| sliding-window max or min | monotonic deque (module 09's 239) | O(1) amortized |
| k-th largest so far | size-k min-heap (703) | O(log k) / O(1) |
| median so far | two heaps (295) | O(log n) / O(1) |
| sliding-window median | two heaps + lazy deletion, or two `multiset`s | O(log n) |
| k most frequent so far | hash map of counts + `set` of (count, key), re-inserting a key when its count changes | O(log n) / O(k) |
| how many words start with p | trie with pass counts | O(L) |

### Pitfalls

- Per-node top-k lists holding string copies: memory multiplies by k·L. Store indices.
- Recomputing each keystroke's suggestions from scratch instead of continuing from the previous prefix's node or position.
- The sorted scan must stop at the first word that doesn't start with the prefix; checking `word < something` instead of starts-with runs past the end of the run.
- A lexicographic DFS must visit children in a–z order: array children do that; `unordered_map` children don't.
- 212: forgetting to restore a cell after the DFS, reporting a word twice, or skipping the pruning (boards full of one letter with many near-identical words time out).

### Recognize it when…

- "suggest", "autocomplete", "after each character typed": one of the three designs (1268).
- "find all dictionary words in a grid": trie + backtracking (212).
- "words within k edits": trie + edit-distance rows.
- "number of distinct substrings" with n up to a few thousand: a trie of suffixes (1698).
- A stream plus a statistic: pick from the table above.

### Worked example: 1268. Search Suggestions System
[LeetCode 1268](https://leetcode.com/problems/search-suggestions-system/) · Medium

**Problem (paraphrased):** given distinct product names and a search word, after each typed character of the search word return up to three product names, lexicographically smallest first, that start with the typed prefix. Up to 1,000 products with total length up to 2·10^4; the search word has up to 1,000 characters.
**Signals:** "after each character", "starts with the prefix", "the three lexicographically smallest".
**Brute force, and why it fails:** sort once, then for every prefix scan the whole list for the first three matches: O(|searchWord| · total length), about 2·10^7 character comparisons. It passes these limits, but it rescans everything on every keystroke; a real autocomplete has millions of names and a per-keystroke budget of microseconds.
**Key insight:** in sorted order, the names starting with a prefix form one contiguous run that begins at `lower_bound(prefix)`, and as the prefix grows the run's start only moves right. Alternatively, a trie built from the sorted names can keep, at every node, the first three names that pass through it: each keystroke is then one step down plus reading at most three names.
**Dry run:** sorted products [mobile, moneypot, monitor, mouse, mousepad], search word "mouse":

| prefix | `lower_bound` lands on | first three of the run |
|---|---|---|
| m | mobile | mobile, moneypot, monitor |
| mo | mobile | mobile, moneypot, monitor |
| mou | mouse | mouse, mousepad |
| mous | mouse | mouse, mousepad |
| mouse | mouse | mouse, mousepad |

Sort + `lower_bound`:

<!-- snippet: modules/13-heaps-tries/examples/1268-search-suggestions-system.cpp#sorted -->
```cpp
class Solution {
public:
    vector<vector<string>> suggestedProducts(vector<string>& products, string searchWord) {
        sort(products.begin(), products.end());
        vector<vector<string>> result;
        string prefix;
        auto start = products.begin();      // first product >= prefix; it only ever moves right
        for (char ch : searchWord) {
            prefix += ch;
            // The words starting with prefix form one contiguous run in sorted order, beginning
            // at the first word >= prefix. A longer prefix is larger, so search from `start` on.
            start = lower_bound(start, products.end(), prefix);
            vector<string> suggestions;
            for (auto it = start; it != products.end() && suggestions.size() < 3; ++it) {
                if (it->compare(0, prefix.size(), prefix) != 0) break;   // left the run (C++20: starts_with)
                suggestions.push_back(*it);
            }
            result.push_back(suggestions);
        }
        return result;
    }
};
```
<!-- /snippet -->

Trie with a top-3 list per node:

<!-- snippet: modules/13-heaps-tries/examples/1268-search-suggestions-system.cpp#trie -->
```cpp
class Solution {
public:
    vector<vector<string>> suggestedProducts(vector<string>& products, string searchWord) {
        // Insert in sorted order: the first 3 words to pass through a node are its 3 smallest.
        sort(products.begin(), products.end());
        vector<Node> nodes(1);                               // pool: nodes[0] is the root
        for (int i = 0; i < (int)products.size(); i++) {
            int cur = 0;
            for (char ch : products[i]) {
                int c = ch - 'a';
                if (nodes[cur].child[c] == -1) {
                    nodes[cur].child[c] = (int)nodes.size(); // index first, then grow the pool
                    nodes.emplace_back();
                }
                cur = nodes[cur].child[c];
                if (nodes[cur].top3.size() < 3) nodes[cur].top3.push_back(i);
            }
        }
        vector<vector<string>> result;
        int cur = 0;                                         // -1 once the prefix has left the trie
        for (char ch : searchWord) {
            if (cur != -1) cur = nodes[cur].child[ch - 'a'];
            vector<string> suggestions;
            if (cur != -1)
                for (int i : nodes[cur].top3) suggestions.push_back(products[i]);
            result.push_back(suggestions);
        }
        return result;
    }

private:
    struct Node {
        array<int, 26> child;
        vector<int> top3;        // indices into the sorted products: 4 bytes each, not a string copy
        Node() { child.fill(-1); }
    };
};
```
<!-- /snippet -->

**Complexity:** with S = total length of the products, L = the longest product and m = the search word's length: sorting is O(n log n) string comparisons for both. Sort + `lower_bound` then costs O(m · L log n) for the searches plus O(m · 3L) to copy results. The trie builds in O(S) and answers each keystroke in O(1) plus the copying; its memory is O(S) nodes of 26 ints and up to 3 indices each.
**Edge cases:**
- The prefix leaves the product set early ("tatiana" against ["havana"]): every later answer is empty. The sorted version's run is empty; the trie version parks `cur` at −1.
- A product that is a prefix of others ("ab", "abc", …) sorts first and is suggested first.
- A search word longer than every product: the trailing answers are empty.

**Follow-ups:**
- *Products added and removed live?* A trie with counts; either update the per-node lists along the word's path or switch to DFS on demand.
- *Rank by popularity instead of alphabetically?* Each node keeps its top three by (−count, name); a count change walks that word's path and refreshes the lists.
- *Which one in an interview?* Start with sort + `lower_bound` (short and obviously correct), then offer the trie when asked to cut the per-keystroke cost.

## Common mistakes

- Forgetting that `priority_queue` is a max-heap; `greater<>` makes a min-heap.
- Reading the comparator backwards: it names who sinks, not who wins.
- Expecting `pop()` to return the element (it returns void), or calling `top()` on an empty heap.
- Building a heap from n known values with n pushes instead of the range constructor or `make_heap`.
- The size-k heap of the wrong type: the k largest belong in a min-heap.
- Integer division or overflow when averaging two middle values.
- Lazy deletion without a separate live count, or reading `top()` before discarding dead entries.
- Ticking a simulation clock one unit at a time instead of jumping to the next event.
- Trie: search without checking the word-end mark; characters outside a–z; holding a `Node&` across a pool `emplace_back`.
- Underestimating trie memory: 26 pointers per node adds up fast.

## Say it out loud

A talk track for 295:

1. **Restate:** "Numbers arrive one at a time, and at any point I must return the median of everything so far, averaging the two middle values when the count is even."
2. **Brute force:** "A sorted vector: O(n) per insert because of shifting, O(1) median. That's O(n²) over the stream."
3. **Insight:** "The median only depends on the two values at the boundary between the lower and upper halves. A max-heap for the lower half and a min-heap for the upper half expose exactly those, and each insert moves at most one element across."
4. **Complexity:** "O(log n) per insert, O(1) per median, O(n) memory."
5. **Edge cases:** "One element, duplicates, and adding two large ints when averaging: I convert to double first."

Follow-ups you'll hear in heap and trie rounds:
- "Why not keep a sorted list?" O(n) inserts; the heaps give O(log n).
- "What if elements can be deleted?" Lazy deletion with live counts, or two `multiset`s.
- "What about the 90th percentile?" Keep the heaps at a 9 : 1 size ratio instead of equal sizes.
- "What if all values are in a small range?" Counting buckets beat heaps.
- "How much memory does your trie use?" Nodes × (26 children × 4 or 8 bytes + counters); then offer the pool or a map per node.
- "Why a trie and not a hash set?" Prefix queries, and walking one character at a time.

## Self-check

1. In a 0-based array heap, what are the children and the parent of index i, and why does a heap have no gaps in its array?
<details><summary>Answer</summary>Children 2i + 1 and 2i + 2, parent (i − 1) / 2. A heap is a complete binary tree (every level full except the last, filled from the left), so n elements occupy exactly indices 0..n − 1.</details>

2. Why does sift-down swap with the larger child in a max-heap?
<details><summary>Answer</summary>The child that moves up becomes the parent of its former sibling, so it must be at least as large as that sibling. Swapping with the smaller child would leave a parent smaller than its new child.</details>

3. Why is bottom-up heapify O(n) when n pushes can cost O(n log n)?
<details><summary>Answer</summary>Sift-down from a node costs its height, and most nodes are near the bottom: n/2 leaves cost 0, n/4 nodes cost at most 1, and so on, which sums to under n swaps (Σ h / 2^h = 2). A push costs its node's depth, and most nodes are deep, so n pushes of ascending values cost about n log2 n.</details>

4. Declare a min-heap of (distance, node) pairs. What does the comparator of a `priority_queue` mean?
<details><summary>Answer</summary>`priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;` The comparator answers "does a rank below b?"; the element that ranks below nothing is on top. `less` (the default) therefore gives a max-heap and `greater` a min-heap.</details>

5. To report the k largest values of a stream, which heap do you keep, of what size, and at what cost?
<details><summary>Answer</summary>A min-heap of size k: its top is the smallest of the current k largest, the one to evict when something bigger arrives. O(log k) per element, O(k) memory.</details>

6. State the two invariants of the two-heap median and where the median comes from.
<details><summary>Answer</summary>Every value in the max-heap `low` is ≤ every value in the min-heap `high`, and |low| = |high| or |low| = |high| + 1. The median is `low.top()` for an odd count and the average of both tops (computed in double) for an even count.</details>

7. `priority_queue` has no erase and no decrease-key. Describe both lazy-deletion variants, including what "size" means.
<details><summary>Answer</summary>Erase by value: record pending deletions in a hash map and pop marked values when they reach the top; the live size is your own counter, not `heap.size()`. Update by id: push (new priority, id) and store `current[id]`; an entry whose priority differs from `current[id]` (or whose id was removed) is stale and is skipped when it surfaces. Each entry is popped at most once, so costs stay O(log P) amortized.</details>

8. Roughly how many bytes does a trie node take with `array<Node*, 26>` and with `array<int, 26>`, and when would you use an `unordered_map` for children?
<details><summary>Answer</summary>About 216 bytes with pointers (plus allocator overhead per node) and about 112 bytes with int indices in a pool. Use a map when the alphabet is large or unknown (Unicode, arbitrary bytes), accepting slower lookups and several allocations per node.</details>

9. What's wrong with `Node& n = nodes[cur]; nodes.emplace_back(); n.child[c] = idx;` in a vector-backed trie?
<details><summary>Answer</summary>`emplace_back` may reallocate the vector, moving every node; `n` then refers to freed memory (heap-use-after-free). Keep an index and re-index `nodes[cur]` after growing, or write the child index before growing.</details>

10. For 1268, compare sort + `lower_bound` with a trie that stores the top 3 names per node. Which would you code first in an interview?
<details><summary>Answer</summary>Sort + `lower_bound`: O(n log n) sort, then O(L log n) per keystroke, no extra structure, short and easy to get right. Trie with top-3 lists: O(S) build, O(1) per keystroke plus copying, more memory and code. Start with the sorted version and offer the trie as the optimization.</details>
