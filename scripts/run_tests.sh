#!/usr/bin/env bash
# Runs the binaries `make test` built. A binary passes if it exits 0 (t::summary() returns 1 on
# any failed CHECK). Stdin/stdout examples: for each <source>.in or <source>.<k>.in next to
# <source>.cpp, the file is fed on stdin and stdout must match the matching .out exactly.
set -u
export ASAN_OPTIONS=detect_leaks=0        # LeetCode-style code never frees nodes; don't flag that
export UBSAN_OPTIONS=print_stacktrace=1

pass=0; fail=0; failed=()
for bin in "$@"; do
  src="${bin#build/}"
  inputs=()
  for f in "$src".in "$src".*.in; do [[ -f "$f" ]] && inputs+=("$f"); done
  if (( ${#inputs[@]} )); then
    ok=1
    for in in "${inputs[@]}"; do
      out="${in%.in}.out"
      if ! diff_out=$(diff <("$bin" < "$in" 2>&1) "$out"); then
        echo "FAIL $src: output for $in differs from $out"; echo "$diff_out" | head -20; ok=0
      fi
    done
    if (( ok )); then echo "ok   $src (${#inputs[@]} stdin case(s))"; pass=$((pass + 1))
    else fail=$((fail + 1)); failed+=("$src"); fi
  elif "$bin"; then
    pass=$((pass + 1))
  else
    fail=$((fail + 1)); failed+=("$src")
  fi
done

echo
if (( fail )); then
  echo "$pass passed, $fail FAILED:"; printf '  %s\n' "${failed[@]}"; exit 1
fi
echo "all $pass test programs passed"
