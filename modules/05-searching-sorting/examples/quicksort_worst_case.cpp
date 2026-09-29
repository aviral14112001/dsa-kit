// When is quicksort O(n^2)? Count the comparisons each variant makes on n = 2000 elements of
// random, sorted, reversed, all-equal and organ-pipe (0 1 2 ... 2 1 0) input. Module 05 section 1.
// The CHECKs pin down the exact quadratic counts; to print the whole table:
//     make test M=quicksort_worst_case
//     ./build/modules/05-searching-sorting/examples/quicksort_worst_case --table
#include <bits/stdc++.h>
#include "test.hpp"
#include "sorting.hpp"
using namespace std;

long long comparisons = 0;

// Lomuto with a random pivot: swap a random element into a[hi], then partition as usual.
template <class Cmp>
void quick_sort_lomuto_random(vector<int>& a, int lo, int hi, Cmp cmp) {
    if (lo >= hi) return;
    swap(a[random_index(lo, hi)], a[hi]);
    int p = lomuto_partition(a, lo, hi, cmp);
    quick_sort_lomuto_random(a, lo, p - 1, cmp);
    quick_sort_lomuto_random(a, p + 1, hi, cmp);
}

int main(int argc, char** argv) {
    bool print_table = argc > 1 && string(argv[1]) == "--table";
    auto counted_less = [](int x, int y) { comparisons++; return x < y; };
    using Sorter = function<void(vector<int>&)>;
    vector<pair<string, Sorter>> variants = {
        {"Lomuto, last pivot", [&](vector<int>& a) { quick_sort_lomuto(a, 0, (int)a.size() - 1, counted_less); }},
        {"Lomuto, random pivot", [&](vector<int>& a) { quick_sort_lomuto_random(a, 0, (int)a.size() - 1, counted_less); }},
        {"Hoare, middle pivot", [&](vector<int>& a) { quick_sort_hoare(a, 0, (int)a.size() - 1, counted_less); }},
        {"3-way, random pivot", [&](vector<int>& a) { quick_sort(a, counted_less); }},
        {"merge_sort", [&](vector<int>& a) { merge_sort(a, counted_less); }},
        {"heap_sort", [&](vector<int>& a) { heap_sort(a, counted_less); }},
        {"std::sort", [&](vector<int>& a) { sort(a.begin(), a.end(), counted_less); }},
    };

    const int n = 2000;
    vector<int> random_in = t::rand_vec(n, 0, 1'000'000'000), sorted_in(n), reversed_in(n), equal_in(n, 7), organ_pipe(n);
    iota(sorted_in.begin(), sorted_in.end(), 0);
    reversed_in.assign(sorted_in.rbegin(), sorted_in.rend());
    for (int i = 0; i < n; i++) organ_pipe[i] = min(i, n - 1 - i);
    vector<pair<string, vector<int>>> inputs = {
        {"random", random_in}, {"sorted", sorted_in}, {"reversed", reversed_in},
        {"all equal", equal_in}, {"organ pipe", organ_pipe},
    };

    // count[v][i] = comparisons made by variant v on input i
    vector<vector<long long>> count(variants.size(), vector<long long>(inputs.size()));
    for (size_t v = 0; v < variants.size(); v++)
        for (size_t i = 0; i < inputs.size(); i++) {
            vector<int> a = inputs[i].second;
            comparisons = 0;
            variants[v].second(a);
            count[v][i] = comparisons;
            CHECK(is_sorted(a.begin(), a.end()));
        }

    const long long quadratic = 1LL * n * (n - 1) / 2;     // 1,999,000: one element peeled per call
    const long long n_log_n = llround(n * log2(n));        // ~21,932
    enum { RANDOM, SORTED, REVERSED, EQUAL, ORGAN };
    enum { LOMUTO_LAST, LOMUTO_RANDOM, HOARE, THREE_WAY, MERGE, HEAP, STD };
    // Fixed last-element pivot: sorted, reversed and all-equal input are all worst cases.
    CHECK_EQ(count[LOMUTO_LAST][SORTED], quadratic);
    CHECK_EQ(count[LOMUTO_LAST][REVERSED], quadratic);
    CHECK_EQ(count[LOMUTO_LAST][EQUAL], quadratic);
    // A random pivot rescues sorted input, but not all-equal input: every pivot "is" the value 7,
    // and Lomuto sends everything equal to it to one side.
    CHECK(count[LOMUTO_RANDOM][SORTED] < 3 * n_log_n);
    CHECK_EQ(count[LOMUTO_RANDOM][EQUAL], quadratic);
    // Hoare stops on equal keys from both sides, so all-equal input splits evenly.
    CHECK(count[HOARE][EQUAL] < 2 * n_log_n);
    CHECK(count[HOARE][SORTED] < 2 * n_log_n);
    // Hoare's middle pivot is still a FIXED rule: organ-pipe input costs it several times more.
    CHECK(count[HOARE][ORGAN] > 3 * count[HOARE][SORTED]);
    // 3-way: all-equal input is a single pass, two comparisons per element.
    CHECK_EQ(count[THREE_WAY][EQUAL], 2LL * n);
    for (int i : {RANDOM, SORTED, REVERSED, ORGAN}) CHECK(count[THREE_WAY][i] < 4 * n_log_n);
    for (int i : {RANDOM, SORTED, REVERSED, EQUAL, ORGAN}) {
        CHECK(count[MERGE][i] <= n_log_n);                 // at most n*ceil(log2 n) - 2^ceil(log2 n) + 1
        CHECK(count[HEAP][i] <= 2 * n_log_n + 2 * n);      // two comparisons per level of each sift
        CHECK(count[STD][i] < 3 * n_log_n);                // introsort: O(n log n) guaranteed
    }

    if (print_table) {
        printf("comparisons for n = %d (n log2 n = %lld, n(n-1)/2 = %lld)\n\n", n, n_log_n, quadratic);
        printf("| %-22s |", "variant");
        for (auto& in : inputs) printf(" %10s |", in.first.c_str());
        printf("\n");
        for (size_t v = 0; v < variants.size(); v++) {
            printf("| %-22s |", variants[v].first.c_str());
            for (size_t i = 0; i < inputs.size(); i++) printf(" %10lld |", count[v][i]);
            printf("\n");
        }
    }
    return t::summary("quicksort_worst_case");
}
