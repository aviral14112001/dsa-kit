// A binary heap you can read end to end: the array layout, sift-up, sift-down and the O(n)
// heapify that std::priority_queue is built on (module 13 section 1).
//
//   BinaryHeap<int> h;                               max-heap, like priority_queue<int>
//   BinaryHeap<int, greater<int>> mn;                min-heap
//   BinaryHeap<int> built(vector<int>{3, 1, 4, 1});  heapify: O(n), not n pushes
//   h.push(x);  h.top();  h.pop();  h.size();  h.empty();
//
// Same comparator convention as priority_queue: cmp(a, b) == true means "a ranks below b", so the
// default less<T> keeps the largest element on top. In real solutions use std::priority_queue;
// this file is here so you can see (and re-derive) what it does.
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:sift]
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
// [/snippet]

// [snippet:heapify]
    explicit BinaryHeap(Compare c = Compare()) : cmp(c) {}

    // Heapify: build from n items in O(n). Sift down every internal node, from the last one
    // back to the root. Nodes n/2 .. n-1 are leaves, and a leaf is already a one-node heap;
    // when we reach node i, both of its subtrees are heaps, so one sift_down(i) fixes subtree i.
    explicit BinaryHeap(vector<T> items, Compare c = Compare()) : a(std::move(items)), cmp(c) {
        for (int i = (int)a.size() / 2 - 1; i >= 0; i--) sift_down(i);
    }
// [/snippet]

// [snippet:ops]
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
// [/snippet]
