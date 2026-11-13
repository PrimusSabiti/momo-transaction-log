// SinglyList.h - Task A1: primary data structure for the Mobile Money Agent Transaction Log.
// A singly linked list that keeps BOTH a head and a tail pointer, so adding at the end is O(1).
#ifndef SINGLY_LIST_H
#define SINGLY_LIST_H

#include <cstddef>
#include <utility>

template <typename T>
class SinglyList {
    struct Node {
        T value;
        Node* next;
    };

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t count_ = 0;

    // Walk to the node at position i (caller has already checked i < count_).
    Node* nodeAt(std::size_t i) const {
        Node* cur = head_;
        while (i-- > 0) cur = cur->next;
        return cur;
    }

    // Unlink and delete 'target', whose predecessor is 'before' (nullptr when target is the head).
    void detach(Node* before, Node* target, T* out) {
        if (out) *out = target->value;
        if (before == nullptr) head_ = target->next;   // case 1: removing the first node
        else before->next = target->next;              // bypass the node
        if (target == tail_) tail_ = before;           // case 2: removing the last node (or the only one)
        delete target;
        --count_;
    }

public:
    SinglyList() = default;
    ~SinglyList() { clear(); }

    // Deep copy: a brand-new chain, so two lists never share a node (no double delete).
    SinglyList(const SinglyList& other) {
        for (Node* n = other.head_; n != nullptr; n = n->next) pushBack(n->value);
    }

    // Copy-and-swap: safe for self-assignment and leaks nothing.
    SinglyList& operator=(const SinglyList& other) {
        SinglyList copy(other);
        swap(copy);
        return *this;
    }

    void swap(SinglyList& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(count_, other.count_);
    }

    // ---------------- adding ----------------
    void pushBack(const T& v) {                        // O(1) thanks to tail_
        Node* n = new Node{v, nullptr};
        if (tail_ == nullptr) head_ = n;               // list was empty
        else tail_->next = n;
        tail_ = n;
        ++count_;
    }

    void pushFront(const T& v) {                       // O(1)
        Node* n = new Node{v, head_};
        head_ = n;
        if (tail_ == nullptr) tail_ = n;               // list was empty
        ++count_;
    }

    // Insert so that the new record ends up at position i. Valid i: 0..size(). O(n).
    bool insertAt(std::size_t i, const T& v) {
        if (i > count_) return false;
        if (i == 0) { pushFront(v); return true; }
        if (i == count_) { pushBack(v); return true; }
        Node* before = nodeAt(i - 1);
        before->next = new Node{v, before->next};
        ++count_;
        return true;
    }

    // ---------------- removing ----------------
    bool popFront(T& out) {                            // O(1); false on an empty list
        if (head_ == nullptr) return false;
        detach(nullptr, head_, &out);
        return true;
    }

    bool removeAt(std::size_t i, T* removed = nullptr) {   // O(n); false if i is invalid
        if (i >= count_) return false;
        Node* before = (i == 0) ? nullptr : nodeAt(i - 1);
        Node* target = (before == nullptr) ? head_ : before->next;
        detach(before, target, removed);
        return true;
    }

    template <typename Pred>
    bool removeWhere(Pred pred, T* removed = nullptr) {    // O(n): first record for which pred is true
        Node* before = nullptr;
        for (Node* cur = head_; cur != nullptr; before = cur, cur = cur->next) {
            if (pred(cur->value)) { detach(before, cur, removed); return true; }
        }
        return false;
    }

    // ---------------- reading ----------------
    const T* front() const { return head_ ? &head_->value : nullptr; }   // O(1)
    const T* back() const { return tail_ ? &tail_->value : nullptr; }    // O(1)

    T* at(std::size_t i) { return i < count_ ? &nodeAt(i)->value : nullptr; }              // O(n)
    const T* at(std::size_t i) const { return i < count_ ? &nodeAt(i)->value : nullptr; }

    template <typename Pred>
    T* findWhere(Pred pred) {                          // O(n): pointer lets the caller edit the record
        for (Node* cur = head_; cur != nullptr; cur = cur->next)
            if (pred(cur->value)) return &cur->value;
        return nullptr;
    }
    template <typename Pred>
    const T* findWhere(Pred pred) const {
        for (Node* cur = head_; cur != nullptr; cur = cur->next)
            if (pred(cur->value)) return &cur->value;
        return nullptr;
    }

    std::size_t size() const { return count_; }
    bool empty() const { return count_ == 0; }

    // ---------------- whole-list operations ----------------
    // Reverse the chain in place: three pointers, one pass, no new nodes. O(n).
    void reverse() {
        Node* prev = nullptr;
        Node* cur = head_;
        tail_ = head_;                                 // the old first node becomes the last
        while (cur != nullptr) {
            Node* nxt = cur->next;                     // save before we overwrite cur->next
            cur->next = prev;
            prev = cur;
            cur = nxt;
        }
        head_ = prev;
    }

    void toArray(T* dest) const {                      // O(n): dest must hold size() records
        std::size_t k = 0;
        for (Node* n = head_; n != nullptr; n = n->next) dest[k++] = n->value;
    }

    void fromArray(const T* src, std::size_t n) {      // O(n): replace the contents
        clear();
        for (std::size_t k = 0; k < n; ++k) pushBack(src[k]);
    }

    void clear() {                                     // O(n): delete every node exactly once
        Node* cur = head_;
        while (cur != nullptr) {
            Node* nxt = cur->next;                     // read next BEFORE deleting
            delete cur;
            cur = nxt;
        }
        head_ = tail_ = nullptr;
        count_ = 0;
    }
};

#endif
