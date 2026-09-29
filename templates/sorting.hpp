// Sorting by hand (module 05): the three comparison sorts you should be able to write and defend
// in an interview, and the linear-time sorts for small integer keys.
//
//   merge_sort(a [, cmp])            stable · O(n log n) always · O(n) extra memory
//   quick_sort(a [, cmp])            random pivot + 3-way partition · expected O(n log n) on every
//                                    input · expected O(log n) stack · not stable
//   heap_sort(a [, cmp])             in place · O(n log n) always · not stable
//   counting_sort(a, max_key, key)   stable · integer keys in [0, max_key] · O(n + K) · returns a copy
//   counting_sort(a)                 plain ints in any range [mn, mx] · O(n + (mx - mn))
//   radix_sort(a [, base])           LSD · non-negative ints · O(d·(n + base)) for d base-digits
//
// Also the two textbook partitions, lomuto_partition and hoare_partition (+ quick_sort_lomuto,
// quick_sort_hoare), kept to compare against the 3-way one (Notes section 1).
//
// cmp(x, y) == true means "x must come before y". It must be a strict weak ordering, exactly as
// for std::sort (Notes section 3). Written interview-style (unqualified std names), like every header here.
#pragma once
#include <bits/stdc++.h>
using namespace std;

// [snippet:merge_sort]
// Sorts a[lo, hi), using buf as scratch space.
template <class T, class Cmp>
void merge_sort(vector<T>& a, vector<T>& buf, int lo, int hi, Cmp& cmp) {
    if (hi - lo < 2) return;                    // 0 or 1 element: already sorted
    int mid = lo + (hi - lo) / 2;
    merge_sort(a, buf, lo, mid, cmp);           // sort each half...
    merge_sort(a, buf, mid, hi, cmp);
    // ...then merge. Invariant: buf[lo, k) holds the k - lo smallest of both halves, in order.
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        if (cmp(a[j], a[i])) buf[k++] = a[j++]; // right one strictly smaller: take it
        else buf[k++] = a[i++];                 // tie: take the LEFT one. That is what makes it stable
    }
    while (i < mid) buf[k++] = a[i++];
    while (j < hi) buf[k++] = a[j++];
    copy(buf.begin() + lo, buf.begin() + hi, a.begin() + lo);
}

// Stable. O(n log n) in every case; O(n) extra memory (one buffer, allocated once) + O(log n) stack.
template <class T, class Cmp = less<T>>
void merge_sort(vector<T>& a, Cmp cmp = Cmp()) {
    vector<T> buf(a.size());
    merge_sort(a, buf, 0, (int)a.size(), cmp);
}
// [/snippet]

// A uniformly random index in [lo, hi]. Seeded from the clock, so no fixed input is bad every run.
inline int random_index(int lo, int hi) {
    static mt19937 rng((unsigned)chrono::steady_clock::now().time_since_epoch().count());
    return uniform_int_distribution<int>(lo, hi)(rng);
}

// [snippet:quick_sort]
// Sorts a[lo..hi], both ends inclusive.
template <class T, class Cmp>
void quick_sort(vector<T>& a, int lo, int hi, Cmp& cmp) {
    if (lo >= hi) return;
    T pivot = a[random_index(lo, hi)];          // a COPY: the elements move while we partition
    // 3-way partition (Dutch national flag). Invariant while i <= gt:
    //   a[lo, lt) < pivot    a[lt, i) == pivot    a[i, gt] not examined yet    a(gt, hi] > pivot
    int lt = lo, i = lo, gt = hi;
    while (i <= gt) {
        if (cmp(a[i], pivot)) swap(a[lt++], a[i++]);
        else if (cmp(pivot, a[i])) swap(a[i], a[gt--]);   // the element swapped in is unexamined: keep i
        else i++;
    }
    quick_sort(a, lo, lt - 1, cmp);             // a[lt, gt] (every copy of the pivot) is already in
    quick_sort(a, gt + 1, hi, cmp);             // its final place: duplicates never recurse
}

// Not stable. Expected O(n log n) time and O(log n) stack on EVERY input (sorted, reversed, all
// equal); the O(n^2) worst case needs a long run of unlucky random pivots.
template <class T, class Cmp = less<T>>
void quick_sort(vector<T>& a, Cmp cmp = Cmp()) {
    quick_sort(a, 0, (int)a.size() - 1, cmp);
}
// [/snippet]

// [snippet:lomuto]
// Lomuto: pivot = a[hi]. Invariant: a[lo, i) < pivot and a[i, j) >= pivot.
// Returns the pivot's final index p: a[lo, p) < a[p] <= a(p, hi].
template <class T, class Cmp = less<T>>
int lomuto_partition(vector<T>& a, int lo, int hi, Cmp cmp = Cmp()) {
    int i = lo;
    for (int j = lo; j < hi; j++)
        if (cmp(a[j], a[hi])) swap(a[i++], a[j]);
    swap(a[i], a[hi]);                          // the pivot lands between the two regions
    return i;
}

template <class T, class Cmp = less<T>>
void quick_sort_lomuto(vector<T>& a, int lo, int hi, Cmp cmp = Cmp()) {   // sorts a[lo..hi]
    if (lo >= hi) return;
    int p = lomuto_partition(a, lo, hi, cmp);
    quick_sort_lomuto(a, lo, p - 1, cmp);       // p is final: exclude it on both sides
    quick_sort_lomuto(a, p + 1, hi, cmp);
}
// [/snippet]

// [snippet:hoare]
// Hoare: two indices walk inward and swap pairs that sit on the wrong side.
// Returns j with a[lo..j] <= pivot <= a[j+1..hi]. The pivot is NOT necessarily at j.
template <class T, class Cmp = less<T>>
int hoare_partition(vector<T>& a, int lo, int hi, Cmp cmp = Cmp()) {
    T pivot = a[lo + (hi - lo) / 2];            // never a[hi]: then j could come back as hi (see below)
    int i = lo - 1, j = hi + 1;
    while (true) {
        do i++; while (cmp(a[i], pivot));       // stop at an element >= pivot
        do j--; while (cmp(pivot, a[j]));       // stop at an element <= pivot
        if (i >= j) return j;
        swap(a[i], a[j]);
    }
}

template <class T, class Cmp = less<T>>
void quick_sort_hoare(vector<T>& a, int lo, int hi, Cmp cmp = Cmp()) {    // sorts a[lo..hi]
    if (lo >= hi) return;
    int j = hoare_partition(a, lo, hi, cmp);
    quick_sort_hoare(a, lo, j, cmp);            // j INCLUDED on the left; j < hi guarantees progress
    quick_sort_hoare(a, j + 1, hi, cmp);
}
// [/snippet]

// [snippet:heap_sort]
// Max-heap on a[0, n): node i has children 2i+1 and 2i+2. Push a[i] down until it is not
// smaller than either child.
template <class T, class Cmp>
void sift_down(vector<T>& a, int i, int n, Cmp& cmp) {
    while (true) {
        int largest = i, l = 2 * i + 1, r = 2 * i + 2;
        if (l < n && cmp(a[largest], a[l])) largest = l;
        if (r < n && cmp(a[largest], a[r])) largest = r;
        if (largest == i) return;
        swap(a[i], a[largest]);
        i = largest;
    }
}

// In place (O(1) extra memory), O(n log n) in every case, not stable.
template <class T, class Cmp = less<T>>
void heap_sort(vector<T>& a, Cmp cmp = Cmp()) {
    int n = (int)a.size();
    for (int i = n / 2 - 1; i >= 0; i--) sift_down(a, i, n, cmp);   // heapify bottom-up: O(n)
    for (int end = n - 1; end > 0; end--) {
        swap(a[0], a[end]);                     // the max moves to its final slot...
        sift_down(a, 0, end, cmp);              // ...and the heap shrinks to a[0, end)
    }
}
// [/snippet]

// [snippet:counting_sort]
// Stable counting sort of records by an integer key in [0, max_key]. O(n + K) time and memory.
template <class T, class Key>
vector<T> counting_sort(const vector<T>& a, int max_key, Key key) {
    vector<int> start(max_key + 2, 0);
    for (const T& x : a) start[key(x) + 1]++;                      // start[k + 1] = count of key k
    for (int k = 0; k <= max_key; k++) start[k + 1] += start[k];   // now start[k] = # of keys < k
    vector<T> out(a.size());
    for (const T& x : a) out[start[key(x)]++] = x;  // left to right: equal keys keep their order
    return out;
}

// Plain ints in any range [mn, mx]: shift every key by -mn. O(n + (mx - mn)) time and memory,
// so only for small ranges (mx - mn up to ~10^7).
inline void counting_sort(vector<int>& a) {
    if (a.empty()) return;
    auto [mn, mx] = minmax_element(a.begin(), a.end());
    int lo = *mn, range = *mx - *mn;
    a = counting_sort(a, range, [lo](int x) { return x - lo; });
}
// [/snippet]

// [snippet:radix_sort]
// LSD radix sort, non-negative ints: one stable counting-sort pass per base-`base` digit, least
// significant digit first. Invariant: after pass d, a is sorted by its last d digits.
// O(d·(n + base)) time for d digits of the maximum; O(n + base) extra memory.
inline void radix_sort(vector<int>& a, int base = 256) {
    if (a.empty()) return;
    assert(*min_element(a.begin(), a.end()) >= 0);
    int mx = *max_element(a.begin(), a.end());
    for (long long place = 1; mx / place > 0; place *= base)   // long long: place passes INT_MAX
        a = counting_sort(a, base - 1, [&](int x) { return (int)(x / place % base); });
}
// [/snippet]
