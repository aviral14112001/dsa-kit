// Milestone 3: a small REPL over FileIndex.   make cli && ./build/file-index
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

#include "file_index.hpp"

namespace fs = std::filesystem;

static void print_all(const std::vector<std::string>& paths) {
    for (const auto& p : paths) std::cout << "  " << p << '\n';
    std::cout << "(" << paths.size() << ")\n";
}

int main() {
    FileIndex idx;
    std::cout << "commands: load <dir> | add <path> | rm <path> | has <path> | complete <prefix> [k]\n"
                 "          count <prefix> | rank <path> | kth <k> | range <lo> <hi> | size | quit\n";
    std::string line;
    while (std::cout << "> " << std::flush && std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string cmd, a, b;
        in >> cmd >> a >> b;
        try {
            if (cmd.empty()) continue;
            if (cmd == "quit") break;
            if (cmd == "load") {
                // TODO(milestone 3): walk `a` with fs::recursive_directory_iterator (pass
                // fs::directory_options::skip_permission_denied), skip directories and anything under
                // .git / node_modules, and insert each file's path relative to `a` (fs::relative).
                // Print how many paths were added and how long it took.
                std::cout << "load: not implemented yet (see the TODO in src/main.cpp)\n";
            } else if (cmd == "add") {
                std::cout << (idx.insert(a) ? "added\n" : "already there\n");
            } else if (cmd == "rm") {
                std::cout << (idx.erase(a) ? "removed\n" : "not found\n");
            } else if (cmd == "has") {
                std::cout << (idx.contains(a) ? "yes\n" : "no\n");
            } else if (cmd == "complete") {
                print_all(idx.complete(a, b.empty() ? 10 : std::stoul(b)));
            } else if (cmd == "count") {
                std::cout << idx.count_prefix(a) << '\n';
            } else if (cmd == "rank") {
                std::cout << idx.rank(a) << '\n';
            } else if (cmd == "kth") {
                auto p = idx.kth(std::stoul(a));
                std::cout << (p ? *p : std::string("(out of range)")) << '\n';
            } else if (cmd == "range") {
                print_all(idx.range(a, b));
            } else if (cmd == "size") {
                std::cout << idx.size() << '\n';
            } else {
                std::cout << "unknown command\n";
            }
        } catch (const std::exception& e) {
            std::cout << "error: " << e.what() << '\n';
        }
    }
}
