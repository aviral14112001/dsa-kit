// Custom comparators (module 05 section 3): multi-key order with tie(), sorting indices by another
// array, lambda captures, stable_sort for ties, priority_queue's inverted comparator, and a
// brute-force checker for the strict weak ordering rules.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

struct Employee {
    string dept;
    int salary;
    string name;
    bool operator==(const Employee&) const = default;
};
ostream& operator<<(ostream& os, const Employee& e) { return os << e.dept << "/" << e.salary << "/" << e.name; }

// [snippet:tie]
// Department ascending, then salary DESCENDING, then name ascending.
// tie(...) builds a tuple of references, and tuples compare field by field (lexicographically).
// To flip one field, take it from the other object: b.salary on the left, a.salary on the right.
bool by_dept_salary_name(const Employee& a, const Employee& b) {
    return tie(a.dept, b.salary, a.name) < tie(b.dept, a.salary, b.name);
}
// The same order spelled out: compare one field; only on a tie, move to the next.
bool by_dept_salary_name_manual(const Employee& a, const Employee& b) {
    if (a.dept != b.dept) return a.dept < b.dept;
    if (a.salary != b.salary) return a.salary > b.salary;
    return a.name < b.name;
}
// [/snippet]

// [snippet:argsort]
// The indices of a, in the order that sorts a (NumPy calls it argsort). You keep the original
// positions, and one order can drive several parallel arrays.
vector<int> argsort(const vector<int>& a) {
    vector<int> idx(a.size());
    iota(idx.begin(), idx.end(), 0);                           // 0, 1, ..., n-1
    stable_sort(idx.begin(), idx.end(), [&a](int i, int j) {   // capture a by reference: no copy
        return a[i] < a[j];                                    // equal values keep index order
    });
    return idx;
}
// [/snippet]

// [snippet:capture]
// Task names by priority (highest first), ties by name. [&priority] captures a reference, which
// costs nothing; [priority] or [=] would copy the whole map into the lambda, and std::sort is
// free to copy its comparator many times.
void sort_by_priority(vector<string>& tasks, const unordered_map<string, int>& priority) {
    sort(tasks.begin(), tasks.end(), [&priority](const string& a, const string& b) {
        int pa = priority.at(a), pb = priority.at(b);   // at(): the map is const, [] won't compile
        if (pa != pb) return pa > pb;
        return a < b;
    });
}
// [/snippet]

struct Person {
    string name;
    int age;
};

struct Task {
    string name;
    int deadline;
};

// [snippet:swo_check]
// Brute-force test of the strict weak ordering rules on sample values: O(n^3), for tests only.
template <class T, class Cmp>
bool is_strict_weak_ordering(const vector<T>& vals, Cmp cmp) {
    auto equiv = [&](const T& x, const T& y) { return !cmp(x, y) && !cmp(y, x); };
    for (const T& a : vals) {
        if (cmp(a, a)) return false;                                          // 1. irreflexive
        for (const T& b : vals) {
            if (cmp(a, b) && cmp(b, a)) return false;                         // 2. asymmetric
            for (const T& c : vals) {
                if (cmp(a, b) && cmp(b, c) && !cmp(a, c)) return false;       // 3. transitive
                if (equiv(a, b) && equiv(b, c) && !equiv(a, c)) return false; // 4. ties are transitive
            }
        }
    }
    return true;
}
// [/snippet]

int main() {
    // ---- multi-key order ----
    vector<Employee> staff{
        {"eng", 120, "ravi"}, {"hr", 90, "zoe"}, {"eng", 150, "anu"}, {"eng", 120, "dev"}, {"hr", 90, "amy"},
    };
    vector<Employee> expected{
        {"eng", 150, "anu"}, {"eng", 120, "dev"}, {"eng", 120, "ravi"}, {"hr", 90, "amy"}, {"hr", 90, "zoe"},
    };
    vector<Employee> a = staff, b = staff;
    sort(a.begin(), a.end(), by_dept_salary_name);
    sort(b.begin(), b.end(), by_dept_salary_name_manual);
    CHECK_EQ(a, expected);
    CHECK_EQ(b, expected);
    for (int iter = 0; iter < 200; iter++) {           // both comparators agree on random records
        Employee x{string(1, (char)t::rand_int('a', 'c')), (int)t::rand_int(1, 3), t::rand_string(2, 'a', 'b')};
        Employee y{string(1, (char)t::rand_int('a', 'c')), (int)t::rand_int(1, 3), t::rand_string(2, 'a', 'b')};
        CHECK_EQ(by_dept_salary_name(x, y), by_dept_salary_name_manual(x, y));
    }

    // ---- sorting indices by another array ----
    CHECK_EQ(argsort({30, 10, 20, 10}), vector<int>{1, 3, 2, 0});   // the two 10s keep index order
    CHECK_EQ(argsort({5, 5, 5}), vector<int>{0, 1, 2});
    CHECK_EQ(argsort({}), vector<int>{});
    vector<string> names{"cyan", "amber", "blue"};
    vector<int> heights{170, 185, 160};
    vector<string> by_height;
    for (int i : argsort(heights)) by_height.push_back(names[i]);    // one order drives a parallel array
    CHECK_EQ(by_height, vector<string>{"blue", "cyan", "amber"});

    // ---- lambda capture ----
    unordered_map<string, int> priority{{"deploy", 3}, {"email", 1}, {"fix", 3}, {"lunch", 2}};
    vector<string> tasks{"email", "lunch", "fix", "deploy"};
    sort_by_priority(tasks, priority);
    CHECK_EQ(tasks, vector<string>{"deploy", "fix", "lunch", "email"});

    // ---- stable_sort: two passes, secondary key first ----
    vector<Person> people{{"mia", 30}, {"ben", 25}, {"ali", 30}, {"cal", 25}, {"eve", 30}};
    auto by_name = [](const Person& x, const Person& y) { return x.name < y.name; };
    auto by_age = [](const Person& x, const Person& y) { return x.age < y.age; };
    // [snippet:two_pass]
    // Sort by the secondary key first, then STABLE-sort by the primary key: people with the same
    // age stay in name order. (LSD radix sort is this idea, one digit per pass.)
    sort(people.begin(), people.end(), by_name);          // secondary: name
    stable_sort(people.begin(), people.end(), by_age);    // primary: age; ties keep name order
    // [/snippet]
    vector<string> order;
    for (auto& p : people) order.push_back(p.name);
    CHECK_EQ(order, vector<string>{"ben", "cal", "ali", "eve", "mia"});

    // ---- priority_queue: the comparator is "inverted" ----
    // [snippet:pq]
    // priority_queue keeps on top the element that a sort with the same comparator puts LAST.
    priority_queue<int> max_heap;                               // less<int>: top() is the largest
    priority_queue<int, vector<int>, greater<int>> min_heap;    // greater<int>: top() is the smallest
    // So to pop the EARLIEST deadline first, the comparator must say "later deadline first":
    auto later_deadline = [](const Task& x, const Task& y) { return x.deadline > y.deadline; };
    priority_queue<Task, vector<Task>, decltype(later_deadline)> by_deadline(later_deadline);
    // [/snippet]
    for (int x : {4, 1, 7, 3}) {
        max_heap.push(x);
        min_heap.push(x);
    }
    CHECK_EQ(max_heap.top(), 7);
    CHECK_EQ(min_heap.top(), 1);
    for (Task tk : vector<Task>{{"report", 5}, {"taxes", 2}, {"slides", 9}, {"email", 1}}) by_deadline.push(tk);
    vector<string> popped;
    while (!by_deadline.empty()) {
        popped.push_back(by_deadline.top().name);
        by_deadline.pop();
    }
    CHECK_EQ(popped, vector<string>{"email", "taxes", "report", "slides"});

    // ---- which comparators are strict weak orderings? ----
    vector<int> sample{-3, -1, 0, 0, 1, 2, 2, 3, 5, 8};
    CHECK(is_strict_weak_ordering(sample, less<int>()));
    CHECK(is_strict_weak_ordering(sample, [](int x, int y) { return abs(x) < abs(y); }));    // any "compare by key" is fine
    CHECK(!is_strict_weak_ordering(sample, less_equal<int>()));                              // <= breaks rule 1
    CHECK(!is_strict_weak_ordering(sample, [](int x, int y) { return x + 1 < y; }));         // "fuzzy" <: breaks rule 4
    vector<pair<int, int>> points{{1, 2}, {2, 1}, {1, 1}, {2, 2}, {0, 3}};
    CHECK(!is_strict_weak_ordering(points, [](const pair<int, int>& p, const pair<int, int>& q) {
        return p.first < q.first || p.second < q.second;                                     // breaks rule 2
    }));
    CHECK(is_strict_weak_ordering(points, [](const pair<int, int>& p, const pair<int, int>& q) {
        return tie(p.first, p.second) < tie(q.first, q.second);
    }));
    return t::summary("sort_comparators");
}
