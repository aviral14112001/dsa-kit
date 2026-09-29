// Public API of the file index. The tests compile against this, so keep the public part as it is.
// Everything under `private:` is yours to redesign (unique_ptr children, a node pool, a treap
// priority, an AVL height...).
#pragma once
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class FileIndex {
public:
    FileIndex();
    ~FileIndex();
    FileIndex(const FileIndex&) = delete;             // owns raw nodes: forbid accidental copies
    FileIndex& operator=(const FileIndex&) = delete;

    // Milestone 1: BST core
    bool insert(const std::string& path);              // false if the path is already stored
    bool erase(const std::string& path);               // false if the path isn't stored
    bool contains(const std::string& path) const;
    std::size_t size() const;
    std::vector<std::string> all() const;              // every path, sorted

    // Milestone 2: order statistics + autocomplete (subtree sizes make these O(h))
    std::size_t rank(const std::string& path) const;                        // # stored paths < path
    std::optional<std::string> kth(std::size_t k) const;                    // 0-based; nullopt if k >= size()
    std::vector<std::string> complete(const std::string& prefix, std::size_t k) const;  // first k with prefix
    std::size_t count_prefix(const std::string& prefix) const;
    std::vector<std::string> range(const std::string& lo, const std::string& hi) const; // paths in [lo, hi)
    bool check_invariants() const;                     // BST order + subtree sizes (+ balance, once added)

private:
    struct Node {
        std::string key;
        std::size_t size = 1;                          // nodes in this subtree
        Node* left = nullptr;
        Node* right = nullptr;
    };
    Node* root_ = nullptr;
};
