#!/usr/bin/env python3
"""Validate every problems.md: LeetCode/CSES links resolve, titles + difficulty + 🔒 match the
official lists in data/, no problem appears twice across modules. Prints a per-module summary."""
import json
import re
import sys

from kit import ROOT, all_modules

lc = {p["slug"]: p for p in json.loads((ROOT / "data/leetcode-problems.json").read_text())}
cses = {p["id"]: p for p in json.loads((ROOT / "data/cses-problems.json").read_text())}

errors, seen = [], {}
rows = []
for mod in all_modules():
    errors += mod.errors
    for it in mod.items:
        where = f"modules/{mod.dir}/problems.md:{it.line_no}"
        if it.section not in ("Core", "Stretch", "Mock pool"):
            errors.append(f"{where}: item outside '## Core' / '## Stretch' / '## Mock pool'")
        if it.kind != "problem":
            continue
        if it.url in seen:
            errors.append(f"{where}: duplicate of {seen[it.url]}")
        seen.setdefault(it.url, where)
        if m := re.fullmatch(r"https://leetcode\.com/problems/([a-z0-9-]+)/", it.url):
            p = lc.get(m[1])
            if not p:
                errors.append(f"{where}: unknown LeetCode slug {m[1]}")
                continue
            if it.label != f"{p['id']}. {p['title']}":
                errors.append(f"{where}: label should be '{p['id']}. {p['title']}' (got '{it.label}')")
            if it.difficulty != p["difficulty"]:
                errors.append(f"{where}: {p['id']} is {p['difficulty']}, not {it.difficulty}")
            if p["paid"] != ("🔒" in it.flags):
                errors.append(f"{where}: {p['id']} is {'Premium: add · 🔒' if p['paid'] else 'free: remove 🔒'}")
        elif m := re.fullmatch(r"https://cses\.fi/problemset/task/(\d+)", it.url):
            p = cses.get(int(m[1]))
            if not p:
                errors.append(f"{where}: unknown CSES task {m[1]}")
            elif it.label != f"CSES {p['id']}. {p['title']}":
                errors.append(f"{where}: label should be 'CSES {p['id']}. {p['title']}' (got '{it.label}')")
        else:
            errors.append(f"{where}: only leetcode.com/problems/<slug>/ and cses.fi/problemset/task/<id> links are allowed")
    core = mod.core()
    probs = [i for i in core if i.kind == "problem"]
    e = sum(i.difficulty == "Easy" for i in probs)
    md = sum(i.difficulty == "Medium" for i in probs)
    h = sum(i.difficulty == "Hard" for i in probs)
    rows.append((mod.dir, len(probs), e, md, h, len([i for i in mod.stretch() if i.kind == "problem"]),
                 sum(i.minutes for i in core) / 60))

if "--quiet" not in sys.argv:
    print(f"{'module':34} {'core':>4} {'E/M/H':>9} {'stretch':>7} {'core hrs':>8}")
    for d, c, e, m, h, s, hrs in rows:
        print(f"{d:34} {c:4} {f'{e}/{m}/{h}':>9} {s:7} {hrs:8.1f}")
    print(f"{'total':34} {sum(r[1] for r in rows):4} {'':>9} {sum(r[5] for r in rows):7} {sum(r[6] for r in rows):8.1f}")

if errors:
    print("\n".join(errors), file=sys.stderr)
    print(f"\n{len(errors)} problem-list error(s)", file=sys.stderr)
    sys.exit(1)
print("problem lists OK")
