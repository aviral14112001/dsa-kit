# DSA in C++: the 18-week plan

Every topic in the syllabus, in order, in C++: notes, tested templates, and a dated plan that fits around a full-time job
(~2 h on weekdays, ~2.5 h each weekend day). Practice problems are the 146 LeetCode links from
[Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under
the syllabus topic it trains, plus 99 LeetCode problems marked **added** for the topics the sheet doesn't reach. Start Mon
28 Sep 2026; finish ~Thu 28 Jan 2027.

## Start here (this weekend)

1. Open **`dsa/` as its own folder** in VS Code, so IntelliSense picks up `compile_flags.txt` / `.vscode/settings.json`.
2. In a terminal inside `dsa/`:
   ```bash
   make test      # compiles every worked example + template test with sanitizers; all should pass
   make today     # today's slice of the plan
   make dashboard # all modules, progress and today's plan in your browser; tick problems there
   ```
3. Skim this README, then read [module 01 section 1](modules/01-language/NOTES.md). If you want a head start, Monday's items are
   fair game today.

## How it fits together

| Piece | What it is | You use it to… |
|---|---|---|
| `modules/NN-*/NOTES.md` | Concepts, invariants, templates, worked examples (📖), pitfalls, "recognize it when…" | learn a subtopic before practising it |
| `modules/NN-*/problems.md` | That module's Striver-sheet problems (with the pattern, why it's there, and Striver's video), **added** problems for gaps, plus Read and project items | tick `[x]` when solved unaided; add `#redo` when you needed help |
| [SCHEDULE.md](SCHEDULE.md) | Day-by-day plan generated from the Core items | know what today is; `make replan` when life happens |
| `templates/` | Tested implementations: binary search, sorting, DSU, graphs, heaps, tries, Fenwick, segment trees, strings, number theory | read, then re-type from memory until it's automatic |
| `practice/` | Your solutions (`practice/06/0003-longest-substring.cpp`), `MISTAKES.md`, `companies.md` | write and run code locally with sanitizers |
| `projects/` | [File index](projects/12-file-index/README.md) (module 12) and [range-query engine](projects/17-range-query-engine/README.md) (module 17) | build real things with the structures, tests first |
| `mocks/` | Post-mortems of timed mock interviews | turn every miss into a fix |

Every code block in the notes is cut from a file that `make test` compiles and runs (with AddressSanitizer + UBSan), and every
problem link is checked against LeetCode's official list (`make check`). Subtopics the sheet has no LeetCode problem for
(prefix sums, matrices, sorting algorithms, Fenwick and segment trees, number theory…) get **added** problems instead.

## Working a problem

1. **Read and restate.** Paste the signature into a copy of [`practice/_template.cpp`](practice/_template.cpp); write the
   examples as `CHECK_EQ`s, then add edge cases (empty, one element, duplicates, extremes).
2. **Brute force + cost.** Compare it with the constraints ([module 03 section 4](modules/03-complexity/NOTES.md)): what
   complexity do they demand?
3. **Signals → pattern.** Use the tables in [module 02](modules/02-patterns/NOTES.md): "contiguous", "sorted", "all
   possible", "minimum steps", "number of ways"…
4. **Code and run:** `make run F=practice/06/0003-longest-substring.cpp`. The sanitizers catch the out-of-bounds read
   LeetCode would have forgiven, and the overflow it wouldn't have.
5. **Dry-run, then submit** on LeetCode.
6. **Timebox:** Easy 20 min · Medium 40 · Hard 60. At the limit, ask Claude for a `hint` (one rung at a time) or watch
   Striver's video linked on the line, finish, and add `#redo` to the line. `[x]` means *you* solved it.
7. **Close it out:** two comment lines at the top of your file (the pattern + the key insight). Log any real miss in
   `practice/MISTAKES.md`.

## The week

- **Mon–Fri:** ~2 h, following `make today`.
- **Saturday:** from 31 Oct, the session opens with a timed 45-minute mock (`/mock`) and its post-mortem.
- **Sunday:** the session opens with a review hour: redo `#redo` items, re-solve two 📖 examples from memory, and do a 10-minute
  pattern-classification drill.
- Rest days are built in for Diwali (8 Nov), Christmas and New Year's Day. Change them with `--rest` in `scripts/schedule.py`.

## Using Claude as a tutor

Open Claude Code in this folder (or anywhere under `~/Documents/VSCode`). [CLAUDE.md](CLAUDE.md) sets the rules: hints
before solutions, run your code rather than eyeball it, keep your progress records.

| Say | What happens |
|---|---|
| `/today` | Today's plan, anything overdue, `#redo` items due, and the one next step |
| `hint` | The next rung of the hint ladder for the problem you're on, never more than one rung |
| `/review practice/06/0003-longest-substring.cpp` | Builds it with sanitizers, hunts edge cases, stress-tests it against a brute force, derives the complexity, then asks the interviewer's follow-up |
| `/mock` · `/mock hard` · `/mock oa` | A timed interview on a problem from a finished module, presented without its title or pattern; then scores and a post-mortem |
| `teach me module 09 section 2` | An interactive walk through the notes, with questions between parts |
| `solved 424` | Ticks it in problems.md (and adds `#redo` if you needed help) |

## Modules

| # | Module | Core | Weeks | Dates |
|---|---|---|---|---|
| 01 | [Programming Language Concepts (C++)](modules/01-language/NOTES.md) · [problems](modules/01-language/problems.md) | ~7 h · 8 added | 1 | 28 Sep – 1 Oct |
| 02 | [Common DSA Patterns](modules/02-patterns/NOTES.md) · [problems](modules/02-patterns/problems.md) | ~5 h · 4 added | 1 | 1 Oct – 3 Oct |
| 03 | [Complexity Analysis & Patterns](modules/03-complexity/NOTES.md) · [problems](modules/03-complexity/problems.md) | ~5 h · 3 added | 1–2 | 4 Oct – 5 Oct |
| 04 | [Arrays & Strings](modules/04-arrays-strings/NOTES.md) · [problems](modules/04-arrays-strings/problems.md) | ~9 h · 4 sheet + 8 added | 2 | 6 Oct – 10 Oct |
| 05 | [Searching + Sorting](modules/05-searching-sorting/NOTES.md) · [problems](modules/05-searching-sorting/problems.md) | ~13 h · 9 sheet + 8 added | 2–3 | 10 Oct – 17 Oct |
| 06 | [Two Pointers + Sliding Window + Prefix Sum](modules/06-two-pointers-window-prefix/NOTES.md) · [problems](modules/06-two-pointers-window-prefix/problems.md) | ~15 h · 12 sheet + 7 added | 3–4 | 17 Oct – 24 Oct |
| 07 | [Hashing](modules/07-hashing/NOTES.md) · [problems](modules/07-hashing/problems.md) | ~9 h · 6 sheet + 5 added | 4–5 | 24 Oct – 29 Oct |
| 08 | [Linked Lists](modules/08-linked-lists/NOTES.md) · [problems](modules/08-linked-lists/problems.md) | ~11 h · 16 sheet | 5–6 | 29 Oct – 4 Nov |
| 09 | [Stacks + Queues + Deque](modules/09-stacks-queues-deque/NOTES.md) · [problems](modules/09-stacks-queues-deque/problems.md) | ~13 h · 12 sheet + 5 added | 6–7 | 5 Nov – 13 Nov |
| 10 | [Recursion + Backtracking](modules/10-recursion-backtracking/NOTES.md) · [problems](modules/10-recursion-backtracking/problems.md) | ~12 h · 10 sheet + 4 added | 7–8 | 13 Nov – 19 Nov |
| 11 | [Binary Search + Greedy + Intervals](modules/11-binary-search-greedy-intervals/NOTES.md) · [problems](modules/11-binary-search-greedy-intervals/problems.md) | ~12 h · 9 sheet + 5 added | 8–9 | 19 Nov – 26 Nov |
| 12 | [Trees + Binary Trees + BST + Project](modules/12-trees-bst/NOTES.md) · [problems](modules/12-trees-bst/problems.md) | ~23 h · 22 sheet + 4 added | 9–11 | 26 Nov – 9 Dec |
| 13 | [Heaps + Priority Queue + Tries](modules/13-heaps-tries/NOTES.md) · [problems](modules/13-heaps-tries/problems.md) | ~10 h · 5 sheet + 7 added | 11–12 | 10 Dec – 15 Dec |
| 14 | [Graphs + BFS + DFS + Topological Sort + DSU](modules/14-graphs-traversal-topo-dsu/NOTES.md) · [problems](modules/14-graphs-traversal-topo-dsu/problems.md) | ~15 h · 15 sheet + 3 added | 12–13 | 16 Dec – 23 Dec |
| 15 | [Shortest Path + MST + Advanced Graphs](modules/15-shortest-path-mst-advanced/NOTES.md) · [problems](modules/15-shortest-path-mst-advanced/problems.md) | ~10 h · 5 sheet + 5 added | 13–14 | 24 Dec – 30 Dec |
| 16 | [Dynamic Programming](modules/16-dynamic-programming/NOTES.md) · [problems](modules/16-dynamic-programming/problems.md) | ~20 h · 14 sheet + 11 added | 14–16 | 30 Dec – 12 Jan |
| 17 | [Advanced Data Structures + Range Queries + Project](modules/17-range-queries/NOTES.md) · [problems](modules/17-range-queries/problems.md) | ~10 h · 4 added | 16 | 12 Jan – 17 Jan |
| 18 | [Tries at Scale + Bit Manipulation + Number Theory + String Algorithms](modules/18-specialist-toolkit/NOTES.md) · [problems](modules/18-specialist-toolkit/problems.md) | ~12 h · 7 sheet + 8 added | 16–17 | 17 Jan – 24 Jan |
| 19 | [Company Specific Questions](modules/19-company-mocks/NOTES.md) · [problems](modules/19-company-mocks/problems.md) | ~9 h · tasks + mocks | 17–18 | 24 Jan – 28 Jan |

Hours are Core work only; weekly reviews, mocks and rest days come on top (the whole plan is ~247 h).

## Commands

| Command | Does |
|---|---|
| `make dashboard` | Browser dashboard on localhost:8765: every module and problem, progress, pace, today/overdue/#redo. Ticking there edits problems.md |
| `make today` | Today's plan with [x]/[ ] status, overdue items, `#redo` list |
| `make progress` | Progress bars per module, what's next |
| `make run F=file.cpp [IN=input.txt]` | Compile with warnings + ASan + UBSan + `-DLOCAL`, then run |
| `make test [M=modules/06-]` | Build and run every worked example and template test (or only the paths matching `M`) |
| `make replan [START=2026-10-20]` | Rewrite SCHEDULE.md from the still-unchecked items, starting today (or START) |
| `make check` | Notes in sync with tested code; every problem link, title, difficulty and 🔒 valid; no duplicates |
| `python3 scripts/lc_statement.py 1423` | Print a LeetCode statement in the terminal |

Pace options: `python3 scripts/schedule.py --start 2026-09-28 --all --weekday 90 --weekend 180` re-plans with other daily
budgets (in minutes).

## Folder map

```
dsa/
├── README.md  SCHEDULE.md  CLAUDE.md  Makefile
├── modules/NN-*/          NOTES.md · problems.md · examples/ (tested worked examples)
├── templates/             tested header-only templates + tests/
├── include/               bits/stdc++.h shim · test.hpp · leetcode.hpp
├── practice/              your solutions · _template.cpp · _cp_template.cpp · MISTAKES.md · companies.md · mocks/
├── projects/              12-file-index · 17-range-query-engine
├── mocks/                 mock post-mortems
├── scripts/               schedule, progress, checks, snippet sync, LeetCode statement fetcher
├── data/                  LeetCode + CSES problem lists (used by make check)
└── .claude/               skills (/today /review /mock) + the authoring guide for kit content
```
