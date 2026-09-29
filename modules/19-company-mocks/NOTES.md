# 19 · Company Specific Questions
> Drill the actual question sets, under the actual time limit.

**Time:** ~9 h of Core work ([problems](problems.md)), plus the weekly Saturday mock that runs from week 5 · **Prereqs:** mocks
start after module 06; the company sets assume modules 01–18 · **You're done when:** (1) you've run 10+ timed mocks, each with a
written post-mortem; (2) you solve a Medium you haven't touched for two weeks, cold, in 30 minutes or less while narrating,
including the dry run; (3) every company on your target list has a one-line loop description and a list of recurring topics.

This module is a practice routine rather than new material. Modules 01–18 gave you the patterns; this one makes them hold up
under a clock, an audience and an unfamiliar problem.

## Map

| # | Subtopic | Core idea | Tool | Go-to items |
|---|---|---|---|---|
| 1 | Company-tagged sets | Recent, per-company practice beats random volume | target-list table below | Research, Company sets 1–2 |
| 2 | Timed mock rounds | A fixed 45-minute script, narrated | `/mock` | Mock A, Mock B + every Saturday |
| 3 | Post-mortems | Classify each miss, then fix its cause | [mocks/_postmortem-template.md](../../mocks/_postmortem-template.md) | Review task |
| 4 | The follow-up turn | Standard follow-ups, answered before they're asked | follow-up bank below | Follow-up drill |

## 1. Company-tagged sets — the patterns that recur at each hiring bar

### Where the sets come from

| Source | What you get | Watch out for |
|---|---|---|
| LeetCode company tags (Premium) | Problems grouped per company, with a recency filter (e.g. last 6 months) | Tags are crowd-reported; sort by recency, not all-time |
| LeetCode Discuss → Interview Experience | Round-by-round write-ups, often with the exact questions | Filter to the last ~12 months and your role/level |
| GeeksforGeeks interview experiences | Very many Indian company write-ups (OA + rounds) | Uneven quality; use it for format more than exact questions |
| Glassdoor interview reviews | Loop format, difficulty, time limits | Vague on problems; good for "how many rounds, what kind" |
| Public GitHub compilations of tagged lists | Free snapshots of company lists | Often stale; check the commit date |
| People who interviewed recently | The most accurate signal there is | Ask about format and topics, never for leaked questions |

Use these sources to learn the **format** (how many rounds, OA platform, time per problem, stdin/stdout or function) and the
**recurring topics**. Don't try to memorize exact questions: the same pattern comes back with a new story more often than the
same problem does.

### What different kinds of companies tend to test

These are broad tendencies, so verify every target with recent write-ups before trusting them:

| Kind of company | Typical DSA exposure | What else is in the loop |
|---|---|---|
| Large product companies (global tech with India offices) | 2–4 live coding rounds, mostly Medium with an occasional Hard; strong weight on communication, clean code and testing | System design (lighter at ~2 years of experience), behavioural |
| Indian product companies / startups | An online assessment (2–4 problems, 60–120 min) and then 1–2 DSA rounds | A **machine-coding / LLD round** (build a working mini-app or class design in ~90 min) is common for full-stack roles |
| Fintech / trading / quant-leaning firms | Often the hardest DSA bar; sometimes maths, probability or puzzles | Low-level performance questions at some firms |
| SaaS / enterprise product | Medium DSA, fewer rounds | Design + depth in your stack |
| Services / consulting | Aptitude + easier coding | Communication, domain |

For **full-stack roles at ~2 years of experience**, DSA is usually necessary but not enough. Expect a frontend or backend depth
round (React rendering, the JS event loop, APIs, SQL, caching), and often machine coding. This kit covers the DSA part; plan
separate prep for the rest.

### Online assessments (OAs)

Many Indian hiring funnels start with an OA on HackerRank, HackerEarth, CodeSignal, Codility or a similar platform. An OA differs
from a live round:

- **Input format.** Often stdin/stdout, not a function signature. Have the fast-I/O template from module 01 ready to type
  from memory, and know how to read "first line T, then T test cases".
- **Scoring.** Often per hidden test case (check the platform's instructions), so partial credit can be real. A correct O(n²) solution that passes 60% of the tests
  beats an unfinished O(n log n) one. Submit the brute force first, then optimize.
- **Order of attack.** Read every problem first (2–3 minutes), solve the easiest one first, and time-box each problem.
- **Silent killers.** `int` overflow (use `long long` for sums and products), a slow `endl` in loops, recursion depth on a
  10⁵-node tree, not resetting global arrays between test cases.
- **Rules.** Proctoring, allowed tabs and copy-paste policies vary; read them before you start.

### Building your target list

Pick 8–10 companies you're targeting and keep this table in
[practice/companies.md](../../practice/companies.md) (`/mock <company>` reads it):

| Company | Role | OA platform + format | Live rounds | Recurring topics (from write-ups) | Source + date |
|---|---|---|---|---|---|

Then, for the **Company set** tasks: take 4 recent tagged problems for your top targets, skipping anything this kit already
had you solve. Solve them timed, in the company's format (stdin/stdout if their OA uses it), and give each one a post-mortem
line in `practice/MISTAKES.md` if anything went wrong.

## 2. Timed mock rounds — 45 minutes, thinking out loud, dry-running your own code

### The 45-minute script

| Minutes | Phase | What you do out loud |
|---|---|---|
| 0–5 | **Clarify** | Restate the problem in your own words. Ask about input size, value ranges, duplicates, empty input, the output format, and whether the input can be modified. Confirm your understanding on the given example. |
| 5–12 | **Approach** | Say the brute force and its cost against the constraints ("n = 10⁵, so O(n²) is 10¹⁰: too slow"). Name the signals you see and the pattern they suggest. State the target complexity, then **get agreement before coding**. |
| 12–32 | **Code** | Write clean code with names that carry meaning; pull out a helper when it reads better. Narrate intent ("this window is always valid after the inner loop"), not keystrokes. |
| 32–40 | **Test** | Dry-run the example with a small variable table. Then the edge cases: empty, one element, all equal, extreme values. Fix bugs calmly and say what caused each one. |
| 40–45 | **Follow-ups** | Complexity with a justification, alternatives, and how it changes at scale (Section 4). |

If you're still in "Approach" at minute 15, move to the best version you can code correctly now, and say so: *"I'll implement
the O(n log n) version and mention how to get to O(n) afterwards."*

### What interviewers score

Interview rubrics vary by company, but they commonly come down to four dimensions:

| Dimension | Strong signal | Red flag |
|---|---|---|
| Problem solving | Reaches the optimal idea with little help; can explain why it works | Jumps into coding without a plan; can't move from brute force |
| Coding | Correct, readable, idiomatic C++ at a steady pace | Needs many attempts to get it working; hard-to-follow code |
| Verification | Tests without being prompted, finds their own bugs | "I think it works" with no trace |
| Communication | Clear narration, clarifying questions, uses hints well | Long silences; argues with hints |

A hint isn't a failure. Taking a hint well (acknowledging it, connecting it and moving on) is itself a positive signal.
Silence is worse than a wrong idea said out loud.

### When you're stuck

1. Say what you *do* know: the brute force and why it's too slow.
2. Work a tiny example by hand and look for structure.
3. Walk the module 02 triage: input shape → what's asked → what the constraints allow → what's special.
4. Ask a targeted question: "Would sorting the input be allowed?" beats "Can I get a hint?"
5. If time is running out, get a brute force working. Working code beats a perfect idea on a whiteboard.

### Dry-running your own code

Trace with a table of variables, one row per loop iteration, on the smallest input that exercises the logic. Check the loop
bounds at the first and last iteration, the empty and one-element cases, and every early return. Interviewers watch *how* you
test: tracing the code you wrote, not the code you meant to write, is what catches bugs.

### Running mocks with Claude

`/mock` turns Claude into the interviewer:
- **Problem choice:** from a module you've already finished: first anything you skipped, then your `#redo` items, then
  problems you solved longest ago.
- **Presented cold:** in Claude's own words, with no title, number or pattern, so recognizing the pattern is part of the test.
- **During the round:** a clock, clarifying questions answered without giving the approach away, your code run from
  `practice/mocks/`, then the follow-up turn.
- **Afterwards:** scores on the four dimensions and a post-mortem written into `mocks/`.
- **Variants:** `/mock hard`, `/mock oa` (two problems, 70 minutes, stdin/stdout), and `/mock <company>` (format and style
  taken from that company's row in `practice/companies.md`).

Also run a few mocks with people: a friend, a colleague, or a peer-mock platform. A human interviewer's pauses and
interruptions are part of the practice. Record yourself once and watch it back.

## 3. Post-mortems — what you missed, and which pattern would have caught it

Write one within an hour of every mock or timed set, using [mocks/_postmortem-template.md](../../mocks/_postmortem-template.md).
The point isn't the score; it's naming each miss so it doesn't happen twice.

### Classify every miss

| Type | Looks like | Fix |
|---|---|---|
| **Recognition** | Didn't see it was a sliding window / BFS / DP | Write the signal that should have triggered the pattern into `practice/MISTAKES.md`; do a 10-minute classification drill on Sunday |
| **Template gap** | Knew the pattern, fumbled the code | Re-type the template from memory 3 times; re-solve a 📖 problem that uses it |
| **Bug** | Off-by-one, overflow, null dereference, wrong reset | Add the bug's shape to your pre-submit checklist; `#redo` the problem |
| **Complexity** | Wrong Big-O, or missed that the constraints forbid your approach | Re-do module 03 section 4's table for that n |
| **Communication** | Coded in silence, skipped clarifying questions, no dry run | Next mock, narrate on purpose; say the script's phase names out loud |
| **Time** | Right idea, ran out of clock | Note where the minutes went; set phase alarms in the next mock |

### Close the loop

1. Every miss gets a line in `practice/MISTAKES.md`: date · problem · type · the signal or fix, in one sentence.
2. Re-solve the problem from a blank file **2–3 days later** (tag it `#redo`; the Sunday review picks it up).
3. Every ~4 mocks, reread the post-mortems and look for your most frequent miss type. That's your weakest skill,
   whatever the scores say.

## 4. The follow-up turn — complexity questions, edge cases, “can you do better?”

In a borderline round, the follow-up answers can tip the decision. Most follow-ups are predictable, so prepare them.

### The standard follow-ups

| Question | How to answer well |
|---|---|
| "What's the time and space complexity?" | Derive it from the code ("each index enters and leaves the deque once, so O(n)"), mention amortization, and count the recursion stack in the space. |
| "Can you do better?" | Argue from a lower bound: you must read the input (Ω(n)); comparison sorting is Ω(n log n). If you're already optimal, say so and why. If not, name what you'd trade (memory, preprocessing). |
| "What if the input doesn't fit in memory?" | Stream it in chunks; external merge sort; a hash-partition into files; one pass with O(k) state (heaps, counters). |
| "What if it's a stream?" | Keep O(k) or O(window) state: a size-k heap, a sliding window, two heaps for the median, reservoir sampling. |
| "Duplicates? Negatives? Empty input? Huge values?" | Walk your code for each; negatives break sliding windows over sums; huge values need `long long` or a modular answer. |
| "O(1) extra space?" | In-place tricks: two pointers, reversing, index-as-hash, Morris traversal, overwriting the input if allowed. |
| "Many queries instead of one?" | Precompute: prefix sums, sort + binary search, a sparse table, a Fenwick or segment tree. |
| "Make it thread-safe / concurrent?" | Locks around shared state, a producer–consumer queue (module 09), or partition the data so each thread owns a part. Full-stack interviewers like this one. |
| "How would you test this?" | The examples, edge cases, then randomized tests against a brute force (the stress test from the modules). |

### Follow-up bank by pattern

| Pattern | Likely follow-up | Short answer |
|---|---|---|
| Hash map lookups | "Input is sorted: less memory?" | Two pointers, O(1) extra space |
| Sliding window | "Numbers can be negative" | The window's monotonicity breaks → prefix sums + hash map, or a deque over prefix sums |
| Sliding window | "Exactly k instead of at most k" | atMost(k) − atMost(k − 1) |
| Binary search | "Array of unknown length" | Exponential search for a bound, then binary search |
| Binary search on answer | "Prove the predicate is monotone" | If capacity c works, any c' > c works too |
| Heap / top-k | "Data is sharded across machines" | Top-k per shard, then merge the k·shards candidates |
| Two heaps (median) | "Remove arbitrary elements" | Lazy deletion with a hash map of pending removals |
| BFS | "Edges have weights" | Dijkstra; with 0/1 weights, 0-1 BFS with a deque |
| BFS | "The graph is huge" | Bidirectional BFS; A* with a heuristic |
| DFS / recursion | "Depth up to 10⁶" | Convert to an iterative, explicit stack |
| Topological sort | "Detect the cycle and show it" | DFS colours + parent pointers to walk the cycle |
| DSU | "Support deleting edges" | Process offline in reverse (deletions become unions) |
| DP | "Reconstruct the actual answer" | Store choices (parent pointers) and walk back |
| DP | "Reduce the memory" | Rolling rows; keep only the dependencies each state needs |
| Trees | "Tree is a BST: faster?" | Use the ordering to prune (O(h) instead of O(n)) |
| Sorting | "Nearly sorted (each element ≤ k from its place)" | Size-k+1 min-heap: O(n log k) |
| Intervals | "Intervals arrive online" | An ordered map keyed by start; merge with the neighbours on insert |
| Linked list | "Do it without extra memory" | Pointer rewiring: reverse half, the fast/slow split |
| Range queries | "Updates too" | Fenwick for sums; segment tree (with lazy tags for range updates) |

### Edge-case checklists

| Input | Always check |
|---|---|
| Array | empty · one element · two elements · all equal · sorted / reverse sorted · negatives · max-size values (overflow) · duplicates |
| String | empty · one char · all the same char · case sensitivity · spaces / punctuation if allowed |
| Linked list | null head · one node · two nodes · a cycle, if possible · the head changing |
| Tree | null root · one node · a skewed chain (recursion depth) · duplicate values in a BST |
| Graph | disconnected · self-loops · parallel edges · cycles · one node · unreachable target |
| Numbers | 0 · negative · INT_MIN (negating overflows) · results past 2³¹ |
| Intervals | touching endpoints (closed or half-open?) · nested · identical · an empty list |

## Common mistakes

- **Practising only untimed.** Untimed solving builds knowledge; only timed solving builds interview performance. From week 5,
  at least one session a week runs under a clock.
- **Grinding company lists before the patterns are there.** Tagged problems are a finishing layer. Before module 12 or so,
  they mostly produce frustration.
- **Mocking without post-mortems.** A mock without a written review is mostly spent time.
- **Coding before agreement.** Five minutes coding the wrong approach costs more than two minutes of checking it with the
  interviewer.
- **Stopping at "it works".** Not dry-running, not stating the complexity, and skipping edge cases unless asked all cost
  points even when the code is correct.
- **Ignoring the rest of the loop.** For full-stack roles, machine-coding and stack-depth rounds are scored as seriously as DSA; strong DSA doesn't make up for a weak one.

## Say it out loud

A compact talk track you can adapt to any problem:

> "Let me restate it: given ___, return ___. Can the input be empty? Values up to ___? Duplicates?
> The brute force is ___, which is O(___). With n up to ___ that's about ___ operations: too slow.
> The signal here is ___, which suggests ___. The idea is ___, so each element is processed ___ times.
> That gives O(___) time and O(___) extra space. Does that sound good before I code it?
> [code] Let me trace the example: ___. Now the edge cases: empty → ___, one element → ___.
> If the input were a stream, I'd ___; if memory were tight, I'd ___."

## Self-check

1. What are the five phases of the 45-minute script, and roughly how long does each get?
<details><summary>Answer</summary>Clarify (~5), approach + agreement (~7), code (~20), test / dry-run (~8), follow-ups (~5).</details>

2. Your OA problem is worth partial credit per test case and you only have an O(n²) idea with n ≤ 10⁵. What do you do?
<details><summary>Answer</summary>Submit the O(n²) solution first: it passes the small tests. Then use the remaining time to look for the optimization, and resubmit if you find it.</details>

3. Name the six miss types in a post-mortem.
<details><summary>Answer</summary>Recognition, template gap, bug, complexity, communication, time.</details>

4. An interviewer asks "can you do better?" and you believe you're optimal. How do you answer?
<details><summary>Answer</summary>Make a lower-bound argument: e.g. every element must be read, so Ω(n); or comparison sorting is Ω(n log n). Then offer what you could trade (memory, preprocessing, assumptions about the input).</details>

5. Why does a sliding window over sums break with negative numbers, and what replaces it?
<details><summary>Answer</summary>Shrinking the window no longer reliably decreases the sum, so the "shrink while invalid" rule loses its monotonicity. Use prefix sums with a hash map (for exact targets) or a monotonic deque over prefix sums (for "at least K").</details>

6. What goes in a `practice/MISTAKES.md` line?
<details><summary>Answer</summary>Date · problem · miss type · the signal or fix, in one sentence, so the Sunday review can drill it.</details>

7. Which parts of a full-stack interview loop does this kit *not* cover?
<details><summary>Answer</summary>Machine coding / LLD, frontend and backend depth (React, JS runtime, APIs, databases), system design, and behavioural rounds.</details>
