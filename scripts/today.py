#!/usr/bin/env python3
"""Today's slice of the plan, checked against the boxes you've ticked, plus anything overdue.

Reads data/schedule.json (the frozen plan written by scripts/schedule.py), so ticking items never
reshuffles the days.   python3 scripts/today.py [--date YYYY-MM-DD]
"""
import argparse
import datetime as dt
import json
import sys

from kit import ROOT, all_modules

ap = argparse.ArgumentParser()
ap.add_argument("--date", default=dt.date.today().isoformat())
today = dt.date.fromisoformat(ap.parse_args().date)

path = ROOT / "data" / "schedule.json"
if not path.exists():
    sys.exit("no data/schedule.json yet: run `make replan` (or `make schedule`)")
days = json.loads(path.read_text(encoding="utf-8"))["days"]
items = {i.key: i for m in all_modules() for i in m.items}

def hm(mins):
    return f"{mins // 60}h{mins % 60:02d}"

def line(e):
    it = items.get(e["key"]) if e["key"] else None
    box = "   " if not e["key"] else ("[x]" if it and it.done else "[ ]")
    mod = f"{e['module'][:2]} " if e["module"] else ""
    return f"  {box} {mod}{e['label']}  ({e['minutes']} min)" if e["minutes"] else f"  {e['label']}"

first, last = days[0]["date"], days[-1]["date"]
todays = next((d["entries"] for d in days if d["date"] == today.isoformat()), None)
if todays is None:
    print(f"{today:%a %-d %b %Y}: not in the plan (it runs {first} → {last}).")
else:
    week = (today - dt.date.fromisoformat(first)).days // 7 + 1
    print(f"Today · {today:%a %-d %b %Y} · week {week} · {hm(sum(e['minutes'] for e in todays))} planned")
    print("\n".join(line(e) for e in todays))

seen, overdue = set(), []
for d in days:
    if d["date"] >= today.isoformat():
        break
    for e in d["entries"]:
        it = items.get(e["key"]) if e["key"] else None
        if it and not it.done and e["key"] not in seen:
            seen.add(e["key"])
            overdue.append(e)
if overdue:
    mins = sum(items[e["key"]].minutes for e in overdue)
    print(f"\nOverdue: {len(overdue)} item(s), ~{hm(mins)}. Catch up, or `make replan` to re-flow the plan from today.")
    print("\n".join(line(e) for e in overdue[:12]) + ("\n  ..." if len(overdue) > 12 else ""))

redo = [i for i in items.values() if i.redo]
if redo:
    print(f"\n#redo ({len(redo)}): " + " · ".join(i.short for i in redo[:8]) + (" ..." if len(redo) > 8 else ""))
