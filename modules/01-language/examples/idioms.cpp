// The idioms you should be able to type without thinking: lambdas (captures, recursion), range-for,
// emplace_back, MOD arithmetic, INF values, memset, grids, printing decimals. Module 01 section 4.
//     make run F=modules/01-language/examples/idioms.cpp
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:constants]
using ll = long long;
const int MOD = 1'000'000'007;                  // prime; a residue fits in int, a product of two doesn't
const int INF = 1'000'000'000;                  // 1e9: INF + INF = 2e9 still fits in int (max ≈ 2.1e9)
const ll LINF = 1'000'000'000'000'000'000LL;    // 1e18: LINF + LINF still fits in long long (max ≈ 9.2e18)
// [/snippet]

int main() {
    {
        // [snippet:lambdas]
        auto square = [](int x) { return x * x; };            // [] captures nothing
        int calls = 0;
        auto counted = [&](int x) { calls++; return x + 1; }; // [&] uses the caller's variables by reference
        int base = 10;
        auto add_base = [=](int x) { return x + base; };      // [=] COPIES base, right now
        base = 100;
        CHECK_EQ(square(4), 16);
        CHECK_EQ(counted(1) + counted(2), 5);
        CHECK_EQ(calls, 2);
        CHECK_EQ(add_base(1), 11);          // still the old base. A C# closure would see 100
        auto add_base_ref = [&base](int x) { return x + base; };
        CHECK_EQ(add_base_ref(1), 101);
        // [/snippet]
    }
    {
        // [snippet:recursive_lambda]
        vector<vector<int>> children{{1, 2}, {3}, {}, {}};   // tree: 0 -> 1, 2 and 1 -> 3
        // A lambda can't refer to itself by name, so it takes itself as a parameter.
        auto count_nodes = [&](auto&& self, int node) -> int {
            int total = 1;
            for (int child : children[node]) total += self(self, child);
            return total;
        };
        CHECK_EQ(count_nodes(count_nodes, 0), 4);

        // std::function can call itself by name, but it's type-erased: every call is an indirect
        // call the compiler can't inline, often several times slower in deep, hot recursion.
        function<int(int)> count_leaves = [&](int node) -> int {
            if (children[node].empty()) return 1;
            int leaves = 0;
            for (int child : children[node]) leaves += count_leaves(child);
            return leaves;
        };
        CHECK_EQ(count_leaves(0), 2);        // nodes 2 and 3
        // [/snippet]
    }
    {
        // [snippet:range_for]
        vector<int> nums{1, 2, 3};
        for (int& x : nums) x *= 10;                             // & to modify in place
        CHECK_EQ(nums, vector<int>{10, 20, 30});
        vector<string> names{"ada", "linus"};
        size_t letters = 0;
        for (const string& name : names) letters += name.size(); // const& to read without copying
        CHECK_EQ(letters, 8);
        map<string, int> age{{"ada", 36}, {"linus", 21}};
        int total_age = 0;
        for (const auto& [who, years] : age) total_age += years; // structured bindings over a map
        CHECK_EQ(total_age, 57);
        // [/snippet]
    }
    {
        // [snippet:emplace]
        vector<pair<int, int>> edges;
        edges.push_back({1, 2});                // builds a pair, then moves it in
        edges.emplace_back(2, 3);               // forwards the arguments to pair's constructor, in place
        CHECK_EQ(edges.back(), pair<int, int>{2, 3});
        vector<vector<int>> adj(4);             // 4 empty neighbour lists: the graph shape
        for (auto [u, v] : edges) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        CHECK_EQ(adj[2], vector<int>{1, 3});
        // [/snippet]
    }
    {
        // [snippet:mod_arith]
        ll a = MOD - 1, b = MOD - 8;                      // two residues: -1 and -8 in disguise
        CHECK_EQ((a + b) % MOD, MOD - 9);                 // add, then reduce
        CHECK_EQ(a * b % MOD, 8);                         // a * b ≈ 1e18 < LLONG_MAX ≈ 9.2e18: fits in ll
        int x = MOD - 1, y = MOD - 8;
        CHECK_EQ(1LL * x * y % MOD, 8);                   // ints: widen BEFORE multiplying
        CHECK_EQ(((b - a) % MOD + MOD) % MOD, MOD - 7);   // subtract: % can go negative, add MOD back
        // [/snippet]
    }
    {
        // [snippet:inf]
        vector<int> dist(5, INF);                         // "unreached"
        dist[0] = 0;
        CHECK(dist[1] + dist[2] > 0);                     // INF + INF fits: adding to INF never overflows
        // With INF = INT_MAX, dist[u] + w overflows (UB) the first time you relax from an unreached u.
        CHECK_EQ(min(dist[1], dist[0] + 7), 7);           // the relaxation shape: min(old, candidate)
        vector<ll> far(3, LINF);
        CHECK(far[0] + far[1] > 0);
        // [/snippet]
    }
    {
        // [snippet:memset]
        int a[5];
        memset(a, 0, sizeof a);            // every byte 0x00 -> every int is 0
        CHECK_EQ(a[3], 0);
        memset(a, -1, sizeof a);           // every byte 0xFF -> every int is -1
        CHECK_EQ(a[3], -1);
        memset(a, 1, sizeof a);            // every byte 0x01 -> every int is 0x01010101, NOT 1
        CHECK_EQ(a[3], 16'843'009);
        memset(a, 0x3f, sizeof a);         // 0x3f3f3f3f ≈ 1.06e9: the one "INF" memset can make
        CHECK_EQ(a[3], 1'061'109'567);
        vector<int> v(5);
        fill(v.begin(), v.end(), 7);       // any other value: fill(), or construct vector<int>(n, 7)
        CHECK_EQ(v, vector<int>(5, 7));
        // [/snippet]
    }
    {
        // [snippet:grid]
        int rows = 3, cols = 4;
        vector<vector<int>> grid(rows, vector<int>(cols, 0));     // rows x cols, all 0
        vector<vector<bool>> seen(rows, vector<bool>(cols, false));
        const int DR[4] = {-1, 1, 0, 0}, DC[4] = {0, 0, -1, 1};   // up, down, left, right
        auto inside = [&](int r, int c) { return r >= 0 && r < rows && c >= 0 && c < cols; };
        int r = 0, c = 0, neighbours = 0;                         // the corner cell
        for (int d = 0; d < 4; d++) {
            int nr = r + DR[d], nc = c + DC[d];
            if (inside(nr, nc) && !seen[nr][nc]) neighbours++;
        }
        CHECK_EQ(neighbours, 2);
        grid[2][3] = 5;
        CHECK_EQ(grid[2][3], 5);
        // [/snippet]
    }
    {
        // [snippet:print_decimal]
        ostringstream out;                                   // stands in for cout here
        out << fixed << setprecision(6) << 2.0 / 3 << '\n';  // fixed: always 6 digits after the point
        out << 1e9 + 0.5 << '\n';                            // fixed also stops 1e+09-style output
        CHECK_EQ(out.str(), "0.666667\n1000000000.500000\n");
        // [/snippet]
    }
    return t::summary("idioms");
}
