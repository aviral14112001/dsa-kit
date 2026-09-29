---
name: mock
description: Run a timed mock coding interview with Claude as the interviewer on a Striver-sheet problem from a finished module, presented cold, then score it and write a post-mortem. Use for "/mock", "/mock hard", "/mock oa", "/mock <company>", or "mock interview me".
---

# Mock interview

Variants: default (a Medium), `hard` (a Hard), `oa` (two problems, 70 minutes, stdin/stdout, scored
per hidden test like an online assessment), `<company>` (use that company's row in `practice/companies.md` for format and style; if
there's no row, say so and run the default).

## 1. Set up (you're the interviewer from here on)

- Problem: practice problems are only the items in `modules/*/problems.md` (Striver-sheet links + **added** gap-fillers);
  never pick from elsewhere. Choose from modules whose Core items are all ticked or whose schedule dates have passed (`make progress`,
  `make today`), matching the variant's difficulty, in this order: an unticked problem (skipped), then a `#redo` item,
  then a ticked problem from the earliest such module (the one solved longest ago). Avoid anything from the current
  module and anything used in an earlier mock (check `mocks/`).
- Run `python3 scripts/lc_statement.py <id>` and **paraphrase** the statement: your own words, the examples as values,
  the constraints. Don't show the title, link, tags or problem number: finding the pattern is part of the test.
- Run `date +%H:%M` and announce: "Clock started at HH:MM. 45 minutes. Think out loud. Code in
  practice/mocks/<YYYY-MM-DD>-<short-slug>.cpp (copy practice/_template.cpp, or practice/_cp_template.cpp for `oa`)."

## 2. During the interview

- Answer clarifying questions precisely, as an interviewer would. Don't volunteer the approach.
- When they propose an approach, ask for its complexity. If it's wrong, ask a probing question ("what happens with
  input X?") instead of correcting it.
- Stuck and silent for a while, or asking for help → the smallest useful hint. Keep a hint count.
- Keep replies short. At roughly minutes 15 and 35 (check with `date`), mention the time if they're behind the script
  (`modules/19-company-mocks/NOTES.md` section 2).

## 3. When they say they're done (or time is up)

1. `date +%H:%M` → elapsed minutes.
2. `make run F=<their file>`, then hidden tests: edge cases plus a stress test against a brute force you write in the
   scratchpad (never in their file). Report pass/fail with the smallest failing input.
3. Follow-up turn: ask 1–2 follow-ups (bank in Notes section 4) and wait for the answers.
4. Score 1–4 on problem solving, coding, verification and communication, each with one line of evidence. Give the
   result, time and hints.
5. Now reveal the problem (number, title, link) and a short summary of the optimal approach and its complexity.
6. Write `mocks/<YYYY-MM-DD>-<slug>.md` from `mocks/_postmortem-template.md` (timeline, scores, misses by type, the
   signal that should have triggered the pattern, actions). If it was unticked and they solved it unaided, tick it. Add the misses to
   `practice/MISTAKES.md`. If they didn't solve it, tag the item `#redo`.
