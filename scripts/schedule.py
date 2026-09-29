#!/usr/bin/env python3
"""Turn the Core items of modules/*/problems.md into a dated, day-by-day plan (SCHEDULE.md).

  python3 scripts/schedule.py --start 2026-09-28 --all   the original plan (every Core item)
  python3 scripts/schedule.py --start 2026-10-20         re-plan: only unchecked items, from that date

Budget: --weekday 120 --weekend 150 (minutes per day). Fixed weekly slots: Sundays open with a 60-min
review (from week 2); Saturdays open with a 60-min timed mock from --mock-from onwards.
Days off: --rest 2026-11-08,2026-12-25 (comma-separated). Output: --out (default SCHEDULE.md).
"""
import argparse
import datetime as dt
import json
import math
import shlex
import sys

from kit import ROOT, all_modules

OVERFLOW = 15      # a task may run this far past the day's budget instead of waiting for tomorrow
SPLIT_OVER = 100   # longer tasks (project milestones) are split into parts of <= 90 minutes


def parser():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--start", required=True, help="YYYY-MM-DD")
    ap.add_argument("--all", action="store_true", help="include items already ticked (the original plan)")
    ap.add_argument("--weekday", type=int, default=120, help="minutes Mon-Fri")
    ap.add_argument("--weekend", type=int, default=150, help="minutes Sat/Sun")
    ap.add_argument("--mock-from", default="2026-10-31", help="first Saturday with a mock round")
    ap.add_argument("--rest", default="2026-11-08,2026-12-25,2027-01-01", help="days off")
    ap.add_argument("--out", default=str(ROOT / "SCHEDULE.md"))
    return ap


def plan(args):
    """Returns [(date, [(label, minutes, module_dir, item_or_None)])]."""
    queue = []
    for m in all_modules():
        for it in m.core():
            if args.all or not it.done:
                parts = math.ceil(it.minutes / 90) if it.minutes > SPLIT_OVER else 1
                for p in range(parts):
                    label = it.short + (f" ({p + 1}/{parts})" if parts > 1 else "")
                    queue.append((label, math.ceil(it.minutes / parts), m.dir, it))

    start = dt.date.fromisoformat(args.start)
    rest = {dt.date.fromisoformat(d) for d in args.rest.split(",") if d}
    mock_from = dt.date.fromisoformat(args.mock_from)
    first_sunday = start + dt.timedelta(days=6 - start.weekday())

    days, day, i = [], start, 0
    while i < len(queue):
        if day in rest:
            days.append((day, [("Rest day", 0, "", None)]))
            day += dt.timedelta(days=1)
            continue
        weekend = day.weekday() >= 5
        cap = args.weekend if weekend else args.weekday
        entries = []
        if day.weekday() == 6 and day > first_sunday:
            entries.append(("Weekly review: redo #redo items, re-solve 2 📖 from memory, 10-min pattern drill", 60, "", None))
        if day.weekday() == 5 and day >= mock_from:
            entries.append(("Timed mock round (`/mock`) + post-mortem", 60, "", None))
        used = sum(e[1] for e in entries)
        while i < len(queue) and (used == 0 or used + queue[i][1] <= cap + OVERFLOW):
            entries.append(queue[i])
            used += queue[i][1]
            i += 1
        days.append((day, entries))
        day += dt.timedelta(days=1)
    return days


def hm(mins):
    return f"{mins // 60}h{mins % 60:02d}"


def render(args, days, argv):
    by_week = {}
    start = days[0][0]
    for d, entries in days:
        by_week.setdefault((d - start).days // 7, []).append((d, entries))

    mods = {m.dir: m.title for m in all_modules()}
    total = sum(e[1] for _, es in days for e in es)
    end = days[-1][0]
    out = [
        "# Schedule",
        "",
        f"<!-- schedule-args: {shlex.join(argv)} -->",
        f"**{start:%a %-d %b %Y} → {end:%a %-d %b %Y}** · {len(by_week)} weeks · ~{total // 60} h planned · "
        f"weekdays {hm(args.weekday)}, weekends {hm(args.weekend)}",
        "",
        "Generated from the `## Core` items in `modules/*/problems.md`; ticking boxes there is what counts. "
        "Behind or ahead? Run `make replan`: it re-plans only the unchecked items, starting today. "
        "`make today` shows today's slice, and anything overdue.",
        "",
        "Every Sunday opens with a review hour, and from "
        f"{dt.date.fromisoformat(args.mock_from):%-d %b} every Saturday opens with a timed mock. Planning times: "
        "Easy 20 min · Medium 40 · Hard 60 · re-solving a 📖 worked example 15/25/35. Hit a timebox? Read a hint, "
        "finish, tag the line `#redo`.",
        "",
        "## At a glance",
        "",
        "| Week | Dates | Modules |",
        "|---|---|---|",
    ]
    for w, wdays in by_week.items():
        ms = sorted({e[2] for _, es in wdays for e in es if e[2]})
        names = ", ".join(f"{m[:2]} {mods[m].split('·', 1)[-1].strip()}" for m in ms)
        out.append(f"| {w + 1} | {wdays[0][0]:%-d %b} – {wdays[-1][0]:%-d %b} | {names} |")
    for w, wdays in by_week.items():
        out += ["", f"## Week {w + 1} · {wdays[0][0]:%-d %b} – {wdays[-1][0]:%-d %b %Y}", "",
                "| Day | Plan | Time |", "|---|---|---|"]
        for d, entries in wdays:
            cells = "<br>".join((f"`{e[2][:2]}` " if e[2] else "") + e[0].replace("|", "\\|") for e in entries)
            out.append(f"| {d:%a %-d %b} | {cells} | {hm(sum(e[1] for e in entries))} |")
    return "\n".join(out) + "\n"


def main():
    args = parser().parse_args()
    days = plan(args)
    if not days:
        print("nothing left to schedule: every Core item is ticked")
        return
    argv = ["scripts/schedule.py"] + sys.argv[1:]
    with open(args.out, "w", encoding="utf-8") as f:
        f.write(render(args, days, argv))
    # Frozen copy for `make today` and the dashboard, so ticking items never reshuffles the plan.
    frozen = {"args": argv, "days": [{"date": d.isoformat(), "entries": [
        {"label": e[0], "minutes": e[1], "module": e[2], "key": e[3].key if e[3] else None} for e in es]} for d, es in days]}
    (ROOT / "data" / "schedule.json").write_text(json.dumps(frozen, ensure_ascii=False, indent=1), encoding="utf-8")
    print(f"wrote {args.out}: {days[0][0]} → {days[-1][0]} ({len(days)} days)")


if __name__ == "__main__":
    main()
