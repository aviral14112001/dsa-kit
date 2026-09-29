#!/usr/bin/env python3
"""Print a LeetCode problem's statement as plain text (used by the /mock skill so Claude paraphrases
the real problem instead of recalling it from memory).

  python3 scripts/lc_statement.py 1423                # by number
  python3 scripts/lc_statement.py two-sum             # by slug
"""
import html
import json
import re
import sys
import urllib.request

from kit import ROOT

arg = sys.argv[1] if len(sys.argv) > 1 else sys.exit(__doc__)
if arg.isdigit():
    probs = {str(p["id"]): p for p in json.loads((ROOT / "data/leetcode-problems.json").read_text())}
    if arg not in probs:
        sys.exit(f"no LeetCode problem #{arg} in data/leetcode-problems.json")
    slug = probs[arg]["slug"]
else:
    slug = arg

query = {"query": "query q($s: String!) { question(titleSlug: $s) { questionFrontendId title difficulty isPaidOnly content topicTags { name } } }",
         "variables": {"s": slug}}
req = urllib.request.Request("https://leetcode.com/graphql", data=json.dumps(query).encode(),
                             headers={"Content-Type": "application/json", "User-Agent": "Mozilla/5.0",
                                      "Referer": f"https://leetcode.com/problems/{slug}/"})
with urllib.request.urlopen(req, timeout=20) as r:
    q = json.load(r)["data"]["question"]
if not q:
    sys.exit(f"LeetCode returned nothing for '{slug}'")
if not q["content"]:
    sys.exit(f"{q['questionFrontendId']}. {q['title']}: statement unavailable (Premium?)")

text = q["content"]
text = re.sub(r"<sup>(.*?)</sup>", r"^\1", text)
text = re.sub(r"<li>", "- ", text)
text = re.sub(r"<(br|/p|/li|/pre|/ul|/ol)\s*/?>", "\n", text)
text = html.unescape(re.sub(r"<[^>]+>", "", text))
text = re.sub(r"\n\s*\n+", "\n\n", text).strip()
print(f"{q['questionFrontendId']}. {q['title']} ({q['difficulty']})")
print("tags (don't reveal during a mock): " + ", ".join(t["name"] for t in q["topicTags"]))
print()
print(text)
