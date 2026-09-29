---
name: review
description: Review Aviral's DSA solution or project code the way an interviewer plus a senior C++ engineer would - compile with sanitizers, hunt edge cases, stress-test against a brute force, derive the complexity, flag C++ pitfalls, then ask a follow-up. Use for "/review", "review this", "check my solution", or a path under practice/ or projects/.
---

# Review a solution

Target: the path given, else the most recently modified `.cpp` under `practice/`. For a directory under `projects/`,
see "Projects" below.

1. **Identify the problem** from the file's header comment or name (ask if unclear). If needed, run
   `python3 scripts/lc_statement.py <id>` to get the exact statement and constraints.
2. **Build and run:** `make run F=<file>`. Report warnings and sanitizer output. A sanitizer error is always the first
   thing to fix.
3. **Correctness.** List the edge cases for this input type (the checklist in `modules/19-company-mocks/NOTES.md` section 4)
   and which ones their tests miss. Copy the file into the scratchpad (don't edit their file unless they ask), add CHECKs
   for the missing cases, and run them. If a brute force is easy, write one there and stress-test ~300 random small
   inputs with `t::rand_*`; report the smallest counterexample found.
4. **Complexity.** Derive time and space from *their* code (recursion stack included) and compare with what the
   constraints need. If it's not good enough, give a hint toward the better approach (rung 2–3 of the hint ladder in
   CLAUDE.md), not the code.
5. **C++ review:** overflow, accidental copies (`const&`), signed/unsigned, `map[]` inserting, iterator invalidation,
   recursion depth, UB, readability. Point at lines (`file:line`).
6. **Interviewer turn:** ask 1–2 follow-ups from the bank in `modules/19-company-mocks/NOTES.md` section 4, then wait for
   their answer before commenting on it.
7. **Verdict** in one line: "Ready to submit" or "Fix X first". Offer to tick the problem in problems.md and log any
   real miss in `practice/MISTAKES.md`.

## Projects

Run the project's `make test` (and `make bench` if it has one). Check their tests against the README's checklist (which
boxes are covered?), that invariant checks run after every operation, and that every operation's complexity is right
(fill the README's table together). Review memory safety and the API contract. Don't write the implementation.
