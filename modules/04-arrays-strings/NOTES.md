# 04 · Arrays & Strings
> The substrate every other structure is built on.

**Time:** ~9 h of Core work ([problems](problems.md)) · **Prereqs:** modules 01 (vectors, strings, the STL), 02 (the two-pointer skeletons), 03 (Big-O) · **You're done when:** you can prove the triple-reverse rotation and the Dutch-flag invariant out loud; you write spiral order with both guards on the first try and it passes a single-row and a single-column matrix; you can explain why `s = s + c` in a loop is quadratic and derive Kadane's recurrence from scratch.

## Map

| # | Subtopic | Core idea | Template | Go-to problems |
|---|---|---|---|---|
| 1 | In-place manipulation | Keep the array split into regions with a loop invariant; move elements with swaps or a write pointer | [in_place.cpp](examples/in_place.cpp) | 283, 189, 75, 31 |
| 2 | Matrix problems | (i, j) ↔ id arithmetic; four shrinking boundaries; rotation = transpose + reverse; row 0 / column 0 as flags | [matrix_basics.cpp](examples/matrix_basics.cpp) | 54, 48, 73 |
| 3 | String mechanics | `std::string` is a mutable char array: append with `+=`, count with `int cnt[26]`, compare from both ends | [string_mechanics.cpp](examples/string_mechanics.cpp) | 680, 151, 5 |
| 4 | Subarrays & substrings | n(n+1)/2 subarrays kill O(n²) at 10⁵; Kadane keeps "best ending here"; count contributions instead of enumerating | [subarrays.cpp](examples/subarrays.cpp) | 53, 152 |

## 1. In-place manipulation — rotation, reversal, partitioning, Dutch national flag

### Concept

"In place" means O(1) extra memory: you rearrange the input itself. Every in-place algorithm is a **loop invariant about regions** of the array. Once you can draw the regions, the code writes itself:

```
read / write:  [ kept, in order | junk (already read) | not read yet ]
                0             write                  read            n
Lomuto:        [   < pivot     |      >= pivot       |   unseen   | pivot ]
                lo             i                     j            hi
Dutch flag:    [ 0s | 1s | unknown | 2s ]
                     low  mid    high
```

**Read / write pointers.** `read` scans every element; `write` marks the end of the kept prefix. Keep an element by copying it to `write` and advancing `write`. Because `write ≤ read` always, a write never lands on an element that hasn't been read yet. The kept elements stay in their original order (the method is *stable*), and the whole pass is O(n). `std::remove_if` is exactly this loop; `erase(remove_if(...), end())` then trims the tail.

**Reversal.** Swap the two ends and step both inward: ⌊len/2⌋ swaps, O(1) space. It's the engine behind rotation (189 below), reversing words in place (reverse everything, then each word), and the last step of next permutation. The identity that makes rotation work: **(XY)ᴿ = Yᴿ Xᴿ**, since reversing a sequence moves the last block to the front and flips everything inside it.

**Partitioning** splits a range around a pivot value. Quicksort and quickselect are built on it (module 05). The two classic schemes:

| | Lomuto | Hoare |
|---|---|---|
| pivot | `a[hi]` | the middle value (or `a[lo]`) |
| returns | p, the pivot's final index | j, a split point; the pivot may be on either side |
| guarantee | `a[lo..p-1] < a[p] <= a[p+1..hi]` | every element of `a[lo..j]` ≤ every element of `a[j+1..hi]` |
| swaps | more | about three times fewer on average |
| all-equal input | p = lo every time: quicksort degrades to O(n²) | splits near the middle |
| easy to get right | yes | the `do … while` details matter |

**Dutch national flag** is a three-way partition in one pass: four regions (0s, 1s, unknown, 2s), and every step shrinks "unknown" by one. It also solves "group into < pivot, = pivot, > pivot", which is what makes quicksort fast on inputs with many duplicates. Worked below (75).

**Next permutation, the idea** ([31](https://leetcode.com/problems/next-permutation/) is yours to code). The permutations of a sequence, in lexicographic order, change the *rightmost* possible position first. Scan from the right for the longest non-increasing suffix: it's already the largest arrangement of its elements, so nothing inside it can grow. The element just before it (the pivot) must grow, by as little as possible, and everything after it must then become as small as possible. Working out those two moves, and what happens when there is no pivot, is the problem. Check your answers against `std::next_permutation`.

### Template

Read / write pointers on a string (the same loop works on any array; LeetCode's array versions return `write` instead of resizing):

<!-- snippet: modules/04-arrays-strings/examples/in_place.cpp#read_write -->
```cpp
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
```
<!-- /snippet -->

Reversal of a sub-range:

<!-- snippet: modules/04-arrays-strings/examples/in_place.cpp#reverse_range -->
```cpp
// Reverse a[l..r] in place: swap the two ends, step both inward. floor(length / 2) swaps.
void reverse_range(vector<int>& a, int l, int r) {
    while (l < r) swap(a[l++], a[r--]);
}
```
<!-- /snippet -->

The two partition schemes. Both are O(hi − lo) time, O(1) space; the test file checks each one's promise on random ranges and sorts with both.

<!-- snippet: modules/04-arrays-strings/examples/in_place.cpp#lomuto -->
```cpp
// Lomuto: pivot = a[hi]. Returns p with a[lo..p-1] < a[p] == pivot <= a[p+1..hi].
// Invariant during the scan: a[lo..i) < pivot, a[i..j) >= pivot, a[j..hi) not looked at yet.
int lomuto_partition(vector<int>& a, int lo, int hi) {
    int pivot = a[hi], i = lo;
    for (int j = lo; j < hi; j++)
        if (a[j] < pivot) swap(a[i++], a[j]);   // grow the "< pivot" region by one
    swap(a[i], a[hi]);                          // put the pivot between the two regions
    return i;
}
```
<!-- /snippet -->

<!-- snippet: modules/04-arrays-strings/examples/in_place.cpp#hoare -->
```cpp
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
```
<!-- /snippet -->

### Pitfalls

- **Advancing `mid` after swapping with `high`** in the Dutch flag: the value that arrives at `mid` came from the unknown region and hasn't been examined.
- **Forgetting `k %= n`** before rotating: with k > n, `nums.begin() + k` is past the end (UB).
- **Hoare with the upper middle** `lo + (hi - lo + 1) / 2` as the pivot can return j = hi, and recursing on `[lo, j]` then never shrinks: infinite recursion. Use the lower middle (as the template does) or `a[lo]`.
- **Lomuto on many equal values** puts everything on one side: quicksort becomes O(n²).
- **`vec.erase(it)` inside a loop** is O(n) per call, so O(n²) overall, and it invalidates iterators. Use read / write pointers (or erase–remove) instead.
- **Stability:** read / write keeps the kept elements' order; swap-based partitions don't.
- **Reference parameters:** LeetCode passes `vector<int>& nums`, so your in-place changes are what gets checked. Copy it (`auto a = nums;`) and your edits go nowhere.

### Recognize it when…

- "in place", "O(1) extra space", "return k such that the first k elements hold the result" → read / write pointers.
- "move all X to the end and keep the others' order" → read / write, then fill the tail.
- "rotate by k" → triple reverse (or cycles of `(i + k) % n`).
- "only values 0, 1, 2", "group into three categories", "everything < x first" → Dutch flag / partition.
- "the next larger arrangement", "lexicographically next" → the next-permutation steps.

### Worked example: 189. Rotate Array
[LeetCode 189](https://leetcode.com/problems/rotate-array/) · Medium

**Problem (paraphrased):** Shift every element of an array k places to the right, with elements falling off the end wrapping around to the front. Modify the array itself.
**Signals:** "rotate"; the follow-up asks for O(1) extra space; k can exceed n (up to 10⁵ each).
**Brute force, and why it fails:** Rotating one step at a time, k times, is O(n·k): up to 10¹⁰ moves. Copying into a second array with `out[(i + k) % n] = nums[i]` is O(n) time but O(n) extra space, which the follow-up rules out.
**Key insight:** Split the array as A B, where B is the last k elements; the answer is B A. Reversing the whole array gives (AB)ᴿ = Bᴿ Aᴿ: the blocks are in the right place, just each reversed. Reverse each block back.
**Dry run:** nums = [1, 2, 3, 4, 5, 6, 7], k = 3:

| step | array |
|---|---|
| start (A = 1 2 3 4, B = 5 6 7) | 1 2 3 4 · 5 6 7 |
| reverse all | 7 6 5 · 4 3 2 1 |
| reverse the first k = 3 | 5 6 7 · 4 3 2 1 |
| reverse the remaining n − k | 5 6 7 · 1 2 3 4 |

<!-- snippet: modules/04-arrays-strings/examples/0189-rotate-array.cpp#solution -->
```cpp
class Solution {
public:
    void rotate(vector<int>& nums, int k) {
        int n = (int)nums.size();
        k %= n;                                       // rotating by n changes nothing, and k may exceed n
        reverse(nums.begin(), nums.end());            // A B  ->  B^R A^R      (B = the last k elements)
        reverse(nums.begin(), nums.begin() + k);      // B^R  ->  B
        reverse(nums.begin() + k, nums.end());        // A^R  ->  A
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time: the three reversals do about n/2 + k/2 + (n − k)/2 = n swaps (at most n, after rounding down). O(1) extra space.
**Edge cases:**
- k = 0 or a multiple of n: `k %= n` makes it 0 and the reversals cancel out.
- k > n: reduce first (tested with n = 1, k = 100 000).
- n = 1: nothing moves.

**Follow-ups:**
- *Rotate left by k?* Reverse the first k, reverse the rest, then reverse everything; or rotate right by n − k.
- *Another O(1)-space method?* Cyclic replacements: move each element to `(i + k) % n` and follow the cycle; there are gcd(n, k) cycles.
- *Does the library do this?* `std::rotate(first, middle, last)` makes `middle` the new first element, so after `k %= n` a right rotation by k is `rotate(nums.begin(), nums.end() - k, nums.end())`.

### Worked example: 75. Sort Colors
[LeetCode 75](https://leetcode.com/problems/sort-colors/) · Medium

**Problem (paraphrased):** An array holds only 0s, 1s and 2s. Rearrange it in place so all 0s come first, then the 1s, then the 2s, without calling a sort.
**Signals:** three categories; in place; the follow-up asks for one pass with constant space.
**Brute force, and why it fails:** A comparison sort is O(n log n) and banned. Counting sort (count each value, then overwrite) is O(n) but takes two passes; the follow-up wants one.
**Key insight:** Keep four regions with three pointers: `[0, low)` holds 0s, `[low, mid)` 1s, `[mid, high]` is unknown, `(high, n)` holds 2s. Examine `nums[mid]` and send it to its region. Every step shrinks the unknown region by one, so a single pass finishes.
**Dry run:** [2, 0, 2, 1, 1, 0]:

| low | mid | high | nums[mid] | action | array after |
|---|---|---|---|---|---|
| 0 | 0 | 5 | 2 | swap with high, high-- | 0 0 2 1 1 2 |
| 0 | 0 | 4 | 0 | swap with low, low++, mid++ | 0 0 2 1 1 2 |
| 1 | 1 | 4 | 0 | swap with low, low++, mid++ | 0 0 2 1 1 2 |
| 2 | 2 | 4 | 2 | swap with high, high-- | 0 0 1 1 2 2 |
| 2 | 2 | 3 | 1 | mid++ | 0 0 1 1 2 2 |
| 2 | 3 | 3 | 1 | mid++ | 0 0 1 1 2 2 |
| 2 | 4 | 3 | — | mid > high: the unknown region is empty | done |

<!-- snippet: modules/04-arrays-strings/examples/0075-sort-colors.cpp#solution -->
```cpp
class Solution {
public:
    void sortColors(vector<int>& nums) {
        // Invariant:  [0, low) all 0  |  [low, mid) all 1  |  [mid, high] unknown  |  (high, n) all 2
        int low = 0, mid = 0, high = (int)nums.size() - 1;
        while (mid <= high) {                     // until the unknown region is empty
            if (nums[mid] == 0) {
                swap(nums[low], nums[mid]);       // nums[low] is a 1 (or low == mid): both regions shift
                low++;
                mid++;
            } else if (nums[mid] == 1) {
                mid++;
            } else {
                swap(nums[mid], nums[high]);      // the value that arrives at mid is unknown:
                high--;                           // look at it next round, so don't advance mid
            }
        }
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time, one pass (each iteration advances `mid` or retreats `high`, so at most n iterations); O(1) space.
**Edge cases:**
- All one colour; already sorted; reverse sorted (all tested).
- n = 1.
- A 0 swapped from `low` when `low == mid` is a swap with itself: harmless.

**Follow-ups:**
- *Why doesn't `mid` advance after the swap with `high`?* The incoming value came from the unknown region; it gets examined next round.
- *k colours instead of 3?* Counting sort, O(n + k). One-pass partitioning doesn't generalize cleanly.
- *Is it stable?* No. That doesn't matter for plain ints, but it does when the values are keys of records.

## 2. Matrix problems — spiral order, transpose, rotation, row-column sweeps

### Concept

**Index maths.** For `vector<vector<int>> grid`, `grid.size()` is the number of **rows** R and `grid[0].size()` the number of **columns** C; `grid[i][j]` is row i, column j, and "down" means i + 1. Each row is its own vector (like a C# jagged array `int[][]`, not `int[,]`).

- **Row-major id:** cell (i, j) ↔ `i * C + j`; back with `(id / C, id % C)`. One int per cell is handy for visited arrays, union–find parents and hash keys, and for binary-searching a sorted matrix as if it were one array.
- **Bounds:** `0 <= i && i < R && 0 <= j && j < C`, checked *before* indexing.
- **Direction arrays** list the neighbour offsets once, so a neighbour loop is four lines instead of four copies.
- **Diagonals:** on a "\" diagonal, i − j is constant; on a "/" anti-diagonal, i + j is constant. N-Queens (module 10) uses these as the keys of its "attacked" sets.
- **Rings:** a cell's spiral layer is its distance to the nearest edge, `min(i, j, R-1-i, C-1-j)`.

**Spiral order** walks the outer ring clockwise, then the next ring. Keep the unvisited block as four boundaries and shrink one after each side you walk:

```
       left       right
        ↓           ↓
 top →  1   2   3   4        the numbers are the visiting order
        12  13  14  5
        11  16  15  6
bottom→ 10  9   8   7
```

The trap is the *inner* ring when it's a single row or column: after walking the top row and the right column, the bottom row or left column may already be gone. The worked example below shows the guards.

**Rotation = transpose + reverse** (the idea for [48](https://leetcode.com/problems/rotate-image/); the code is yours). Rotating an n × n matrix 90° clockwise sends the element at (i, j) to (j, n−1−i). The transpose sends (i, j) to (j, i), and reversing each row then sends (j, i) to (j, n−1−i): the same destination, so the two simple passes equal the rotation. For counter-clockwise, transpose and then reverse each column. A non-square R × C matrix transposes into a C × R one, so only square matrices can be rotated in place.

**Row 0 and column 0 as markers** (the idea for [73](https://leetcode.com/problems/set-matrix-zeroes/)). To zero every row and column that contains a 0, you need to remember which rows and columns those are. Boolean arrays cost O(R + C); the O(1) trick stores those flags in the matrix's own first row and first column. The catch is that row 0 and column 0 are also data: work out what marking destroys, and in which order the passes must run so no flag is lost before it's read.

### Template

Ids, bounds and direction arrays, with a small example that uses all three:

<!-- snippet: modules/04-arrays-strings/examples/matrix_basics.cpp#index_math -->
```cpp
// Row-major numbering: cell (i, j) of a grid with `cols` columns gets the id i * cols + j.
// One int per cell is handy for visited arrays, DSU, hash keys, or treating a sorted matrix as
// one sorted array.
int to_id(int i, int j, int cols) { return i * cols + j; }
pair<int, int> to_cell(int id, int cols) { return {id / cols, id % cols}; }

bool in_bounds(int i, int j, int rows, int cols) { return 0 <= i && i < rows && 0 <= j && j < cols; }

// The 4 neighbours, clockwise from up: (i + DR[d], j + DC[d]) for d = 0..3.
const int DR[4] = {-1, 0, 1, 0};
const int DC[4] = {0, 1, 0, -1};

// Example: count the cells that are strictly greater than every neighbour that exists.
int count_peaks(const vector<vector<int>>& g) {
    int rows = (int)g.size(), cols = (int)g[0].size(), peaks = 0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            bool peak = true;
            for (int d = 0; d < 4; d++) {
                int ni = i + DR[d], nj = j + DC[d];
                if (in_bounds(ni, nj, rows, cols) && g[ni][nj] >= g[i][j]) peak = false;
            }
            peaks += peak;
        }
    return peaks;
}
```
<!-- /snippet -->

Diagonal keys and ring numbers:

<!-- snippet: modules/04-arrays-strings/examples/matrix_basics.cpp#diagonals -->
```cpp
// Along a "\" diagonal i - j is constant; along a "/" anti-diagonal i + j is constant.
// i + j runs 0 .. rows + cols - 2. i - j runs -(cols - 1) .. rows - 1, so shift it by cols - 1
// before using it as an index.
pair<vector<long long>, vector<long long>> diagonal_sums(const vector<vector<int>>& g) {
    int rows = (int)g.size(), cols = (int)g[0].size();
    vector<long long> anti(rows + cols - 1, 0), diag(rows + cols - 1, 0);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            anti[i + j] += g[i][j];
            diag[i - j + cols - 1] += g[i][j];
        }
    return {anti, diag};
}

// Which spiral ring (layer) a cell is on: its distance to the nearest edge.
int ring_of(int i, int j, int rows, int cols) { return min({i, j, rows - 1 - i, cols - 1 - j}); }
```
<!-- /snippet -->

Everything here is O(R·C) time; the test file checks each function against a brute force that walks the cells explicitly.

### Pitfalls

- **Rows and columns swapped** on non-square inputs: `grid.size()` is R. Test with a 2 × 3 matrix, never only with squares.
- **`matrix[0].size()` on an empty matrix** is UB; check `matrix.empty()` when the constraints allow zero rows.
- **Transposing in place by visiting every (i, j)** swaps each pair twice: nothing changes.
- **Direction arrays that don't match** (`DR` for one order, `DC` for another) give wrong neighbours with no crash. Print the four neighbours of a centre cell once.
- **4 vs 8 neighbours:** read the statement.
- **Spiral order without the guards** re-reads the last row or column (`1 2 3 2 1` on a single row). Square test cases hide this bug.
- **Copying a matrix** (`auto g = grid;`) is O(R·C): fine once, not inside a loop.

### Recognize it when…

- "spiral", "clockwise order", "layer by layer" → four boundaries, or rings.
- "rotate the matrix / image 90°" → transpose + reverse.
- "set the whole row and column to zero", O(1) extra space → row 0 / column 0 as markers.
- "diagonal", "anti-diagonal", "zigzag" → the i − j and i + j keys.
- "treat the matrix as one sorted list" → id = i·C + j.

### Worked example: 54. Spiral Matrix
[LeetCode 54](https://leetcode.com/problems/spiral-matrix/) · Medium

**Problem (paraphrased):** Return all elements of an R × C matrix in clockwise spiral order, starting from the top-left corner.
**Signals:** "spiral order"; tiny constraints (R, C ≤ 10), so the difficulty is correctness, not speed.
**Brute force, and why it fails:** Speed isn't the issue: O(R·C) is optimal, since every element is output. A "robot" simulation (walk straight, turn right when the next cell is off the grid or visited) is correct but needs an R × C visited matrix. The boundary version needs O(1) extra space, and its only hard part is the loop conditions.
**Key insight:** Keep the unvisited block as `top, bottom, left, right`. Each side you walk is one row or column of that block; after walking it, move that boundary inward. After the top row and right column, the block may be empty in one direction, so walk the bottom row only if `top <= bottom` and the left column only if `left <= right`.
**Dry run:** the 3 × 4 matrix [[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12]]:

| pass | top, bottom, left, right before | cells walked | appended |
|---|---|---|---|
| → | 0, 2, 0, 3 | row 0, columns 0 → 3 | 1 2 3 4 |
| ↓ | 1, 2, 0, 3 | column 3, rows 1 → 2 | 8 12 |
| ← | 1, 2, 0, 2 | row 2, columns 2 → 0 | 11 10 9 |
| ↑ | 1, 1, 0, 2 | column 0, rows 1 → 1 | 5 |
| → | 1, 1, 1, 2 | row 1, columns 1 → 2 | 6 7 |
| ↓ | 2, 1, 1, 2 | column 2, rows 2 → 1: none | — |
| ← | 2, 1, 1, 1 | guard fails (top > bottom): skipped | — |
| ↑ | 2, 1, 1, 1 | column 1, rows 1 → 2: none | — |

The loop ends because top > bottom. Output: 1 2 3 4 8 12 11 10 9 5 6 7.

<!-- snippet: modules/04-arrays-strings/examples/0054-spiral-matrix.cpp#solution -->
```cpp
class Solution {
public:
    vector<int> spiralOrder(vector<vector<int>>& matrix) {
        vector<int> out;
        int top = 0, bottom = (int)matrix.size() - 1;      // unvisited block: rows [top, bottom]
        int left = 0, right = (int)matrix[0].size() - 1;   //            and columns [left, right]
        while (top <= bottom && left <= right) {
            for (int j = left; j <= right; j++) out.push_back(matrix[top][j]);          // → top row
            top++;
            for (int i = top; i <= bottom; i++) out.push_back(matrix[i][right]);        // ↓ right column
            right--;
            if (top <= bottom) {                                                        // a bottom row remains
                for (int j = right; j >= left; j--) out.push_back(matrix[bottom][j]);   // ← bottom row
                bottom--;
            }
            if (left <= right) {                                                        // a left column remains
                for (int i = bottom; i >= top; i--) out.push_back(matrix[i][left]);     // ↑ left column
                left++;
            }
        }
        return out;
    }
};
```
<!-- /snippet -->

**Complexity:** O(R·C) time; O(1) extra space besides the output.
**Edge cases:**
- A single row or a single column: exactly where the guards matter. Without them, [[1, 2, 3]] comes out as 1 2 3 2 1 and [[1], [2], [3]] as 1 2 3 2 (both checked in the test file).
- 1 × 1.
- Tall or wide matrices (3 × 2 tested).

**Follow-ups:**
- *Fill an n × n matrix with 1..n² in spiral order?* Same boundaries; write instead of read.
- *Counter-clockwise?* Walk down the left column first, then along the bottom, up the right, back along the top, with the matching guards.
- *Another way?* The robot: a direction index `d` into the clockwise direction arrays, turning with `d = (d + 1) % 4` when blocked. The test file uses it as the brute force.

## 3. String mechanics — immutability, builders, character counts, palindromes

### Concept

**`std::string` is a mutable, contiguous array of `char` with a length.** Think of C#'s `string` and `StringBuilder` merged into one type. `s[i] = 'x'` changes `s` itself in O(1); in C#, every "change" to a `System.String` builds a new object.

**Value semantics.** `string t = s;` copies every character, O(n); changing `t` leaves `s` alone. Pass `const string&` to read, `string&` to modify. LeetCode signatures often take `string s` by value: a copy you may change freely.

**Building strings: `+=`, never `s = s + c`.** `s + c` constructs a brand-new string (copying all of `s`), then assigns it back, so each step costs O(current length) and building n characters costs 1 + 2 + … + n = O(n²). `s += c` (or `push_back`) appends in place, amortized O(1), because the capacity grows geometrically like a `vector`'s. At n = 10⁵ that's roughly 5·10⁹ character copies versus 10⁵ appends. C# has the same trap, which is why `StringBuilder` exists; in C++, `+=` *is* the builder.

**Counting characters.** For lowercase letters, `int cnt[26] = {}` indexed by `c - 'a'` beats `unordered_map<char, int>`: no hashing, no allocation, and two arrays compare with a 26-step loop. For arbitrary bytes use `int cnt[256]` indexed by `(unsigned char)c`. `array<int, 26>` supports `==` and `<` directly, so it can be a `map` key (an anagram signature); `unordered_map` would need a custom hash.

**Palindromes.** Compare the two ends and walk inward: O(n), O(1). To find the longest palindromic *substring*, expand outward from each of the 2n − 1 centers (a character, or the gap between two characters): worked below (5).

**The API**, with its C# counterpart (every C++ line is a `CHECK` in the tour below):

| Task | C++ | Cost | C# |
|---|---|---|---|
| length | `s.size()` | O(1) | `s.Length` |
| substring | `s.substr(pos, len)` (len, not end) | O(len) | `s.Substring(pos, len)` |
| search | `s.find(x)`, `s.rfind(x)`; `string::npos` if absent | up to O(n·m) | `IndexOf` → −1 |
| append | `s += x`, `s.push_back(c)` | amortized O(1) per char | `StringBuilder.Append` |
| insert / erase | `s.insert(pos, x)`, `s.erase(pos, count)` | O(n) | `Insert`, `Remove` (new string) |
| compare | `==`, `<` (lexicographic) | O(n) | `==`, `string.CompareOrdinal` |
| number ↔ text | `to_string(x)`, `stoi(s)`, `stoll(s)` | O(digits) | `x.ToString()`, `int.Parse` |
| reverse / sort | `reverse(s.begin(), s.end())`, `sort(...)` | O(n), O(n log n) | via `char[]` |
| split on whitespace | `istringstream in(s); while (in >> w)` | O(n) | `s.Split(...)` |
| character tests | `isalnum((unsigned char)c)`, `tolower(...)` | O(1) | `char.IsLetterOrDigit` |

### Template

The API tour. Each line states a fact and checks it, including the `getline` trap that bites stdin-based online assessments:

<!-- snippet: modules/04-arrays-strings/examples/string_mechanics.cpp#api -->
```cpp
void string_api_tour() {
    string s = "hello world";
    CHECK_EQ(s.size(), 11);
    CHECK_EQ(s.substr(6), "world");                  // from index 6 to the end
    CHECK_EQ(s.substr(0, 4), "hell");                // (start, LENGTH), like C#'s Substring
    CHECK_EQ(s.find('o'), 4);                        // first match
    CHECK_EQ(s.rfind('o'), 7);                       // last match
    CHECK(s.find("xyz") == string::npos);            // not found: npos (a huge size_t), not -1
    s[0] = 'H';                                      // strings are mutable: no new object
    s += '!';                                        // append in place, amortized O(1)
    CHECK_EQ(s, "Hello world!");
    s.insert(5, ",");                                // O(n): shifts the tail right
    CHECK_EQ(s, "Hello, world!");
    s.erase(5, 1);                                   // (start, COUNT), O(n)
    CHECK_EQ(s, "Hello world!");
    s.pop_back();
    CHECK_EQ(s.back(), 'd');

    string copy = s;                                 // a real copy (value semantics), not a reference
    copy[0] = 'J';
    CHECK_EQ(s[0], 'H');
    reverse(copy.begin(), copy.end());               // <algorithm> works on strings
    CHECK_EQ(copy, "dlrow olleJ");
    string letters = "banana";
    sort(letters.begin(), letters.end());
    CHECK_EQ(letters, "aaabnn");                     // sorted letters: an anagram signature

    CHECK_EQ(string(3, 'z'), "zzz");
    CHECK_EQ(to_string(-42), "-42");
    CHECK_EQ(stoi("123") + 1, 124);
    CHECK_EQ(stoll("9000000000"), 9'000'000'000LL);  // stoi would throw: out of int range
    CHECK_EQ('7' - '0', 7);                          // digit character -> its value
    CHECK_EQ(char('a' + 2), 'c');                    // letter arithmetic
    CHECK(string("apple") < string("banana"));       // <, ==, != compare contents lexicographically
    CHECK(isalnum((unsigned char)'x') && !isalnum((unsigned char)','));   // cast first: see Pitfalls

    istringstream in("  split   on\twhitespace ");
    vector<string> words;
    for (string w; in >> w;) words.push_back(w);     // >> skips any run of spaces, tabs, newlines
    CHECK_EQ(words, vector<string>{"split", "on", "whitespace"});

    istringstream input("3\nhello world\n");         // an OA-style input: a number, then a line
    int n;
    string line;
    input >> n;
    getline(input, line);                            // reads the rest of the FIRST line: ""
    CHECK_EQ(line, "");
    getline(input, line);
    CHECK_EQ(line, "hello world");
}
```
<!-- /snippet -->

The two builders (both are tested to produce the same string):

<!-- snippet: modules/04-arrays-strings/examples/string_mechanics.cpp#builder -->
```cpp
// Both build "abcabc...". Same output, very different cost.
string build_slow(int n) {
    string s;
    for (int i = 0; i < n; i++) s = s + char('a' + i % 3);   // s + c builds a NEW string: O(length) per step
    return s;
}

string build_fast(int n) {
    string s;
    s.reserve(n);                                           // optional: one allocation up front
    for (int i = 0; i < n; i++) s += char('a' + i % 3);      // appends in place: amortized O(1) per step
    return s;
}
```
<!-- /snippet -->

A counting array, on a toy question:

<!-- snippet: modules/04-arrays-strings/examples/string_mechanics.cpp#counts -->
```cpp
// Can the letters of s (lowercase a-z) be rearranged into a palindrome?
// A palindrome mirrors every letter, except possibly one in the middle: at most one odd count.
bool can_rearrange_to_palindrome(const string& s) {
    int cnt[26] = {};                        // = {} zero-fills; a bare `int cnt[26];` holds garbage
    for (char c : s) cnt[c - 'a']++;         // 'a' -> 0, ..., 'z' -> 25
    int odd = 0;
    for (int x : cnt) odd += x % 2;
    return odd <= 1;
}
```
<!-- /snippet -->

The two-pointer palindrome check on a range, a building block for [680](https://leetcode.com/problems/valid-palindrome-ii/) and the brute force for 5:

<!-- snippet: modules/04-arrays-strings/examples/string_mechanics.cpp#palindrome -->
```cpp
// Is s[l..r] a palindrome? Compare the two ends, step inward. O(r - l) time, O(1) space.
bool is_palindrome(const string& s, int l, int r) {
    while (l < r)
        if (s[l++] != s[r--]) return false;
    return true;
}
```
<!-- /snippet -->

### Pitfalls

- **`int cnt[26];` without `= {}`** holds garbage: local arrays aren't zero-initialized.
- **`char` may be signed.** A byte ≥ 128 becomes a negative index in `cnt[c]`, and `isalpha`, `isdigit`, `tolower` have undefined behaviour on negative values. Cast: `(unsigned char)c`.
- **`s.size() - 1` on an empty string** wraps around (unsigned) to a huge number; `for (int i = 0; i < s.size() - 1; i++)` then reads out of bounds. Cast `(int)s.size()` first.
- **`substr(pos, len)` takes a length**, not an end index. `substr(pos)` with `pos > size()` throws `out_of_range`.
- **`find` returns `string::npos`**, a huge `size_t`. Compare with `string::npos`, not with −1 stored in an `int`.
- **`stoi` throws** on text that isn't a number or doesn't fit in an `int`; use `stoll` for 64-bit values.
- **`cin >> n` then `getline`** returns the rest of the first line (usually empty). Write `getline(cin >> ws, line)` to skip that leftover newline (module 01 section 4).
- **`"abc" + 'd'`** is pointer arithmetic on a `const char*`: it compiles (with only a warning) and is undefined behaviour. Write `string("abc") + 'd'`.
- **Case and ASCII order:** 'Z' (90) sorts before 'a' (97).
- **`s = s + c` in a loop**: O(n²), a time limit waiting to happen at 10⁵.

### Recognize it when…

- "anagram", "permutation of", "same letters", "frequency" → a counting array or a sorted-letters signature.
- "palindrome" → two pointers from both ends; "longest palindromic substring" → expand around centers.
- "reverse the words", "in place" → reverse everything, then each word.
- Building the output one character at a time → `+=`, with `reserve` if you know the final size.
- Input parsing in an online assessment → `>>` skips whitespace; `getline` reads whole lines.

### Worked example: 5. Longest Palindromic Substring
[LeetCode 5](https://leetcode.com/problems/longest-palindromic-substring/) · Medium

**Problem (paraphrased):** Return the longest contiguous substring of s that reads the same forwards and backwards.
**Signals:** "palindromic substring"; length ≤ 1000, so O(n²) = 10⁶ is comfortable and O(n³) is not.
**Brute force, and why it fails:** Check every substring: O(n²) substrings × O(n) check = O(n³). On "aaaa…a" with n = 1000 that's about 8·10⁷ character comparisons (n³/12): borderline, and the version an interviewer asks you to improve.
**Key insight:** Every palindrome has a center: a character (odd length) or the gap between two characters (even length), 2n − 1 centers in all. Expanding from a center while both ends match finds the longest palindrome around that center, so trying every center finds the overall longest. Each expansion is O(n): O(n²) total and O(1) space.
**Dry run:** s = "cbbd":

| center | kind | expansion | length |
|---|---|---|---|
| 0 | odd, "c" | (0,0) match; next (−1,1) is out of range | 1 |
| 0 | even, (0,1) | c ≠ b | 0 |
| 1 | odd, "b" | (1,1) match; (0,2): c ≠ b | 1 |
| 1 | even, (1,2) | b = b; (0,3): c ≠ d | **2** → best = "bb" |
| 2 | odd, "b" | (2,2) match; (1,3): b ≠ d | 1 |
| 2 | even, (2,3) | b ≠ d | 0 |
| 3 | odd, "d" | (3,3) match; (2,4) out of range | 1 |

<!-- snippet: modules/04-arrays-strings/examples/0005-longest-palindromic-substring.cpp#solution -->
```cpp
class Solution {
public:
    string longestPalindrome(string s) {
        int n = (int)s.size(), bestStart = 0, bestLen = 0;
        // Grow outward while both ends match; return the length of the palindrome found.
        auto expand = [&](int l, int r) {
            while (l >= 0 && r < n && s[l] == s[r]) {
                l--;
                r++;
            }
            return r - l - 1;                  // the loop overshot by one on each side
        };
        for (int c = 0; c < n; c++) {
            int len = max(expand(c, c),        // odd length: centered on s[c]        "aba"
                          expand(c, c + 1));   // even length: centered between c, c+1 "abba"
            if (len > bestLen) {
                bestLen = len;
                bestStart = c - (len - 1) / 2; // works for both parities
            }
        }
        return s.substr(bestStart, bestLen);
    }
};
```
<!-- /snippet -->

**Complexity:** O(n²) time in the worst case ("aaaa…a", where every expansion runs long); O(1) extra space besides the returned substring.
**Edge cases:**
- Length 1; no palindrome longer than one character ("ac" → "a").
- The whole string (odd "racecar", even "aaaa").
- Ties: any longest answer is accepted; this code returns the leftmost.
- The tempting shortcut "longest common substring of s and reverse(s)" is wrong: for "abacdfgdcaba" the reversed string shares "abacd", which isn't a palindrome (tested).

**Follow-ups:**
- *DP instead?* `isPal[i][j] = s[i] == s[j] && isPal[i+1][j-1]`: also O(n²) time, but O(n²) space.
- *Faster than O(n²)?* Manacher's algorithm is O(n) (module 18 section 4).
- *Count all palindromic substrings?* The same 2n − 1 centers work; think about what each successful expansion step means (module 18).

## 4. Subarrays & substrings — enumerating them, and why brute force dies at scale

### Concept

**Vocabulary.** A *subarray* (or *substring*) is contiguous. A *subsequence* keeps the order but may skip elements. A *subset* ignores order. They're different problems with different techniques: windows and prefix sums for subarrays, DP or greedy for subsequences, backtracking for subsets.

**How many?** An array of n elements has n(n+1)/2 subarrays (n of length 1, n − 1 of length 2, …, 1 of length n) and 2ⁿ subsequences. The brute force for a subarray question enumerates them:

| n | subarrays ≈ n²/2 | re-sum each: ≈ n³/6 steps | running sum: ≈ n²/2 steps | one pass: n steps |
|---|---|---|---|---|
| 10 | 55 | 220 | 55 | 10 |
| 10³ | 5·10⁵ | 1.7·10⁸ (seconds) | 5·10⁵ | 10³ |
| 10⁵ | 5·10⁹ | 1.7·10¹⁴ (days) | 5·10⁹ (about a minute) | 10⁵ |

At the usual ~10⁸ simple operations per second, n = 10⁵ rules out even the O(n²) enumeration. Any subarray problem at that size needs an O(n) or O(n log n) idea: Kadane (below), windows and prefix sums (module 06), prefix sums + hash maps (07), monotonic stacks (09). The O(n²) enumeration is still worth writing: it's the brute force you stress-test those ideas against.

**Kadane's algorithm.** Let E(i) be the best sum of a subarray that *ends exactly at* i. Such a subarray is either a[i] alone or a subarray ending at i − 1 extended by a[i]; the best extension uses the best subarray ending at i − 1. So E(i) = max(a[i], E(i−1) + a[i]), and since every subarray ends somewhere, the answer is the maximum E(i). In words: a running total that has gone negative can only hurt whatever follows, so drop it and start fresh. That's DP with rolling variables (02 section 3) in its most famous form. Worked below (53).

**The contribution technique.** Instead of asking what each subarray adds up to, ask how many subarrays each element belongs to. a[i] lies in a[l..r] exactly when l ≤ i ≤ r: i + 1 choices of l times n − i choices of r. So the sum of all subarray sums is Σ a[i]·(i+1)·(n−i), in O(n) instead of O(n²). The same flip counts things like "subarrays in which a[i] is the minimum" (module 09) or per-bit contributions (module 18).

### Template

Enumerating all subarrays in O(n²) with a running aggregate:

<!-- snippet: modules/04-arrays-strings/examples/subarrays.cpp#enumerate -->
```cpp
// Visit every subarray a[l..r] with its sum in O(n^2): fix l, extend r, keep a running sum.
// (Re-adding a[l..r] from scratch for each pair would be O(n^3).)
// Example: the total of the sums of all n(n+1)/2 subarrays.
long long sum_of_subarray_sums_quadratic(const vector<int>& a) {
    long long total = 0;
    int n = (int)a.size();
    for (int l = 0; l < n; l++) {
        long long sum = 0;
        for (int r = l; r < n; r++) {
            sum += a[r];   // now sum == a[l] + ... + a[r]
            total += sum;
        }
    }
    return total;
}
```
<!-- /snippet -->

The same total by contribution, in O(n):

<!-- snippet: modules/04-arrays-strings/examples/subarrays.cpp#contribution -->
```cpp
// Contribution technique: instead of "what does each subarray add up to?", ask "how many
// subarrays contain a[i]?". a[i] is in a[l..r] exactly when l <= i <= r: (i + 1) choices for l
// times (n - i) choices for r. O(n). (Fits in long long while |a[i]| * n^3 / 6 < 9.2e18.)
long long sum_of_subarray_sums(const vector<int>& a) {
    long long total = 0, n = (long long)a.size();
    for (long long i = 0; i < n; i++) total += a[i] * (i + 1) * (n - i);
    return total;
}
```
<!-- /snippet -->

### Pitfalls

- **Initializing Kadane's `best` to 0** is wrong on all-negative arrays: the subarray can't be empty, so the answer is the largest element.
- **Overflow:** sums of 10⁵ values up to 10⁹ need `long long`; so does the count n(n+1)/2 = 5·10⁹ at n = 10⁵; so do contribution factors like (i+1)(n−i), up to 2.5·10⁹.
- **Subarray vs subsequence:** decide which one the statement means before choosing a technique.
- **Re-summing each subarray** from scratch (O(n³)) when a running sum gives O(n²).
- **Products** ([152](https://leetcode.com/problems/maximum-product-subarray/)): a negative number turns the smallest running product into the largest, so a single "best ending here" isn't enough.

### Recognize it when…

- "contiguous subarray" + "largest / smallest sum" → Kadane (or prefix sums).
- "sum over all subarrays of …", "total of every subarray's …" → contribution per element.
- "subarray" with n ≈ 10⁵ and your plan enumerates subarrays → stop and find the one-pass invariant.
- "subsequence" → not this section: DP (module 16) or greedy (module 11).

### Worked example: 53. Maximum Subarray
[LeetCode 53](https://leetcode.com/problems/maximum-subarray/) · Medium

**Problem (paraphrased):** Find the largest sum of a non-empty contiguous subarray.
**Signals:** "contiguous subarray", "largest sum"; n up to 10⁵; values can be negative.
**Brute force, and why it fails:** Every subarray, each re-summed, is O(n³):

<!-- snippet: modules/04-arrays-strings/examples/0053-maximum-subarray.cpp#cubic -->
```cpp
// O(n^3): every subarray, each re-summed from scratch.
int max_subarray_cubic(const vector<int>& a) {
    int n = (int)a.size(), best = INT_MIN;
    for (int l = 0; l < n; l++)
        for (int r = l; r < n; r++) {
            int sum = 0;
            for (int k = l; k <= r; k++) sum += a[k];
            best = max(best, sum);
        }
    return best;
}
```
<!-- /snippet -->

A running sum removes the innermost loop, O(n²), but that's still 5·10⁹ steps at n = 10⁵:

<!-- snippet: modules/04-arrays-strings/examples/0053-maximum-subarray.cpp#quadratic -->
```cpp
// O(n^2): sum(l..r) = sum(l..r-1) + a[r], so extend a running sum instead of re-adding.
int max_subarray_quadratic(const vector<int>& a) {
    int n = (int)a.size(), best = INT_MIN;
    for (int l = 0; l < n; l++) {
        int sum = 0;
        for (int r = l; r < n; r++) {
            sum += a[r];
            best = max(best, sum);
        }
    }
    return best;
}
```
<!-- /snippet -->

**Key insight:** Track `endingHere`, the best sum of a subarray ending at the current index. It either starts fresh at i or extends the best run ending at i − 1: `max(nums[i], endingHere + nums[i])`. The answer is the best `endingHere` seen.
**Dry run:** [−2, 1, −3, 4, −1, 2, 1, −5, 4]:

| i | nums[i] | endingHere = max(nums[i], previous + nums[i]) | best |
|---|---|---|---|
| 0 | −2 | −2 | −2 |
| 1 | 1 | max(1, −1) = 1 | 1 |
| 2 | −3 | max(−3, −2) = −2 | 1 |
| 3 | 4 | max(4, 2) = 4 | 4 |
| 4 | −1 | max(−1, 3) = 3 | 4 |
| 5 | 2 | max(2, 5) = 5 | 5 |
| 6 | 1 | max(1, 6) = 6 | **6** |
| 7 | −5 | max(−5, 1) = 1 | 6 |
| 8 | 4 | max(4, 5) = 5 | 6 |

<!-- snippet: modules/04-arrays-strings/examples/0053-maximum-subarray.cpp#solution -->
```cpp
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int endingHere = nums[0];   // best sum of a subarray that ends exactly at index i
        int best = nums[0];         // best sum seen anywhere so far
        for (int i = 1; i < (int)nums.size(); i++) {
            endingHere = max(nums[i], endingHere + nums[i]);   // start fresh at i, or extend
            best = max(best, endingHere);
        }
        return best;
    }
};
```
<!-- /snippet -->

**Complexity:** O(n) time, O(1) space.
**Edge cases:**
- All negative: the answer is the largest element, which is why both variables start at `nums[0]`, not 0 (tested).
- A single element.
- The largest possible total is 10⁵ × 10⁴ = 10⁹, which still fits in `int` (tested at the limit); with bigger values, use `long long`.

**Follow-ups:**
- *Return the subarray itself?* Remember where the current run started (reset it whenever you start fresh), and save (start, i) whenever `best` improves.
- *Divide and conquer (the problem's own follow-up)?* The best subarray lies in the left half, in the right half, or crosses the middle (best suffix of the left + best prefix of the right): O(n log n). Slower, but a classic discussion.
- *The array is circular?* The best subarray either doesn't wrap (plain Kadane) or wraps, in which case it's the total minus a minimum-sum subarray. Watch the case where every value is negative.

## Common mistakes

- Advancing `mid` after the Dutch-flag swap with `high`.
- Rotating without `k %= n`.
- Testing matrix code only on squares, which hides swapped rows/columns and missing spiral guards.
- Transposing in place by swapping every pair (so each pair is swapped twice).
- `int cnt[26];` left uninitialized; indexing with a signed `char`.
- Building strings with `s = s + c` inside a loop.
- `s.size() - 1` or `v.size() - 1` on an empty container.
- Starting Kadane from 0 instead of the first element.
- `int` for sums of 10⁵ values or for subarray counts.

## Say it out loud

A talk track for Maximum Subarray (53):

1. *Restate:* "I need the largest sum of a non-empty contiguous block. Values can be negative, and n can be 10⁵."
2. *Brute force:* "All n(n+1)/2 subarrays with a running sum is O(n²), 5·10⁹ steps at 10⁵. Too slow."
3. *Insight:* "Define the best sum of a subarray that ends at i. It's either a[i] alone or the best one ending at i − 1 plus a[i]. Every subarray ends somewhere, so the answer is the maximum of these."
4. *Complexity:* "One pass, two variables: O(n) time, O(1) space."
5. *Edge cases:* "All negative, so I start from a[0] rather than 0. One element. Totals up to 10⁹ fit in int here; otherwise I'd use long long."

Follow-ups interviewers commonly ask in this module:

- *"Return the indices, not just the sum."* Track where the current run started.
- *"Do it in O(1) extra space."* For rotation: triple reverse. For the Dutch flag: three pointers. For marking rows and columns: reuse row 0 and column 0.
- *"What if the matrix isn't square?"* Rotation in place needs a square; spiral order and transposition must use R and C separately.
- *"Why is your string building linear?"* `+=` appends in place with geometric capacity growth; `s = s + c` would copy the whole string each time.
- *"Prove it's correct."* State the invariant: the Dutch flag's four regions, Kadane's "best ending here", the spiral's unvisited block.

## Self-check

1. Prove that the three reversals rotate an array right by k.
<details><summary>Answer</summary>

Write the array as A B with B the last k elements; the goal is B A. Reversing everything gives (AB)ᴿ = Bᴿ Aᴿ. Reversing the first k elements turns Bᴿ into B, and reversing the rest turns Aᴿ into A: B A. Reduce k modulo n first.
</details>

2. State the Dutch-flag invariant. Why doesn't `mid` advance after swapping with `high`?
<details><summary>Answer</summary>

[0, low) are 0s, [low, mid) are 1s, [mid, high] is unknown, (high, n) are 2s. The value swapped in from `high` comes from the unknown region and hasn't been examined, so `mid` must look at it next.
</details>

3. What do Lomuto and Hoare partitioning each return, and which one degrades on an array of equal values?
<details><summary>Answer</summary>

Lomuto returns the pivot's final index p (a[lo..p−1] < pivot ≤ a[p+1..hi]); Hoare returns a split point j with a[lo..j] ≤ pivot ≤ a[j+1..hi], the pivot not necessarily at j. On all-equal input Lomuto returns p = lo every time, so quicksort goes O(n²); Hoare splits near the middle.
</details>

4. Why is `s = s + c` in a loop O(n²), while `s += c` is O(n) in total?
<details><summary>Answer</summary>

`s + c` builds a new string by copying all of `s`, which costs O(length) per step: 1 + 2 + … + n = O(n²). `+=` appends in place; the capacity grows geometrically, so reallocation copies add up to O(n) overall (amortized O(1) per append).
</details>

5. How do you rotate an n × n matrix 90° clockwise in place, and what's the classic bug?
<details><summary>Answer</summary>

Transpose, then reverse each row: (i, j) → (j, i) → (j, n−1−i), which is the rotation's mapping. The bug: transposing by swapping (i, j) with (j, i) for *every* pair swaps each pair twice; only swap when j > i.
</details>

6. Which two guards does spiral order need, and which inputs expose a missing guard?
<details><summary>Answer</summary>

Walk the bottom row only if top ≤ bottom, and the left column only if left ≤ right (both checked after the top row and right column have been walked). A single row, a single column, or any non-square matrix whose innermost ring is one row or column exposes the bug; square matrices hide it.
</details>

7. How many subarrays and subsequences does an array of n elements have? Why can't an O(n²) enumeration handle n = 10⁵?
<details><summary>Answer</summary>

n(n+1)/2 subarrays and 2ⁿ subsequences. At n = 10⁵ that's 5·10⁹ subarrays: about a minute at ~10⁸ simple operations per second, far past a typical time limit.
</details>

8. Derive Kadane's recurrence. What goes wrong if `best` starts at 0?
<details><summary>Answer</summary>

E(i), the best sum ending exactly at i, is max(a[i], E(i−1) + a[i]), because a subarray ending at i is a[i] alone or extends one ending at i − 1. The answer is max E(i). Starting `best` at 0 returns 0 for an all-negative array, but the subarray must be non-empty.
</details>

9. How many subarrays contain a[i]? Use it to get the sum of all subarray sums.
<details><summary>Answer</summary>

(i + 1)·(n − i): any l ≤ i and any r ≥ i. The sum of all subarray sums is Σ a[i]·(i+1)·(n−i), computed in O(n) with 64-bit arithmetic.
</details>

10. Why cast to `unsigned char` before indexing a count array or calling `isalpha`?
<details><summary>Answer</summary>

`char` may be signed, so a byte ≥ 128 becomes negative: a negative array index, and undefined behaviour in the `<cctype>` functions, which accept only `unsigned char` values or EOF.
</details>
