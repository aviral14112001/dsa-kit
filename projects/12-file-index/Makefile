# make test | make cli | make bench | make clean
CXX        := clang++
CXXFLAGS   := -std=c++20 -g -O1 -Wall -Wextra -Wshadow -fsanitize=address,undefined -fno-sanitize-recover=undefined \
              -fno-omit-frame-pointer -Iinclude -I../../include
BENCHFLAGS := -std=c++20 -O2 -Iinclude -I../../include
SRC        := src/file_index.cpp
HDR        := include/file_index.hpp

.PHONY: test cli bench clean

test: build/file_index_test
	@./build/file_index_test

cli: build/file-index
	@echo "built: ./build/file-index"

bench: build/bench
	@./build/bench 20000

build/file_index_test: tests/file_index_test.cpp $(SRC) $(HDR)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) tests/file_index_test.cpp $(SRC) -o $@

build/file-index: src/main.cpp $(SRC) $(HDR)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) src/main.cpp $(SRC) -o $@

build/bench: bench/bench.cpp $(SRC) $(HDR)
	@mkdir -p build
	$(CXX) $(BENCHFLAGS) bench/bench.cpp $(SRC) -o $@

clean:
	rm -rf build
