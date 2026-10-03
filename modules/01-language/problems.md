# 01 · Programming Language Concepts (C++)
> Pick one language and know it well enough that syntax never costs you a round.

Practice = the LeetCode links from [Striver's 180 – Master DSA Patterns](https://takeuforward.org/prep-hub/strivers-180-master-dsa-patterns), each placed under the syllabus subtopic it trains. "sheet:" gives the sheet's name for a problem when it differs from LeetCode's, and [video] is Striver's walkthrough. Lines marked **added** aren't on the sheet: they cover subtopics the sheet has no LeetCode problem for, or stand in for a sheet item that has no LeetCode link.

Work top to bottom. Each subtopic opens with a **Read** item for that section of [NOTES.md](NOTES.md), then its problems. 📖 = worked step by step in NOTES: read it, close it, then re-solve from a blank file. 🔒 = LeetCode Premium.

Timebox: Easy 20 min · Medium 40 · Hard 60. At the timebox, take one hint (ask Claude for a `hint`, or watch the video), finish, and add `#redo` to the end of the line. Tick `[x]` only when you solved it yourself. These items are the plan [SCHEDULE.md](../../SCHEDULE.md) is built from.

## Core

### 1. Language choice — C++, the judge environment, this kit's toolchain

- [x] **Read** · Notes section 1, then run `make test` and `make run F=practice/_template.cpp` once · 45m
- [x] [1929. Concatenation of Array](https://leetcode.com/problems/concatenation-of-array/) · Easy · `vector basics` — added · your first C++ submission: build a vector, return it by value
- [x] [1480. Running Sum of 1d Array](https://leetcode.com/problems/running-sum-of-1d-array/) · Easy · `in-place loop` — added · modify the input in place; notice LeetCode passes `vector<int>&`

### 2. Types & memory — value vs reference semantics, mutability, integer overflow

- [x] **Read** · Notes section 2 + examples/value_vs_reference.cpp + examples/overflow.cpp · 60m
- [x] [7. Reverse Integer](https://leetcode.com/problems/reverse-integer/) · Medium · `overflow guard` — added · detect 32-bit overflow before it happens, without using 64-bit
- [x] [1822. Sign of the Product of an Array](https://leetcode.com/problems/sign-of-the-product-of-an-array/) · Easy · `avoid overflow` — added · multiplying would overflow, so track only the sign

### 3. Standard library fluency — collections, sorting, iterators, string builders

- [x] **Read** · Notes section 3 + examples/stl_tour.cpp + examples/comparators.cpp · 60m
- [x] [217. Contains Duplicate](https://leetcode.com/problems/contains-duplicate/) · Easy · `unordered_set` — added · the same question three ways: set, sort + adjacent compare, and why O(n²) dies #redo
- [x] [242. Valid Anagram](https://leetcode.com/problems/valid-anagram/) · Easy · `counting array` — added · `int cnt[26]` beats a map; learn `c - 'a'`
- [x] [1636. Sort Array by Increasing Frequency](https://leetcode.com/problems/sort-array-by-increasing-frequency/) · Easy · 📖 · `custom comparator` — added · multi-key sort with a lambda that captures a frequency map
- [x] [1768. Merge Strings Alternately](https://leetcode.com/problems/merge-strings-alternately/) · Easy · `string building` — added · `std::string` is mutable, and `+=` is amortized O(1), unlike C# strings

### 4. Idioms & fast I/O — the boilerplate you can type from muscle memory

- [x] **Read** · Notes section 4 + examples/fast_io.cpp (the stdin template OAs expect) · 45m
- [ ] **Drill** · Type the LeetCode skeleton and the stdin template from memory until each takes under 2 minutes · 30m
