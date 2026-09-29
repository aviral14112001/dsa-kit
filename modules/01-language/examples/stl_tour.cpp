// A guided tour of the standard library pieces DSA rounds use, with the C# equivalent in the comments.
// Module 01 section 3. Read each CHECK as "this is what you get".
//     make run F=modules/01-language/examples/stl_tour.cpp
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

int main() {
    {
        // [snippet:vector]
        vector<int> v{5, 1, 4};
        v.push_back(2);                   // amortized O(1), like List<T>.Add
        v.emplace_back(3);                // same, but constructs the element in place
        CHECK_EQ(v.back(), 3);
        v.pop_back();                     // O(1), and returns nothing
        v.insert(v.begin(), 9);           // O(n): shifts everything right
        v.erase(v.begin() + 1);           // O(n): shifts everything left
        CHECK_EQ(v, vector<int>{9, 1, 4, 2});

        vector<int> zeros(5);             // FIVE ZEROS. C#'s new List<int>(5) is empty with capacity 5
        vector<int> sevens(3, 7);         // {7, 7, 7}
        vector<int> room;
        room.reserve(5);                  // this is the C# constructor's meaning: capacity, size 0
        CHECK_EQ(zeros.size(), 5);
        CHECK_EQ(sevens, vector<int>{7, 7, 7});
        CHECK(room.empty());
        vector<vector<int>> grid(2, vector<int>(3, -1));   // 2 rows x 3 columns, all -1
        CHECK_EQ(grid[1][2], -1);
        // [/snippet]
    }
    {
        // [snippet:unordered_map]
        unordered_map<string, int> freq;                    // Dictionary<string, int>
        vector<string> words{"to", "be", "or", "not", "to", "be"};
        for (const string& w : words) freq[w]++;            // [] creates a missing key with value 0
        CHECK_EQ(freq["to"], 2);
        CHECK_EQ(freq.size(), 4);
        CHECK(freq.contains("or"));                         // C++20; count(key) == 1 works everywhere
        CHECK(freq.find("xyz") == freq.end());              // find() never inserts
        int missing = freq["xyz"];                          // [] DOES insert, even when you only read
        CHECK_EQ(missing, 0);
        CHECK_EQ(freq.size(), 5);                           // "xyz" is a key now
        if (auto it = freq.find("not"); it != freq.end()) it->second += 10;   // look up once, update
        CHECK_EQ(freq.at("not"), 11);                       // at() throws on a missing key, like C#'s indexer
        freq.erase("xyz");
        CHECK_EQ(freq.size(), 4);
        // [/snippet]
    }
    {
        // [snippet:map]
        map<int, string> grade{{70, "C"}, {90, "A"}, {80, "B"}};   // SortedDictionary: a balanced tree
        CHECK_EQ(grade.begin()->first, 70);                    // smallest key
        CHECK_EQ(grade.rbegin()->first, 90);                   // largest key
        CHECK_EQ(grade.lower_bound(75)->first, 80);            // first key >= 75, O(log n)
        CHECK_EQ(grade.upper_bound(80)->first, 90);            // first key > 80
        CHECK(grade.upper_bound(90) == grade.end());           // none
        CHECK_EQ(prev(grade.end())->second, "A");              // the last element
        vector<int> keys;
        for (const auto& [score, letter] : grade) keys.push_back(score);   // iterates in key order
        CHECK_EQ(keys, vector<int>{70, 80, 90});
        // [/snippet]
    }
    {
        // [snippet:sets]
        set<int> s{5, 1, 3};                   // SortedSet<int>
        s.insert(3);                           // already there: no change
        CHECK_EQ(s.size(), 3);
        CHECK_EQ(*s.begin(), 1);
        CHECK_EQ(*s.lower_bound(2), 3);        // the MEMBER lower_bound: O(log n)
        // std::lower_bound(s.begin(), s.end(), 2) compiles too, but is O(n): set iterators can't jump

        multiset<int> bag{2, 2, 2, 7};         // a sorted bag: duplicates allowed
        bag.erase(bag.find(2));                // erase ONE copy (find() must not be end(): check first)
        CHECK_EQ(bag.count(2), 2);
        bag.erase(2);                          // erase(value) removes EVERY copy
        CHECK_EQ(bag.count(2), 0);

        unordered_set<int> seen{4, 8};         // HashSet<int>
        CHECK(seen.contains(4));
        CHECK(!seen.contains(5));
        // [/snippet]
    }
    {
        // [snippet:priority_queue]
        priority_queue<int> max_heap;                              // MAX-heap by default
        for (int x : {3, 1, 4, 1, 5}) max_heap.push(x);            // O(log n) each
        CHECK_EQ(max_heap.top(), 5);
        max_heap.pop();                                            // returns void: read top() first
        CHECK_EQ(max_heap.top(), 4);

        priority_queue<int, vector<int>, greater<int>> min_heap;   // min-heap, like C#'s PriorityQueue
        for (int x : {3, 1, 4}) min_heap.push(x);
        CHECK_EQ(min_heap.top(), 1);

        // (distance, node): smallest distance first, ties by node. The Dijkstra shape.
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.push({5, 0});
        pq.push({2, 1});
        pq.push({2, 0});
        CHECK_EQ(pq.top(), pair<int, int>{2, 0});
        // [/snippet]
    }
    {
        // [snippet:queue_stack_deque]
        queue<int> q;                 // Queue<T>: Enqueue -> push, Peek -> front, Dequeue -> front + pop
        q.push(1);
        q.push(2);
        CHECK_EQ(q.front(), 1);
        q.pop();                      // void, unlike Dequeue()
        CHECK_EQ(q.front(), 2);

        stack<int> st;                // Stack<T>: Push -> push, Peek -> top, Pop -> top + pop
        st.push(1);
        st.push(2);
        CHECK_EQ(st.top(), 2);
        st.pop();
        CHECK_EQ(st.top(), 1);

        deque<int> dq{2, 3};          // O(1) at both ends, plus O(1) indexing
        dq.push_front(1);
        dq.push_back(4);
        CHECK_EQ(dq.front(), 1);
        CHECK_EQ(dq[3], 4);
        dq.pop_front();
        CHECK_EQ(dq.front(), 2);
        // [/snippet]
    }
    {
        // [snippet:sort_search]
        vector<int> a{5, 2, 8, 2, 9, 1};
        sort(a.begin(), a.end());                                    // O(n log n), not stable
        CHECK_EQ(a, vector<int>{1, 2, 2, 5, 8, 9});
        CHECK_EQ(lower_bound(a.begin(), a.end(), 2) - a.begin(), 1);   // first >= 2
        CHECK_EQ(upper_bound(a.begin(), a.end(), 2) - a.begin(), 3);   // first > 2
        CHECK_EQ(lower_bound(a.begin(), a.end(), 7) - a.begin(), 4);   // absent: where 7 would go
        auto [first, last] = equal_range(a.begin(), a.end(), 2);      // both bounds at once
        CHECK_EQ(last - first, 2);                                     // = how many 2s
        CHECK(binary_search(a.begin(), a.end(), 8));                   // yes/no only
        sort(a.begin(), a.end(), greater<int>());                      // descending
        CHECK_EQ(a, vector<int>{9, 8, 5, 2, 2, 1});
        vector<int> b{3, 1, 2};
        sort(b.rbegin(), b.rend());                // ALSO descending: sorts the reversed view ascending
        CHECK_EQ(b, vector<int>{3, 2, 1});
        // [/snippet]
    }
    {
        // [snippet:algorithms]
        vector<int> v{3, 1, 4, 1, 5, 9, 2, 6};
        CHECK_EQ(accumulate(v.begin(), v.end(), 0LL), 31);
        CHECK_EQ(*max_element(v.begin(), v.end()), 9);
        CHECK_EQ(min_element(v.begin(), v.end()) - v.begin(), 1);   // index of the FIRST minimum
        CHECK_EQ(count(v.begin(), v.end(), 1), 2);                   // O(n)
        CHECK(find(v.begin(), v.end(), 7) == v.end());               // O(n); end() means "not found"
        reverse(v.begin(), v.end());
        CHECK_EQ(v, vector<int>{6, 2, 9, 5, 1, 4, 1, 3});

        vector<int> idx(5);
        iota(idx.begin(), idx.end(), 0);                             // 0, 1, 2, 3, 4
        rotate(idx.begin(), idx.begin() + 2, idx.end());             // idx[2] becomes the first element
        CHECK_EQ(idx, vector<int>{2, 3, 4, 0, 1});
        fill(idx.begin(), idx.end(), -1);
        CHECK_EQ(idx, vector<int>(5, -1));

        vector<int> d{3, 1, 3, 2, 1};
        sort(d.begin(), d.end());                                    // unique() only drops ADJACENT repeats,
        d.erase(unique(d.begin(), d.end()), d.end());                // so sort first, then erase the tail
        CHECK_EQ(d, vector<int>{1, 2, 3});

        CHECK_EQ(popcount(13u), 3);                    // 13 = 0b1101. C++20 <bit>; takes unsigned types
        CHECK_EQ(__builtin_popcount(13), 3);           // GCC/Clang builtin, any standard
        CHECK_EQ(__builtin_popcountll(1LL << 40), 1);  // the long long version
        CHECK_EQ(gcd(12, 18), 6);
        CHECK_EQ(lcm(4, 6), 12);
        // [/snippet]
    }
    {
        // [snippet:permutations_selection]
        vector<int> p{1, 2, 3};                          // start sorted to visit all n! orders
        vector<vector<int>> all;
        do {
            all.push_back(p);
        } while (next_permutation(p.begin(), p.end()));  // false after the last (descending) order
        CHECK_EQ(all.size(), 6);
        CHECK_EQ(all[1], vector<int>{1, 3, 2});

        vector<int> w{7, 2, 9, 4, 1, 8};
        nth_element(w.begin(), w.begin() + 2, w.end());  // w[2] = 3rd smallest, smaller ones before it
        CHECK_EQ(w[2], 4);                               // O(n) on average; the rest stays unordered

        vector<int> z{7, 2, 9, 4, 1, 8};
        partial_sort(z.begin(), z.begin() + 3, z.end()); // the 3 smallest, sorted, in front: O(n log k)
        CHECK_EQ(vector<int>(z.begin(), z.begin() + 3), vector<int>{1, 2, 4});
        // [/snippet]
    }
    {
        // [snippet:iterators]
        vector<int> v{10, 20, 30, 40};
        auto it = find(v.begin(), v.end(), 30);
        CHECK_EQ(it - v.begin(), 2);                  // iterator -> index (random-access iterators only)
        CHECK_EQ(*prev(it), 20);
        CHECK_EQ(*next(it), 40);
        CHECK_EQ(distance(v.begin(), it), 2);         // any iterator; O(n) on list/set/map

        set<int> s{1, 5, 9};
        auto sit = s.find(5);
        CHECK_EQ(*next(sit), 9);                      // tree iterators step one at a time
        CHECK_EQ(*prev(sit), 1);

        vector<int> nums{1, 2, 3, 4, 5, 6};
        erase_if(nums, [](int x) { return x % 2 == 0; });   // C++20: remove while "iterating", safely
        CHECK_EQ(nums, vector<int>{1, 3, 5});

        map<string, int> stock{{"apple", 0}, {"kiwi", 3}, {"pear", 0}};
        for (auto m = stock.begin(); m != stock.end();) {
            if (m->second == 0) m = stock.erase(m);   // erase returns the next valid iterator
            else ++m;
        }
        CHECK_EQ(stock.size(), 1);
        // [/snippet]
    }
    {
        // [snippet:strings]
        string s = "hello world";
        CHECK_EQ(s.substr(6), "world");               // substr(pos, len) COPIES: O(len)
        CHECK_EQ(s.substr(0, 5), "hello");
        CHECK_EQ(s.find('o'), 4);                     // first match
        CHECK_EQ(s.find('o', 5), 7);                  // search from index 5 on
        CHECK(s.find("xyz") == string::npos);         // not found is npos, never -1
        string built;
        for (int i = 0; i < 3; i++) built += to_string(i);   // += is amortized O(1) per char
        CHECK_EQ(built, "012");
        CHECK_EQ(stoi("-42"), -42);
        CHECK_EQ(stoll("9000000000"), 9'000'000'000LL);      // stoi would throw out_of_range
        CHECK_EQ(string(3, 'z'), "zzz");
        reverse(s.begin(), s.end());                  // a string is a container of chars
        CHECK_EQ(s, "dlrow olleh");
        CHECK(string("apple") < string("banana"));    // lexicographic <, ==, compare()
        ostringstream out;                            // the other StringBuilder: format mixed types
        out << "x=" << 5 << ", y=" << 2.5;
        CHECK_EQ(out.str(), "x=5, y=2.5");
        // [/snippet]
    }
    {
        // [snippet:split]
        string csv = "alice,bob,,carol";
        vector<string> fields;
        stringstream ss(csv);
        string field;
        while (getline(ss, field, ',')) fields.push_back(field);   // split on ','; keeps empty fields
        CHECK_EQ(fields, vector<string>{"alice", "bob", "", "carol"});

        stringstream text("  split   on   spaces ");
        vector<string> tokens;
        string token;
        while (text >> token) tokens.push_back(token);             // >> skips any run of whitespace
        CHECK_EQ(tokens, vector<string>{"split", "on", "spaces"});
        // [/snippet]
    }
    {
        // [snippet:pairs]
        pair<int, string> p{2, "b"};                  // ValueTuple<int, string>
        auto [num, name] = p;                         // structured binding: C#'s var (num, name) = p;
        CHECK_EQ(num, 2);
        CHECK_EQ(name, "b");
        vector<pair<int, string>> v{{2, "b"}, {1, "z"}, {2, "a"}};
        sort(v.begin(), v.end());                     // pairs compare .first, then .second
        CHECK_EQ(v[0], pair<int, string>{1, "z"});
        CHECK_EQ(v[1], pair<int, string>{2, "a"});
        tuple<int, char, double> t3{3, 'x', 1.5};
        auto [i, c, d] = t3;
        CHECK_EQ(i, 3);
        CHECK_EQ(c, 'x');
        CHECK_EQ(get<2>(t3), d);
        // [/snippet]
    }
    return t::summary("stl_tour");
}
