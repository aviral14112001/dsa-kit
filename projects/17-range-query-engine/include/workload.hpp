// Random operation streams. `allowed` restricts the op types (so an engine that can't assign never
// sees an assign), and `query_share` sets how read-heavy the stream is.
#pragma once
#include <random>
#include <vector>

#include "engine.hpp"

struct Workload {
    std::vector<long long> init;
    std::vector<Op> ops;
};

inline Workload make_workload(int n, int q, const std::vector<OpType>& allowed, double query_share,
                              unsigned seed, long long max_value = 1'000'000) {
    std::mt19937_64 rng(seed);
    auto rnd = [&](long long lo, long long hi) { return std::uniform_int_distribution<long long>(lo, hi)(rng); };
    std::vector<OpType> queries, updates;
    for (OpType t : allowed) (is_query(t) ? queries : updates).push_back(t);

    Workload w;
    w.init.resize(n);
    for (auto& x : w.init) x = rnd(-max_value, max_value);
    w.ops.reserve(q);
    for (int k = 0; k < q; k++) {
        bool query = updates.empty() || (!queries.empty() && rnd(0, 999'999) < query_share * 1'000'000);
        const auto& pool = query ? queries : updates;
        Op op{pool[rnd(0, (long long)pool.size() - 1)]};
        int a = (int)rnd(0, n - 1), b = (int)rnd(0, n - 1);
        op.l = std::min(a, b);
        op.r = std::max(a, b) + 1;   // non-empty [l, r)
        op.v = op.type == OpType::RangeAdd ? rnd(-1000, 1000) : rnd(-max_value, max_value);
        if (op.type == OpType::PointSet) op.r = op.l + 1;
        w.ops.push_back(op);
    }
    return w;
}
