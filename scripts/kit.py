"""Shared parsing for problems.md files (used by check_problems.py, progress.py, schedule.py).

Two item kinds, one per line:

  Problem: - [ ] [3. Longest Substring Without Repeating Characters](https://leetcode.com/problems/longest-substring-without-repeating-characters/) · Medium · `sliding window` — why it's here
           optional flags after the difficulty: " · 🔒" (LeetCode Premium), " · 📖" (worked example in NOTES)
  Task:    - [ ] **Read** · Notes section 1 In-place manipulation · 45m

Anything you append at the end of a line (#redo, a date, a note) is kept and ignored by the parsers.
Tick a box by changing [ ] to [x].
"""
import pathlib
import re
from dataclasses import dataclass, field

ROOT = pathlib.Path(__file__).resolve().parent.parent
MODULES = ROOT / "modules"

PROBLEM = re.compile(
    r"^- \[(?P<done>[ xX])\] \[(?P<label>[^\]]+)\]\((?P<url>[^)\s]+)\) · (?P<diff>Easy|Medium|Hard)"
    r"(?P<flags>(?: · (?:🔒|📖))*) · `(?P<pattern>[^`]+)` — (?P<why>.+)$"
)
TASK = re.compile(r"^- \[(?P<done>[ xX])\] \*\*(?P<kind>[A-Za-z ]+)\*\* · (?P<text>.+?) · (?P<mins>\d+)m\b(?P<rest>.*)$")

# Planning estimates for someone rebuilding from zero. A worked example (📖) is re-solved from a
# blank file after reading it, so it's quicker.
MINUTES = {"Easy": 20, "Medium": 40, "Hard": 60}
MINUTES_WORKED = {"Easy": 15, "Medium": 25, "Hard": 35}


@dataclass
class Item:
    module: str          # "06-two-pointers-window-prefix"
    section: str         # "Core" / "Stretch"
    subtopic: str        # "1. Two pointers"
    line_no: int
    raw: str
    done: bool
    kind: str            # "problem" or a task kind such as "Read", "Drill", "Project"
    label: str
    minutes: int
    url: str = ""
    difficulty: str = ""
    flags: str = ""
    pattern: str = ""
    redo: bool = False

    @property
    def key(self) -> str:
        """Stable id that survives line-number changes: module + URL (problems) or module + label (tasks)."""
        return f"{self.module}|{self.url or self.kind + ':' + self.label}"

    @property
    def short(self) -> str:
        """Compact label for schedules: 'LC 3 Longest Substring…' or 'Read: Notes section 1 …'."""
        if self.kind == "problem":
            num, _, title = self.label.partition(". ")
            prefix = "" if num.startswith("CSES") else "LC "
            title = title if len(title) <= 40 else title[:39] + "…"
            return f"{prefix}{num} {title}" + (" 📖" if "📖" in self.flags else "")
        return f"{self.kind}: {self.label}"


@dataclass
class Module:
    dir: str
    number: str
    title: str
    items: list = field(default_factory=list)
    errors: list = field(default_factory=list)

    def core(self):
        return [i for i in self.items if i.section == "Core"]

    def stretch(self):
        return [i for i in self.items if i.section == "Stretch"]


def parse_module(path: pathlib.Path) -> Module:
    text = path.read_text(encoding="utf-8").splitlines()
    mdir = path.parent.name
    title = next((l[2:].strip() for l in text if l.startswith("# ")), mdir)
    mod = Module(dir=mdir, number=mdir[:2], title=title)
    section, subtopic = "", ""
    for n, line in enumerate(text, 1):
        if line.startswith("## "):
            section, subtopic = line[3:].strip(), ""
            continue
        if line.startswith("### "):
            subtopic = line[4:].strip()
            continue
        if not line.startswith("- ["):
            continue
        if m := PROBLEM.match(line):
            flags = m["flags"]
            mins = (MINUTES_WORKED if "📖" in flags else MINUTES)[m["diff"]]
            mod.items.append(Item(mdir, section, subtopic, n, line, m["done"] != " ", "problem", m["label"], mins,
                                  url=m["url"], difficulty=m["diff"], flags=flags, pattern=m["pattern"],
                                  redo="#redo" in m["why"]))
        elif m := TASK.match(line):
            mod.items.append(Item(mdir, section, subtopic, n, line, m["done"] != " ", m["kind"], m["text"],
                                  int(m["mins"]), redo="#redo" in m["rest"]))
        else:
            mod.errors.append(f"{path.relative_to(ROOT)}:{n}: malformed item: {line[:90]}")
    return mod


def all_modules() -> list:
    return [parse_module(p) for p in sorted(MODULES.glob("*/problems.md"))]
