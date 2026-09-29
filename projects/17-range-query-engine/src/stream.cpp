// A "live" stream: reads an array and then one op per line from stdin, answering queries as they arrive.
//   ./build/gen 100000 200000 mixed 1 > /tmp/stream.txt
//   ./build/stream segtree < /tmp/stream.txt > /tmp/out.txt
// Format:  n / n values / then lines like  set i v | add l r v | assign l r v | sum l r | min l r
#include <bits/stdc++.h>
#include "engines.hpp"
using namespace std;

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto engines = make_engines();
    string want = argc > 1 ? argv[1] : engines.back()->name();
    auto it = find_if(engines.begin(), engines.end(), [&](const auto& e) { return e->name() == want; });
    if (it == engines.end()) {
        cerr << "no engine named '" << want << "'. registered:";
        for (auto& e : engines) cerr << ' ' << e->name();
        cerr << '\n';
        return 1;
    }
    RangeEngine& engine = **it;

    int n;
    cin >> n;
    vector<long long> a(n);
    for (auto& x : a) cin >> x;
    engine.build(a);

    map<string, OpType> kinds{{"set", OpType::PointSet}, {"add", OpType::RangeAdd}, {"assign", OpType::RangeAssign},
                              {"sum", OpType::RangeSum}, {"min", OpType::RangeMin}};
    string word;
    while (cin >> word) {
        Op op{kinds.at(word)};
        if (op.type == OpType::PointSet) cin >> op.l >> op.v;
        else cin >> op.l >> op.r;
        if (!is_query(op.type) && op.type != OpType::PointSet) cin >> op.v;
        long long ans = engine.apply(op);
        if (is_query(op.type)) cout << ans << '\n';
    }
}
