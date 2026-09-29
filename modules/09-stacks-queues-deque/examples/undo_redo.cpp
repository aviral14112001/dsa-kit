// Module 09 section 1: undo/redo with two stacks of commands (the Command pattern).
// Checked against a snapshot-history model that stores every version of the text.
#include <bits/stdc++.h>
#include "test.hpp"
using namespace std;

// [snippet:undo_redo]
// A text buffer with two edit commands. Each command stores what it needs to reverse itself.
//   new edit: apply it, push it on `undos`, and CLEAR `redos` (the undone future is gone)
//   undo:     pop from `undos`, reverse it, push it on `redos`
//   redo:     pop from `redos`, apply it again, push it on `undos`
class Editor {
public:
    const string& text() const { return doc; }

    void type(const string& s) { run({Cmd::Type, s}); }
    void backspace(int k) {  // delete the last k characters (all of them if there are fewer)
        k = min(k, (int)doc.size());
        run({Cmd::Erase, doc.substr(doc.size() - k)});  // remember the erased text for undo
    }
    bool undo() { return move_top(undos, redos, /*forward=*/false); }
    bool redo() { return move_top(redos, undos, /*forward=*/true); }

private:
    struct Cmd {
        enum Kind { Type, Erase } kind;
        string text;  // typed text, or the text that was erased
    };
    string doc;
    stack<Cmd> undos, redos;

    void apply(const Cmd& c, bool forward) {
        bool adds = (c.kind == Cmd::Type) == forward;  // typing forward, or undoing an erase
        if (adds) doc += c.text;
        else doc.erase(doc.size() - c.text.size());
    }
    void run(const Cmd& c) {
        apply(c, true);
        undos.push(c);
        redos = {};                                    // std::stack has no clear()
    }
    bool move_top(stack<Cmd>& from, stack<Cmd>& to, bool forward) {
        if (from.empty()) return false;                // nothing to undo / redo
        Cmd c = from.top();                            // top() then pop(): pop() returns void
        from.pop();
        apply(c, forward);
        to.push(c);
        return true;
    }
};
// [/snippet]

// Reference model: keep every version of the text and a cursor into that history.
struct SnapshotEditor {
    vector<string> versions{""};
    size_t cur = 0;
    void edit(const string& next) {
        versions.resize(cur + 1);  // a new edit discards the redo future
        versions.push_back(next);
        cur++;
    }
    void type(const string& s) { edit(versions[cur] + s); }
    void backspace(int k) {
        const string& s = versions[cur];
        edit(s.substr(0, s.size() - min<size_t>(k, s.size())));
    }
    bool undo() { return cur > 0 ? (cur--, true) : false; }
    bool redo() { return cur + 1 < versions.size() ? (cur++, true) : false; }
    const string& text() const { return versions[cur]; }
};

int main() {
    Editor e;
    e.type("hello");
    e.type(" world");
    CHECK_EQ(e.text(), "hello world");
    CHECK(e.undo());
    CHECK_EQ(e.text(), "hello");
    CHECK(e.redo());
    CHECK_EQ(e.text(), "hello world");
    e.backspace(6);
    CHECK_EQ(e.text(), "hello");
    CHECK(e.undo());                      // the erased " world" comes back
    CHECK_EQ(e.text(), "hello world");
    CHECK(e.undo());
    CHECK(e.undo());
    CHECK_EQ(e.text(), "");
    CHECK(!e.undo());                     // nothing left to undo
    CHECK(e.redo());
    CHECK_EQ(e.text(), "hello");
    e.type("!");                          // a new edit after undo...
    CHECK(!e.redo());                     // ...wipes the redo stack
    CHECK_EQ(e.text(), "hello!");
    e.backspace(100);                     // more than the length: erase everything
    CHECK_EQ(e.text(), "");
    CHECK(e.undo());
    CHECK_EQ(e.text(), "hello!");

    // stress: random edits, undos and redos vs the snapshot model
    for (int iter = 0; iter < 300; iter++) {
        Editor ed;
        SnapshotEditor ref;
        for (int step = 0; step < 30; step++) {
            int op = (int)t::rand_int(0, 3);
            if (op == 0) {
                string s = t::rand_string((int)t::rand_int(0, 3), 'a', 'c');
                ed.type(s);
                ref.type(s);
            } else if (op == 1) {
                int k = (int)t::rand_int(0, 4);
                ed.backspace(k);
                ref.backspace(k);
            } else if (op == 2) {
                CHECK_EQ(ed.undo(), ref.undo());
            } else {
                CHECK_EQ(ed.redo(), ref.redo());
            }
            CHECK_EQ(ed.text(), ref.text());
        }
    }
    return t::summary("undo_redo");
}
