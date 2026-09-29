// Value vs reference semantics: what C++ copies where C# would share. Module 01 section 2.
// Every CHECK states a fact. Predict each one before you run:
//     make run F=modules/01-language/examples/value_vs_reference.cpp
#include <bits/stdc++.h>
#include "test.hpp"
#include "leetcode.hpp"
using namespace std;

// [snippet:params]
void add_by_value(vector<int> v) { v.push_back(4); }   // gets a private copy (O(n) to make)
void add_by_ref(vector<int>& v) { v.push_back(4); }    // works on the caller's vector
long long sum_by_const_ref(const vector<int>& v) {     // no copy, and changes won't compile
    long long total = 0;
    for (int x : v) total += x;
    return total;
}
// [/snippet]

// [snippet:return_by_value]
vector<int> squares(int n) {
    vector<int> result;
    for (int i = 1; i <= n; i++) result.push_back(i * i);
    return result;   // not a deep copy: the buffer is moved out (or built in place)
}
// [/snippet]

// squares() again, but it reports where its buffer lived, so main() can prove nothing was copied.
vector<int> squares_reporting(int n, const int*& buffer) {
    vector<int> result;
    for (int i = 1; i <= n; i++) result.push_back(i * i);
    buffer = result.data();
    return result;
}

// [snippet:raii]
struct Tracker {                 // counts how many Trackers are alive right now
    static inline int alive = 0;
    Tracker() { alive++; }
    ~Tracker() { alive--; }      // the destructor runs the moment the object's scope ends
};
// [/snippet]

int main() {
    {
        // [snippet:copy_on_assign]
        vector<int> a{1, 2, 3};
        vector<int> b = a;       // deep copy. In C#, `var b = a;` would share one List
        b[0] = 99;
        CHECK_EQ(a[0], 1);       // a is untouched
        CHECK_EQ(b[0], 99);
        // [/snippet]
    }
    {
        // [snippet:params_demo]
        vector<int> v{1, 2, 3};
        add_by_value(v);
        CHECK_EQ(v.size(), 3);   // the copy grew, then died with the function
        add_by_ref(v);
        CHECK_EQ(v.size(), 4);
        CHECK_EQ(sum_by_const_ref(v), 10);
        // [/snippet]
    }
    {
        // [snippet:auto_copy]
        vector<int> v{1, 2, 3};
        auto x = v[0];           // auto drops the reference: x is a copy
        x = 100;
        CHECK_EQ(x, 100);
        CHECK_EQ(v[0], 1);       // the copy changed, v didn't
        auto& y = v[0];          // auto& is an alias for v[0]
        y = 100;
        CHECK_EQ(v[0], 100);

        vector<vector<int>> grid{{1, 2}, {3, 4}};
        for (auto row : grid) row[0] = 0;    // row is a COPY of each row: grid unchanged
        CHECK_EQ(grid[0][0], 1);
        for (auto& row : grid) row[0] = 0;   // row aliases the real row
        CHECK_EQ(grid[0][0], 0);
        // [/snippet]
    }
    {
        // [snippet:string_mutable]
        string s = "cat";
        s[0] = 'b';              // std::string is mutable; System.String is not
        string t = s;            // an independent copy
        t += "s";
        CHECK_EQ(s, "bat");
        CHECK_EQ(t, "bats");
        // [/snippet]
    }
    {
        // [snippet:ref_vs_pointer]
        int x = 5;
        int& ref = x;            // alias: bound once at birth, never null, never re-bound
        int* ptr = &x;           // address: may be nullptr, may be re-pointed
        ref = 6;
        CHECK_EQ(x, 6);
        *ptr = 7;                // * follows the pointer
        CHECK_EQ(x, 7);
        int y = 0;
        ptr = &y;                // re-point: x is no longer affected
        *ptr = 8;
        CHECK_EQ(x, 7);
        CHECK_EQ(y, 8);
        // [/snippet]
    }
    {
        // [snippet:node_pointers]
        ListNode* head = make_list({1, 2, 3});
        ListNode* p = head;      // copies the address: both name the same node (like C# references)
        p->val = 10;
        CHECK_EQ(head->val, 10);
        ListNode copy = *head;   // copies the struct: val AND the next pointer (a shallow copy)
        copy.val = 20;
        copy.next->val = 30;     // ...so this reaches the real second node
        CHECK_EQ(head->val, 10);
        CHECK_EQ(head->next->val, 30);
        // [/snippet]
    }
    {
        // [snippet:move]
        vector<int> big(1'000'000, 7);
        const int* buffer = big.data();
        vector<int> stolen = std::move(big);   // O(1): takes over the buffer instead of copying it
        CHECK(stolen.data() == buffer);
        CHECK(big.empty());               // a moved-from vector is left empty
        vector<int> copied = stolen;      // O(n): a new buffer, every element copied
        CHECK(copied.data() != stolen.data());
        // [/snippet]
    }
    {
        const int* buffer = nullptr;
        vector<int> sq = squares_reporting(5, buffer);
        CHECK(sq.data() == buffer);       // the caller got the function's buffer: nothing was copied
        CHECK_EQ(sq, vector<int>{1, 4, 9, 16, 25});
        CHECK_EQ(squares(3), vector<int>{1, 4, 9});
        CHECK_EQ(sizeof(vector<int>), 3 * sizeof(int*));   // the object is 3 pointers; elements are on the heap
    }
    {
        // [snippet:raii_demo]
        {
            Tracker on_stack;
            auto on_heap = make_unique<Tracker>();   // owns a heap object, deletes it in its destructor
            vector<Tracker> three(3);
            CHECK_EQ(Tracker::alive, 5);
        }                                            // all five destroyed right here: no GC, no leak
        CHECK_EQ(Tracker::alive, 0);
        // [/snippet]
    }
    return t::summary("value_vs_reference");
}
