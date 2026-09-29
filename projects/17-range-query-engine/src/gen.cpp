// Writes a random op stream for src/stream.cpp:   ./build/gen n q profile seed
// profiles: static-min | sums | mixed
#include <bits/stdc++.h>
#include "workload.hpp"
using namespace std;

int main(int argc, char** argv) {
    if (argc < 5) {
        cerr << "usage: gen n q static-min|sums|mixed seed\n";
        return 1;
    }
    int n = stoi(argv[1]), q = stoi(argv[2]);
    string profile = argv[3];
    unsigned seed = (unsigned)stoul(argv[4]);
    vector<OpType> ops;
    double query_share = 0.5;
    if (profile == "static-min") ops = {OpType::RangeMin}, query_share = 1.0;
    else if (profile == "sums") ops = {OpType::PointSet, OpType::RangeAdd, OpType::RangeSum};
    else ops = {OpType::PointSet, OpType::RangeAdd, OpType::RangeAssign, OpType::RangeSum, OpType::RangeMin};
    Workload w = make_workload(n, q, ops, query_share, seed);

    string out = to_string(n) + '\n';
    for (int i = 0; i < n; i++) out += to_string(w.init[i]) + (i + 1 < n ? ' ' : '\n');
    for (const Op& op : w.ops) out += to_string(op) + '\n';
    fwrite(out.data(), 1, out.size(), stdout);
}
