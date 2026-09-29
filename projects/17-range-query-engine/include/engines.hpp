// Register your engines here as you build them (milestones 1 and 2). The tester, the benchmark and
// the stream CLI all get their engines from this list.
#pragma once
#include <memory>
#include <vector>

#include "brute.hpp"
// #include "../engines/fenwick_engine.hpp"   // milestone 1
// #include "../engines/sparse_engine.hpp"    // milestone 1
// #include "../engines/segtree_engine.hpp"   // milestone 2

inline std::vector<std::unique_ptr<RangeEngine>> make_engines() {
    std::vector<std::unique_ptr<RangeEngine>> engines;
    engines.push_back(std::make_unique<BruteEngine>());
    // engines.push_back(std::make_unique<FenwickEngine>());
    // engines.push_back(std::make_unique<SparseTableEngine>());
    // engines.push_back(std::make_unique<LazySegTreeEngine>());
    return engines;
}
