# make test | make bench | make stream | make clean
CXX        := clang++
CXXFLAGS   := -std=c++20 -g -O1 -Wall -Wextra -Wshadow -fsanitize=address,undefined -fno-sanitize-recover=undefined \
              -fno-omit-frame-pointer -Iinclude -I../../include -I../../templates
FASTFLAGS  := -std=c++20 -O2 -Iinclude -I../../include -I../../templates
DEPS       := $(wildcard include/*.hpp engines/*.hpp)

.PHONY: test bench stream clean

test: build/diff_test
	@./build/diff_test

bench: build/bench
	@./build/bench

stream: build/stream build/gen
	@echo "built: ./build/gen n q static-min|sums|mixed seed > s.txt && ./build/stream <engine> < s.txt"

build/diff_test: tests/diff_test.cpp $(DEPS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o $@

build/bench: bench/bench.cpp $(DEPS)
	@mkdir -p build
	$(CXX) $(FASTFLAGS) $< -o $@

build/stream: src/stream.cpp $(DEPS)
	@mkdir -p build
	$(CXX) $(FASTFLAGS) $< -o $@

build/gen: src/gen.cpp $(DEPS)
	@mkdir -p build
	$(CXX) $(FASTFLAGS) $< -o $@

clean:
	rm -rf build
