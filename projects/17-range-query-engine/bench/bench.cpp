// Milestone 3: time every registered engine on each scenario it supports.
//   make bench                 (built with -O2, no sanitizers)
// Brute force is skipped once n * q gets too large to finish quickly.
#include <bits/stdc++.h>
#include "engines.hpp"
#include "workload.hpp"
using namespace std;

struct Scenario {
    string name;
    vector<OpType> ops;
    double query_share;
};

int main(int argc, char** argv) {
    int q = argc > 1 ? stoi(argv[1]) : 100'000;
    vector<Scenario> scenarios = {
        {"static-min (queries only)", {OpType::RangeMin}, 1.0},
        {"sums, 50% updates", {OpType::PointSet, OpType::RangeAdd, OpType::RangeSum}, 0.5},
        {"min, 1% point updates", {OpType::PointSet, OpType::RangeMin}, 0.99},
        {"everything, 50% updates", {OpType::PointSet, OpType::RangeAdd, OpType::RangeAssign, OpType::RangeSum, OpType::RangeMin}, 0.5},
    };
    auto engines = make_engines();
    using clock = chrono::steady_clock;

    for (const auto& sc : scenarios) {
        printf("\n%s, q = %d\n%10s", sc.name.c_str(), q, "n");
        vector<RangeEngine*> able;
        for (auto& e : engines)
            if (all_of(sc.ops.begin(), sc.ops.end(), [&](OpType t) { return e->supports(t); })) able.push_back(e.get());
        for (auto* e : able) printf("%14s", e->name().c_str());
        printf("   (ms)\n");
        for (int n : {1'000, 10'000, 100'000, 1'000'000}) {
            Workload w = make_workload(n, q, sc.ops, sc.query_share, 42);
            printf("%10d", n);
            long long reference = LLONG_MIN;
            for (auto* e : able) {
                if (e->name() == "brute" && (double)n * q > 2e9) { printf("%14s", "skip"); continue; }
                auto t0 = clock::now();
                e->build(w.init);
                long long checksum = 0;
                for (const Op& op : w.ops) checksum += e->apply(op);   // use the answers so nothing is optimized away
                double ms = chrono::duration<double, milli>(clock::now() - t0).count();
                if (reference == LLONG_MIN) reference = checksum;
                printf("%13.1f%s", ms, checksum == reference ? " " : "!");   // "!" = answers differ from the first engine
            }
            printf("\n");
        }
    }
}
