// Write each milestone's tests HERE, BEFORE implementing it. Checklist: README.md.
// Run: make test   (sanitizers on; a method that still throws "not implemented" shows up as a failure)
#include <bits/stdc++.h>
#include "test.hpp"
#include "file_index.hpp"
using namespace std;

// One throwing method shouldn't hide the rest of the report: each group runs inside `run`.
template <class F>
void run(const char* group, F body) {
    try {
        body();
    } catch (const exception& e) {
        t::report(false, group, e.what(), __FILE__, __LINE__);
    }
}

int main() {
    run("empty index", [] {
        FileIndex idx;
        CHECK_EQ(idx.size(), 0);
        CHECK(!idx.contains("src/main.cpp"));
        CHECK(!idx.erase("src/main.cpp"));
        CHECK(idx.all().empty());
    });

    // TODO(milestone 1): duplicates, sorted order after random inserts, the three erase cases,
    //                    erase the root until empty, erase a missing path, 10k random ops vs std::set.
    // TODO(milestone 2): check_invariants() after every op, rank/kth round trip, every complete()
    //                    edge case in the README, count_prefix, range.

    return t::summary("file_index");
}
