// The unordered_map / unordered_set toolkit (module 07 section 1): the API and its traps, the counting,
// grouping and de-duplication patterns, erasing while iterating, and when std::map is the
// better choice.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

int main() {
    // [snippet:api]
    unordered_map<string, int> age;
    age["ana"] = 31;                            // insert, or overwrite if present
    age.insert({"bo", 25});                     // insert only if absent; returns {iterator, inserted?}
    bool inserted = age.try_emplace("bo", 99).second;   // "bo" exists: nothing changes -> false
    int a = age.at("ana");                      // a read that throws std::out_of_range when absent
    bool has_cy = age.count("cy") > 0;          // 0 or 1 for a map; C++20: age.contains("cy")
    if (auto it = age.find("bo"); it != age.end()) it->second++;   // ONE lookup to test and update
    age.erase("bo");                            // by key; returns how many were erased (0 or 1)
    int z = age["zed"];                         // TRAP: [] on a missing key INSERTS {"zed", 0}
    // [/snippet]
    CHECK(!inserted);
    CHECK_EQ(a, 31);
    CHECK(!has_cy);
    CHECK_EQ(z, 0);
    CHECK_EQ(age.size(), 2);                    // ana, and the accidental zed
    CHECK_EQ(age.count("zed"), 1);
    CHECK_EQ(age.count("bo"), 0);

    vector<int> nums{4, 1, 4, 2, 1, 4};
    // [snippet:patterns]
    // Frequency count: [] value-initializes a missing int to 0, so ++ just works.
    unordered_map<int, int> freq;
    for (int x : nums) freq[x]++;
    // Grouping: [] default-constructs an empty vector the first time a key shows up.
    unordered_map<int, vector<int>> positions;
    for (int i = 0; i < (int)nums.size(); i++) positions[nums[i]].push_back(i);
    // De-duplication in first-seen order: insert(x).second is true only the first time.
    unordered_set<int> seen;
    vector<int> first_seen;
    for (int x : nums)
        if (seen.insert(x).second) first_seen.push_back(x);
    // [/snippet]
    CHECK_EQ(freq[4], 3);
    CHECK_EQ(freq[2], 1);
    CHECK_EQ(positions[1], vector<int>{1, 4});
    CHECK_EQ(first_seen, vector<int>{4, 1, 2});

    // [snippet:erase_loop]
    // Erasing while iterating: erase(it) returns the iterator after it. Never ++ an erased one.
    for (auto it = freq.begin(); it != freq.end();) {
        if (it->second < 2) it = freq.erase(it);
        else ++it;
    }
    // C++20 says the same in one line: erase_if(freq, [](const auto& kv) { return kv.second < 2; });
    // [/snippet]
    CHECK_EQ(freq.size(), 2);                   // 4 (x3) and 1 (x2) survive
    CHECK_EQ(freq.count(2), 0);

    // [snippet:ordered]
    // std::map: keys come out sorted, pair keys work with no extra code, and you get lower_bound,
    // prev and next. Every operation is O(log n), worst case included.
    map<pair<int, int>, string> cell{{{2, 1}, "c"}, {{0, 5}, "a"}, {{0, 7}, "b"}};
    string in_order;
    for (const auto& [pos, label] : cell) in_order += label;   // "abc": sorted by (row, col)
    auto below_row_0 = cell.lower_bound({1, INT_MIN});         // first key with row >= 1: (2, 1)
    // unordered_map<pair<int, int>, string> would not compile: std::hash has no pair version (Section 3).
    // [/snippet]
    CHECK_EQ(in_order, "abc");
    CHECK_EQ(below_row_0->first, make_pair(2, 1));

    // Iteration order of an unordered container is unspecified: it can differ between
    // compilers and change after a rehash. Only the CONTENTS are guaranteed.
    unordered_set<int> bag;
    for (int i = 0; i < 100; i++) bag.insert(i * 37 % 101);
    vector<int> listed(bag.begin(), bag.end());
    sort(listed.begin(), listed.end());          // sort before comparing or printing
    vector<int> expected;
    for (int i = 0; i < 100; i++) expected.push_back(i * 37 % 101);
    sort(expected.begin(), expected.end());
    CHECK_EQ(listed, expected);

    // reserve(n): room for n elements without rehashing (bucket_count >= n / max_load_factor).
    unordered_map<int, int> pre;
    pre.reserve(1000);
    size_t buckets_before = pre.bucket_count();
    for (int i = 0; i < 1000; i++) pre[i] = i;
    CHECK_EQ(pre.bucket_count(), buckets_before);   // no rehash happened
    CHECK(pre.load_factor() <= pre.max_load_factor());
    return t::summary("hash_basics");
}
