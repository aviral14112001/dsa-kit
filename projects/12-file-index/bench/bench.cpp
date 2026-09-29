// Milestone 2: sorted inserts turn a plain BST into a linked list. Time it, balance the tree, time it again.
//   make bench            (n = 20000; built with -O2 and no sanitizers)
//   ./build/bench 200000  (try this once the tree is balanced)
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

#include "file_index.hpp"

int main(int argc, char** argv) {
    std::size_t n = argc > 1 ? std::stoul(argv[1]) : 20000;
    std::vector<std::string> keys;
    keys.reserve(n);
    char buf[32];
    for (std::size_t i = 0; i < n; i++) {
        std::snprintf(buf, sizeof buf, "src/file%08zu.cpp", i);   // already sorted: the worst case for a plain BST
        keys.emplace_back(buf);
    }

    using clock = std::chrono::steady_clock;
    auto ms = [](clock::duration d) { return std::chrono::duration<double, std::milli>(d).count(); };

    FileIndex idx;
    auto t0 = clock::now();
    for (const auto& k : keys) idx.insert(k);
    auto t1 = clock::now();
    std::size_t hits = 0;
    for (const auto& k : keys) hits += idx.contains(k);
    auto t2 = clock::now();
    std::size_t completed = idx.complete("src/file0000", 50).size();
    auto t3 = clock::now();

    std::printf("n=%zu  insert %.1f ms  lookup %.1f ms  complete %.3f ms  (hits %zu, completions %zu)\n",
                n, ms(t1 - t0), ms(t2 - t1), ms(t3 - t2), hits, completed);
}
