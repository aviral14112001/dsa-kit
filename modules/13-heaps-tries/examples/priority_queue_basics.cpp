// std::priority_queue in practice: max-heap by default, min-heap with greater<>, pair ordering,
// custom comparators; the k-way merge; heap sort with the <algorithm> heap functions. Module 13 section 1.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

struct Task {
    string name;
    int deadline;
};

// [snippet:k_way_merge]
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
// [/snippet]

// [snippet:heap_sort]
// Heap sort with the <algorithm> heap functions, which work on any random-access range.
void heap_sort(vector<int>& v) {
    make_heap(v.begin(), v.end());                  // O(n) heapify: max at v[0]
    for (auto end = v.end(); end != v.begin(); --end)
        pop_heap(v.begin(), end);                   // swap the max to end-1, sift down in [begin, end-1)
}   // v is now ascending (std::sort_heap is exactly this loop). O(n log n), in place, not stable.
// [/snippet]

int main() {
    // [snippet:declare]
    priority_queue<int> max_heap;                               // default: the LARGEST element on top
    priority_queue<int, vector<int>, greater<int>> min_heap;    // greater<int> flips it: smallest on top

    // pairs and tuples compare field by field (lexicographically), so with greater<> a heap of
    // (distance, node) pops the smallest distance first, ties broken by the smaller node.
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> by_distance;

    // A custom order. The comparator answers "does a rank BELOW b?"; the top is the element that
    // ranks below nothing. Here a later deadline ranks below, so the earliest deadline is on top.
    auto later_deadline = [](const Task& a, const Task& b) { return a.deadline > b.deadline; };
    priority_queue<Task, vector<Task>, decltype(later_deadline)> by_deadline(later_deadline);
    // [/snippet]

    for (int x : {3, 1, 4, 1, 5}) max_heap.push(x), min_heap.push(x);
    CHECK_EQ(max_heap.top(), 5);
    CHECK_EQ(min_heap.top(), 1);
    min_heap.pop();                                 // pop() returns nothing: read top() first
    CHECK_EQ(min_heap.top(), 1);                    // duplicates stay
    CHECK_EQ(max_heap.size(), 5);

    by_distance.push({7, 2});
    by_distance.push({3, 9});
    by_distance.push({3, 4});
    CHECK_EQ(by_distance.top(), make_pair(3, 4));
    by_distance.pop();
    CHECK_EQ(by_distance.top(), make_pair(3, 9));

    by_deadline.push({"report", 5});
    by_deadline.push({"invoice", 1});
    by_deadline.push({"review", 3});
    CHECK_EQ(by_deadline.top().name, "invoice");
    by_deadline.pop();
    CHECK_EQ(by_deadline.top().name, "review");

    // k-way merge
    CHECK_EQ(merge_k_sorted({{1, 4, 5}, {1, 3, 4}, {2, 6}}), vector<int>{1, 1, 2, 3, 4, 4, 5, 6});
    CHECK_EQ(merge_k_sorted({}), vector<int>{});
    CHECK_EQ(merge_k_sorted({{}, {}, {7}}), vector<int>{7});

    // heap sort
    vector<int> v{5, 2, 9, 1, 5, 6};
    heap_sort(v);
    CHECK_EQ(v, vector<int>{1, 2, 5, 5, 6, 9});

    // stress: merge and heap sort against std::sort
    for (int iter = 0; iter < 300; iter++) {
        int k = (int)t::rand_int(0, 6);
        vector<vector<int>> lists(k);
        vector<int> all;
        for (auto& one_list : lists) {
            one_list = t::rand_vec((int)t::rand_int(0, 8), -20, 20);
            sort(one_list.begin(), one_list.end());
            all.insert(all.end(), one_list.begin(), one_list.end());
        }
        sort(all.begin(), all.end());
        CHECK_EQ(merge_k_sorted(lists), all);
        vector<int> w = t::rand_vec((int)t::rand_int(0, 50), -100, 100), expected = w;
        sort(expected.begin(), expected.end());
        heap_sort(w);
        CHECK_EQ(w, expected);
    }
    return t::summary("priority_queue_basics");
}
