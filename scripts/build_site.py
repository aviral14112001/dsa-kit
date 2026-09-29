#!/usr/bin/env python3
"""Build the static GitHub Pages site into _site/ (run by .github/workflows/pages.yml on every push).

The site is the same dashboard page as `make dashboard`, plus a read-only copy of the files it reads
(problem lists, the frozen plan, notes, examples, templates). Connecting a GitHub token in the page
switches reads and writes to the GitHub API, so ticks become commits.
  python3 scripts/build_site.py [--out _site]
"""
import argparse
import json
import os
import shutil

from dashboard import PAGE, manifest
from kit import ROOT

ap = argparse.ArgumentParser()
ap.add_argument("--out", default=str(ROOT / "_site"))
out = ROOT / ap.parse_args().out if not os.path.isabs(ap.parse_args().out) else ap.parse_args().out
out = __import__("pathlib").Path(out)

if out.exists():
    shutil.rmtree(out)
(out / "files").mkdir(parents=True)

m = manifest(root=False)
if os.environ.get("GITHUB_REPOSITORY"):                  # set by GitHub Actions: owner/name
    owner, name = os.environ["GITHUB_REPOSITORY"].split("/")
    m["repo"] = {"owner": owner, "name": name, "branch": os.environ.get("GITHUB_REF_NAME", "main")}
(out / "manifest.json").write_text(json.dumps(m, indent=1), encoding="utf-8")
shutil.copy(PAGE, out / "index.html")
(out / ".nojekyll").write_text("")

KEEP = {".md", ".cpp", ".hpp", ".in", ".out", ".json"}
count = 0
for rel in ["modules", "templates", "include", "projects", "mocks", "data/schedule.json", "README.md", "SCHEDULE.md"]:
    src = ROOT / rel
    files = [src] if src.is_file() else [p for p in src.rglob("*") if p.is_file() and p.suffix in KEEP and "build" not in p.parts]
    for f in files:
        dst = out / "files" / f.relative_to(ROOT)
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy(f, dst)
        count += 1
print(f"built {out}: index.html + {count} files for {m['repo']['owner']}/{m['repo']['name']}")
