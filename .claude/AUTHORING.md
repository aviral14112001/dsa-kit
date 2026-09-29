# Authoring spec for module content

This kit teaches DSA in C++ to one learner. Everything in `modules/NN-*/NOTES.md`, `modules/NN-*/examples/` and
`templates/` follows these rules. Read the whole file before writing anything.

## The learner

- ~2 years as a full-stack developer (React + C#/.NET); B.Tech CSE. Preparing for the DSA rounds of full-stack interviews: online assessments (often stdin/stdout on HackerRank,
  HackerEarth, CodeSignal) and live 45-minute coding interviews.
- Chose **C++** for interviews. Knows C# far better, so connect C++ ideas to C# where that clarifies
  (`std::map` ≈ `SortedDictionary`, value vs reference semantics, `std::string` is mutable unlike `System.String`).
- Rusty: starting DSA from zero. ~15 hrs/week alongside a job. Explain from first principles, but densely: no filler, no
  motivational fluff, no "in this section we will".

## What exists already (don't modify)

- `modules/NN-*/problems.md`: the curriculum's problem list, generated and validated. Read yours closely: its `### N.`
  headings are the subtopics, and the Read items say "NOTES sectionN", so NOTES section numbers must match. Problems marked
  📖 are the worked examples you must write. Report (don't fix) anything you think is misplaced.
- `include/bits/stdc++.h`: shim so `#include <bits/stdc++.h>` works with Apple clang.
- `include/test.hpp`: `CHECK`, `CHECK_EQ(actual, expected)`, `CHECK_NEAR`, `t::summary("name")`, and seeded
  randomness: `t::rand_int(lo, hi)`, `t::rand_vec(n, lo, hi)`, `t::rand_string(n, 'a', 'z')`. Read it.
- `include/leetcode.hpp`: LeetCode's exact `ListNode`/`TreeNode` + `make_list`, `make_cycle_list`, `to_vector`,
  `make_tree("[3,9,20,null,null,15,7]")`, `tree_to_string`, `find_node`. Never redefine these structs.
- `templates/binary_search.hpp`: `first_true(lo, hi, ok)`, `last_true`, `first_true_real`. Modules 05 and 11 build on it.
- `Makefile`, `scripts/`: the build and checks. Don't edit them.

## Files you write

| Path | What |
|---|---|
| `modules/NN-*/NOTES.md` | The module's teaching notes (structure below) |
| `modules/NN-*/examples/<id>-<slug>.cpp` | One file per 📖 problem (LeetCode), e.g. `0003-longest-substring-without-repeating-characters.cpp` (4-digit id) |
| `modules/NN-*/examples/cses-<id>-<slug>.cpp` + `.in`/`.out` | One per 📖 CSES problem: a full stdin/stdout program + sample files |
| `modules/NN-*/examples/<topic>.cpp` | Extra runnable demos the notes reference (e.g. `value_vs_reference.cpp`) |
| `templates/<name>.hpp` + `templates/tests/<name>_test.cpp` | Only the templates assigned to you |

Other agents are writing other modules **at the same time**. Touch only your own files. Always pass a filter to the
build and snippet tools so you never process someone else's half-written files:

```bash
make test M=modules/06-              # builds + runs only module 06 (M is a substring match: "06-" alone also hits 0006-*.cpp)
make test M=dsu_test                 # your template's test
python3 scripts/snippets.py modules/06-two-pointers-window-prefix            # fill snippets in YOUR notes only
python3 scripts/snippets.py --check modules/06-two-pointers-window-prefix
```

Never run bare `make snippets`, `make test` without `M=`, or `make clean`.

## NOTES.md structure (required)

```markdown
# NN · Title                                   ← same title line as problems.md
> tagline                                      ← same tagline

**Time:** ~X h of Core work ([problems](problems.md)) · **Prereqs:** modules … · **You're done when:** 2–3 concrete, checkable criteria

## Map
| # | Subtopic | Core idea | Template | Go-to problems |      ← one row per subtopic; problems by number

## 1. <subtopic name exactly as in problems.md, without the number>
## 2. …
## 3. …
## 4. …
## Common mistakes
## Say it out loud
## Self-check
```

Inside each numbered section, in this order (skip a part only when it truly doesn't apply):

1. **Concept.** The idea, the invariant that makes it correct, and a short proof sketch where one exists (loop invariant,
   exchange argument, amortized argument). ASCII diagrams are welcome for pointers, windows, trees and grids.
2. **Template.** Tested code pulled in by a snippet marker (see below), then its time/space complexity.
3. **Pitfalls.** Especially C++ ones: overflow, signed/unsigned comparisons, iterator invalidation, `map[]` inserting,
   `priority_queue` being a max-heap, recursion depth, copying a vector by accident.
4. **Recognize it when…** Concrete cues: phrases in the statement, constraint sizes, input shapes.
5. **Worked example(s)** for the 📖 problems of that subtopic, in the format below.

`## Say it out loud`: how to narrate this module's patterns in an interview: a 4–8 line example talk track (restate,
brute force + cost, insight, complexity, edge cases), plus the follow-ups interviewers commonly ask.

`## Self-check`: 6–10 questions a learner answers without looking; each answer inside `<details><summary>Answer</summary> … </details>`.

Length: roughly 450–900 lines per module, scaled to the module's size. Dense and scannable: short paragraphs, tables,
bullets. Tone: a senior engineer tutoring a colleague, second person ("you").

## Worked example format

```markdown
### Worked example: 3. Longest Substring Without Repeating Characters
[LeetCode 3](https://leetcode.com/problems/longest-substring-without-repeating-characters/) · Medium

**Problem (paraphrased):** one or two sentences in your own words. Never paste the LeetCode statement.
**Signals:** the words and constraints that point at this pattern.
**Brute force, and why it fails:** the approach and its complexity against the constraints.
**Key insight:** the one idea that unlocks the efficient solution.
**Dry run:** a small table or trace on a tiny input.

<!-- snippet: modules/06-two-pointers-window-prefix/examples/0003-longest-substring-without-repeating-characters.cpp#solution -->
<!-- /snippet -->

**Complexity:** time and space, with a one-line justification.
**Edge cases:** bullets.
**Follow-ups:** 2–3 questions an interviewer might ask next, with short answers.
```

## Snippets: all code in the notes is compiled and tested

- In a `.cpp`/`.hpp` file, wrap the code to show in `// [snippet:name]` … `// [/snippet]` (on their own lines). A file can
  hold several regions (`brute`, `solution`, `iterative`…).
- In NOTES.md, place `<!-- snippet: <path from kit root>#<name> -->` and `<!-- /snippet -->` on consecutive lines, then
  run `python3 scripts/snippets.py modules/<your module dir>`: it writes the fenced code between them.
- **Every ` ```cpp ` fence must come from a snippet marker**; the checker rejects anything else. For short
  illustrative fragments that aren't compiled (a pitfall demo, a one-line idiom, deliberately wrong code), use a
  ` ```c++ ` fence and keep it under ~15 lines. Real code (anything a learner might copy) goes in a tested file.
- Show only non-📖 problems' *ideas*, never their full solutions: the learner has to solve those themselves. At most,
  give the pattern name and a one-line nudge.

## Example file shape (LeetCode-style)

```c++
// 3. Longest Substring Without Repeating Characters: https://leetcode.com/problems/longest-substring-without-repeating-characters/
// Pattern: variable sliding window (longest). Module 06 section 2.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:solution]
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        ...
    }
};
// [/snippet]

// Brute force for the stress test: obviously correct, too slow for real inputs.
int brute(const string& s) { ... }

int main() {
    Solution sol;
    CHECK_EQ(sol.lengthOfLongestSubstring("abcabcbb"), 3);   // the official examples, as values
    CHECK_EQ(sol.lengthOfLongestSubstring(""), 0);            // edge cases
    for (int iter = 0; iter < 300; iter++) {                  // stress test vs brute force
        string s = t::rand_string((int)t::rand_int(0, 12), 'a', 'd');
        CHECK_EQ(sol.lengthOfLongestSubstring(s), brute(s));
    }
    return t::summary("0003-longest-substring");
}
```

- Use LeetCode's exact method signature, so the snippet can be pasted into LeetCode unchanged.
- Test with the official examples (typed as values), then edge cases, then, whenever a brute force is easy to write,
  a randomized stress test of ≥ 200 cases against it. Stress testing is a skill the notes should teach, so show it.
- Keep each test program under ~1 s. The build uses `-O1 -Wall -Wextra -Wshadow -Werror` with ASan + UBSan, so code
  must be warning-clean and free of UB.
- CSES examples are full stdin/stdout programs (fast I/O, `long long` where needed) plus `<name>.in`/`<name>.out`
  sample files. You may add `<name>.2.in`/`<name>.2.out` etc. for more cases (e.g. hand-made edge cases).

## Code style

Clang gotchas under this Makefile's `-Werror`: write `std::move` (unqualified `move(x)` warns), and wrap braced
arguments containing commas in parentheses inside `CHECK`/`CHECK_EQ` (`CHECK_EQ(m[{1, 2}], 3)` splits the macro;
write `CHECK_EQ((m[{1, 2}]), 3)`).

- C++20, `#include <bits/stdc++.h>`, `using namespace std;`, 4-space indent. Whiteboard-quality code: no macro soup,
  no `#define int long long`. `using ll = long long;` is fine.
- Name things the way you'd say them out loud in an interview. Comment the invariant, not the obvious.
- Headers in `templates/`: `#pragma once`, `#include <bits/stdc++.h>`, `using namespace std;` (interview-style on
  purpose; see `templates/binary_search.hpp`), snippet regions around the parts the notes show, and a test file with
  deterministic cases + a stress test against a brute force.

## Linking problems

Refer to problems with the same label as problems.md: `[3. Longest Substring Without Repeating Characters](https://leetcode.com/problems/longest-substring-without-repeating-characters/)`,
`[CSES 1068. Weird Algorithm](https://cses.fi/problemset/task/1068)`. The Map's "Go-to problems" column lists Core
problem numbers from problems.md.

## Quality bar

- **Correct first.** Every complexity claim right, every template tested, every edge case you mention handled by the
  code you show.
- **Explain why, not just how**: invariants, proofs, and the reason the naive approach fails.
- **Recognition signals** are the most valuable thing in the notes for this learner; make them concrete.
- **No copyrighted text:** paraphrase problems; don't copy from sites or books.
- Don't claim specifics you can't verify (exact judge compiler versions, what a named company asks).

## Before you finish

1. `make test M=modules/NN-` and `make test M=<template>_test` for each template you own: all pass, zero warnings.
2. `python3 scripts/snippets.py modules/<dir>` then `python3 scripts/snippets.py --check modules/<dir>`: OK.
3. Read your NOTES.md top to bottom once, as the learner would, and fix anything unclear or wrong.
4. Report back: files created, test-program count and check count, and anything in problems.md or the shared files
   you think should change.
