# Project · Range-query engine over a live stream (module 17)

An array of `long long` values receives a stream of operations:

| Op | Meaning (0-based, half-open `[l, r)`) |
|---|---|
| `set i v` | a[i] = v |
| `add l r v` | a[l..r) += v |
| `assign l r v` | a[l..r) = v |
| `sum l r` | Σ a[l..r) |
| `min l r` | min a[l..r) |

You'll answer it with three engines you write (Fenwick tree, sparse table, lazy segment tree),
prove them correct against a brute-force reference with a **differential tester**, then
**benchmark** them and explain where each one wins. The templates in `templates/` (from
[module 17](../../modules/17-range-queries/NOTES.md)) are on the include path, but write the
engines yourself first; compare afterwards.

## What's provided

```
include/engine.hpp     RangeEngine interface + Op; apply(op) dispatches to the five methods
include/brute.hpp      BruteEngine: O(n) per op, the source of truth
include/workload.hpp   make_workload(n, q, allowed ops, query share, seed): random op streams
include/engines.hpp    make_engines(): register your engines here
tests/diff_test.cpp    YOUR differential tester (starts as a deliberate failure)
engines/               YOUR engines go here
bench/bench.cpp        timing harness across 4 scenarios × n = 10³…10⁶
src/stream.cpp         reads a stream on stdin and answers queries as they arrive
src/gen.cpp            writes random streams for src/stream.cpp
```

`make test` (sanitizers on) · `make bench` (-O2) · `make stream` · `make clean`

## Differential testing (milestone 1, part a)

This is the most useful debugging habit in competitive programming and interviews alike: when a
fast solution is wrong on some input you can't find by hand, let a random generator and a slow,
obviously correct solution find it for you. Write `tests/diff_test.cpp` to:

1. For each engine in `make_engines()` other than `brute`: build the list of op types it
   `supports()`.
2. Repeat ≥ 2 000 times with a fresh seed: pick a small random `n` (1–40) and `q` (1–60),
   generate a workload with `make_workload` restricted to those ops, `build` the engine and a
   `BruteEngine` from the same `init`, `apply` every op to both, and compare every query answer.
3. On the first mismatch, print the engine name, the seed, `n`, the initial array, and the op log
   up to the failing op (`to_string(op)` is provided), then stop. Small `n` and `q` keep failing
   cases short enough to debug by hand.
4. Finish with `t::summary("diff_test")`.

Prove the tester works before trusting it: add a throwaway engine with a planted bug (say,
`range_sum` ignores the last element) and check it's caught within a few iterations.

## Milestone 1 (part b): Fenwick + sparse table

- `engines/fenwick_engine.hpp` → `class FenwickEngine`: `point_set`, `range_add`, `range_sum`
  (range add + range sum needs **two** BITs; module 17 section 2). `supports()` is false for
  `RangeMin` and `RangeAssign`: say why in a comment.
- `engines/sparse_engine.hpp` → `class SparseTableEngine`: `range_min` in O(1) after an
  O(n log n) build. Decide whether it supports `point_set` by rebuilding the whole table, and
  measure what that costs in milestone 3.
- Register both in `include/engines.hpp`; `make test` must pass.

## Milestone 2: lazy segment tree

`engines/segtree_engine.hpp` → `class LazySegTreeEngine` supporting **all five** ops. The hard
part is composing the two lazy tags: an `assign` wipes out any pending `add` below it, and an
`add` on top of a pending `assign` just changes the assigned value. Both `sum` and `min` must
stay correct under both tags. Pass the tester with 10 000+ iterations, then run it once more with
`n` up to 200 to exercise deeper trees.

## Milestone 3: benchmark + write-up

`make bench`, then fill in the table and answer the questions from your numbers, not from
theory.

| Scenario | n | brute | Fenwick | sparse | segtree |
|---|---|---|---|---|---|
| static-min | 10⁵ | | — | | |
| sums, 50% updates | 10⁵ | | | — | |
| min, 1% point updates | 10⁵ | | — | | |
| everything | 10⁶ | skip | — | — | |

1. Below what `n` does brute force beat the segment tree? Why does a "worse" algorithm win there?
2. Fenwick vs segment tree on the sums scenario: how big is the constant-factor gap, and why?
3. At what update rate does rebuilding the sparse table lose to the segment tree?
4. Which engine would you ship for each scenario, and what would change your mind?

Then ask Claude for `/review projects/17-range-query-engine`.

## Stretch

- An iterative (bottom-up) segment tree for the sums scenario; compare with your recursive one.
- sqrt decomposition as a fourth engine: it's easy to write, so where does it land in the table?
- Replay a 10⁶-op stream file through `./build/stream` for each engine and `diff` the outputs.
