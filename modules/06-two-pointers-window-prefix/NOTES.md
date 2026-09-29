# 06 · Two Pointers + Sliding Window + Prefix Sum
> Turn nested loops into a single pass — the most reused trick in the book.

**Time:** ~15 h of Core work ([problems](problems.md)) · **Prereqs:** modules 02 section 2 (the skeletons), 03 (amortized cost), 04 (arrays, strings, counting subarrays) · **You're done when:** you can prove the converging-pointer invariant on 167 and explain why 11 moves the shorter wall; you can write the longest, shortest and counting window templates from memory and pick the right one from a problem statement; you can derive the 2D inclusion–exclusion query and the difference-array update on paper, inclusive and exclusive ends included.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | Two pointers | Sortedness lets one comparison discard a whole row or column of pairs; fix one element and repeat for triplets | [two_pointers.cpp](examples/two_pointers.cpp), 02's converging skeleton | 167, 11, 15, 18, 42 |
| 2 | Fixed & variable windows | Grow r, shrink l while the monotone condition allows; r − l + 1 counts; exactly(K) = atMost(K) − atMost(K−1) | [windows.cpp](examples/windows.cpp), 02's window skeletons | 3, 1423, 1004, 424, 340, 1358, 930, 1248, 76 |
| 3 | Prefix & suffix sums | sum(l..r) = pre[r+1] − pre[l]; four-corner inclusion–exclusion in 2D; prefix × suffix | [prefix_sum.hpp](../../templates/prefix_sum.hpp), [prefix_suffix.cpp](examples/prefix_suffix.cpp) | 303, 238, 304 |
| 4 | Difference arrays | Range add = two point updates (+v at l, −v at r+1); one prefix sum rebuilds; events sorted by position | [prefix_sum.hpp](../../templates/prefix_sum.hpp), [sweep.cpp](examples/sweep.cpp) | 1109, 1094 |

## 1. Two pointers — opposite ends, same direction, pair and triplet sums

### Concept

**Converging pointers.** Draw every pair (i, j) with i < j as the upper triangle of a grid. The brute force visits every cell: O(n²). On a **sorted** array, one comparison at (l, r) settles an entire row or column:

- a[l] + a[r] < target: every pair (l, j) with j ≤ r sums to at most a[l] + a[r], so it's too small too. **Row l is dead**: l++.
- a[l] + a[r] > target: every pair (i, r) with i ≥ l sums to at least a[l] + a[r], so it's too big too. **Column r is dead**: r--.

```
        j=1  j=2  j=3  j=4          one comparison at (l, r) deletes a whole row l
  i=0    ·    ·    ·    ·           (sum too small) or a whole column r (too big):
  i=1         ·    ·    ·           at most n − 1 comparisons instead of n(n−1)/2
  i=2              ·    ·
  i=3                   ·
```

**Invariant:** no pair that uses an index outside [l, r] can be the answer (or, when counting, it has already been counted). It holds at the start, each move keeps it, and each move shrinks r − l by one. That gives both correctness and the O(n) bound; 167 below turns it into a full proof.

The same elimination idea works with other pair scores. [11](https://leetcode.com/problems/container-with-most-water/) scores a pair by width × the shorter height; its list line asks you to *say why* one of the two walls can be discarded at every step. Try it before opening the proof.

<details><summary>Proof for 11 (after you've tried)</summary>

Area(l, r) = (r − l) · min(h[l], h[r]). Suppose h[l] ≤ h[r]. Every other pair that uses line l with a line inside the current range, (l, j) with l < j < r, is narrower, and its height is capped by h[l]. So none of them beats (l, r): line l can be discarded without losing the optimum. Discarding the taller line r instead is not safe: a pair (i, r) with a tall h[i] inside the range could still be the best. Each step discards one line: O(n).
</details>

**Same-direction pointers.** Both pointers move only forward: a slow one marks a boundary (the write position, the left edge of a valid range, the next unmatched element) and a fast one scans. Each pointer moves at most n times in total, so even with a `while` inside a `for`, the whole thing is O(n). You've met two shapes already: read / write filtering (04 section 1) and walking two sorted sequences together (02 section 2). The template below is the third common one: for each right end j, keep the leftmost i that is still "close enough".

**Triplets with dedupe.** k-sum problems reduce to pairs: sort, fix the smallest element, and run the pair scan on the rest with target − fixed. Sorting also puts equal values next to each other, which is what makes skipping duplicates a one-line check (15 below). One more fixed element gives four numbers ([18](https://leetcode.com/problems/4sum/)): O(n³), and at that point the sums need 64 bits. [42](https://leetcode.com/problems/trapping-rain-water/) is a converging scan too, driven by the running maxima seen from each end.

### Template

Converging, from module 02 (counting pairs with sum < target shows the "count a whole row at once" move):

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#converging -->
```cpp
// How many pairs i < j have a[i] + a[j] < target? `a` is sorted ascending.
// Invariant: every pair that uses an index outside [l, r] is already counted or ruled out.
long long count_pairs_below(const vector<int>& a, long long target) {
    long long count = 0;
    int l = 0, r = (int)a.size() - 1;
    while (l < r) {
        if ((long long)a[l] + a[r] < target) {
            count += r - l;   // a[l] + a[j] <= a[l] + a[r] for every j in (l, r]: all r - l pairs count
            l++;              // a[l] is finished
        } else {
            r--;              // a[r] + a[i] >= a[l] + a[r] >= target for every i in [l, r): a[r] is finished
        }
    }
    return count;
}
```
<!-- /snippet -->

Same direction, one array:

<!-- snippet: modules/06-two-pointers-window-prefix/examples/two_pointers.cpp#same_direction -->
```cpp
// How many pairs i < j have a[j] - a[i] <= d?  `a` sorted ascending, d >= 0.
// For a right end j, the valid left ends are exactly [i, j): a suffix of the indices before j.
// When j moves right, a[j] grows, so i never has to move back: both pointers only advance, and the
// whole loop is O(n) (after an O(n log n) sort, if the input isn't sorted yet).
long long count_close_pairs(const vector<int>& a, long long d) {
    long long count = 0;
    int i = 0;
    for (int j = 0; j < (int)a.size(); j++) {
        while ((long long)a[j] - a[i] > d) i++;   // a[i] is too far from a[j], and from every later a[j]
        count += j - i;                           // pairs (i, j), (i+1, j), ..., (j-1, j)
    }
    return count;
}
```
<!-- /snippet -->

Both are O(n) after an O(n log n) sort, with O(1) extra space.

### Pitfalls

- **Unsorted input:** the elimination argument needs sorted data. Sorting loses the original indices; if the answer needs them, sort `(value, index)` pairs or use a hash map (07 section 1).
- **Overflow:** two values near 10⁹ sum past `INT_MAX`, and four certainly do. Compute in `long long`.
- **3Sum dedupe on the wrong side:** skipping i when `nums[i] == nums[i+1]` throws away the first of a run of equal values before it's used, losing triplets like [−1, −1, 2]. Compare with the *previous* element.
- **`while (l < r)`, not `l <= r`:** a pair needs two different indices.
- **`int r = nums.size() - 1`** wraps on empty input; write `(int)nums.size() - 1`.
- **Same direction:** make sure the inner `while` can't push the slow pointer past the fast one (in the template, i stops at j because a[j] − a[j] = 0 ≤ d).

### Recognize it when…

- "sorted" + "two numbers / pair / triplet" + a target sum or difference → converging pointers.
- "maximize something scored by a pair from the two ends" (width × height) → converging with an elimination proof.
- "unique triplets / quadruplets" → sort + fix + pair scan + skip duplicates.
- "pairs within distance d", "merge", "is a subsequence of", two arrays walked in order → same direction.
- "trapped water between bars" → converging with running maxima.

### Worked example: 167. Two Sum II - Input Array Is Sorted
[LeetCode 167](https://leetcode.com/problems/two-sum-ii-input-array-is-sorted/) · Medium

**Problem (paraphrased):** A non-decreasing array contains exactly one pair of different positions whose values add up to a target. Return the two positions, 1-indexed, using constant extra space.

**Signals:** "sorted", "two numbers that add up to target", O(1) extra space.

**Brute force, and why it fails:** All pairs is O(n²): about 4.5·10⁸ at n = 3·10⁴, and it ignores the sortedness. Binary-searching target − a[i] for each i is O(n log n): fine, but not optimal. A hash map is O(n) time but O(n) space, which is forbidden.

**Key insight:** At (l, r), a sum that's too small can't be fixed by pairing a[l] with anything, because a[r] is already the largest partner left, so drop l; too big, and a[r] fails even with the smallest partner, so drop r.

**Proof:** let (i*, j*) be the answer. Show that the pointers never step past it: while l ≤ i* and r ≥ j*, one of four cases holds.
- l < i* and r > j*: either move keeps l ≤ i* and r ≥ j*.
- l = i* and r > j*: the sum is > target (a[r] ≥ a[j*], and equality would make (i*, r) a second solution), so r moves, toward j*.
- l < i* and r = j*: by the mirror argument the sum is < target, so l moves, toward i*.
- l = i* and r = j*: found.

Each step shrinks the range, so the pointers reach the answer.

**Dry run:** numbers = [2, 7, 11, 15], target 9:

| l | r | sum | action |
|---|---|---|---|
| 0 | 3 | 17 | > 9: 15 is too big for any partner, r-- |
| 0 | 2 | 13 | > 9: r-- |
| 0 | 1 | 9 | found: return [1, 2] |

<!-- snippet: modules/06-two-pointers-window-prefix/examples/0167-two-sum-ii-input-array-is-sorted.cpp#solution -->
```cpp
class Solution {
public:
    vector<int> twoSum(vector<int>& numbers, int target) {
        int l = 0, r = (int)numbers.size() - 1;
        while (l < r) {
            int sum = numbers[l] + numbers[r];          // |values| <= 1000: no overflow
            if (sum == target) return {l + 1, r + 1};   // the answer is 1-indexed
            if (sum < target) l++;                      // numbers[l] is too small even with the largest partner
            else r--;                                   // numbers[r] is too big even with the smallest partner
        }
        return {};                                      // unreachable: a solution is guaranteed
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time: each step discards one index. O(1) extra space.

**Edge cases:**
- Equal values such as [3, 3] with target 6: two different indices, handled.
- Negative numbers; the answer at the two ends.
- The output is 1-indexed.

**Follow-ups:**
- *The input isn't sorted?* Hash map from value to index (07 section 1), O(n) time and space; or sort `(value, index)` pairs and scan, O(n log n).
- *Count every pair with the target sum, duplicates included?* When the sum matches, count the run of equal values on each side and multiply; if a[l] == a[r], the whole range is one run, so add C(len, 2).
- *Closest sum instead of an exact match?* Same scan; track the smallest |sum − target|.

### Worked example: 15. 3Sum
[LeetCode 15](https://leetcode.com/problems/3sum/) · Medium

**Problem (paraphrased):** Return every distinct triple of values, taken from three different positions, that sums to zero. No triple may appear twice.

**Signals:** "triplets", "sum to zero", "no duplicate triplets"; n ≤ 3000, so O(n²) ≈ 9·10⁶ is fine and O(n³) ≈ 2.7·10¹⁰ isn't.

**Brute force, and why it fails:** Three nested loops plus a set of sorted triples: O(n³ log n).

**Key insight:** Sort. Fix the smallest element `nums[i]`; the other two must sum to −nums[i] inside the sorted suffix, which is 167. Deduplicate at both levels: skip an i whose value equals the previous one, and after recording a triple, move l past equal values (an equal r value then overshoots the sum and gets skipped by `r--`).

**Dry run:** sorted [−4, −1, −1, 0, 1, 2]:

| i | nums[i] | l, r | sum | action |
|---|---|---|---|---|
| 0 | −4 | 1, 5 | −3 | too small: l++ (then −3, −2, −1 at l = 2, 3, 4; l meets r) |
| 1 | −1 | 2, 5 | 0 | record [−1, −1, 2]; l = 3, r = 4 |
| 1 | −1 | 3, 4 | 0 | record [−1, 0, 1]; l = 4, r = 3: stop |
| 2 | −1 | — | — | same value as nums[1]: skip |
| 3 | 0 | 4, 5 | 3 | too big: r-- (l meets r) |

<!-- snippet: modules/06-two-pointers-window-prefix/examples/0015-3sum.cpp#solution -->
```cpp
class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
        sort(nums.begin(), nums.end());
        int n = (int)nums.size();
        vector<vector<int>> result;
        for (int i = 0; i + 2 < n; i++) {
            if (nums[i] > 0) break;                              // the smallest of the three is positive
            if (i > 0 && nums[i] == nums[i - 1]) continue;       // same first value: same triplets again
            int l = i + 1, r = n - 1;
            while (l < r) {                                      // 167 on nums[i+1..n-1], target -nums[i]
                int sum = nums[i] + nums[l] + nums[r];           // |sum| <= 3 * 10^5: fits in int
                if (sum < 0) {
                    l++;
                } else if (sum > 0) {
                    r--;
                } else {
                    result.push_back({nums[i], nums[l], nums[r]});
                    l++;
                    r--;
                    while (l < r && nums[l] == nums[l - 1]) l++; // skip equal second values; an equal
                }                                                // third value then overshoots and r-- skips it
            }
        }
        return result;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n²) time: n pair scans of O(n) each, after an O(n log n) sort. O(1) extra space besides the output (the sort works in place).

**Edge cases:**
- All zeros: exactly one triple [0, 0, 0], even with 3000 zeros (tested).
- No triple at all; exactly three elements.
- Runs of duplicates on both sides, such as [−2, −2, 0, 0, 2, 2] → only [−2, 0, 2].

**Follow-ups:**
- *Why compare `nums[i]` with `nums[i-1]` and not with `nums[i+1]`?* The latter skips the first element of a run before it has served as the smallest element, which loses triples that use two equal values.
- *Closest sum, count of sums below a target, four numbers?* The same frame with a different test inside the scan; four numbers add one more loop and need `long long` sums.
- *A hash-set version?* Also O(n²), but deduplication gets messy. The sorted version is the one to write in an interview.

## 2. Fixed & variable windows — longest / shortest window under a constraint

### Concept

A window is a range [l, r] whose two ends both move only to the right: same-direction two pointers. Each element enters once (r passes it) and leaves at most once (l passes it), so the pass is O(n) no matter how the inner `while` loops are distributed.

**Pick the template from the question:**

| The question asks for… | Template | Where you record the answer |
|---|---|---|
| a result for every window of size k | fixed | once the window is full, every step |
| the longest window satisfying P | variable, longest | after the shrink loop |
| the shortest window satisfying P | variable, shortest | inside the shrink loop, while still valid |
| the number of subarrays satisfying P | counting | after the shrink loop: add r − l + 1 |
| the number with some measure exactly K | exactly(K) | atMost(K) − atMost(K − 1) |

**Why it works: the condition must be monotone.**

- *Longest* needs P to be **shrink-closed**: if [l, r] satisfies P, so does every window inside it. Then for each r the valid left ends form one block [L(r), r], and L(r) never decreases as r grows, so l simply follows L(r).
- *Shortest* needs P to be **grow-closed**: if [l, r] satisfies P, so does every window containing it. Then, for each r, the shrink loop walks l forward exactly as long as the window stays valid, recording each valid window on the way.
- *Counting* reuses L(r): every start in [L(r), r] gives a valid subarray ending at r. That's r − L(r) + 1 subarrays, added in O(1).

**Exactly K.** Suppose the measure (a count, a maximum frequency, a number of distinct values) takes integer values and "measure ≤ K" is shrink-closed. The subarrays with measure ≤ K split into those with measure exactly K and those with measure ≤ K − 1, with no overlap. So **exactly(K) = atMost(K) − atMost(K − 1)**: two counting passes. An "exactly" condition is *not* monotone by itself (shrinking can drop the measure below K), which is why it needs this detour.

**When windows fail.** The monotonicity comes from the data. Sums are monotone only when every value is ≥ 0: with a negative number, "sum ≤ budget" isn't shrink-closed ([−10, 5] sums to −5, but its part [5] doesn't fit budget 3), and the window gives wrong answers with no error. The test file pins down a concrete case with the module 02 longest-window skeleton:

| r | a[r] | sum | shrink | window | best |
|---|---|---|---|---|---|
| 0 | 2 | 2 | — | [0, 0] | 1 |
| 1 | 2 | 4 | over budget 3: drop index 0 → 2 | [1, 1] | 1 |
| 2 | −10 | −8 | — | [1, 2] | 2 |
| 3 | 2 | −6 | — | [1, 3] | 3 |

The window reports 3, but the whole array [2, 2, −10, 2] sums to −4 and fits: the answer is 4. Index 0 was dropped at r = 1 and can never come back. With negatives, use **prefix sums**: + a hash map for "sum exactly k" (07 section 4), + a monotonic deque for "shortest subarray with sum ≥ k" (module 09's deque technique), + binary search over running prefix maxima for "longest with sum ≤ k".

### Template

**Fixed size**, from module 02. The window's summary (here a count map) is updated for one element in and one out. When you must compare two frequency tables at every step, keep a counter of how many letters currently match and update it as counts change, instead of re-comparing all 26 entries.

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#fixed_window -->
```cpp
// Number of distinct values in every window of length k (1 <= k <= n), left to right.
// Each step one value enters on the right and one leaves on the left: update, never rebuild.
vector<int> distinct_in_windows(const vector<int>& a, int k) {
    unordered_map<int, int> count;                   // value -> occurrences inside the window
    vector<int> result;
    for (int r = 0; r < (int)a.size(); r++) {
        count[a[r]]++;                               // a[r] enters
        if (r >= k) {                                // a[r - k] leaves
            if (--count[a[r - k]] == 0) count.erase(a[r - k]);
        }
        if (r >= k - 1) result.push_back((int)count.size());   // window a[r-k+1..r] is complete
    }
    return result;
}
```
<!-- /snippet -->

**Variable, longest**, from module 02 (worked in full on strings in 3 below):

<!-- snippet: modules/02-patterns/examples/skeletons.cpp#variable_window -->
```cpp
// Length of the longest subarray with sum <= budget. Needs every a[i] >= 0 and budget >= 0.
// Removing elements never raises the sum, so once [l, r] is over budget the only fix is to move
// l right, and l never has to move back: each index enters once and leaves once. O(n).
int longest_within_budget(const vector<int>& a, long long budget) {
    long long sum = 0;
    int best = 0;
    for (int l = 0, r = 0; r < (int)a.size(); r++) {
        sum += a[r];                           // grow: take a[r]
        while (sum > budget) sum -= a[l++];    // shrink until the window is valid again
        best = max(best, r - l + 1);           // a[l..r] is the longest valid window ending at r
    }
    return best;
}
```
<!-- /snippet -->

**Variable, shortest.** "At least k distinct values" is grow-closed. Record inside the loop, then shrink (worked in full with character counts in 76 below):

<!-- snippet: modules/06-two-pointers-window-prefix/examples/windows.cpp#shortest -->
```cpp
// Length of the shortest subarray that contains at least k distinct values (k >= 1); 0 if none does.
// Shortest-window template: grow r; WHILE the window is valid, record it and shrink from the left.
// Adding elements never removes a distinct value, so for each r the loop ends with the shortest
// valid window ending at r already recorded.
int shortest_with_k_distinct(const vector<int>& a, int k) {
    unordered_map<int, int> count;              // value -> occurrences in a[l..r]
    int best = INT_MAX;
    for (int l = 0, r = 0; r < (int)a.size(); r++) {
        count[a[r]]++;
        while ((int)count.size() >= k) {        // valid: record, then try a shorter one
            best = min(best, r - l + 1);
            if (--count[a[l]] == 0) count.erase(a[l]);
            l++;
        }
    }
    return best == INT_MAX ? 0 : best;
}
```
<!-- /snippet -->

Trace on [1, 2, 2, 3, 1], k = 3: the window first becomes valid at r = 3 ([1, 2, 2, 3], length 4); dropping the 1 breaks it. At r = 4, [2, 2, 3, 1] is valid (4), then [2, 3, 1] (**3**), then dropping a 2 breaks it. Answer 3.

**Counting.** "No character more than K times" is shrink-closed, so every start in [l, r] is valid:

<!-- snippet: modules/06-two-pointers-window-prefix/examples/windows.cpp#count_at_most -->
```cpp
// Number of substrings in which no character appears more than K times (K >= 0).
// The rule is shrink-closed: if s[l..r] obeys it, so does every substring of it. So once l is the
// smallest valid left end for r, EVERY start in [l, r] is valid: r - l + 1 substrings end at r.
long long count_at_most(const string& s, int K) {
    int freq[256] = {};
    long long total = 0;
    for (int l = 0, r = 0; r < (int)s.size(); r++) {
        unsigned char c = s[r];
        freq[c]++;
        while (freq[c] > K) freq[(unsigned char)s[l++]]--;   // only s[r]'s count can break the rule
        total += r - l + 1;
    }
    return total;
}
```
<!-- /snippet -->

Trace on "abca", K = 1: r = 0, 1, 2 add 1, 2, 3 (total 6). At r = 3 the second 'a' forces l to 1, then r − l + 1 = 3 more: **9**, which is all 10 substrings except "abca" itself.

**Exactly K:**

<!-- snippet: modules/06-two-pointers-window-prefix/examples/windows.cpp#exactly -->
```cpp
// Number of substrings whose most frequent character appears exactly K times (K >= 1).
// {max frequency <= K} = {max frequency == K} + {max frequency <= K - 1}, and the two parts don't
// overlap, so:  exactly(K) = atMost(K) - atMost(K - 1).
long long count_exactly(const string& s, int K) {
    return count_at_most(s, K) - count_at_most(s, K - 1);
}
```
<!-- /snippet -->

For "aab" and K = 2: atMost(2) counts all 6 substrings, atMost(1) counts the 4 without a repeat ("a", "a", "b", "ab"), and 6 − 4 = 2 ("aa", "aab"). Every template here is O(n) time; the space is the size of the count structure.

### Pitfalls

- **Negative numbers with sum conditions:** not a window problem (see the table above).
- **Recording in the wrong place:** longest records after the shrink loop, shortest inside it. Swap them and you get the widest valid window or garbage.
- **A shrink loop that runs l past r:** make sure the condition becomes false by the time the window is empty (e.g. `K >= 0` in the counting template, `k >= 1` in the shortest one), or guard with `l <= r`.
- **Zombie keys:** a map entry whose count dropped to 0 still counts in `size()`. Erase it.
- **`r - l + 1` versus `r - l`:** the window [l, r] holds r − l + 1 elements.
- **Counts of subarrays overflow `int`:** up to n(n+1)/2 = 5·10⁹ at n = 10⁵. Use `long long`.
- **atMost(K − 1) with K = 0:** it must return 0 (nothing has a negative count); write that case explicitly if your window code can't handle a negative limit.
- **Signed `char` indices:** cast to `unsigned char` before indexing a count array.
- **A stale maximum is sometimes fine:** in [424](https://leetcode.com/problems/longest-repeating-character-replacement/), the list line asks why the tracked maximum frequency never has to decrease. Work out why before trusting it.

### Recognize it when…

- "substring / subarray" + "longest / shortest / at most / at least", with counts or non-negative values → variable window.
- "of length k", "k consecutive" → fixed window.
- "number of subarrays / substrings where…" with a shrink-closed condition → counting (r − l + 1).
- "exactly K distinct / odd / ones / occurrences" → atMost(K) − atMost(K − 1).
- "contains every character of t" → shortest window with counts.
- "flip / replace at most k elements" → a longest window whose budget is the number of elements that don't fit.
- Negative values + a sum condition → prefix sums instead.

### Worked example: 3. Longest Substring Without Repeating Characters
[LeetCode 3](https://leetcode.com/problems/longest-substring-without-repeating-characters/) · Medium

**Problem (paraphrased):** Return the length of the longest substring of s in which no character appears twice.

**Signals:** "longest substring", "without repeating" (a shrink-closed condition); length up to 5·10⁴; letters, digits, symbols and spaces.

**Brute force, and why it fails:** Checking every substring with a set is O(n³). Extending each start until the first repeat is much better, O(n·k) where k ≤ 95 is the alphabet size (a repeat-free substring can't be longer than k), and would pass here, but it depends on the alphabet being small. The window is O(n) regardless, and it's the answer interviewers expect.

**Key insight:** "No repeats" is shrink-closed: every piece of a repeat-free substring is repeat-free. So use the longest-window template: add s[r]; while s[r] now appears twice, drop s[l] and advance l; the window is then the longest valid one ending at r.

**Dry run:** s = "abcabcbb":

| r | s[r] | shrink | window | best |
|---|---|---|---|---|
| 0 | a | — | a | 1 |
| 1 | b | — | ab | 2 |
| 2 | c | — | abc | **3** |
| 3 | a | drop a | bca | 3 |
| 4 | b | drop b | cab | 3 |
| 5 | c | drop c | abc | 3 |
| 6 | b | drop a, drop b | cb | 3 |
| 7 | b | drop c, drop b | b | 3 |

<!-- snippet: modules/06-two-pointers-window-prefix/examples/0003-longest-substring-without-repeating-characters.cpp#solution -->
```cpp
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        int count[256] = {};                   // occurrences of each byte inside the window s[l..r]
        int best = 0;
        for (int l = 0, r = 0; r < (int)s.size(); r++) {
            unsigned char c = s[r];            // unsigned: a negative char would index out of bounds
            count[c]++;
            while (count[c] > 1)               // only s[r] can be duplicated: shrink until it isn't
                count[(unsigned char)s[l++]]--;
            best = max(best, r - l + 1);       // s[l..r] is the longest valid window ending at r
        }
        return best;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time: l and r each move at most n times. O(1) space: 256 counters.

**Edge cases:**
- The empty string (0); a single space (1: spaces are characters).
- All the same character (1).
- "abba" and "dvdf": both tested; "abba" is the trap for the jumping variant below.

**Follow-ups:**
- *Skip the one-by-one shrinking?* Store each character's last index and jump l past it. The `max` matters: in "abba", the second 'a' must not pull l back to index 1.

<!-- snippet: modules/06-two-pointers-window-prefix/examples/0003-longest-substring-without-repeating-characters.cpp#jump -->
```cpp
// Variant: remember where each character was last seen and jump l past it in one step.
int longest_by_jumping(const string& s) {
    vector<int> last(256, -1);                 // last index of each byte, -1 = not seen yet
    int best = 0;
    for (int l = 0, r = 0; r < (int)s.size(); r++) {
        unsigned char c = s[r];
        l = max(l, last[c] + 1);               // max: never move l backwards to an old occurrence
        last[c] = r;
        best = max(best, r - l + 1);
    }
    return best;
}
```
<!-- /snippet -->

- *Why counts and not a `set<char>`?* A set works too (erase s[l] while s[r] is still in it), but a count array is faster, and it generalizes to rules like "at most k copies of each character".
- *Return the substring itself?* Save l together with the best length, then `substr`.

### Worked example: 76. Minimum Window Substring
[LeetCode 76](https://leetcode.com/problems/minimum-window-substring/) · Hard

**Problem (paraphrased):** Return the shortest substring of s that contains every character of t, counting repeats; return "" if there is none.

**Signals:** "minimum window", "contains all the characters of t"; both strings up to 10⁵ long, and the follow-up asks for O(m + n).

**Brute force, and why it fails:** Every substring (about m²/2 = 5·10⁹ of them) with a count check each: hopeless at 10⁵.

**Key insight:** "Contains all of t" is grow-closed (adding characters never loses coverage), so use the shortest-window template. To test validity in O(1), keep `formed` = the number of distinct characters of t whose required count is currently met; the window is valid exactly when `formed == required`. A count only changes `formed` when it *crosses* its requirement: up when `have` reaches `need`, down when it drops below.

**Dry run:** s = "ADOBECODEBANC", t = "ABC" (required = 3):

| r | s[r] | event | windows recorded while valid, then the break | best |
|---|---|---|---|---|
| 5 | C | formed = 3 | [0..5] "ADOBEC" (6); dropping A breaks it | ADOBEC |
| 10 | A | formed = 3 | [1..10] (10) down to [5..10] "CODEBA" (6), none shorter; dropping C breaks it | ADOBEC |
| 12 | C | formed = 3 | [6..12] (7), [7..12] (6), [8..12] "EBANC" (5), [9..12] "BANC" (4); dropping B breaks it | **BANC** |

In the second row, dropping the B at index 3 doesn't break the window: the B at index 9 still covers the need.

<!-- snippet: modules/06-two-pointers-window-prefix/examples/0076-minimum-window-substring.cpp#solution -->
```cpp
class Solution {
public:
    string minWindow(string s, string t) {         // constraints: s and t are non-empty
        int need[256] = {}, have[256] = {};
        int required = 0;                          // distinct characters in t
        for (unsigned char c : t)
            if (need[c]++ == 0) required++;
        int formed = 0;                            // distinct characters whose need is met by s[l..r]
        int bestStart = 0, bestLen = INT_MAX;
        for (int l = 0, r = 0; r < (int)s.size(); r++) {
            unsigned char c = s[r];
            if (++have[c] == need[c]) formed++;    // c just reached its required count
            while (formed == required) {           // valid: record it, then try a shorter one
                if (r - l + 1 < bestLen) {
                    bestStart = l;
                    bestLen = r - l + 1;
                }
                unsigned char d = s[l++];
                if (have[d]-- == need[d]) formed--; // d just dropped below its required count
            }
        }
        return bestLen == INT_MAX ? "" : s.substr(bestStart, bestLen);
    }
};
```
<!-- /snippet -->

**Complexity:** O(m + n): t is counted once, and every index of s enters and leaves the window at most once. O(1) extra space: two 256-entry arrays.

**Edge cases:**
- t longer than s, or t needing more copies than s has ("a" vs "aa") → "".
- t with repeated letters; case sensitivity ("xyz" vs "Z" → "").
- The answer is all of s ("abc" vs "cba").

**Follow-ups:**
- *Why count distinct characters in `formed` rather than characters?* Either works: a `missing` counter of t's characters not yet covered (decremented only when a real need is met) gives the same O(1) validity test.
- *Only lowercase letters, or a huge alphabet?* Arrays of 26 for the former; `unordered_map` for the latter.
- *Which window wins a tie?* This code keeps the leftmost shortest window, because it records only strictly shorter ones.

## 3. Prefix & suffix sums — range queries, equilibrium points, 2D prefix grids

### Concept

**1D prefix sums.** Let pre[i] be the sum of the first i elements, so pre[0] = 0 and pre has n + 1 entries. Then a[l] + … + a[r] = pre[r+1] − pre[l]: "everything up to r" minus "everything before l". One O(n) pass, then O(1) per query.

```
index:       0    1    2    3    4
a:           3   -1    4    1   -5
pre:    0    3    2    6    7    2         (pre[0] = 0 is the empty prefix)
sum(1..3) = pre[4] − pre[1] = 7 − 3 = 4    (−1 + 4 + 1)
```

The extra slot pre[0] removes the special case l = 0. The trick works for any operation you can undo: sums, XOR (prefix XOR), counts ("how many vowels in s[l..r]"). It does **not** work for min or max, which can't be undone; for static range minimum, use a sparse table (module 17).

**Running sum + total.** If you only walk left to right, you often don't need the array at all: the left part's sum is a running total, and the right part's is total − left (minus the current element, when it belongs to neither side). The "equilibrium index", where the left sum equals the right sum, is found this way in one pass with O(1) extra space; the template below counts split points with the same idea.

**Prefix × suffix.** Anything of the form "combine everything except position i" is (everything before i) ⊕ (everything after i), where ⊕ is the combining operation (+, ×, XOR, …). For products this avoids division, which fails on zeros: 238 below.

**2D prefix sums.** Let P[i][j] be the sum of the top-left block of i rows and j columns (row 0 and column 0 of P are zeros). Build and query both come from inclusion–exclusion:

```
         c1          c2
     +---------+-----------+
     |    A    |     B     |       sum(D) = (A+B+C+D) − (A+B) − (A+C) + A
  r1 +---------+-----------+              = P[r2+1][c2+1] − P[r1][c2+1] − P[r2+1][c1] + P[r1][c1]
     |    C    |     D     |
  r2 +---------+-----------+       build:  P[i+1][j+1] = a[i][j] + P[i][j+1] + P[i+1][j] − P[i][j]
```

Removing the strip above (A+B) and the strip to the left (A+C) removes the corner A twice, so it's added back once. The build is the same argument: the block above plus the block to the left count their overlap twice.

Check on the grid [[1, 2, 3], [4, 5, 6], [7, 8, 9]]: the P entries needed are P[3][3] = 45, P[1][3] = 6, P[3][1] = 12 and P[1][1] = 1, so the bottom-right 2 × 2 block sums to 45 − 6 − 12 + 1 = 28 = 5 + 6 + 8 + 9.

### Template

The kit's `templates/prefix_sum.hpp` holds both structures, tested against brute force. [303](https://leetcode.com/problems/range-sum-query-immutable/) and [304](https://leetcode.com/problems/range-sum-query-2d-immutable/) are these structures wrapped in LeetCode's classes, so the code is folded: write those two from the formulas above first, then compare.

<details><summary>PrefixSum, PrefixSum2D and a usage example (open after you've solved 303 and 304)</summary>

<!-- snippet: templates/prefix_sum.hpp#prefix1d -->
```cpp
// pre[i] = a[0] + ... + a[i-1]: pre[0] = 0 (the empty prefix), and pre has n + 1 entries.
// a[l] + ... + a[r] = pre[r + 1] - pre[l]: "everything up to r" minus "everything before l".
struct PrefixSum {
    vector<long long> pre;

    explicit PrefixSum(const vector<int>& a) : pre(a.size() + 1, 0) {
        for (size_t i = 0; i < a.size(); i++) pre[i + 1] = pre[i] + a[i];
    }
    // Sum of a[l..r], inclusive. Needs 0 <= l <= r + 1 <= n; l == r + 1 is the empty range (0).
    long long sum(int l, int r) const { return pre[r + 1] - pre[l]; }
};
```
<!-- /snippet -->

<!-- snippet: modules/06-two-pointers-window-prefix/examples/prefix_suffix.cpp#using_prefix -->
```cpp
// Average of each query range [l, r], after one O(n) build: O(n + q) instead of O(n * q).
vector<double> range_averages(const vector<int>& a, const vector<pair<int, int>>& queries) {
    PrefixSum ps(a);
    vector<double> out;
    for (auto [l, r] : queries) out.push_back((double)ps.sum(l, r) / (r - l + 1));
    return out;
}
```
<!-- /snippet -->

<!-- snippet: templates/prefix_sum.hpp#prefix2d -->
```cpp
// P[i][j] = sum of the top-left block a[0..i-1][0..j-1]; row 0 and column 0 of P stay 0.
// Build: the block above plus the block to the left count their overlap twice: subtract it once.
// Query: the big block, minus the strip above, minus the strip to the left, plus the corner
//        that both strips removed.
struct PrefixSum2D {
    vector<vector<long long>> P;

    explicit PrefixSum2D(const vector<vector<int>>& a) {
        int rows = (int)a.size(), cols = rows ? (int)a[0].size() : 0;
        P.assign(rows + 1, vector<long long>(cols + 1, 0));
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                P[i + 1][j + 1] = a[i][j] + P[i][j + 1] + P[i + 1][j] - P[i][j];
    }
    // Sum of the rectangle with top-left (r1, c1) and bottom-right (r2, c2), both inclusive.
    long long sum(int r1, int c1, int r2, int c2) const {
        return P[r2 + 1][c2 + 1] - P[r1][c2 + 1] - P[r2 + 1][c1] + P[r1][c1];
    }
};
```
<!-- /snippet -->

O(n) and O(R·C) to build, O(1) per query; O(n) and O(R·C) space.
</details>

Running sum + total, no array (note the `0LL`):

<!-- snippet: modules/06-two-pointers-window-prefix/examples/prefix_suffix.cpp#split_points -->
```cpp
// How many split points i (left = a[0..i], right = a[i+1..n-1], both non-empty) have
// left sum >= right sum? right = total - left, so a running sum and the total are enough: O(1) space.
int count_heavy_left_splits(const vector<int>& a) {
    long long total = accumulate(a.begin(), a.end(), 0LL);   // 0LL, not 0: the sum is done in the init's type
    long long left = 0;
    int count = 0;
    for (int i = 0; i + 1 < (int)a.size(); i++) {
        left += a[i];
        if (left >= total - left) count++;
    }
    return count;
}
```
<!-- /snippet -->

O(n) time, O(1) space. On [10, 4, −8, 7] (total 13) the left sums at the three split points are 10, 14 and 6, against right sums 3, −1 and 7, so two splits qualify.

### Pitfalls

- **Off by one:** pre has n + 1 entries, and the range [l, r] uses pre[r+1] − pre[l].
- **Overflow:** a prefix array of `int` over 10⁵ values of 10⁹ overflows long before the end. Store `long long`.
- **`accumulate(v.begin(), v.end(), 0)`** adds in the type of its initial value, `int`. Write `0LL`.
- **2D:** mixing up rows and columns, or forgetting the `+ P[r1][c1]` term.
- **Rebuilding the prefix array for each query** throws away the whole point.
- **Updates:** one change to a[i] invalidates O(n) prefix entries. Many updates → Fenwick tree (module 17).
- **Products:** zeros break division; large products need `long long` or a modulus.

### Recognize it when…

- Many "sum / count over [l, r]" queries on data that doesn't change → prefix sums.
- "sum of a rectangle / submatrix", many queries → 2D prefix sums.
- "left part versus right part", "balance point", "pivot index" → running sum + total.
- "product (or sum, or XOR) of everything except i" → prefix ⊕ suffix.
- "subarray sum equals k", "sum divisible by k" → prefix sums + hash map (07 section 4).

### Worked example: 238. Product of Array Except Self
[LeetCode 238](https://leetcode.com/problems/product-of-array-except-self/) · Medium

**Problem (paraphrased):** Return an array whose i-th entry is the product of every element except nums[i]. Don't use division; run in O(n). Every prefix and suffix product fits in 32 bits.

**Signals:** "except self", "without division", O(n); the follow-up asks for O(1) extra space besides the output.

**Brute force, and why it fails:** Multiplying the other n − 1 elements for each i is O(n²) = 10¹⁰ at n = 10⁵. Dividing the total product by nums[i] is banned, and would fail on zeros anyway.

**Key insight:** answer[i] = (product of nums[0..i−1]) × (product of nums[i+1..n−1]). Fill the output with prefix products left to right, then sweep right to left multiplying in a single running suffix product. The output array doubles as the prefix array, so the extra space is O(1).

**Dry run:** nums = [1, 2, 3, 4]:

| i | answer after pass 1 (prefix) | suffix when pass 2 reaches i | final answer[i] |
|---|---|---|---|
| 0 | 1 | 2 · 3 · 4 = 24 | 24 |
| 1 | 1 | 3 · 4 = 12 | 12 |
| 2 | 1 · 2 = 2 | 4 | 8 |
| 3 | 1 · 2 · 3 = 6 | 1 | 6 |

<!-- snippet: modules/06-two-pointers-window-prefix/examples/0238-product-of-array-except-self.cpp#solution -->
```cpp
class Solution {
public:
    vector<int> productExceptSelf(vector<int>& nums) {
        int n = (int)nums.size();
        vector<int> answer(n, 1);
        for (int i = 1; i < n; i++)                 // pass 1: answer[i] = nums[0] * ... * nums[i-1]
            answer[i] = answer[i - 1] * nums[i - 1];
        int suffix = 1;                             // nums[i+1] * ... * nums[n-1], built right to left
        for (int i = n - 1; i >= 0; i--) {          // pass 2: multiply in everything to the right
            answer[i] *= suffix;
            suffix *= nums[i];
        }
        return answer;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time (two passes), O(1) extra space; the output doesn't count, per the problem.

**Edge cases:**
- One zero: every entry is 0 except the zero's own position, which gets the product of the rest.
- Two or more zeros: every entry is 0 (tested).
- Negative values; n = 2.

**Follow-ups:**
- *Why not total / nums[i]?* It's banned, and a zero breaks it. You'd have to count zeros: none → divide; one → only that index is non-zero; two or more → all zeros.
- *What if products could overflow?* Use `long long`, or the modulus the problem specifies.
- *Explain it as prefix sums?* It's the same pattern with × instead of +: prefix and suffix "sums" under multiplication.

## 4. Difference arrays — range updates in O(1), sweep-line counting

### Concept

**The inverse of prefix sums.** Define d[i] = a[i] − a[i−1] (with a[−1] = 0); a prefix sum over d gives a back. Adding v to every element of a[l..r] changes only two differences: the step into l grows by v, and the step out of r shrinks by v. So **d[l] += v, d[r+1] −= v**, and one prefix sum at the end rebuilds every value.

```
add +5 to a[1..3] of a 6-element array (d has one spare slot for index r + 1 = 6):
d:        0   +5    0    0   -5    0   (0)
prefix:   0    5    5    5    0    0
```

Use it when **all the updates come first and the values are read once** at the end: O(1) per update, O(n) to rebuild. If reads are mixed with updates, put the difference array inside a Fenwick tree (module 17: range add, point query).

**Sweep line.** The same idea on a timeline: each interval contributes +1 at its start and −1 at its end. Sort these *events* by position and walk them in order with a running count.

- Sorting replaces array indexing, so coordinates can be huge or even non-integer.
- At equal positions, the processing order decides whether touching intervals overlap. Sorting `(position, +1 or -1)` pairs puts −1 first, which is right for **half-open** intervals [s, e): one ending at x and another starting at x don't overlap.
- For **closed** intervals [s, e] on integers, turn them into half-open ones [s, e + 1).

**Sparse coordinates.** When positions go up to 10⁹ but there are only 10⁵ events, a 10⁹-slot array is out of the question. Store the differences in a `map<int, int>` keyed by position (only positions where something changes) and walk the map in key order: that *is* the sweep. O(n log n). Coordinate compression + an ordinary difference array is the other option.

**2D difference arrays.** Adding v to a whole rectangle takes four corner updates, and a 2D prefix sum rebuilds the grid (template below). Use it when many rectangles are stamped onto a grid and then read once.

### Template

The difference array from `templates/prefix_sum.hpp` (1109 below is this with 1-indexed input):

<!-- snippet: templates/prefix_sum.hpp#diff1d -->
```cpp
// d[i] = value[i] - value[i-1] (with value[-1] = 0), so value[i] = d[0] + ... + d[i].
// Adding v to value[l..r] raises the step INTO l by v and lowers the step OUT OF r by v:
// only d[l] and d[r + 1] change. One running sum over d rebuilds every value.
struct DiffArray {
    vector<long long> d;   // n + 1 slots, so d[r + 1] exists even when r = n - 1

    explicit DiffArray(int n) : d(n + 1, 0) {}
    explicit DiffArray(const vector<int>& initial) : d(initial.size() + 1, 0) {
        for (size_t i = 0; i < initial.size(); i++) {
            long long previous = i ? initial[i - 1] : 0;
            d[i] = initial[i] - previous;
        }
    }
    void add(int l, int r, long long v) {   // value[l..r] += v, for 0 <= l <= r < n
        d[l] += v;
        d[r + 1] -= v;
    }
    vector<long long> build() const {       // O(n): value[i] is the running sum of d[0..i]
        vector<long long> value(d.size() - 1);
        long long running = 0;
        for (size_t i = 0; i < value.size(); i++) {
            running += d[i];
            value[i] = running;
        }
        return value;
    }
};
```
<!-- /snippet -->

A sweep over sorted events: the total length covered by a set of intervals.

<!-- snippet: modules/06-two-pointers-window-prefix/examples/sweep.cpp#union_length -->
```cpp
// Total length covered by at least one interval. Intervals are half-open [start, end), start < end.
// Each interval becomes two events; sort them by position and walk left to right, keeping
// `active` = how many intervals cover the stretch since the previous event.
long long union_length(const vector<pair<int, int>>& intervals) {
    vector<pair<int, int>> events;                   // (position, +1 = an interval starts, -1 = one ends)
    for (auto [start, end] : intervals) {
        events.push_back({start, +1});
        events.push_back({end, -1});
    }
    sort(events.begin(), events.end());
    long long covered = 0;
    int active = 0;
    for (size_t k = 0; k < events.size(); k++) {
        if (k > 0 && active > 0) covered += (long long)events[k].first - events[k - 1].first;
        active += events[k].second;
    }
    return covered;
}
```
<!-- /snippet -->

Trace on [1, 4), [2, 6), [8, 9): the sorted events are (1, +1), (2, +1), (4, −1), (6, −1), (8, +1), (9, −1). The active count is positive on [1, 2), [2, 4), [4, 6) and [8, 9): 1 + 2 + 2 + 1 = **6**.

The map-based sweep for huge coordinates: the length covered by at least k intervals.

<!-- snippet: modules/06-two-pointers-window-prefix/examples/sweep.cpp#covered_by_k -->
```cpp
// Total length covered by at least k intervals (k >= 1), coordinates up to 1e9.
// A difference ARRAY would need 1e9 slots; a MAP stores only the positions where coverage changes,
// and iterates them in sorted order, which is exactly the sweep. O(n log n).
long long covered_by_at_least(const vector<pair<int, int>>& intervals, int k) {
    map<int, int> delta;                             // position -> change in coverage at that position
    for (auto [start, end] : intervals) {            // half-open [start, end)
        delta[start]++;                              // operator[] creates the key at 0 first: wanted here
        delta[end]--;
    }
    long long covered = 0;
    int coverage = 0, prev = 0;
    for (auto [pos, change] : delta) {
        if (coverage >= k) covered += (long long)pos - prev;   // coverage was constant on [prev, pos)
        coverage += change;
        prev = pos;
    }
    return covered;
}
```
<!-- /snippet -->

The 2D version:

<!-- snippet: templates/prefix_sum.hpp#diff2d -->
```cpp
// Add v to a whole rectangle with four O(1) corner updates; a 2D prefix sum over d rebuilds the grid.
// +v at (r1, c1) switches v on for everything below-right of it; the -v at (r1, c2+1) and (r2+1, c1)
// switch it off right of the rectangle and below it; the +v at (r2+1, c2+1) cancels the region that
// got switched off twice.
struct DiffArray2D {
    vector<vector<long long>> d;   // (rows + 1) x (cols + 1)

    DiffArray2D(int rows, int cols) : d(rows + 1, vector<long long>(cols + 1, 0)) {}
    void add(int r1, int c1, int r2, int c2, long long v) {   // inclusive corners
        d[r1][c1] += v;
        d[r1][c2 + 1] -= v;
        d[r2 + 1][c1] -= v;
        d[r2 + 1][c2 + 1] += v;
    }
    vector<vector<long long>> build() const {                // 2D running sum: same recurrence as P
        int rows = (int)d.size() - 1, cols = (int)d[0].size() - 1;
        vector<vector<long long>> grid(rows, vector<long long>(cols, 0));
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++) {
                long long up = i ? grid[i - 1][j] : 0, left = j ? grid[i][j - 1] : 0;
                long long overlap = (i && j) ? grid[i - 1][j - 1] : 0;
                grid[i][j] = d[i][j] + up + left - overlap;
            }
        return grid;
    }
};
```
<!-- /snippet -->

Costs: range add O(1) and rebuild O(n) (O(R·C) in 2D); the sweeps are O(n log n) for the sort or the map.

### Pitfalls

- **Size n + 1:** the update touches d[r + 1], which is index n when r = n − 1.
- **1-indexed input** (flights, days, stops): convert once and consistently.
- **Inclusive versus exclusive ends:** closed [l, r] → −v at r + 1; half-open [l, r) → −v at r. In [1094](https://leetcode.com/problems/car-pooling/), decide from the statement whether a passenger still occupies a seat at the stop where they get off.
- **Tie order in sweeps:** check what should happen when one interval ends exactly where another starts.
- **Coordinates up to 10⁹:** a map or coordinate compression, never a vector of that size.
- **Reading values before the rebuild:** d holds differences, not values.
- **Running sums overflow:** use `long long` unless the bounds prove `int` is enough (1109's are).

### Recognize it when…

- "add v to every element in [l, r]", many times, then report everything → difference array.
- "bookings", "trips", "shifts", "painting segments", "how many are active at time t" → difference array over time, or a sweep.
- "the busiest moment", "the maximum number of overlapping intervals", "total covered length" → sweep over sorted events.
- Coordinates up to 10⁹ with few events → a map-based sweep or coordinate compression.
- "add v to a rectangle", many times → 2D difference array.

### Worked example: 1109. Corporate Flight Bookings
[LeetCode 1109](https://leetcode.com/problems/corporate-flight-bookings/) · Medium

**Problem (paraphrased):** Flights are numbered 1 to n. Each booking reserves a number of seats on every flight from `first` to `last`, inclusive. Return the total seats reserved on each flight.

**Signals:** every booking touches a whole range; many updates first, and all the values are read once at the end.

**Brute force, and why it fails:** Adding each booking to every flight in its range is O(bookings × n) = 4·10⁸ at the limits (2·10⁴ each): too slow.

**Key insight:** Each booking is a range add. Record +seats where the range starts and −seats just after it ends (0-indexed, that's position `last` itself, since the input is 1-indexed), then rebuild all totals with one prefix sum: O(n + bookings).

**Dry run:** bookings = [[1, 2, 10], [2, 3, 20], [2, 5, 25]], n = 5 (0-indexed flights):

| after booking | diff[0..5] |
|---|---|
| [1, 2, 10] | 10, 0, −10, 0, 0, 0 |
| [2, 3, 20] | 10, 20, −10, −20, 0, 0 |
| [2, 5, 25] | 10, 45, −10, −20, 0, −25 |

The running sum of diff[0..4] gives 10, 55, 45, 25, 25.

<!-- snippet: modules/06-two-pointers-window-prefix/examples/1109-corporate-flight-bookings.cpp#solution -->
```cpp
class Solution {
public:
    vector<int> corpFlightBookings(vector<vector<int>>& bookings, int n) {
        vector<int> diff(n + 1, 0);           // 0-indexed flights; slot n absorbs the "-seats" after flight n
        for (auto& b : bookings) {
            int first = b[0] - 1, last = b[1] - 1, seats = b[2];
            diff[first] += seats;             // seats switch on at `first`...
            diff[last + 1] -= seats;          // ...and off right after `last`
        }
        vector<int> total(n);
        int running = 0;                      // at most 2*10^4 bookings * 10^4 seats = 2*10^8: fits in int
        for (int i = 0; i < n; i++) {
            running += diff[i];
            total[i] = running;
        }
        return total;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n + bookings) time, O(n) space.

**Edge cases:**
- `last == n`: the −seats lands in the spare slot and never affects a flight (tested).
- Single-flight bookings; one booking covering every flight.
- The largest possible total is 2·10⁴ × 10⁴ = 2·10⁸, which fits in `int`.

**Follow-ups:**
- *Queries arrive between bookings ("seats on flight i right now?")?* A Fenwick tree over the difference array: O(log n) per booking and per query (module 17 section 2).
- *Flight numbers up to 10⁹, few bookings?* Keep the differences in a map (the map-based sweep above) or compress coordinates.
- *Cancel a booking?* Apply the same range add with −seats.

## Common mistakes

- Two pointers on unsorted data, or dedupe that compares with the next element instead of the previous one.
- A sliding window over data with negative numbers.
- Recording the answer outside the shrink loop in a shortest-window problem, or inside it in a longest-window problem.
- Map entries left at count 0, so `size()` lies.
- `int` for subarray counts, prefix sums or pair sums.
- `accumulate(..., 0)` instead of `0LL`.
- Prefix arrays of size n instead of n + 1; difference arrays without the spare slot.
- Mixing inclusive and exclusive ends in range updates and sweeps.

## Say it out loud

A talk track for Longest Substring Without Repeating Characters (3):

1. *Restate:* "I need the length of the longest substring with all-distinct characters. The string can be 5·10⁴ long."
2. *Brute force:* "Checking every substring with a set is O(n³); extending each start until a repeat is O(n·alphabet)."
3. *Insight:* "If a substring has no repeats, neither does any piece of it, so valid windows are closed under shrinking. I can slide: add s[r], and while s[r] is duplicated, drop s[l]. The window is then the longest valid one ending at r."
4. *Complexity:* "Each index enters and leaves once: O(n) time, O(1) space for 256 counters."
5. *Edge cases:* "Empty string, all one character, spaces and symbols, and 'abba' if I jump l directly."

Follow-ups interviewers commonly ask in this module:

- *"Why is your nested loop O(n)?"* Both pointers only move forward, n steps each in total.
- *"What if the values can be negative?"* The window's monotonicity breaks; switch to prefix sums with a hash map or a deque.
- *"Count the subarrays instead of finding the longest."* Add r − l + 1 after each shrink; for "exactly K", subtract atMost(K − 1).
- *"Prove the two-pointer move is safe."* State the invariant: every pair outside [l, r] has already been decided.
- *"Now support updates."* Prefix sums and difference arrays are static; move to a Fenwick tree (module 17).

## Self-check

1. State the converging-pointer invariant and use it to prove 167 finds the answer.
<details><summary>Answer</summary>

Every pair using an index outside [l, r] is ruled out. If a[l] + a[r] < target, a[l] fails with every partner ≤ r, so dropping it is safe; if > target, a[r] fails with every partner ≥ l. The unique answer (i*, j*) is never discarded, because the pointers can't step past it (see the proof in 167's worked example), and each step shrinks the range, so they reach it.
</details>

2. In 3Sum, why skip i when `nums[i] == nums[i-1]`, and why does no triple repeat?
<details><summary>Answer</summary>

Equal first values would produce exactly the same triples, and comparing with the *previous* element keeps the first of each run, which can still use its equal neighbours as the second or third value. Inside the scan, moving l past equal values after a match (with r adjusting by the sum) prevents repeats of the same pair.
</details>

3. What property must P have for the longest-window template? For the shortest-window template?
<details><summary>Answer</summary>

Longest: shrink-closed (every window inside a valid one is valid), so the smallest valid left end never moves left. Shortest: grow-closed (every window containing a valid one is valid), so shrinking while valid finds the shortest valid window ending at each r.
</details>

4. Why does the counting template add r − l + 1?
<details><summary>Answer</summary>

After shrinking, l is the smallest valid left end for r. Because the condition is shrink-closed, every start in [l, r] is also valid: r − l + 1 subarrays end at r.
</details>

5. Prove exactly(K) = atMost(K) − atMost(K − 1) and state when it applies.
<details><summary>Answer</summary>

The subarrays with measure ≤ K split, with no overlap, into those with measure exactly K and those with measure ≤ K − 1. It applies when the measure is integer-valued and "measure ≤ K" is shrink-closed, so that each atMost can be computed with a counting window.
</details>

6. Show that the window breaks with negative numbers. What do you use instead?
<details><summary>Answer</summary>

On [2, 2, −10, 2] with budget 3, the window drops index 0 once the sum reaches 4 and reports 3, but the whole array sums to −4 and fits: the answer is 4. Use prefix sums: with a hash map (exact sums), a monotonic deque (shortest with sum ≥ k), or binary search over running prefix maxima (longest with sum ≤ k).
</details>

7. Write the 2D query formula and explain each term.
<details><summary>Answer</summary>

sum = P[r2+1][c2+1] − P[r1][c2+1] − P[r2+1][c1] + P[r1][c1]: the block from the origin to the bottom-right corner, minus the rows above the rectangle, minus the columns to its left, plus the top-left corner block that both subtractions removed.
</details>

8. Why does `accumulate(v.begin(), v.end(), 0)` overflow on large inputs?
<details><summary>Answer</summary>

It adds in the type of the initial value: `int`. The total of 10⁵ values of 10⁵ is 10¹⁰, past `INT_MAX`. Pass `0LL` to add in `long long`.
</details>

9. Why does a difference array need n + 1 slots? What changes between closed and half-open intervals?
<details><summary>Answer</summary>

The update writes −v at index r + 1, which is n when r = n − 1. For a closed [l, r] the −v goes at r + 1; for a half-open [l, r) it goes at r.
</details>

10. When does a sweep need a `map` instead of a `vector`, and what does the tie order at equal positions decide?
<details><summary>Answer</summary>

When coordinates are large and sparse (up to 10⁹ with few events): the map stores only the positions where coverage changes and iterates them in order. The tie order decides whether an interval ending at x overlaps one starting at x; with (position, −1/+1) pairs, ends sort first, which is correct for half-open intervals.
</details>
