// Custom comparators: multi-key sorts, heaps and sets with your own order, and the strict weak
// ordering rule every comparator must obey. Module 01 section 3.
//     make run F=modules/01-language/examples/comparators.cpp
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:struct_sort]
struct Player {
    string name;
    int score;
};

// true iff a must come BEFORE b: higher score first, equal scores by name A to Z.
bool ranks_before(const Player& a, const Player& b) {
    if (a.score != b.score) return a.score > b.score;   // decide on the first key that differs...
    return a.name < b.name;                              // ...and only then look at the next one
}
// [/snippet]

// [snippet:spaceship]
struct Point {
    int x, y;
    auto operator<=>(const Point&) const = default;   // C++20: compare x, then y; also defines ==
};
// [/snippet]

// [snippet:swo_check]
// Brute-force test that cmp is a strict weak ordering on a small sample: O(k^3), for tests only.
template <class T, class Cmp>
bool is_strict_weak_order(const vector<T>& sample, Cmp cmp) {
    auto equiv = [&](const T& a, const T& b) { return !cmp(a, b) && !cmp(b, a); };
    for (const T& a : sample) {
        if (cmp(a, a)) return false;                                  // irreflexive: never a < a
        for (const T& b : sample) {
            if (cmp(a, b) && cmp(b, a)) return false;                 // asymmetric
            for (const T& c : sample) {
                if (cmp(a, b) && cmp(b, c) && !cmp(a, c)) return false;          // transitive
                if (equiv(a, b) && equiv(b, c) && !equiv(a, c)) return false;    // ties are transitive too
            }
        }
    }
    return true;
}
// [/snippet]

vector<string> names_of(const vector<Player>& players) {
    vector<string> out;
    for (const Player& p : players) out.push_back(p.name);
    return out;
}

int main() {
    vector<Player> roster{{"bob", 70}, {"amy", 90}, {"cat", 70}, {"dan", 85}};
    {
        // [snippet:multi_key]
        vector<Player> players = roster;
        sort(players.begin(), players.end(), ranks_before);
        // C#: players.OrderByDescending(p => p.score).ThenBy(p => p.name)
        CHECK_EQ(names_of(players), vector<string>{"amy", "dan", "bob", "cat"});

        // The same order as an inline lambda: compare (-score, name) pairs lexicographically.
        vector<Player> again = roster;
        sort(again.begin(), again.end(), [](const Player& a, const Player& b) {
            return make_pair(-a.score, a.name) < make_pair(-b.score, b.name);   // negate to flip one key
        });
        CHECK_EQ(names_of(again), names_of(players));
        // [/snippet]
    }
    {
        // [snippet:sort_variants]
        vector<int> v{5, -3, 8, 3, -8};
        sort(v.begin(), v.end(), greater<int>());                   // descending
        CHECK_EQ(v, vector<int>{8, 5, 3, -3, -8});
        sort(v.begin(), v.end(), [](int a, int b) {                 // by |x|, ties: negative first
            if (abs(a) != abs(b)) return abs(a) < abs(b);
            return a < b;
        });
        CHECK_EQ(v, vector<int>{-3, 3, 5, -8, 8});

        vector<string> words{"bb", "a", "ccc", "dd", "e"};
        stable_sort(words.begin(), words.end(), [](const string& a, const string& b) {
            return a.size() < b.size();                             // equal lengths keep input order
        });
        CHECK_EQ(words, vector<string>{"a", "e", "bb", "dd", "ccc"});
        // [/snippet]
    }
    {
        // [snippet:heap_set_order]
        // priority_queue asks "does a have LOWER priority than b?", so a > on the key puts the
        // smallest key on top. A lambda comparator goes in as decltype(...) plus a constructor argument.
        auto later_deadline = [](const pair<int, string>& a, const pair<int, string>& b) {
            return a.first > b.first;
        };
        priority_queue<pair<int, string>, vector<pair<int, string>>, decltype(later_deadline)>
            tasks(later_deadline);
        tasks.push({3, "deploy"});
        tasks.push({1, "review"});
        tasks.push({2, "test"});
        CHECK_EQ(tasks.top().second, "review");                     // earliest deadline on top

        set<int, greater<int>> desc{4, 9, 1};                       // a set that iterates largest-first
        CHECK_EQ(*desc.begin(), 9);
        set<Point> points{{2, 1}, {1, 5}, {1, 2}};                  // ordered by Point's operator<=>
        CHECK_EQ(*points.begin(), Point{1, 2});
        CHECK_EQ(*points.rbegin(), Point{2, 1});
        // [/snippet]
    }
    {
        // [snippet:strict_weak]
        auto less_than = [](int a, int b) { return a < b; };
        auto less_equal = [](int a, int b) { return a <= b; };      // claims 2 < 2: NOT a valid comparator
        vector<int> sample{1, 2, 2, 3};
        CHECK(is_strict_weak_order(sample, less_than));
        CHECK(!is_strict_weak_order(sample, less_equal));

        // The classic multi-key bug: joining the keys with || instead of checking the first key for a tie.
        auto sloppy = [](const Player& a, const Player& b) {
            return a.score > b.score || a.name < b.name;   // dan(85) vs bob(70): each "comes before" the other
        };
        CHECK(sloppy(Player{"dan", 85}, Player{"bob", 70}));
        CHECK(sloppy(Player{"bob", 70}, Player{"dan", 85}));
        CHECK(is_strict_weak_order(roster, ranks_before));
        CHECK(!is_strict_weak_order(roster, sloppy));
        // sort(v.begin(), v.end(), less_equal) is undefined behaviour: it may run past the array's end.
        // [/snippet]
    }
    return t::summary("comparators");
}
