// The contract every engine implements. Indices are 0-based and ranges are half-open: [l, r).
#pragma once
#include <stdexcept>
#include <string>
#include <vector>

enum class OpType { PointSet, RangeAdd, RangeAssign, RangeSum, RangeMin };

struct Op {
    OpType type;
    int l = 0, r = 0;    // PointSet uses l as the index
    long long v = 0;     // value for updates; unused by queries
};

inline bool is_query(OpType t) { return t == OpType::RangeSum || t == OpType::RangeMin; }

inline const char* op_name(OpType t) {
    switch (t) {
        case OpType::PointSet: return "set";
        case OpType::RangeAdd: return "add";
        case OpType::RangeAssign: return "assign";
        case OpType::RangeSum: return "sum";
        case OpType::RangeMin: return "min";
    }
    return "?";
}

inline std::string to_string(const Op& op) {
    std::string s = op_name(op.type);
    if (op.type == OpType::PointSet) return s + " " + std::to_string(op.l) + " " + std::to_string(op.v);
    s += " " + std::to_string(op.l) + " " + std::to_string(op.r);
    if (!is_query(op.type)) s += " " + std::to_string(op.v);
    return s;
}

class RangeEngine {
public:
    virtual ~RangeEngine() = default;
    virtual std::string name() const = 0;
    virtual bool supports(OpType t) const = 0;
    virtual void build(const std::vector<long long>& a) = 0;

    // Override the ones your engine supports; the rest throw.
    virtual void point_set(int, long long) { unsupported("point_set"); }
    virtual void range_add(int, int, long long) { unsupported("range_add"); }
    virtual void range_assign(int, int, long long) { unsupported("range_assign"); }
    virtual long long range_sum(int, int) { unsupported("range_sum"); }
    virtual long long range_min(int, int) { unsupported("range_min"); }   // requires l < r

    // Runs any op; returns the answer for queries and 0 for updates.
    long long apply(const Op& op) {
        switch (op.type) {
            case OpType::PointSet: point_set(op.l, op.v); return 0;
            case OpType::RangeAdd: range_add(op.l, op.r, op.v); return 0;
            case OpType::RangeAssign: range_assign(op.l, op.r, op.v); return 0;
            case OpType::RangeSum: return range_sum(op.l, op.r);
            case OpType::RangeMin: return range_min(op.l, op.r);
        }
        return 0;
    }

protected:
    [[noreturn]] void unsupported(const char* what) const {
        throw std::logic_error(name() + " does not support " + what);
    }
};
