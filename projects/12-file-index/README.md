# Project · File index with autocomplete (module 12)

You'll build an in-memory index of file paths on top of a **binary search tree you write yourself**,
augmented with subtree sizes. It answers: is this path stored, the first *k* paths starting with a
prefix (autocomplete), how many paths start with a prefix, the rank of a path, the *k*-th path, and
every path in a range. A small CLI then loads a real directory into it.

It forces every BST skill from [module 12](../../modules/12-trees-bst/NOTES.md) into one piece of
code with real inputs: insert and delete with all three cases, in-order traversal, subtree-size
augmentation for rank / k-th, and balancing. It also sets up module 13, where a trie answers the
same prefix queries a different way.

## Rules

1. **Tests first.** For each milestone, write the failing tests from its checklist, run
   `make test` and watch them fail, *then* implement. The stubs throw `not implemented`, so a
   fresh checkout starts red.
2. **The tree is the point.** No `std::set` / `std::map` inside `FileIndex` (they're fine in
   tests, as the reference to compare against).
3. **Sanitizers on.** `make test` builds with AddressSanitizer + UBSan. Free every node in the
   destructor.

## Layout

```
include/file_index.hpp     public API (tests compile against it); the private part is yours
src/file_index.cpp         your implementation, starting as throwing stubs
tests/file_index_test.cpp  your tests: one example group + TODOs
src/main.cpp               CLI (milestone 3); `load` is left for you
bench/bench.cpp            sorted-insert benchmark (milestone 2)
Makefile                   make test · make cli · make bench · make clean
```

## Milestone 1: BST core (~2 h)

`insert`, `contains`, `erase`, `size`, `all` (in-order).

Write these tests first:

- [ ] empty index: `size() == 0`, `contains` false, `erase` false, `all()` empty
- [ ] `insert` returns true, then false for the same path; `size()` counts unique paths
- [ ] after 200 random inserts, `all()` equals a sorted, de-duplicated `std::vector` of the same paths
- [ ] erase a **leaf**, a node with **one child**, a node with **two children** (the in-order
      successor case); `all()` stays sorted after each
- [ ] erase the root repeatedly until the index is empty
- [ ] erase a missing path: returns false, `size()` unchanged
- [ ] **differential test:** 10 000 random insert / erase / contains operations on short random
      paths, mirrored on a `std::set<std::string>`; every return value and the final `all()` must
      match

## Milestone 2: order statistics + autocomplete (~1.5 h)

Maintain `size` in every node through insert and erase, then add `rank`, `kth`, `complete`,
`count_prefix`, `range` and `check_invariants`.

Tests first:

- [ ] `check_invariants()` is true after every operation of the milestone-1 differential test
      (update that test to call it)
- [ ] `rank(kth(i)) == i` for every `i`; `kth(size())` is `nullopt`
- [ ] `rank` of a missing path = the number of stored paths smaller than it
- [ ] `complete`: a prefix equal to a whole stored path; a prefix with no matches; `k == 0`; `k`
      larger than the number of matches; the empty prefix (everything, in order); a prefix that is
      a prefix of the *next* key too (`"src/a"` vs `"src/ab"`)
- [ ] `count_prefix` matches a brute-force count over `all()` on random data
- [ ] `range(lo, hi)`: the half-open bounds are right; `lo >= hi` gives an empty result

<details><summary>Hint: count_prefix in O(h)</summary>

All paths starting with `p` form one contiguous block in sorted order: `[p, p')`, where `p'` is
`p` with its last character incremented (drop trailing `'\xff'` characters first; if nothing is
left, the block runs to the end). So `count_prefix(p) = rank(p') - rank(p)`.
</details>

**Then balance it.** Run `make bench`: the benchmark inserts paths that are already sorted, which
turns a plain BST into a linked list, so every operation costs O(n). Turn the tree into a
**treap** (a random priority per node, rotations on insert, rotate down on erase) or an
**AVL tree**. Keep every test green, rerun `make bench`, then try `./build/bench 200000`. Insert
time should fall from seconds to milliseconds. (A recursive insert on the degenerate tree may
even overflow the stack at 200k nodes, which is the same lesson.)

## Milestone 3: CLI + review (~45 min)

1. Implement `load <dir>` in `src/main.cpp` (the TODO there lists the `std::filesystem` calls).
2. `make cli && ./build/file-index`, then:
   ```
   > load /Users/aviralgupta/Documents/VSCode
   > count src/
   > complete portfolio/src/ 10
   > kth 1000
   ```
3. Ask Claude for `/review projects/12-file-index`: the complexity of every operation, invariant
   coverage, missing edge cases, memory safety.

Fill this in as you go:

| Operation | Plain BST (worst) | Your balanced tree | Why |
|---|---|---|---|
| insert / erase / contains | | | |
| rank / kth | | | |
| complete(prefix, k) | | | |
| count_prefix | | | |
| range(lo, hi) returning m paths | | | |

## Stretch

- `TrieIndex` with the same `complete` / `count_prefix` API (module 13), then benchmark memory
  and time against your tree.
- Save the index to a file and load it back.
- Fuzzy completion: paths within edit distance 1 of the query (a trie DFS carrying a DP row; see
  module 16).
