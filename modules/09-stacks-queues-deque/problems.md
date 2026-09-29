# 09 · Stacks + Queues + Deque
> Order of processing, chosen deliberately.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Stack applications — balanced parentheses, expression evaluation, undo semantics

- [ ] **Read** · Notes section 1 · 45m
- [ ] [20. Valid Parentheses](https://leetcode.com/problems/valid-parentheses/) · Easy · 📖 · `matching stack` — added · push openers, match closers, and check the stack is empty at the end
- [ ] [150. Evaluate Reverse Polish Notation](https://leetcode.com/problems/evaluate-reverse-polish-notation/) · Medium · `operand stack` — added · evaluate postfix; the operand order matters for − and /
- [ ] [227. Basic Calculator II](https://leetcode.com/problems/basic-calculator-ii/) · Medium · `precedence without parens` — added · apply * and / right away, defer + and −
- [ ] [155. Min Stack](https://leetcode.com/problems/min-stack/) · Medium · `stack of (value, min)` — O(1) min by storing the min alongside each value · [video](https://youtu.be/NdDIaH91P0g?si=4_Jbsq5trFvfSdUY)
- [ ] [735. Asteroid Collision](https://leetcode.com/problems/asteroid-collision/) · Medium · `collision stack` — resolve against the stack top · [video](https://youtu.be/_eYGqw_VDR4?si=YyxibcHq800RqgIQ)
- [ ] [277. Find the Celebrity](https://leetcode.com/problems/find-the-celebrity/) · Medium · 🔒 · `elimination` — sheet: Celebrity Problem · one knows(a, b) call rules out one candidate: n − 1 calls leave one suspect, then verify · [video](https://youtu.be/cEadsbTeze4?si=olXYfOs7l-SEn2zl)

### 2. Monotonic stacks — next greater element, largest rectangle, stock span

- [ ] **Read** · Notes section 2 · 60m
- [ ] [739. Daily Temperatures](https://leetcode.com/problems/daily-temperatures/) · Medium · 📖 · `monotonic stack (indices)` — added · the next-greater template the sheet's 496 and 503 build on; each index is pushed and popped once, so O(n)
- [ ] [496. Next Greater Element I](https://leetcode.com/problems/next-greater-element-i/) · Easy · `next greater + map` — 739 plus a lookup table · [video](https://youtu.be/e7XQLtOQM3I?si=QdcHpTtx6gAHsext)
- [ ] [503. Next Greater Element II](https://leetcode.com/problems/next-greater-element-ii/) · Medium · `circular next greater` — sheet: Next Greater Element - 2 · loop 2n times over indices mod n · [video](https://youtu.be/7PrncD7v9YQ?si=UkBc7eVy9HGlBpeW)
- [ ] [901. Online Stock Span](https://leetcode.com/problems/online-stock-span/) · Medium · `previous greater + counts` — sheet: Stock span problem · collapse popped spans into the new entry · [video](https://youtu.be/eay-zoSRkVc?si=deNNe5i38BOAntha)
- [ ] [907. Sum of Subarray Minimums](https://leetcode.com/problems/sum-of-subarray-minimums/) · Medium · `contribution + tie-breaking` — count subarrays where arr[i] is the min; strict on one side only · [video](https://youtu.be/v0e8p9JCgRc?si=XAU7ekECgS5nboRw)
- [ ] [2104. Sum of Subarray Ranges](https://leetcode.com/problems/sum-of-subarray-ranges/) · Medium · `contribution × 2` — sum of max − sum of min · [video](https://youtu.be/gIrMptNPf5M?si=Q_GHuBvzZVs27X_U)
- [ ] [402. Remove K Digits](https://leetcode.com/problems/remove-k-digits/) · Medium · `monotonic greedy` — pop larger digits while you still have removals left · [video](https://youtu.be/jmbuRzYPGrg?si=WN387gwQ7aXWkUao)
- [ ] [84. Largest Rectangle in Histogram](https://leetcode.com/problems/largest-rectangle-in-histogram/) · Hard · 📖 · `previous / next smaller` — sheet: Largest rectangle in a histogram · each bar's widest rectangle, via two boundaries · [video](https://youtu.be/Bzat9vgD0fs?si=DiBlLejXcr6EJoyB)
- [ ] [85. Maximal Rectangle](https://leetcode.com/problems/maximal-rectangle/) · Hard · `histogram per row` — sheet: Maximum Rectangles · 84, run on each row of the matrix · [video](https://youtu.be/tOylVCugy9k)

### 3. Queues & circular buffers — BFS frontiers, rate limiting, producer-consumer

- [ ] **Read** · Notes section 3: ring buffers, rate limiters, a blocking queue with mutex + condition_variable · 45m
- [ ] [622. Design Circular Queue](https://leetcode.com/problems/design-circular-queue/) · Medium · 📖 · `ring buffer` — added · head index + count, modulo capacity
- [ ] [933. Number of Recent Calls](https://leetcode.com/problems/number-of-recent-calls/) · Easy · `sliding time window` — added · drop requests older than t − 3000: a rate limiter

### 4. Deque tricks — sliding-window maximum, palindrome checks, min-stack

- [ ] **Read** · Notes section 4 · 45m
- [ ] [239. Sliding Window Maximum](https://leetcode.com/problems/sliding-window-maximum/) · Hard · 📖 · `monotonic deque` — keep candidates decreasing; pop the front when it leaves the window · [video](https://youtu.be/NwBvene4Imo?si=eU1PY-bcQfk5wdog)
