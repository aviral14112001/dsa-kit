// Tiny zero-dependency test helpers used by every examples/*.cpp and templates/tests/*.cpp.
//
//   CHECK(cond)                 passes if cond is true
//   CHECK_EQ(actual, expected)  prints both values on failure (vectors, pairs, maps... all print)
//   CHECK_NEAR(a, b, eps)       for doubles
//   return t::summary("name");  last line of main(): prints ok/FAIL, returns the exit code
//
// Put the call under test FIRST and the expected value SECOND. The expected value may contain
// top-level commas, so `CHECK_EQ(sol.f(v), vector<int>{1, 2, 3})` works without extra parens.
// Stress tests: t::rand_int / t::rand_vec / t::rand_string use a fixed seed, so a failing
// random case reproduces on every run.
#pragma once
#include <bits/stdc++.h>

namespace t {

namespace detail {
template <class T> concept Streamable = requires(std::ostream& os, const T& v) { os << v; };
template <class T> concept Range = requires(const T& v) { std::begin(v); std::end(v); };
template <class T> struct is_pair : std::false_type {};
template <class A, class B> struct is_pair<std::pair<A, B>> : std::true_type {};
template <class T> struct is_tuple : std::false_type {};
template <class... A> struct is_tuple<std::tuple<A...>> : std::true_type {};
template <class T> struct is_optional : std::false_type {};
template <class T> struct is_optional<std::optional<T>> : std::true_type {};
template <class T> concept CharLike =
    std::is_same_v<T, char> || std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char> ||
    std::is_same_v<T, wchar_t> || std::is_same_v<T, char8_t> || std::is_same_v<T, char16_t> ||
    std::is_same_v<T, char32_t>;
// std::cmp_equal accepts only "real" integers: no bool, no character types.
template <class T> concept PlainInt = std::is_integral_v<T> && !std::is_same_v<T, bool> && !CharLike<T>;
}  // namespace detail

// Render any value readably: "str", 'c', true, (a, b), [1, 2, 3], nested containers, maps...
template <class T> std::string show(const T& v) {
    using U = std::decay_t<T>;
    std::ostringstream os;
    if constexpr (std::is_same_v<U, std::string> || std::is_same_v<U, std::string_view> ||
                  std::is_same_v<U, const char*> || std::is_same_v<U, char*>) {
        os << '"' << v << '"';
    } else if constexpr (std::is_same_v<U, char>) {
        os << '\'' << v << '\'';
    } else if constexpr (std::is_same_v<U, bool>) {
        os << (v ? "true" : "false");
    } else if constexpr (detail::is_pair<U>::value) {
        os << '(' << show(v.first) << ", " << show(v.second) << ')';
    } else if constexpr (detail::is_tuple<U>::value) {
        os << '(';
        std::apply([&](const auto&... xs) { size_t i = 0; ((os << (i++ ? ", " : "") << show(xs)), ...); }, v);
        os << ')';
    } else if constexpr (detail::is_optional<U>::value) {
        if (v) os << show(*v); else os << "nullopt";
    } else if constexpr (detail::Range<U> && !detail::Streamable<U>) {
        os << '[';
        bool first = true;
        for (const auto& x : v) { os << (first ? "" : ", ") << show(x); first = false; }
        os << ']';
    } else if constexpr (detail::Streamable<U>) {
        os << v;
    } else {
        os << "<unprintable>";
    }
    return os.str();
}

inline int checks = 0, failures = 0;

inline void report(bool ok, const std::string& expr, const std::string& detail, const char* file, int line) {
    ++checks;
    if (ok) return;
    ++failures;
    std::cerr << file << ":" << line << ": FAIL: " << expr << "\n";
    if (!detail.empty()) std::cerr << "    " << detail << "\n";
}

template <class A, class B>
void check_eq(const A& a, const B& b, const char* ea, const char* eb, const char* file, int line) {
    bool ok;
    if constexpr (detail::PlainInt<A> && detail::PlainInt<B>) ok = std::cmp_equal(a, b);  // no signed/unsigned traps
    else ok = (a == b);
    report(ok, std::string(ea) + " == " + eb, ok ? "" : "got " + show(a) + ", expected " + show(b), file, line);
}

inline void check_near(double a, double b, double eps, const char* ea, const char* eb, const char* file, int line) {
    bool ok = std::fabs(a - b) <= eps;
    std::ostringstream d;
    if (!ok) d << std::setprecision(12) << "got " << a << ", expected " << b << " (eps " << eps << ")";
    report(ok, std::string(ea) + " ~= " + eb, d.str(), file, line);
}

// Call as the last statement of main(). Exit code 0 = all passed.
inline int summary(const char* name = "") {
    if (failures) std::cerr << "FAIL " << name << ": " << failures << "/" << checks << " checks failed\n";
    else std::cout << "ok   " << name << " (" << checks << " checks)\n";
    return failures ? 1 : 0;
}

// ---------- deterministic randomness for stress tests ----------
inline std::mt19937_64& rng() { static std::mt19937_64 gen(20260928); return gen; }
inline long long rand_int(long long lo, long long hi) { return std::uniform_int_distribution<long long>(lo, hi)(rng()); }
inline std::vector<int> rand_vec(int n, int lo, int hi) {
    std::vector<int> v(n);
    for (auto& x : v) x = (int)rand_int(lo, hi);
    return v;
}
inline std::string rand_string(int n, char lo = 'a', char hi = 'z') {
    std::string s(n, lo);
    for (auto& c : s) c = (char)rand_int(lo, hi);
    return s;
}

}  // namespace t

#define CHECK(cond) t::report(static_cast<bool>(cond), #cond, "", __FILE__, __LINE__)
#define CHECK_EQ(actual, ...) t::check_eq((actual), (__VA_ARGS__), #actual, #__VA_ARGS__, __FILE__, __LINE__)
#define CHECK_NEAR(a, b, eps) t::check_near((a), (b), (eps), #a, #b, __FILE__, __LINE__)
