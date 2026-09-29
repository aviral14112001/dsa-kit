// The reference: obviously correct, O(n) per operation. Every other engine is checked against it.
#pragma once
#include <algorithm>
#include <numeric>

#include "engine.hpp"

class BruteEngine : public RangeEngine {
public:
    std::string name() const override { return "brute"; }
    bool supports(OpType) const override { return true; }
    void build(const std::vector<long long>& a) override { a_ = a; }
    void point_set(int i, long long v) override { a_[i] = v; }
    void range_add(int l, int r, long long v) override { for (int i = l; i < r; i++) a_[i] += v; }
    void range_assign(int l, int r, long long v) override { std::fill(a_.begin() + l, a_.begin() + r, v); }
    long long range_sum(int l, int r) override { return std::accumulate(a_.begin() + l, a_.begin() + r, 0LL); }
    long long range_min(int l, int r) override { return *std::min_element(a_.begin() + l, a_.begin() + r); }

private:
    std::vector<long long> a_;
};
