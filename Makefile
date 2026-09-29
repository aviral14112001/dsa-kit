# DSA kit. `make help` lists the commands.
CXX      := clang++
STD      := -std=c++20
WARN     := -Wall -Wextra -Wshadow -Wno-unused-parameter
SAN      := -fsanitize=address,undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer
BASE     := $(STD) -g $(SAN) -Iinclude -Itemplates
CXXFLAGS := $(BASE) -O1 $(WARN) -Werror -MMD -MP

# `make test M=modules/06-` (or M=templates, M=dsu_test ...) only builds/runs paths containing that text.
M ?=
SRCS := $(shell find templates/tests modules -name '*.cpp' 2>/dev/null | grep -- '$(M)' | sort)
BINS := $(patsubst %.cpp,build/%,$(SRCS))

NPROC := $(shell sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)
MAKEFLAGS += -j$(NPROC) --no-print-directory

START ?= $(shell date +%Y-%m-%d)

.PHONY: help test check snippets progress schedule replan today dashboard dashboard-stop run clean

help:
	@echo "make run F=practice/x.cpp [IN=in.txt]   compile your file with sanitizers + -DLOCAL and run it"
	@echo "make test [M=06]                        build + run every worked example and template test"
	@echo "make dashboard                          open the progress dashboard in your browser (localhost:8765)"
	@echo "make today                              today's plan from SCHEDULE.md + anything overdue"
	@echo "make progress                           progress bars per module, #redo list, what's next"
	@echo "make replan [START=YYYY-MM-DD]          rebuild SCHEDULE.md from unchecked items (default: today)"
	@echo "make check                              notes in sync with tested code + problem links valid"
	@echo "make snippets                           copy tested code regions into the notes"
	@echo "make clean                              delete build/"

test: $(BINS)
	@./scripts/run_tests.sh $(BINS)

build/%: %.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX   $<"
	@$(CXX) $(CXXFLAGS) $< -o $@

-include $(BINS:=.d)

run:
	@test -n "$(F)" || (echo "usage: make run F=path/to/file.cpp [IN=input.txt]"; exit 1)
	@mkdir -p build/run
	@$(CXX) $(BASE) -O1 $(WARN) -DLOCAL "$(F)" -o build/run/a.out
	@if [ -n "$(IN)" ]; then ./build/run/a.out < "$(IN)"; else ./build/run/a.out; fi

check:
	@python3 scripts/snippets.py --check
	@python3 scripts/check_problems.py

snippets:
	@python3 scripts/snippets.py

progress:
	@python3 scripts/progress.py

# The original plan: every item, starting 2026-09-28.
schedule:
	@python3 scripts/schedule.py --start 2026-09-28 --all

# Fell behind or got ahead? Re-plan only what's still unchecked, from START (default today).
replan:
	@python3 scripts/schedule.py --start $(START)

clean:
	rm -rf build

today:
	@python3 scripts/today.py

dashboard:
	@python3 scripts/dashboard.py

dashboard-stop:
	@pkill -f scripts/dashboard.py && echo "dashboard stopped" || echo "dashboard was not running"
