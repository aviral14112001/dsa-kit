#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

int main() {
    vector<int> v{1, 2, 3};
    CHECK_EQ(v.size(), 3);
    CHECK_EQ(v, vector<int>{1, 2, 3});
    map<string, int> mp{{"a", 1}};
    CHECK_EQ(mp.at("a"), 1);
    CHECK_EQ(to_vector(make_list({4, 5})), vector<int>{4, 5});
    CHECK_EQ(tree_to_string(make_tree("[3,9,20,null,null,15,7]")), "[3,9,20,null,null,15,7]");
    CHECK_EQ(tree_to_string(make_tree("[]")), "[]");
    CHECK_EQ(to_vector(make_cycle_list({1, 2, 3}, 0), 5), vector<int>{1, 2, 3, 1, 2});
    CHECK_EQ(find_node(make_tree("[1,2,3]"), 3)->val, 3);
    CHECK_EQ(t::show(make_pair(1, string("x"))), "(1, \"x\")");
    CHECK_EQ(t::show(vector<vector<int>>{{1}, {2, 3}}), "[[1], [2, 3]]");
    CHECK_EQ(t::show(optional<int>{}), "nullopt");
    CHECK_EQ(t::show(tuple<int, char, bool>{1, 'c', true}), "(1, 'c', true)");
    CHECK_NEAR(0.1 + 0.2, 0.3, 1e-9);
    CHECK(t::rand_int(1, 6) >= 1);
    return t::summary("harness");
}
