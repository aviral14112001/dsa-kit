# 05 · Searching + Sorting
> The two operations that quietly decide the complexity of everything else.

**Time:** ~13 h of Core work ([problems](problems.md)) · **Prereqs:** modules 01 (STL, lambdas), 03 (recurrences, master theorem), 04 (Dutch national flag) · **You're done when:** you can write merge sort, randomized 3-way quicksort and heap sort from a blank file (about 10 minutes each) and state each one's worst case, extra memory and stability · you can look at a comparator and say whether it is a strict weak ordering · you can derive first ≥ x, first > x, last ≤ x and last < x from `first_true` with no off-by-one.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Comparison sorts | split + merge, partition, or a heap; pick by worst case, memory, stability | `merge_sort`, `quick_sort`, `heap_sort` in [sorting.hpp](../../templates/sorting.hpp) | 912, 88, 493 |
| 2 | Non-comparison sorts | small integer keys: use the key as an array index, O(n + K) | `counting_sort`, `radix_sort`, [bucket_sort.cpp](examples/bucket_sort.cpp) | 1122, 451 |
| 3 | Custom comparators | cmp must be a strict weak ordering; `tie()` for multi-key order | [sort_comparators.cpp](examples/sort_comparators.cpp) | 179, 973 |
| 4 | Search on sorted data | a monotone predicate over indices → `first_true` | `first_true` in [binary_search.hpp](../../templates/binary_search.hpp), [sorted_queries.cpp](examples/sorted_queries.cpp) | 704, 34, 33, 153, 81, 162, 540, 240, 1901, 4 |

## 1. Comparison sorts — merge, quick, heap: stability, in-place, worst cases

**Vocabulary.** A *comparison sort* learns about the input only through `cmp(a, b)`. *Stable*: equal keys keep their input order. *In place*: O(1) extra memory (people often allow the O(log n) recursion stack; say "extra memory" and give the number instead).

| | merge sort | quicksort (random pivot, 3-way) | heap sort |
|---|---|---|---|
| idea | sort both halves, merge them | partition around a pivot, sort both sides | build a max-heap, move the max to the end n times |
| worst case | Θ(n log n) | Θ(n²), with vanishing probability | Θ(n log n) |
| expected / typical | Θ(n log n) | Θ(n log n), usually the fastest | Θ(n log n), usually the slowest (cache-unfriendly jumps) |
| extra memory | Θ(n) buffer + O(log n) stack | O(log n) stack, expected | O(1) |
| stable | yes | no | no |

### Merge sort

Split [lo, hi) at mid, sort both halves recursively, merge. The merge's invariant: `buf[lo, k)` holds the k − lo smallest elements of the two halves, in order. It holds because both halves are sorted, so the smaller of the two front elements is the smallest one left. T(n) = 2T(n/2) + Θ(n) = Θ(n log n) (module 03, master theorem case 2): log₂ n levels, and each level merges n elements in total.

```
split   [5 2 4 1 3]
        [5 2]         [4 1 3]
        [5] [2]       [4] [1 3]
                          [1] [3]
merge   [2 5]         [4] [1 3] -> [1 3 4]
        [1 2 3 4 5]
```

Stability hangs on one comparison: on a tie the merge takes the **left** element (`cmp(a[j], a[i])` is strict, so equal keys fall to the `else`). Take the right one on ties and merge sort is no longer stable.

### Quicksort

Partition: choose a pivot and rearrange so smaller elements sit left of it and larger ones right; the pivot (or the whole block of pivot-equal elements) is now in its final position. Sort the two sides independently, and the whole range is sorted. The three textbook partitions:

| | Lomuto | Hoare | 3-way (Dijkstra) |
|---|---|---|---|
| pivot | `a[hi]` (swap a random element there first) | a middle element | a random element, copied |
| invariant | `a[lo,i) < p <= a[i,j)` | left of i: ≤ p · right of j: ≥ p | `< p` · `== p` · unexamined · `> p` |
| returns | pivot's final index p | split point j (pivot not necessarily at j) | the block `[lt, gt]` equal to p |
| recurse on | `[lo, p-1]`, `[p+1, hi]` | `[lo, j]`, `[j+1, hi]` | `[lo, lt-1]`, `[gt+1, hi]` |
| all-equal input | Θ(n²): every element lands on one side | Θ(n log n): stops on equal keys, splits in half | Θ(n): a single pass |
| notes | simplest to write | about 3× fewer swaps than Lomuto on average | module 04's Dutch national flag with the pivot as the middle colour |

**The O(n²) worst case, and when it happens.** If every pivot is the smallest or largest element, each partition peels off one element: (n−1) + (n−2) + … + 1 = n(n−1)/2 comparisons, and the recursion goes n deep (at n ≈ 10⁵ that can overflow the stack before the time limit hits). That happens with:

- a **fixed pivot position** (first or last element) on sorted or reverse-sorted input;
- **Lomuto on all-equal input, whatever the pivot**: nothing is `< pivot`, so everything lands on one side;
- **any deterministic rule** on inputs built against it: the middle-element pivot is quadratic on organ-pipe input (0 1 2 … 2 1 0);
- **random pivots** only through a long run of bad luck, which gets exponentially unlikely.

[quicksort_worst_case.cpp](examples/quicksort_worst_case.cpp) counts comparisons for n = 2000; after `make test M=quicksort_worst_case`, run `./build/modules/05-searching-sorting/examples/quicksort_worst_case --table` to print the table yourself (random-pivot counts vary by 10% or so between runs). For scale, n log₂ n ≈ 22k and n(n−1)/2 = 1,999,000:

| variant | random | sorted | all equal | organ pipe |
|---|---|---|---|---|
| Lomuto, last pivot | 25k | **1,999,000** | **1,999,000** | 325k |
| Lomuto, random pivot | 25k | 24k | **1,999,000** | 24k |
| Hoare, middle pivot | 32k | 24k | 26k | **1,008,722** |
| 3-way, random pivot | 38k | 39k | 4,000 | 36k |
| `merge_sort` | 19k | 11k | 11k | 12k |
| `heap_sort` | 38k | 39k | 6k | 39k |
| `std::sort` (libc++ here) | 25k | 4k | 4k | 47k |

3-way pays about 1.5× Lomuto's comparisons on distinct keys (two questions per element), and in exchange duplicates cost nothing.

**Why random pivots give O(n log n) expected.** Name the elements by rank, z₁ < z₂ < … < zₙ. zᵢ and zⱼ are compared exactly when one of them is the first pivot picked from zᵢ … zⱼ: before that they stay on the same side, and once a pivot strictly between them is picked they are separated for good. Every element of zᵢ … zⱼ is equally likely to be that first pivot, so this happens with probability 2/(j − i + 1). Summed over all pairs, that is at most 2n·Hₙ, where Hₙ = 1 + 1/2 + … + 1/n ≈ ln n: about 1.39 n log₂ n comparisons, O(n log n). No input is bad; only the coin flips can be.

### Heap sort

Treat the array as a complete binary tree: the children of index i are 2i+1 and 2i+2 (module 13 builds the full heap structure). In a *max-heap* every node is ≥ its children, so the root `a[0]` is the maximum. *Sift down* node i: while it is smaller than a child, swap it with its larger child; O(height) swaps.

1. **Heapify** bottom-up: sift down every internal node, the last one first. This costs O(n), not O(n log n): a node of height h costs O(h), there are at most ⌈n/2^(h+1)⌉ of them, and Σ h·n/2^(h+1) = O(n).
2. **Extract** n − 1 times: swap the root (the max) to the end of the heap region, shrink the region by one, and sift the new root down in O(log n).

Not stable: the swap throws the root past equal keys (the template test shows `(1,a) (1,b)` coming out as `(1,b) (1,a)`). The code is the [912 worked example](#worked-example-912-sort-an-array) below; `heap_sort` in sorting.hpp is the same algorithm with a comparator parameter.

### Stability, with an example

```
input:            (Ann,B) (Bob,A) (Cat,B) (Dan,A)
stable by grade:  (Bob,A) (Dan,A) (Ann,B) (Cat,B)   <- Bob stays before Dan, Ann before Cat
also "sorted":    (Dan,A) (Bob,A) (Cat,B) (Ann,B)   <- what an unstable sort may return
```

Why it matters: multi-pass sorting (sort by name, then stable-sort by grade, and each grade stays in name order; section 3); LSD radix sort works only because every pass is stable (Section 2); and statements that say "ties keep their original order". Any sort becomes stable if you sort `(key, original index)` pairs.

### What the library guarantees

| call | guarantee |
|---|---|
| `sort(b, e [, cmp])` | O(n log n) comparisons worst case (since C++11); not stable. In practice it is introsort: quicksort that switches to heap sort when the recursion gets too deep, plus insertion sort on small ranges |
| `stable_sort(b, e [, cmp])` | stable; O(n log n) when it can allocate a buffer, O(n log² n) when it can't |
| `partial_sort(b, m, e)` | the smallest `m − b` elements, sorted, in `[b, m)`; about n log(m − b) comparisons |
| `nth_element(b, nth, e)` | `*nth` becomes what a full sort would put there; nothing before it is greater, nothing after it smaller; O(n) average |
| `lst.sort()` | `std::sort` needs random access, so `std::list` has its own stable member sort |

**The Ω(n log n) lower bound.** Draw any comparison sort as a binary decision tree: each internal node is one comparison, each leaf a final ordering. A correct sort needs a different leaf for each of the n! input orders, so the tree's height, which is the worst-case number of comparisons, is at least log₂(n!) ≈ n log₂ n − 1.44n = Ω(n log n) (Stirling). Merge sort and heap sort meet the bound, so no comparison sort beats them asymptotically; beating n log n means looking inside the keys (Section 2).

### Template

<!-- snippet: templates/sorting.hpp#merge_sort -->
```cpp
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
```
<!-- /snippet -->

<!-- snippet: templates/sorting.hpp#quick_sort -->
```cpp
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
```
<!-- /snippet -->

The two textbook partitions, for comparison (both are tested in `sorting_test.cpp`):

<!-- snippet: templates/sorting.hpp#lomuto -->
```cpp
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
```
<!-- /snippet -->

<!-- snippet: templates/sorting.hpp#hoare -->
```cpp
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
```
<!-- /snippet -->

**Complexity:** merge sort Θ(n log n) time in every case, Θ(n) extra memory. Quicksort Θ(n log n) expected time and O(log n) expected stack on every input. Heap sort ([below](#worked-example-912-sort-an-array)) Θ(n log n) worst case with O(1) extra memory.

### Pitfalls

- **Allocating in every merge.** A merge sort that creates a new vector per call does O(n log n) allocation work and runs several times slower. Allocate one buffer and pass it down.
- `mid = (lo + hi) / 2` overflows when `lo + hi > INT_MAX`; write `lo + (hi - lo) / 2`.
- **Pivot by reference.** `T& pivot = a[...]` changes under you as elements swap. Copy the pivot's value.
- **Hoare bounds.** Recurse on `[lo, j]` and `[j+1, hi]` (j included on the left, not `j - 1`), and never use `a[hi]` as the pivot: j can come back as hi and the recursion never shrinks.
- **3-way:** after swapping with `a[gt]`, don't advance i: the element that just arrived hasn't been examined.
- `rand() % n`: `RAND_MAX` may be as small as 32767 (it is on MSVC), so for n > 32768 the indices above 32767 can never be picked. Use `mt19937` (see `random_index`).
- **Recursion depth.** A quadratic quicksort also recurses n levels deep; at n = 10⁵ you may see a runtime error (stack overflow) rather than a TLE.
- **Heap indices** here are 0-based: children 2i+1 and 2i+2, parent (i−1)/2. The 1-based versions (2i, 2i+1, i/2) are different formulas; don't mix them.

### Recognize it when…

- "Sort without built-in functions", "implement a sort": this section. Say which trade-off you picked and why (912).
- "O(1) extra space" + "guaranteed O(n log n)": heap sort.
- "Stable", "ties keep input order", sorting by several keys in passes: merge sort or `stable_sort`.
- "Merge two sorted arrays / lists", k-way merge: the merge step. In-place variants fill from the back when the free space is at the end (88).
- "Count pairs i < j with a[i] > c·a[j]", inversions: count during merge sort's merge, while both halves are sorted (493).
- "k-th smallest / largest": quickselect (the partition applied to one side only), `nth_element`, O(n) average; the heap alternatives are module 13.
- "Sort a linked list": merge sort. Heap sort needs random access, and quicksort on a list keeps its O(n²) worst case (module 08).
- Heavy duplicates: 3-way partition. Data larger than memory: external merge sort (sort chunks that fit, then k-way merge them).

### Worked example: 912. Sort an Array
[LeetCode 912](https://leetcode.com/problems/sort-an-array/) · Medium

**Problem (paraphrased):** Return the array sorted ascending without library sort functions, in O(n log n) time, using as little extra memory as possible. n ≤ 5·10⁴, values in [−5·10⁴, 5·10⁴].

**Signals:** "without built-in functions", "O(n log n)", "smallest space complexity possible". The space clause means merge sort's O(n) buffer is not the best answer.

**Brute force, and why it fails:** insertion or bubble sort is Θ(n²): about n²/2 = 1.25·10⁹ steps at n = 5·10⁴, well past the ~10⁸ simple steps per second you can budget for.

**Key insight:** You need an O(n log n) comparison sort, and the candidates differ exactly where the statement is strict:

- **Merge sort:** always O(n log n), but Θ(n) extra memory.
- **Quicksort:** only as good as its pivot. A fixed-pivot version is Θ(n²) on sorted input, and Lomuto is Θ(n²) on all-equal input even with random pivots (≈1.25·10⁹ comparisons at this n). Random pivot + 3-way partition is safe in expectation. Submit a last-element-pivot Lomuto version once and see what the judge's tests do to it.
- **Heap sort:** O(n log n) on every input with O(1) extra memory. It meets every requirement, so it is the answer here.

**Dry run:** `[2, 5, 1, 4, 3]`

| step | array (heap \| sorted suffix) |
|---|---|
| heapify i = 1 | 2 5 1 4 3 (5 already beats its children 4 and 3) |
| heapify i = 0 | 5 2 1 4 3 → 5 4 1 2 3 (the 2 sinks two levels) |
| end = 4 | swap: 3 4 1 2 \| 5 → sift: 4 3 1 2 \| 5 |
| end = 3 | swap: 2 3 1 \| 4 5 → sift: 3 2 1 \| 4 5 |
| end = 2 | swap: 1 2 \| 3 4 5 → sift: 2 1 \| 3 4 5 |
| end = 1 | swap: 1 \| 2 3 4 5 (done) |

<!-- snippet: modules/05-searching-sorting/examples/0912-sort-an-array.cpp#solution -->
```cpp
class Solution {
public:
    vector<int> sortArray(vector<int>& nums) {
        int n = nums.size();
        // Phase 1, heapify: sift down every internal node, the last one first. Leaves
        // (i >= n/2) are one-node heaps already, so when we reach i both subtrees are heaps.
        for (int i = n / 2 - 1; i >= 0; i--) siftDown(nums, i, n);
        // Phase 2. Invariant: nums[0, end] is a max-heap, and nums(end, n) holds the largest
        // values in sorted order. Swapping the root (the heap's max) into slot `end` grows
        // the sorted suffix by one; one sift-down repairs the smaller heap.
        for (int end = n - 1; end > 0; end--) {
            swap(nums[0], nums[end]);
            siftDown(nums, 0, end);
        }
        return nums;
    }

private:
    // Max-heap on a[0, n): the children of i are 2i+1 and 2i+2. Moves a[i] down until it is
    // not smaller than either child: one level per iteration, so O(log n).
    void siftDown(vector<int>& a, int i, int n) {
        while (true) {
            int largest = i, left = 2 * i + 1, right = 2 * i + 2;
            if (left < n && a[left] > a[largest]) largest = left;
            if (right < n && a[right] > a[largest]) largest = right;
            if (largest == i) return;
            swap(a[i], a[largest]);
            i = largest;
        }
    }
};
```
<!-- /snippet -->

**Complexity:** O(n log n) time on every input: O(n) heapify, then n − 1 sift-downs of O(log n). O(1) extra memory; returning `nums` copies it only because LeetCode's signature returns the result.

**Edge cases:**
- n = 1: both loops are empty (`n / 2 - 1 = -1`, `end` starts at 0).
- All equal: every sift-down stops at once, so the whole sort is O(n).
- Negative values need nothing special (unlike counting sort, no value is used as an index).

**Follow-ups:**
- *Why not quicksort?* Its O(n log n) is only expected, it needs random pivots and a 3-way partition to survive sorted and all-equal inputs, and it uses O(log n) stack. Heap sort's bound holds for every input.
- *Can you beat O(n log n) here?* Yes: the values span only 10⁵ + 1, so counting sort with an offset runs in O(n + 10⁵) (Section 2). The lower bound only applies to comparison sorts.
- *Is it stable? What if it had to be?* No. Use merge sort (O(n) extra), or heap-sort `(value, index)` pairs.

## 2. Non-comparison sorts — counting, radix, bucket, and when they beat O(n log n)

**Concept.** The Ω(n log n) bound only limits algorithms that learn by comparing. When the keys are small integers you can use the key itself as an array index, and each element's place is computed rather than discovered.

### Counting sort (the stable version)

Keys in [0, K]:

1. Count each key.
2. Prefix-sum the counts, so that `start[k]` = the number of keys smaller than k, which is the first output slot for key k.
3. Scan the input **left to right** and write each record to `start[key]++`.

Step 3 is what makes it stable: records with equal keys go into consecutive slots in input order. Records `(key, name)`, K = 2:

```
input      (2,a) (0,b) (2,c) (1,d)
counts     key 0: 1   key 1: 1   key 2: 2
start      [0, 1, 2]              # keys < 0, < 1, < 2
place      (2,a) -> slot 2   (0,b) -> slot 0   (2,c) -> slot 3   (1,d) -> slot 1
output     (0,b) (1,d) (2,a) (2,c)     <- a still before c
```

O(n + K) time and memory. Worth it when K is about n or smaller (letters, ages, scores, values ≤ 10⁶ with n = 10⁶). Useless when K = 10⁹: that count array alone is 4 GB. The simpler version (count, then write value k `count[k]` times) is fine for bare integers, since equal ints are indistinguishable, but it can't carry records.

### LSD radix sort

LSD = least significant digit first. Sort by the least significant digit with a stable sort, then by the next digit, and so on. **Invariant:** after pass d, the array is sorted by its last d digits. Pass d+1 orders by digit d+1, and elements tied on that digit keep their previous order (stability), which by the invariant is sorted by the last d digits. Base 10 on `[170, 45, 75, 90, 2, 24]`:

```
by 1s:     170 90 2 24 45 75      (digits 0 0 2 4 5 5)
by 10s:    2 24 45 170 75 90      (digits 0 2 4 7 7 9)   170 before 75: pass 1's order kept
by 100s:   2 24 45 75 90 170      sorted
```

d passes of counting sort cost O(d·(n + b)) for base b. With b = 256 a non-negative 32-bit int has d = 4 byte-digits, so O(4(n + 256)) = O(n). A bigger base means fewer passes but a bigger count array (b = 2¹⁶: 2 passes, 65,536 counters). The template takes non-negative ints; for negatives, add an offset (−min) or sort the negatives and non-negatives separately.

### Bucket sort

For values spread evenly over a known range, e.g. doubles in [0, 1): n buckets, bucket i takes [i/n, (i+1)/n). Sort each bucket, then concatenate. Each bucket holds O(1) values on average, so the whole sort is O(n) expected. Skewed input can pile everything into one bucket, and then you pay that bucket's sort (O(n log n) with `std::sort`). The same idea works with any bounded key: when frequencies are the keys, they are at most n, so bucket index = frequency.

### When they beat O(n log n)

| sort | time | extra memory | use when |
|---|---|---|---|
| counting | O(n + K) | O(n + K) | integer keys in a range K up to about n (or a tiny K: letters, digits, scores) |
| radix (LSD) | O(d·(n + b)) | O(n + b) | fixed-width integer or string keys, large n |
| bucket | O(n) expected | O(n) | keys spread evenly over a known range |
| comparison sorts | O(n log n) | O(1) to O(n) | everything else |

In an interview, `std::sort` is fast enough for n up to ~10⁶. Reach for these when the key range makes O(n + K) clearly better, or when the interviewer asks for linear time.

### Template

<!-- snippet: templates/sorting.hpp#counting_sort -->
```cpp
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
```
<!-- /snippet -->

<!-- snippet: templates/sorting.hpp#radix_sort -->
```cpp
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
```
<!-- /snippet -->

<!-- snippet: modules/05-searching-sorting/examples/bucket_sort.cpp#bucket_sort -->
```cpp
// Bucket i holds the values in [i/n, (i+1)/n). Uniform input puts O(1) values in each bucket
// on average, so the whole sort is O(n) expected. Skewed input can pile everything into one
// bucket: then it is just std::sort, O(n log n).
void bucket_sort(vector<double>& a) {
    int n = a.size();
    vector<vector<double>> buckets(n);
    for (double x : a) buckets[min(n - 1, (int)(x * n))].push_back(x);  // min(): guard x*n rounding up to n
    a.clear();
    for (auto& bucket : buckets) {
        sort(bucket.begin(), bucket.end());
        a.insert(a.end(), bucket.begin(), bucket.end());   // buckets are already in value order
    }
}
```
<!-- /snippet -->

### Pitfalls

- **Count array one too small.** Keys in [0, K] need K + 1 counters (K + 2 for the shifted-prefix version above); the largest key writes out of bounds. ASan catches it here; a judge may not.
- **Negative keys as indices.** Shift by −min first (`counting_sort(a)` does).
- `vector<int> cnt(maxValue + 1)` with maxValue = 10⁹ is 4 GB: memory limit exceeded, or `bad_alloc`. Check K before choosing counting sort.
- A radix pass that isn't stable gives wrong answers: stability is the whole proof.
- `place *= base` overflows `int` after a few passes with large bases; keep `place` in `long long`.
- `(int)(x * n)` can round up to n for x just below 1; clamp the bucket index.

### Recognize it when…

- Tight value bounds in the constraints: `0 <= a[i] <= 1000`, lowercase letters only, digits, ages, scores. Count, then emit in whatever order the problem wants (1122).
- "Sort in O(n)" / "linear time" with integer keys.
- The keys are counts or frequencies (bounded by n): bucket by count (451).
- Fixed-length keys (IDs, zero-padded strings, 32-bit ints) and very large n: radix.

Before the problems, dry-run `counting_sort` and `radix_sort` by hand on the inputs above, then rewrite both from a blank file and run `make test M=sorting_test` against your version.

## 3. Custom comparators — multi-key ordering, sorting objects, breaking ties

**Concept.** `sort(b, e, cmp)` needs `cmp(x, y)` to mean "x must come before y", and it must be a **strict weak ordering**:

| rule | meaning | broken by |
|---|---|---|
| 1. irreflexive | `cmp(a, a)` is false | `<=`, `>=` |
| 2. asymmetric | `cmp(a, b)` implies `!cmp(b, a)` | `a.x < b.x \|\| a.y < b.y` |
| 3. transitive | `cmp(a, b)` and `cmp(b, c)` imply `cmp(a, c)` | cyclic "orders" (rock–paper–scissors) |
| 4. ties are transitive | a ~ b and b ~ c imply a ~ c, where a ~ b means neither comes first | "fuzzy" compares: `a + eps < b`, "close enough counts as equal" |

Rule 4 is what lets ties form groups that line up in one order, which is what "sorted" means. The easy way to stay safe: **order by a key**. `cmp(a, b) = key(a) < key(b)` with a number, a string or a tuple as the key satisfies all four rules automatically.

**What goes wrong with `<=`.** It is undefined behavior, not merely an unstable sort. Library sorts use `cmp(x, x) == false` to stop inner loops without bounds checks; with `<=` and enough equal elements, such a loop can run past the end of the array, giving garbage or a segfault (GCC's libstdc++ is the well-known example). Sanitizers don't reliably catch it.

```c++
sort(v.begin(), v.end(), [](int a, int b) { return a <= b; });  // UB: cmp(a, a) is true
sort(v.begin(), v.end(), [](int a, int b) { return a < b; });   // fine
sort(v.begin(), v.end(), greater<int>());                       // descending: > is strict too
```

**Multi-key order with `std::tie`.** `tie(...)` builds a tuple of references, and tuples compare field by field. C# bridge: this is `OrderBy(e => e.Dept).ThenByDescending(e => e.Salary).ThenBy(e => e.Name)`. .NET's `IComparer.Compare` returns an int (negative, zero, positive); a C++ comparator returns a bool "comes before", which is exactly how `<=` sneaks in.

<!-- snippet: modules/05-searching-sorting/examples/sort_comparators.cpp#tie -->
```cpp
// Department ascending, then salary DESCENDING, then name ascending.
// tie(...) builds a tuple of references, and tuples compare field by field (lexicographically).
// To flip one field, take it from the other object: b.salary on the left, a.salary on the right.
bool by_dept_salary_name(const Employee& a, const Employee& b) {
    return tie(a.dept, b.salary, a.name) < tie(b.dept, a.salary, b.name);
}
// The same order spelled out: compare one field; only on a tie, move to the next.
bool by_dept_salary_name_manual(const Employee& a, const Employee& b) {
    if (a.dept != b.dept) return a.dept < b.dept;
    if (a.salary != b.salary) return a.salary > b.salary;
    return a.name < b.name;
}
```
<!-- /snippet -->

**Sorting indices by another array** (argsort): sort `0..n-1` with a comparator that looks the values up.

<!-- snippet: modules/05-searching-sorting/examples/sort_comparators.cpp#argsort -->
```cpp
// The indices of a, in the order that sorts a (NumPy calls it argsort). You keep the original
// positions, and one order can drive several parallel arrays.
vector<int> argsort(const vector<int>& a) {
    vector<int> idx(a.size());
    iota(idx.begin(), idx.end(), 0);                           // 0, 1, ..., n-1
    stable_sort(idx.begin(), idx.end(), [&a](int i, int j) {   // capture a by reference: no copy
        return a[i] < a[j];                                    // equal values keep index order
    });
    return idx;
}
```
<!-- /snippet -->

**Lambda captures.** Capture by reference (`[&x]`, `[&]`): free, and safe because the lambda dies with the `sort` call. `[x]` and `[=]` copy what they capture:

<!-- snippet: modules/05-searching-sorting/examples/sort_comparators.cpp#capture -->
```cpp
// Task names by priority (highest first), ties by name. [&priority] captures a reference, which
// costs nothing; [priority] or [=] would copy the whole map into the lambda, and std::sort is
// free to copy its comparator many times.
void sort_by_priority(vector<string>& tasks, const unordered_map<string, int>& priority) {
    sort(tasks.begin(), tasks.end(), [&priority](const string& a, const string& b) {
        int pa = priority.at(a), pb = priority.at(b);   // at(): the map is const, [] won't compile
        if (pa != pb) return pa > pb;
        return a < b;
    });
}
```
<!-- /snippet -->

**`stable_sort` keeps ties in order.** Use it alone when the statement says ties keep their input order, or in two passes (secondary key first) to build a multi-key order:

<!-- snippet: modules/05-searching-sorting/examples/sort_comparators.cpp#two_pass -->
```cpp
// Sort by the secondary key first, then STABLE-sort by the primary key: people with the same
// age stay in name order. (LSD radix sort is this idea, one digit per pass.)
sort(people.begin(), people.end(), by_name);          // secondary: name
stable_sort(people.begin(), people.end(), by_age);    // primary: age; ties keep name order
```
<!-- /snippet -->

**`priority_queue` inverts the comparator.** Its `top()` is the element that a sort with the same comparator would put **last**. So `less` (the default) gives a max-heap, and a min-heap needs `greater`. C# bridge: .NET's `PriorityQueue<TElement, TPriority>` dequeues the *smallest* priority first; `std::priority_queue` pops the largest.

<!-- snippet: modules/05-searching-sorting/examples/sort_comparators.cpp#pq -->
```cpp
// priority_queue keeps on top the element that a sort with the same comparator puts LAST.
priority_queue<int> max_heap;                               // less<int>: top() is the largest
priority_queue<int, vector<int>, greater<int>> min_heap;    // greater<int>: top() is the smallest
// So to pop the EARLIEST deadline first, the comparator must say "later deadline first":
auto later_deadline = [](const Task& x, const Task& y) { return x.deadline > y.deadline; };
priority_queue<Task, vector<Task>, decltype(later_deadline)> by_deadline(later_deadline);
```
<!-- /snippet -->

**Test a comparator you're unsure of.** On a handful of sample values, brute-force the four rules. The tests in sort_comparators.cpp confirm that `<=`, the fuzzy compare and `a.x < b.x || a.y < b.y` all fail, while every key-based comparator passes.

<!-- snippet: modules/05-searching-sorting/examples/sort_comparators.cpp#swo_check -->
```cpp
// Brute-force test of the strict weak ordering rules on sample values: O(n^3), for tests only.
template <class T, class Cmp>
bool is_strict_weak_ordering(const vector<T>& vals, Cmp cmp) {
    auto equiv = [&](const T& x, const T& y) { return !cmp(x, y) && !cmp(y, x); };
    for (const T& a : vals) {
        if (cmp(a, a)) return false;                                          // 1. irreflexive
        for (const T& b : vals) {
            if (cmp(a, b) && cmp(b, a)) return false;                         // 2. asymmetric
            for (const T& c : vals) {
                if (cmp(a, b) && cmp(b, c) && !cmp(a, c)) return false;       // 3. transitive
                if (equiv(a, b) && equiv(b, c) && !equiv(a, c)) return false; // 4. ties are transitive
            }
        }
    }
    return true;
}
```
<!-- /snippet -->

### Pitfalls

- `<=` or `>=` in a comparator: UB (rule 1).
- `return a.x < b.x || a.y < b.y;` is not an ordering: (1,2) and (2,1) each come before the other. Use `tie` or compare field by field.
- **Doubles with an epsilon** break rule 4. Compare exact keys instead; compare fractions a/b < c/d as `a*d < c*b` in `long long` (with b, d > 0).
- Capturing big containers by value copies them; capture by reference.
- `m[key]` inside a comparator: won't compile on a const map, and inserts on a non-const one. Use `.at()` or `find`.
- `sort(lst.begin(), lst.end())` on a `std::list` doesn't compile; call `lst.sort(cmp)`.
- Numbers sorted as strings: `"10" < "9"`. Convert, or (for non-negative numbers without leading zeros) compare by (length, string).
- `priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>>` orders by `.first`, then `.second`; put the field you want to order by first.

### Recognize it when…

- "Sort by X, then by Y", "if tied, …", "in order of …": a `tie` comparator.
- "Return the indices / original positions in sorted order": argsort.
- "Arrange the items to maximize or minimize the result": find a pairwise swap rule (exchange argument), then check it is a strict weak ordering (179).
- "Ties keep their input order": `stable_sort`.
- Only the k smallest (or largest) by some key are needed, in any order: `nth_element` or `partial_sort` with your comparator, not a full sort (973).
- A heap of structs that should pop the smallest or earliest first: invert the comparator.

### Worked example: 179. Largest Number
[LeetCode 179](https://leetcode.com/problems/largest-number/) · Medium

**Problem (paraphrased):** Arrange non-negative integers so that their concatenation is the largest possible number; return it as a string. n ≤ 100, values ≤ 10⁹.

**Signals:** "arrange", "largest", and the answer depends only on the order, so this is a sort with the right comparator. The result can be ~1000 digits long, so it has to be a string.

**Brute force, and why it fails:** try all n! orders. 100! is hopeless; even 12! ≈ 4.8·10⁸.

**Key insight:** Decide each pair by trying both orders: a goes first iff `a+b > b+a` (`+` is string concatenation; both results have the same length, so string comparison is numeric comparison). Plain lexicographic order fails: `"30" > "3"` puts 30 first and gives "303", but "330" is larger.

- *Why it's a valid comparator:* it secretly sorts by a key. a·10^len(b) + b > b·10^len(a) + a ⟺ a/(10^len(a) − 1) > b/(10^len(b) − 1), and that key is the repeating decimal 0.aaa…: 3 → 0.333…, 30 → 0.3030…, 34 → 0.3434…. A key-based order is a strict weak ordering. The test file checks the identity and the four rules on random triples.
- *Why the sorted order is optimal (exchange argument):* in any arrangement, swapping two adjacent parts that are out of order makes the string larger (only those two parts change, and the result has the same length), and swapping tied neighbours leaves it unchanged. So bubble-sorting any arrangement into the sorted order never makes it smaller: the sorted order beats or ties every arrangement.

**Dry run:** `[3, 30, 34, 5, 9]`

| pair | a+b vs b+a | goes first |
|---|---|---|
| 3, 30 | 330 vs 303 | 3 |
| 3, 34 | 334 vs 343 | 34 |
| 5, 34 | 534 vs 345 | 5 |
| 9, 5 | 95 vs 59 | 9 |

Sorted: 9, 5, 34, 3, 30 → `"9534330"`. The keys descend: 0.999…, 0.555…, 0.3434…, 0.333…, 0.3030….

<!-- snippet: modules/05-searching-sorting/examples/0179-largest-number.cpp#solution -->
```cpp
class Solution {
public:
    string largestNumber(vector<int>& nums) {
        vector<string> parts;
        for (int x : nums) parts.push_back(to_string(x));
        // a goes before b iff "a then b" beats "b then a". Both concatenations have the same
        // length, so comparing the strings compares the numbers.
        sort(parts.begin(), parts.end(), [](const string& a, const string& b) {
            return a + b > b + a;
        });
        if (parts[0] == "0") return "0";     // the best first part is "0": every part is "0"
        string result;
        for (const string& p : parts) result += p;
        return result;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n log n) comparisons, each O(L) for L ≤ 10 digits (plus two small string allocations), so O(L · n log n) time and O(nL) memory.

**Edge cases:**
- All zeros: return `"0"`, not `"000"`. If the first (largest) part is `"0"`, every part is.
- A single element; `[10]` → `"10"`.
- One number a prefix of another: 432 vs 43243 → "43243432", because 43243|432 beats 432|43243.

**Follow-ups:**
- *Why is this comparator safe for `std::sort`?* It sorts by the key a/(10^len(a) − 1), and any key-based order is a strict weak ordering.
- *Can you compare without building strings?* Compare `a·(10^len(b) − 1)` with `b·(10^len(a) − 1)`. With values ≤ 10⁹ the products stay below 10¹⁹, which fits `unsigned long long` but not `long long` (`key_before` in the test does this).
- *Negative numbers?* The problem stops making sense (where does "-" go?); ask the interviewer.

## 4. Search on sorted data — lower / upper bound, first & last occurrence, rotated arrays

**Concept: search for a boundary, not a value.** Binary search works on any predicate `ok(i)` that is monotone over the index range: false…false true…true. Sorted data hands you such predicates (`a[i] >= x`, `a[i] > x`), but the array itself need not be sorted. Only the predicate has to be monotone.

```
i:          0  1  2  3  4  5
a[i]:       1  3  3  3  7  9
a[i] >= 3:  F  T  T  T  T  T      first true: 1  (lower_bound)
a[i] >  3:  F  F  F  F  T  T      first true: 4  (upper_bound)
```

**The invariant.** On the half-open range [lo, hi): every index < lo is false, every index ≥ hi is true, and [lo, hi) is unknown. Each step tests a mid inside [lo, hi) and moves one end past it, so the unknown range shrinks every iteration; when lo == hi, lo is the first true. Write the invariant before the loop, and every `+1` and `-1` follows from it.

<!-- snippet: templates/binary_search.hpp#first_true -->
```cpp
// Smallest x in [lo, hi] with ok(x) true, for ok shaped false...false true...true.
// Returns hi + 1 when ok is false everywhere. Keep hi + 1 representable (use long long if unsure).
template <class T, class Pred>
T first_true(T lo, T hi, Pred ok) {
    hi++;                               // search the half-open range [lo, hi)
    while (lo < hi) {
        T mid = lo + (hi - lo) / 2;     // not (lo + hi) / 2, which can overflow
        if (ok(mid)) hi = mid;          // mid works: the answer is mid or left of it
        else lo = mid + 1;              // mid fails: the answer is right of it
    }
    return lo;
}
```
<!-- /snippet -->

### Every question is one of two boundaries

| question | `first_true` predicate | STL | "none" |
|---|---|---|---|
| first index with a[i] ≥ x | `a[i] >= x` | `lower_bound(x)` | n |
| first index with a[i] > x | `a[i] > x` | `upper_bound(x)` | n |
| last index with a[i] ≤ x | (first `a[i] > x`) − 1 | `upper_bound(x) − 1` | −1 |
| last index with a[i] < x | (first `a[i] >= x`) − 1 | `lower_bound(x) − 1` | −1 |
| how many equal x | | `upper_bound(x) − lower_bound(x)`, or `equal_range(x)` | 0 |
| how many in [l, r] | | `upper_bound(r) − lower_bound(l)` | 0 |

`lower_bound` / `upper_bound` return iterators (subtract `begin()` for an index), are O(log n) on random-access iterators, and need the range sorted by the same order you search with.

<!-- snippet: modules/05-searching-sorting/examples/sorted_queries.cpp#bounds -->
```cpp
// a is sorted ascending. Results are indices; "none" is a.size() for first_* and -1 for last_*.
int first_ge(const vector<int>& a, int x) { return lower_bound(a.begin(), a.end(), x) - a.begin(); }
int first_gt(const vector<int>& a, int x) { return upper_bound(a.begin(), a.end(), x) - a.begin(); }
int last_le(const vector<int>& a, int x) { return first_gt(a, x) - 1; }
int last_lt(const vector<int>& a, int x) { return first_ge(a, x) - 1; }
int count_equal(const vector<int>& a, int x) { return first_gt(a, x) - first_ge(a, x); }
int count_between(const vector<int>& a, int lo, int hi) {   // how many v with lo <= v <= hi (lo <= hi)
    return first_gt(a, hi) - first_ge(a, lo);
}
```
<!-- /snippet -->

On a `set` or `map`, use the member functions:

<!-- snippet: modules/05-searching-sorting/examples/sorted_queries.cpp#set_bounds -->
```cpp
set<int> s{10, 20, 30};
auto it = s.lower_bound(15);                            // MEMBER lower_bound: O(log n)
int successor = it == s.end() ? -1 : *it;               // smallest element >= 15: 20
int predecessor = it == s.begin() ? -1 : *prev(it);     // largest element < 15: 10
// std::lower_bound(s.begin(), s.end(), 15) compiles and returns the same iterator, in O(n):
// set iterators step one node at a time, so the "binary" search walks the whole range.
```
<!-- /snippet -->

A descending array needs the comparator it is sorted by, and any monotone predicate works with `partition_point` or `first_true`:

<!-- snippet: modules/05-searching-sorting/examples/sorted_queries.cpp#descending -->
```cpp
vector<int> desc{9, 7, 7, 4, 1};
// "Sorted" now means sorted by greater<int>, and the bounds follow that order:
int first_le_7 = lower_bound(desc.begin(), desc.end(), 7, greater<int>()) - desc.begin();  // 1: first v <= 7
int first_lt_7 = upper_bound(desc.begin(), desc.end(), 7, greater<int>()) - desc.begin();  // 3: first v < 7
```
<!-- /snippet -->

<!-- snippet: modules/05-searching-sorting/examples/sorted_queries.cpp#predicate -->
```cpp
// partition_point wants the shape true...true false...false (first_true's mirror image)
// and returns the first false.
vector<string> words{"a", "be", "cat", "door", "eagle"};          // sorted by length
auto first_long = partition_point(words.begin(), words.end(),
                                  [](const string& w) { return w.size() < 3; });   // -> "cat"
// The same boundary with the kit's template, over indices instead of iterators:
int idx = first_true(0, (int)words.size() - 1, [&](int i) { return words[i].size() >= 3; });   // 2
```
<!-- /snippet -->

**Complexity:** O(log n) predicate calls; with an O(1) predicate, O(log n) time and O(1) memory.

### First and last occurrence

Two boundary searches: first = the first index with a[i] ≥ x; last = (the first index with a[i] > x) − 1. x is present iff first < n and a[first] == x. That is the [34 worked example](#worked-example-34-find-first-and-last-position-of-element-in-sorted-array).

### Rotated sorted arrays

Rotating `[0 1 2 4 5 6 7]` gives something like `[4 5 6 7 0 1 2]`: two ascending runs, and (with distinct values) every value in the left run is larger than every value in the right run.

```
index:  0  1  2  3  4  5  6
value:  4  5  6  7  0  1  2
        |-- left run --|  |- right run -|      every left value > every right value
```

Facts to build a search on:

- "Is index i in the right run?" is monotone over i (false…false true…true), so the start of the right run (the minimum) is a `first_true` boundary. You need a membership test that takes one comparison; finding it is 153.
- For any mid, at least one of [lo, mid] and [mid, hi] is sorted, and comparing endpoints tells you which. A target can only be in the sorted half if it lies between that half's ends (33).
- **Duplicates break both facts.** With `a[lo] == a[mid] == a[hi]` you can't tell which side the drop is on (compare `[1 1 1 0 1]` with `[1 0 1 1 1]`), so the worst case degrades to O(n). That is 81's whole difficulty.

### Binary search without a sorted array

You need a monotone predicate, or an invariant that one comparison at mid can maintain. Not sortedness. For "find any peak", keep the invariant "[lo, hi] contains a peak" and ask what comparing a[mid] with a neighbour tells you about which half must still contain one (162; 1901 lifts the same argument to a grid, one column at a time). Same loop, different proof: show that the half you discard can't hold the only answer.

### Pitfalls

- **Mixing conventions.** `while (lo <= hi)` goes with the closed range [lo, hi] and `hi = mid - 1`; `while (lo < hi)` goes with the half-open [lo, hi) and `hi = mid`. Pick one (`first_true` is half-open) and never mix them in one loop.
- **`lo = mid` with a floor mid loops forever** once hi = lo + 1. If a branch must set `lo = mid`, round up: `mid = lo + (hi - lo + 1) / 2`. `first_true`'s branches (`hi = mid`, `lo = mid + 1`) never need it.
- `(lo + hi) / 2` overflows for large bounds: rare over indices, common over values (module 11).
- **Using the result unchecked.** `lower_bound` can return `end()` (index n) or point at a different value; `lower_bound(...) - 1` can be −1.
- `std::lower_bound(s.begin(), s.end(), x)` on a `set` or `map` is correct but O(n); use `s.lower_bound(x)`.
- A descending array searched with the default `<` gives garbage; pass `greater<int>()`.
- The range must be sorted by the key you search by. For a vector of pairs searched by `.first`, pass a comparator (or C++20 `ranges::lower_bound` with a projection).

### Recognize it when…

- "Sorted array" plus "O(log n)", stated or implied: binary search. Write the plain version once, invariant first, and keep it (704).
- "First / last position", "insertion point", "how many equal x", "how many in [l, r]", "closest value": lower / upper bound (34).
- "Rotated sorted array": the run facts above (33, 153, and 81 with duplicates).
- "Find a peak / local maximum in O(log n)", in an array or a grid: an invariant-based binary search on unsorted data (162, 1901).
- A sorted array where every value appears twice except one: a parity predicate over indices (540).
- A matrix sorted along its rows and its columns: start from a corner where one comparison discards a whole row or column, O(m + n) (240).
- "Median of two sorted arrays" in O(log(min(m, n))): binary-search the cut in the smaller array (4).
- "Minimum x such that feasible(x)" with monotone feasibility: binary search on the answer (module 11).

### Worked example: 34. Find First and Last Position of Element in Sorted Array
[LeetCode 34](https://leetcode.com/problems/find-first-and-last-position-of-element-in-sorted-array/) · Medium

**Problem (paraphrased):** In a non-decreasing array, return `[first index, last index]` of target, or `[-1, -1]` if it's absent; O(log n) time is required. n ≤ 10⁵, values and target in [−10⁹, 10⁹].

**Signals:** sorted input, "first and last", O(log n) demanded.

**Brute force, and why it fails:** a linear scan is O(n), which breaks the stated requirement. The tempting half-fix (binary-search any occurrence, then walk outward) is O(n) again when most of the array equals target.

**Key insight:** first and last are two boundaries of monotone predicates: first = the first index with `x >= target`; last = (the first index with `x > target`) − 1. It's one search written once and called twice. Checking `nums[first] == target` tells you whether target exists at all.

**Dry run:** `nums = [5, 7, 7, 8, 8, 10]`, target 8, test `x >= 8`:

| lo | hi | mid | nums[mid] | passes? | action |
|---|---|---|---|---|---|
| 0 | 6 | 3 | 8 | yes | hi = 3 |
| 0 | 3 | 1 | 7 | no | lo = 2 |
| 2 | 3 | 2 | 7 | no | lo = 3 |
| 3 | 3 | | | | first = 3 |

With `x > 8`: mid 3 (8, no) → lo = 4; mid 5 (10, yes) → hi = 5; mid 4 (8, no) → lo = 5. The boundary is 5, so last = 4. Answer `[3, 4]`.

<!-- snippet: modules/05-searching-sorting/examples/0034-find-first-and-last-position-of-element-in-sorted-array.cpp#solution -->
```cpp
class Solution {
public:
    vector<int> searchRange(vector<int>& nums, int target) {
        int first = firstPassing(nums, [&](int x) { return x >= target; });  // = lower_bound
        if (first == (int)nums.size() || nums[first] != target) return {-1, -1};
        int last = firstPassing(nums, [&](int x) { return x > target; }) - 1;  // = upper_bound - 1
        return {first, last};
    }

private:
    // First index whose value passes `test`, or nums.size() if none does. Along a sorted array
    // the test must read false...false true...true.
    // Invariant: every index < lo fails, every index >= hi passes; [lo, hi) is still unknown.
    template <class Test>
    int firstPassing(const vector<int>& nums, Test test) {
        int lo = 0, hi = nums.size();
        while (lo < hi) {                   // unknown range non-empty
            int mid = lo + (hi - lo) / 2;   // lo <= mid < hi, so both branches shrink [lo, hi)
            if (test(nums[mid])) hi = mid;  // mid passes: the boundary is mid or to its left
            else lo = mid + 1;              // mid fails: the boundary is right of mid
        }
        return lo;                          // lo == hi: the first passing index
    }
};
```
<!-- /snippet -->

The same with the STL, which is fine in an OA; in an interview, expect to be asked to write the search yourself:

<!-- snippet: modules/05-searching-sorting/examples/0034-find-first-and-last-position-of-element-in-sorted-array.cpp#stl -->
```cpp
vector<int> searchRange(vector<int>& nums, int target) {
    auto [lo, hi] = equal_range(nums.begin(), nums.end(), target);  // [lower_bound, upper_bound)
    if (lo == hi) return {-1, -1};                                   // empty range: absent
    return {int(lo - nums.begin()), int(hi - nums.begin()) - 1};
}
```
<!-- /snippet -->

**Complexity:** O(log n) time (two searches of about log₂ n steps each), O(1) memory.

**Edge cases:**
- Empty array: first = 0 = n. The check tests `first == n` before reading `nums[first]`, so there's no out-of-bounds read.
- Target below or above every element: first = 0 with `nums[0] != target`, or first = n.
- The whole array equals target: `[0, n - 1]`.
- target = INT_MAX: the shortcut "search for target + 1" would overflow; the strict predicate `x > target` doesn't need it.

**Follow-ups:**
- *Count the occurrences?* last − first + 1, i.e. `upper_bound − lower_bound`.
- *Array too big to know its size (a stream or an unbounded reader)?* Exponential search: probe indices 1, 2, 4, 8, … until you pass target, then binary-search the last gap. O(log p) for position p.
- *Descending array?* Flip both predicates (`x <= target` for the first index, `x < target` for the one after the last), or pass `greater<int>()` to the STL calls.

## Common mistakes

- Writing a comparator with `<=`, or with `||` across fields. Both break the strict weak ordering, and that is undefined behavior, not just a wrong order.
- Assuming "random pivot" fixes quicksort on duplicates: Lomuto with random pivots is still Θ(n²) on all-equal input. You need a 3-way (or Hoare) partition.
- Calling `sort` when the statement needs ties kept in order; use `stable_sort`.
- Choosing counting sort without checking the value range (10⁹ is 4 GB of counters), or indexing with negative keys.
- Off-by-ones from mixing binary-search conventions; write the invariant first and use one template (`first_true`).
- Using a binary-search result without checking it: `end()`, index n, index −1, or a different value.
- Treating `std::lower_bound` on a `set` as O(log n); it is O(n). Use the member function.
- Assuming a binary search needs a sorted array. It needs a monotone predicate or a maintainable invariant (peaks, rotated arrays).

## Say it out loud

A talk track for 34:

1. "Restating: the array is sorted non-decreasing, I return the first and last index of target or [−1, −1], and you want O(log n)."
2. "Brute force is a linear scan, O(n). Finding one occurrence and expanding is still O(n) when most of the array is target."
3. "Both answers are boundaries: the first index with value ≥ target, and the first index with value > target, minus one."
4. "I'll write one boundary search with the invariant 'everything left of lo fails, everything from hi on passes', and call it twice."
5. "Two searches: O(log n) time, O(1) space."
6. "Edge cases: empty array, target absent (I check first < n and nums[first] == target), all equal, and target = INT_MAX, which is why I don't search for target + 1."

Follow-ups interviewers commonly ask in this module:
- "Is your sort stable? Does it matter here?"
- "What is quicksort's worst case? When does it happen, and how do you avoid it?" (random pivot + 3-way partition; introsort)
- "What does `std::sort` use? Is it guaranteed O(n log n)?" (introsort; yes, since C++11)
- "Can you sort in O(1) extra space?" (heap sort)
- "The values are in [0, 1000]. Can you beat n log n?" (counting sort)
- "How would you sort 100 GB with 1 GB of RAM?" (external merge sort: sort 1 GB runs to disk, then k-way merge them with a heap)
- "Why is your comparator correct?" (a key-based order is a strict weak ordering; an exchange argument shows optimality)
- "What if the array has duplicates / is rotated / has unknown length?"

## Self-check

1. Which of merge sort, quicksort and heap sort are stable? Which is in place? What is each one's worst case?
<details><summary>Answer</summary>

Only merge sort is stable. Heap sort is in place (O(1) extra); quicksort needs O(log n) expected stack; merge sort needs an O(n) buffer. Worst cases: merge Θ(n log n), heap Θ(n log n), quicksort Θ(n²) (vanishingly unlikely with random pivots and a 3-way partition).
</details>

2. Why does Lomuto quicksort with a random pivot still take Θ(n²) on an all-equal array? What fixes it?
<details><summary>Answer</summary>

Every pivot has the same value, and Lomuto sends everything that is not `< pivot` to one side. Each partition then peels off a single element: n(n−1)/2 comparisons. A 3-way partition puts the whole equal block in its final place in one pass (O(n) total); Hoare also works, since it stops on equal keys from both sides and splits evenly.
</details>

3. Why does Hoare's quicksort recurse on `[lo, j]` and `[j+1, hi]`, and why can't its pivot be `a[hi]`?
<details><summary>Answer</summary>

Hoare's partition only guarantees a[lo..j] ≤ pivot ≤ a[j+1..hi]; the pivot isn't necessarily at j, so j belongs in the left call. With pivot = a[hi], if every other element is smaller, i and j both stop at hi and the partition returns j = hi. The left call is then the whole range again: infinite recursion.
</details>

4. Heapify is O(n), not O(n log n). Why?
<details><summary>Answer</summary>

A sift-down costs the node's height, not log n. About half the nodes are leaves (cost 0), a quarter have height 1, an eighth height 2, … Σ h·n/2^(h+1) = O(n). Only the n − 1 extraction sift-downs from the root cost O(log n) each.
</details>

5. State the comparison-sort lower bound and its one-line proof.
<details><summary>Answer</summary>

Ω(n log n) comparisons in the worst case. A comparison sort is a binary decision tree that needs a distinct leaf for each of the n! input orders, so its height is at least log₂(n!) = Θ(n log n).
</details>

6. In counting sort, what makes the placement pass stable? When is counting sort a bad idea?
<details><summary>Answer</summary>

It scans the input left to right and writes each record to `start[key]++`, so equal keys fill consecutive slots in input order. It's a bad idea when the key range K is much larger than n (O(n + K) time and memory; K = 10⁹ is 4 GB), or when the keys aren't integers.
</details>

7. Why does LSD radix sort need a stable per-digit sort? Give a two-number counterexample.
<details><summary>Answer</summary>

The invariant "sorted by the last d digits" survives pass d+1 only if ties on digit d+1 keep their previous order. [12, 11]: pass 1 (ones) gives [11, 12]; pass 2 (tens) sees two 1s, and an unstable sort may output [12, 11], which is wrong.
</details>

8. Is `[](const P& a, const P& b) { return a.x < b.x || a.y < b.y; }` a valid comparator? Fix it.
<details><summary>Answer</summary>

No. For a = (1, 2) and b = (2, 1), both `cmp(a, b)` and `cmp(b, a)` are true, which breaks asymmetry. Fix: `return tie(a.x, a.y) < tie(b.x, b.y);` (or compare y only when the x values are equal).
</details>

9. For a sorted `vector<int> a`, give the index of the last element < x using the STL. What does it return when there is none?
<details><summary>Answer</summary>

`lower_bound(a.begin(), a.end(), x) - a.begin() - 1`. It is −1 when every element is ≥ x (lower_bound returns begin), so check before indexing.
</details>

10. In a rotated sorted array with duplicates, why can binary search degrade to O(n)?
<details><summary>Answer</summary>

When `a[lo] == a[mid] == a[hi]`, the comparison can't tell which half holds the rotation point ([1 1 1 0 1] vs [1 0 1 1 1]), so you can only shrink the range by one element at a time. Inputs that are almost all equal force that at every step.
</details>
