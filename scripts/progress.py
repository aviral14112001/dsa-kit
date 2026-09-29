#!/usr/bin/env python3
"""Progress across all modules, read straight from the [x] boxes in modules/*/problems.md.

  python3 scripts/progress.py          bars per module, items tagged #redo, what's next
  python3 scripts/progress.py --json   the same data for tools (the /today skill uses it)
"""
import json
import sys

from kit import all_modules


def bar(done, total, width=20):
    filled = round(width * done / total) if total else 0
    return "█" * filled + "░" * (width - filled)


mods = all_modules()
data = []
for m in mods:
    core, stretch = m.core(), m.stretch()
    data.append({
        "module": m.dir, "title": m.title,
        "core_done": sum(i.done for i in core), "core_total": len(core),
        "stretch_done": sum(i.done for i in stretch), "stretch_total": len(stretch),
        "hours_left": round(sum(i.minutes for i in core if not i.done) / 60, 1),
        "next": next((i.short for i in core if not i.done), None),
    })
redo = [{"module": i.module, "item": i.short, "url": i.url} for m in mods for i in m.items if i.redo]
current = next((d for d in data if d["core_done"] < d["core_total"]), None)

if "--json" in sys.argv:
    print(json.dumps({"modules": data, "redo": redo, "current": current}, ensure_ascii=False, indent=1))
    sys.exit(0)

tot_done = sum(d["core_done"] for d in data)
tot = sum(d["core_total"] for d in data)
for d in data:
    mark = "▶" if d is current else " "
    stretch = f" · stretch {d['stretch_done']}/{d['stretch_total']}" if d["stretch_total"] else ""
    print(f"{mark} {d['module'][:32]:32} {bar(d['core_done'], d['core_total'])} {d['core_done']:3}/{d['core_total']:<3} core{stretch}")
print(f"\n  overall {bar(tot_done, tot, 40)} {tot_done}/{tot} core items "
      f"({100 * tot_done // max(tot, 1)}%) · ~{sum(d['hours_left'] for d in data):.0f} h of core work left")
if current:
    print(f"\n  next up ({current['module']}): {current['next']}")
if redo:
    print(f"\n  #redo ({len(redo)}):")
    for r in redo:
        print(f"    {r['module'][:2]} {r['item']}")
