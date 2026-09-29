// Your implementation. Every method starts as a stub that throws, so the tests are red until you
// write it. Keep the tree yours: no std::set / std::map in here (std::vector and std::string are fine).
#include "file_index.hpp"

#include <stdexcept>

namespace {
[[noreturn]] void todo(const char* what) { throw std::logic_error(std::string("not implemented: ") + what); }
}  // namespace

FileIndex::FileIndex() = default;

FileIndex::~FileIndex() {
    // TODO(milestone 1): free every node. Recursion is fine once the tree is balanced; a degenerate
    // tree of 100k nodes can overflow the stack, so an explicit stack is the safer choice.
    (void)root_;  // silences "unused field" until your code uses root_; delete this line then
}

bool FileIndex::insert(const std::string&) { todo("insert"); }
bool FileIndex::erase(const std::string&) { todo("erase"); }
bool FileIndex::contains(const std::string&) const { todo("contains"); }
std::size_t FileIndex::size() const { todo("size"); }
std::vector<std::string> FileIndex::all() const { todo("all"); }

std::size_t FileIndex::rank(const std::string&) const { todo("rank"); }
std::optional<std::string> FileIndex::kth(std::size_t) const { todo("kth"); }
std::vector<std::string> FileIndex::complete(const std::string&, std::size_t) const { todo("complete"); }
std::size_t FileIndex::count_prefix(const std::string&) const { todo("count_prefix"); }
std::vector<std::string> FileIndex::range(const std::string&, const std::string&) const { todo("range"); }
bool FileIndex::check_invariants() const { todo("check_invariants"); }
