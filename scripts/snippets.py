#!/usr/bin/env python3
"""Keep the code shown in the notes identical to code that is compiled and tested.

In a .cpp/.hpp file, mark a region:
    // [snippet:name]
    ...code...
    // [/snippet]

In any Markdown file, reference it (the fence in between is generated):
    <!-- snippet: templates/dsu.hpp#dsu -->
    <!-- /snippet -->

  python3 scripts/snippets.py [paths]          refresh snippets in modules/ and projects/ (or only in paths)
  python3 scripts/snippets.py --check [paths]  fail if a note is stale or references a missing region, or if a
                                       ```cpp fence sits outside snippet markers (that would be untested
                                       code; short illustrative fragments use ```c++ instead)
"""
import re
import sys
import textwrap

from kit import ROOT

MARK = re.compile(
    r"(?P<open><!-- snippet: (?P<path>[^#\s]+)#(?P<name>[\w.-]+) -->\n)(?P<body>.*?)(?P<close><!-- /snippet -->)", re.S
)


def region(path: str, name: str) -> str:
    src = (ROOT / path).read_text(encoding="utf-8")
    m = re.search(r"^[ \t]*// \[snippet:" + re.escape(name) + r"\][^\n]*\n(.*?)^[ \t]*// \[/snippet\]", src, re.S | re.M)
    if not m:
        raise KeyError(f"no region '// [snippet:{name}]' in {path}")
    return textwrap.dedent(m.group(1)).rstrip("\n")


def main() -> int:
    check = "--check" in sys.argv
    unknown = [x for x in sys.argv[1:] if x.startswith("-") and x != "--check"]
    if unknown or any(not (ROOT / x).exists() for x in sys.argv[1:] if not x.startswith("-")):
        print(__doc__)
        return 2
    roots = [ROOT / a for a in sys.argv[1:] if not a.startswith("--")] or [ROOT / "modules", ROOT / "projects"]
    files = sorted({md for r in roots for md in ([r] if r.is_file() else r.rglob("*.md"))})
    errors, stale = [], []
    for md in files:
        rel = md.relative_to(ROOT)
        text = md.read_text(encoding="utf-8")

        def fill(m):
            try:
                code = region(m["path"], m["name"])
            except (KeyError, FileNotFoundError) as e:
                errors.append(f"{rel}: {e}")
                return m.group(0)
            return f"{m['open']}```cpp\n{code}\n```\n{m['close']}"

        new = MARK.sub(fill, text)
        spans = [m.span() for m in MARK.finditer(new)]
        for fm in re.finditer(r"^```cpp[ \t]*$", new, re.M):
            if not any(a <= fm.start() < b for a, b in spans):
                line = new.count("\n", 0, fm.start()) + 1
                errors.append(f"{rel}:{line}: ```cpp fence outside snippet markers "
                              "(put the code in a tested file and reference it, or use ```c++ for a short fragment)")
        if new != text:
            stale.append(str(rel))
            if not check:
                md.write_text(new, encoding="utf-8")

    if check and stale:
        errors += [f"{s}: snippets out of date (run `make snippets`)" for s in stale]
    elif stale:
        print("updated: " + ", ".join(stale))
    if errors:
        print("\n".join(errors), file=sys.stderr)
        return 1
    print("snippets OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
