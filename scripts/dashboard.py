#!/usr/bin/env python3
"""Local progress dashboard: every module, every problem, today's plan and your pace.

  make dashboard                       (or: python3 scripts/dashboard.py [--port 8765] [--no-open])

Serves http://127.0.0.1:8765 (this machine only). Ticking a box in the page rewrites that line in
modules/NN-*/problems.md, so the dashboard, `make today`, `make progress` and Claude's /today always agree.
Standard library only; Ctrl+C stops it.
"""
import argparse
import datetime as dt
import hashlib
import html
import subprocess
import json
import re
import threading
import webbrowser
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

import schedule
from kit import PROBLEM, ROOT, all_modules

PAGE = ROOT / "scripts" / "dashboard.html"
LOCK = threading.Lock()
VIDEO = re.compile(r"\s*·\s*\[video\]\((https?://[^)]+)\)")
MDLINK = re.compile(r"\[([^\]]+)\]\((https?://[^)]+)\)")


def why_html(why: str) -> str:
    why = re.sub(r"\s*#redo\b", "", VIDEO.sub("", why)).strip()
    why = re.sub(r"^added · ", "", why)
    out = html.escape(why, quote=False)
    out = re.sub(r"\[([^\]]+)\]\((https?://[^)]+)\)", r'<a href="\2" target="_blank" rel="noopener">\1</a>', out)
    return re.sub(r"`([^`]+)`", r"<code>\1</code>", out)


def item_json(it):
    d = {"id": f"{it.module}:{it.line_no}", "key": it.key, "raw": it.raw, "kind": it.kind, "label": it.label,
         "minutes": it.minutes, "done": it.done, "redo": it.redo, "subtopic": it.subtopic}
    if it.kind == "problem":
        m = PROBLEM.match(it.raw)
        why = m["why"] if m else ""
        v = VIDEO.search(why)
        d.update(url=it.url, difficulty=it.difficulty, pattern=it.pattern, worked="📖" in it.flags,
                 premium="🔒" in it.flags, added="— added" in it.raw, video=v[1] if v else None, why=why_html(why))
    return d


def load_plan():
    path = ROOT / "data" / "schedule.json"
    return json.loads(path.read_text(encoding="utf-8"))["days"] if path.exists() else []


def state():
    today = dt.date.today().isoformat()
    mods = all_modules()
    by_key = {i.key: i for m in mods for i in m.items}
    days = load_plan()

    modules = []
    for m in mods:
        core = m.core()
        subtopics = []
        for it in core:
            if not subtopics or subtopics[-1]["name"] != it.subtopic:
                subtopics.append({"name": it.subtopic, "items": []})
            subtopics[-1]["items"].append(item_json(it))
        probs = [i for i in core if i.kind == "problem"]
        modules.append({
            "dir": m.dir, "number": m.number, "title": m.title.split("·", 1)[-1].strip(),
            "notes": str(ROOT / "modules" / m.dir / "NOTES.md"),
            "done": sum(i.done for i in core), "total": len(core),
            "problems_done": sum(i.done for i in probs), "problems_total": len(probs),
            "hours_left": round(sum(i.minutes for i in core if not i.done) / 60, 1),
            "hours_total": round(sum(i.minutes for i in core) / 60, 1),
            "next": next((i.short for i in core if not i.done), None),
            "subtopics": subtopics,
        })

    # Pace: minutes the plan expected done by the end of today vs minutes actually done (in any order).
    planned_keys, before_keys, today_entries, overdue, seen = [], [], [], [], set()
    for d in days:
        if d["date"] > today:
            break
        for e in d["entries"]:
            if e["key"] and e["key"] not in seen:
                seen.add(e["key"])
                planned_keys.append(e["key"])
                if d["date"] < today:
                    before_keys.append(e["key"])
            if d["date"] == today:
                today_entries.append(e)
            elif e["key"] and e["key"] in by_key and not by_key[e["key"]].done and e not in overdue:
                overdue.append(e)
    planned_min = sum(by_key[k].minutes for k in planned_keys if k in by_key)          # through today
    planned_before = sum(by_key[k].minutes for k in before_keys if k in by_key)        # through yesterday
    done_min = sum(i.minutes for m in mods for i in m.core() if i.done)

    def entry(e):
        it = by_key.get(e["key"]) if e["key"] else None
        return {"label": e["label"], "minutes": e["minutes"], "module": e["module"],
                "id": f"{it.module}:{it.line_no}" if it else None, "done": bool(it and it.done)}

    weeks = {}
    if days:
        start = dt.date.fromisoformat(days[0]["date"])
        for d in days:
            w = (dt.date.fromisoformat(d["date"]) - start).days // 7 + 1
            weeks.setdefault(w, {"week": w, "from": d["date"], "to": d["date"], "modules": set()})
            weeks[w]["to"] = d["date"]
            weeks[w]["modules"].update(e["module"][:2] for e in d["entries"] if e["module"])
    total_min = sum(i.minutes for m in mods for i in m.core())
    return {
        "root": str(ROOT),
        "today": today,
        "plan": {"start": days[0]["date"] if days else None, "end": days[-1]["date"] if days else None,
                 "weeks": [{**w, "modules": sorted(w["modules"])} for w in weeks.values()]},
        "pace": {"planned_min": planned_min, "planned_before_min": planned_before, "done_min": done_min, "total_min": total_min},
        "today_items": [entry(e) for e in today_entries],
        "overdue": [entry(e) for e in overdue],
        "modules": modules,
    }


def set_item(item_id, raw, done, redo):
    mdir, _, line_no = item_id.rpartition(":")
    path = ROOT / "modules" / mdir / "problems.md"
    if not re.fullmatch(r"\d\d-[a-z0-9-]+", mdir) or not path.exists():
        raise ValueError("unknown module")
    with LOCK:
        lines = path.read_text(encoding="utf-8").split("\n")
        i = int(line_no) - 1
        if not (0 <= i < len(lines)) or lines[i] != raw:
            raise LookupError("stale")   # the file changed since the page loaded; the page reloads
        line = re.sub(r"^- \[[ xX]\]", "- [x]" if done else "- [ ]", lines[i])
        line = re.sub(r"\s*#redo\b", "", line).rstrip()
        if redo:
            line += " #redo"
        lines[i] = line
        path.write_text("\n".join(lines), encoding="utf-8")


def replan():
    args = schedule.parser().parse_args(["--start", dt.date.today().isoformat()])
    days = schedule.plan(args)
    if not days:
        return
    argv = ["scripts/schedule.py", "--start", args.start]
    with open(args.out, "w", encoding="utf-8") as f:
        f.write(schedule.render(args, days, argv))
    frozen = {"args": argv, "days": [{"date": d.isoformat(), "entries": [
        {"label": e[0], "minutes": e[1], "module": e[2], "key": e[3].key if e[3] else None} for e in es]} for d, es in days]}
    (ROOT / "data" / "schedule.json").write_text(json.dumps(frozen, ensure_ascii=False, indent=1), encoding="utf-8")


DOC_TYPES = {".md", ".cpp", ".hpp", ".in", ".out", ".txt", ".json"}


def repo_info():
    """owner/name of the GitHub remote, so the page knows where web ticks would commit."""
    try:
        url = subprocess.run(["git", "-C", str(ROOT), "remote", "get-url", "origin"], capture_output=True, text=True).stdout
        m = re.search(r"github\.com[:/]([^/]+)/([^/.\s]+)", url)
        if m:
            return {"owner": m[1], "name": m[2], "branch": "main"}
    except OSError:
        pass
    return {"owner": "", "name": "", "branch": "main"}


def manifest(root=True):
    return {"repo": repo_info(), "modules": sorted(p.parent.name for p in (ROOT / "modules").glob("*/problems.md")),
            "root": str(ROOT) if root else ""}


def kit_path(rel):
    path = (ROOT / rel).resolve()
    if ROOT.resolve() not in path.parents or path.suffix not in DOC_TYPES:
        raise ValueError(f"not a kit file: {rel}")
    return path


def raw_read(rel):
    path = kit_path(rel)
    data = path.read_bytes()
    return {"path": rel, "text": data.decode("utf-8"), "sha": hashlib.sha1(data).hexdigest()}


def raw_write(rel, text, sha):
    path = kit_path(rel)
    if not (path.suffix == ".md" and path.name == "problems.md"):
        raise ValueError("only problems.md files can be edited from the page")
    with LOCK:
        if hashlib.sha1(path.read_bytes()).hexdigest() != sha:
            raise LookupError("stale")
        data = text.encode("utf-8")
        path.write_bytes(data)
        return hashlib.sha1(data).hexdigest()


def doc(raw_path):
    """A kit file for the in-page reader (notes, examples, templates). Only files inside the kit."""
    from urllib.parse import unquote
    rel = unquote(raw_path).lstrip("/")
    path = (ROOT / rel).resolve()
    if ROOT.resolve() not in path.parents or path.suffix not in DOC_TYPES or not path.is_file():
        return 404, {"error": f"not found: {rel}"}
    return 200, {"path": str(path.relative_to(ROOT.resolve())), "text": path.read_text(encoding="utf-8")}


class Handler(BaseHTTPRequestHandler):
    def _send(self, code, body, ctype="application/json; charset=utf-8"):
        data = body if isinstance(body, bytes) else json.dumps(body, ensure_ascii=False).encode()
        self.send_response(code)
        self.send_header("Content-Type", ctype)
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(data)

    def do_GET(self):
        if self.path.split("?")[0] in ("/", "/index.html"):
            self._send(200, PAGE.read_bytes(), "text/html; charset=utf-8")
        elif self.path == "/api/state":
            self._send(200, state())
        elif self.path.startswith("/api/doc?path="):
            self._send(*doc(self.path.split("=", 1)[1]))
        elif self.path.split("?")[0] == "/manifest.json":
            self._send(200, manifest())
        elif self.path.startswith("/api/raw?path="):
            from urllib.parse import unquote
            try:
                self._send(200, raw_read(unquote(self.path.split("=", 1)[1])))
            except (ValueError, OSError) as e:
                self._send(404, {"error": str(e)})
        else:
            self._send(404, {"error": "not found"})

    def do_POST(self):
        try:
            body = json.loads(self.rfile.read(int(self.headers.get("Content-Length", 0))) or b"{}")
            if self.path == "/api/item":
                set_item(body["id"], body["raw"], bool(body["done"]), bool(body["redo"]))
            elif self.path == "/api/replan":
                replan()
            else:
                return self._send(404, {"error": "not found"})
            self._send(200, state())
        except LookupError:
            self._send(409, {"error": "problems.md changed on disk; reloaded", "state": state()})
        except (ValueError, KeyError) as e:
            self._send(400, {"error": str(e)})

    def do_PUT(self):
        if self.path != "/api/raw":
            return self._send(404, {"error": "not found"})
        try:
            body = json.loads(self.rfile.read(int(self.headers.get("Content-Length", 0))) or b"{}")
            self._send(200, {"sha": raw_write(body["path"], body["text"], body["sha"])})
        except LookupError:
            self._send(409, {"error": "changed on disk"})
        except (ValueError, KeyError, OSError) as e:
            self._send(400, {"error": str(e)})

    def log_message(self, *args):
        pass


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, default=8765)
    ap.add_argument("--no-open", action="store_true")
    a = ap.parse_args()
    url = f"http://127.0.0.1:{a.port}"
    try:
        server = ThreadingHTTPServer(("127.0.0.1", a.port), Handler)
    except OSError:   # already running (e.g. in another terminal): just open it
        print(f"port {a.port} is busy; opening {url} (use --port N for a separate instance)")
        if not a.no_open:
            webbrowser.open(url)
        return
    print(f"DSA dashboard on {url}  (Ctrl+C to stop)")
    if not a.no_open:
        threading.Timer(0.5, lambda: webbrowser.open(url)).start()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nstopped")


if __name__ == "__main__":
    main()
