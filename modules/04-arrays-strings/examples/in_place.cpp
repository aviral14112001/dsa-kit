// Module 04 section 1: in-place building blocks (read/write pointers, reversal, Lomuto and Hoare partitions),
// each checked against a simple reference or against the property it promises.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:read_write]
// Remove every vowel from s in place, keeping everything else in order. O(n) time, O(1) extra space.
// Invariant: s[0..write) holds exactly the kept characters of s[0..read), in their original order.
// write <= read always, so a write never lands on a character that hasn't been read yet.
void remove_vowels(string& s) {
    int write = 0;
    for (int read = 0; read < (int)s.size(); read++) {
        char c = (char)tolower((unsigned char)s[read]);
        bool vowel = c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
        if (!vowel) s[write++] = s[read];   // keep it: copy it down to the write position
    }
    s.resize(write);                        // LeetCode's array versions return `write` instead
}
// [/snippet]

// [snippet:reverse_range]
// Reverse a[l..r] in place: swap the two ends, step both inward. floor(length / 2) swaps.
void reverse_range(vector<int>& a, int l, int r) {
    while (l < r) swap(a[l++], a[r--]);
}
// [/snippet]

// [snippet:lomuto]
// Lomuto: pivot = a[hi]. Returns p with a[lo..p-1] < a[p] == pivot <= a[p+1..hi].
// Invariant during the scan: a[lo..i) < pivot, a[i..j) >= pivot, a[j..hi) not looked at yet.
int lomuto_partition(vector<int>& a, int lo, int hi) {
    int pivot = a[hi], i = lo;
    for (int j = lo; j < hi; j++)
        if (a[j] < pivot) swap(a[i++], a[j]);   // grow the "< pivot" region by one
    swap(a[i], a[hi]);                          // put the pivot between the two regions
    return i;
}
// [/snippet]

// [snippet:hoare]
// Hoare: pivot = the middle value. Returns j with every element of a[lo..j] <= pivot <= every
// element of a[j+1..hi], and lo <= j < hi, so both halves are non-empty. The pivot itself can end up
// on either side. Both scans stop AT values equal to the pivot, which keeps all-equal input balanced.
int hoare_partition(vector<int>& a, int lo, int hi) {
    int pivot = a[lo + (hi - lo) / 2];
    int i = lo - 1, j = hi + 1;
    while (true) {
        do i++; while (a[i] < pivot);   // a left element that belongs on the right (or equals pivot)
        do j--; while (a[j] > pivot);   // a right element that belongs on the left (or equals pivot)
        if (i >= j) return j;
        swap(a[i], a[j]);
    }
}
// [/snippet]

void quicksort_lomuto(vector<int>& a, int lo, int hi) {
    if (lo >= hi) return;
    int p = lomuto_partition(a, lo, hi);
    quicksort_lomuto(a, lo, p - 1);
    quicksort_lomuto(a, p + 1, hi);
}

void quicksort_hoare(vector<int>& a, int lo, int hi) {
    if (lo >= hi) return;
    int j = hoare_partition(a, lo, hi);
    quicksort_hoare(a, lo, j);
    quicksort_hoare(a, j + 1, hi);
}

int main() {
    // ---- read / write pointers ----
    string s = "Interview";
    remove_vowels(s);
    CHECK_EQ(s, "ntrvw");
    string none = "rhythm", all = "aeiou", empty;
    remove_vowels(none);
    remove_vowels(all);
    remove_vowels(empty);
    CHECK_EQ(none, "rhythm");
    CHECK_EQ(all, "");
    CHECK_EQ(empty, "");
    for (int iter = 0; iter < 300; iter++) {
        string x = t::rand_string((int)t::rand_int(0, 15), 'a', 'j'), expected;
        for (char c : x)
            if (string("aeiou").find(c) == string::npos) expected += c;
        remove_vowels(x);
        CHECK_EQ(x, expected);
    }

    // ---- reverse ----
    vector<int> r{1, 2, 3, 4, 5};
    reverse_range(r, 1, 3);
    CHECK_EQ(r, vector<int>{1, 4, 3, 2, 5});
    reverse_range(r, 2, 2);                           // one element: no swaps
    CHECK_EQ(r, vector<int>{1, 4, 3, 2, 5});
    for (int iter = 0; iter < 300; iter++) {
        auto a = t::rand_vec((int)t::rand_int(1, 12), -9, 9), b = a;
        int lo = (int)t::rand_int(0, (int)a.size() - 1), hi = (int)t::rand_int(lo, (int)a.size() - 1);
        reverse_range(a, lo, hi);
        reverse(b.begin() + lo, b.begin() + hi + 1);
        CHECK_EQ(a, b);
    }

    // ---- partitions: check the promised property on random ranges ----
    for (int iter = 0; iter < 400; iter++) {
        auto original = t::rand_vec((int)t::rand_int(1, 14), 0, 6);
        int n = (int)original.size();
        int lo = (int)t::rand_int(0, n - 1), hi = (int)t::rand_int(lo, n - 1);

        auto a = original;
        int pivot = a[hi];
        int p = lomuto_partition(a, lo, hi);
        CHECK(lo <= p && p <= hi);
        CHECK_EQ(a[p], pivot);
        for (int k = lo; k < p; k++) CHECK(a[k] < pivot);
        for (int k = p + 1; k <= hi; k++) CHECK(a[k] >= pivot);
        auto sortedA = a, sortedO = original;
        sort(sortedA.begin(), sortedA.end());
        sort(sortedO.begin(), sortedO.end());
        CHECK_EQ(sortedA, sortedO);                                   // same values, just moved
        for (int k = 0; k < n; k++)
            if (k < lo || k > hi) CHECK_EQ(a[k], original[k]);        // outside the range: untouched

        if (hi > lo) {
            auto b = original;
            int j = hoare_partition(b, lo, hi);
            CHECK(lo <= j && j < hi);
            int leftMax = *max_element(b.begin() + lo, b.begin() + j + 1);
            int rightMin = *min_element(b.begin() + j + 1, b.begin() + hi + 1);
            CHECK(leftMax <= rightMin);
            auto sortedB = b;
            sort(sortedB.begin(), sortedB.end());
            CHECK_EQ(sortedB, sortedO);
        }
    }

    // All-equal input: Lomuto puts everything on one side (quicksort goes quadratic), Hoare splits
    // in the middle.
    vector<int> same(8, 5);
    CHECK_EQ(lomuto_partition(same, 0, 7), 0);
    CHECK_EQ(hoare_partition(same, 0, 7), 3);

    for (int iter = 0; iter < 200; iter++) {                          // both sort correctly
        auto a = t::rand_vec((int)t::rand_int(0, 30), -20, 20), b = a, c = a;
        if (!a.empty()) {
            quicksort_lomuto(a, 0, (int)a.size() - 1);
            quicksort_hoare(b, 0, (int)b.size() - 1);
        }
        sort(c.begin(), c.end());
        CHECK_EQ(a, c);
        CHECK_EQ(b, c);
    }
    return t::summary("04-in-place");
}
