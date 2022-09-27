#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <stack>

template<typename V>
class SplayTree {

    class Node : public std::enable_shared_from_this<Node> {
        using node_ptr_t = std::shared_ptr<Node>;
        using node_weakptr_t = std::weak_ptr<Node>;

        node_ptr_t right, left;
        node_weakptr_t parent;
        V value;

        node_ptr_t get_ptr() {
            return this->shared_from_this();
        }

        node_ptr_t get_parent() {
            return parent.lock();
        }

        void rotate_right() {
            assert(get_parent()->left == get_ptr());

            auto grandparent = get_parent()->get_parent();
            auto current_parent = get_parent();
            auto this_ptr = get_ptr();

            current_parent->set_left(right);
            set_right(current_parent);

            parent = grandparent;

            if (grandparent != nullptr) {
                if (grandparent->left == current_parent) {
                    grandparent->left = get_ptr();
                }
                else {
                    grandparent->right = get_ptr();
                }
            }
        }

        void rotate_left() {
            assert(get_parent()->right == get_ptr());

            auto grandparent = get_parent()->get_parent();
            auto current_parent = get_parent();
            auto this_ptr = get_ptr();

            current_parent->set_right(left);
            set_left(current_parent);

            parent = grandparent;

            if (grandparent != nullptr) {
                if (grandparent->left == current_parent) {
                    grandparent->left = get_ptr();
                }
                else {
                    grandparent->right = get_ptr();
                }
            }
        }

        void local_splay() {
            auto grandparent = get_parent()->get_parent();

            if (grandparent == nullptr) {
                assert(get_parent()->get_left() == get_ptr() || get_parent()->get_right() == get_ptr());

                if (get_parent()->get_left() == get_ptr()) {
                    rotate_right();
                }
                else {
                    rotate_left();
                }
            }
            else {
                auto grandgrandparent = grandparent->get_parent();
                auto _parent = get_parent();

                if (get_parent()->get_left() == get_ptr() && grandparent->get_left() == get_parent()) {
                    get_parent()->rotate_right();
                    rotate_right();
                }
                else if (get_parent()->get_right() == get_ptr() && grandparent->get_right() == get_parent()) {
                    get_parent()->rotate_left();
                    rotate_left();
                }
                else if (get_parent()->get_right() == get_ptr() && grandparent->get_left() == get_parent()) {
                    rotate_left();
                    rotate_right();
                }
                else if (get_parent()->get_left() == get_ptr() && grandparent->get_right() == get_parent()) {
                    rotate_right();
                    rotate_left();
                }
            }
        }

        void splay() {
            while (get_parent() != nullptr) {
                local_splay();
            }
        }

    public:
        explicit Node(V _value) : value(_value) {
            right = nullptr;
            left = nullptr;
            parent = node_weakptr_t();
        }

        void set_left(node_ptr_t node) {
            left = node;
            if (node != nullptr) {
                node->parent = get_ptr();
            }
        }

        void set_right(node_ptr_t node) {
            right = node;
            if (node != nullptr) {
                node->parent = get_ptr();
            }
        }

        node_ptr_t get_left() {
            return left;
        }

        node_ptr_t get_right() {
            return right;
        }

        V &get_value() {
            return value;
        }

        void print() {
            std::cout << value
                      << ": left: " << (left != nullptr ? left->get_value() : -1)
                      << ", right: " << (right != nullptr ? right->get_value() : -1)
                      << ", parent: " << (get_parent() != nullptr ? get_parent()->get_value() : -1)
                      << std::endl;
        }

        void _print_all() {
            print();
            if (left != nullptr) {
                left->_print_all();
            }
            if (right != nullptr) {
                right->_print_all();
            }
        }

        void print_all() {
            _print_all();
            std::cout << std::endl;
        }

        void set_value(V _value) {
            value = _value;
        }

        node_ptr_t search(V v, SplayTree<V> *splay_tree) {
            if (v < value) {
                if (left != nullptr) {
                    return left->search(v, splay_tree);
                }
                else {
                    splay();
                    splay_tree->root = get_ptr();
                    return get_ptr();
                }
            }
            else if (v > value) {
                if (right != nullptr) {
                    return right->search(v, splay_tree);
                }
                else {
                    splay();
                    splay_tree->root = get_ptr();
                    return get_ptr();
                }
            }
            else {
                splay();
                splay_tree->root = get_ptr();
                return get_ptr();
            }
        }

        node_ptr_t insert(V v, SplayTree<V> *splay_tree) {
            node_ptr_t node = nullptr;

            if (v < value) {
                if (left != nullptr) {
                    return left->insert(v, splay_tree);
                }
                else {
                    node = std::make_shared<Node>(v);

                    set_left(node);
                    node->splay();
                    splay_tree->root = node;

                    return node;
                }
            }
            else if (v > value) {
                if (right != nullptr) {
                    return right->insert(v, splay_tree);
                }
                else {
                    node = std::make_shared<Node>(v);

                    set_right(node);
                    node->splay();
                    splay_tree->root = node;

                    return node;
                }
            }
            else {
                splay();
                splay_tree->root = get_ptr();
                return get_ptr();
            }
        }

        node_ptr_t remove(V v, SplayTree<V> *splay_tree) {
            if (v < value) {
                if (left != nullptr) {
                    return left->remove(v, splay_tree);
                }
                else {
                    splay();
                    splay_tree->root = get_ptr();
                    return get_ptr();
                }
            }
            else if (v > value) {
                if (right != nullptr) {
                    return right->remove(v, splay_tree);
                }
                else {
                    splay();
                    splay_tree->root = get_ptr();
                    return get_ptr();
                }
            }
            else {
                node_ptr_t new_root = get_parent();

                if (left == nullptr && right == nullptr) {
                    if (get_parent() != nullptr) {
                        if (get_parent()->left == get_ptr()) {
                            get_parent()->left = nullptr;
                        }
                        else {
                            get_parent()->right = nullptr;
                        }
                    }
                }
                else if ((left != nullptr && right == nullptr) || (right != nullptr && left == nullptr)) {
                    node_ptr_t child = left == nullptr ? right : left;

                    if (get_parent() == nullptr) {
                        new_root = child;
                    }
                    else if (get_parent()->left == get_ptr()) {
                        get_parent()->set_left(child);
                    }
                    else {
                        get_parent()->set_right(child);
                    }
                }
                else {
                    node_ptr_t node = left;
                    while (node->right != nullptr) {
                        node = node->right;
                    }

                    set_value(node->get_value());
                    if (node == left) {
                        set_left(node->left);
                    }
                    else {
                        node->get_parent()->set_right(node->left);
                    }
                }

                if (new_root != nullptr) {
                    new_root->splay();
                    splay_tree->root = new_root;
                }

                return new_root;
            }
        }

        void get_next(std::stack<node_ptr_t> &traversal) {
            if (right != nullptr) {
                auto node = right;

                while (node->left != nullptr) {
                    traversal.push(node);
                    node = node->left;
                }
                traversal.push(node);
            }
        }

        void get_previous(std::stack<node_ptr_t> &traversal) {
            if (left != nullptr) {
                auto node = left;

                while (node->right != nullptr) {
                    traversal.push(node);
                    node = node->right;
                }
                traversal.push(node);
            }
        }

        node_ptr_t unpin_left_subtree() {
            auto node = left;
            if (node != nullptr) {
                node->parent = node_weakptr_t();
            }
            left = nullptr;

            return node;
        }

        node_ptr_t unpin_right_subtree() {
            auto node = right;
            if (node != nullptr) {
                node->parent = node_weakptr_t();
            }
            right = nullptr;

            return node;
        }
    };

    using node_ptr_t = std::shared_ptr<Node>;

    node_ptr_t root;

    template<bool increasing_direction>
    class Iterator {
        std::stack<node_ptr_t> traversal;

        void next() {
            node_ptr_t node = traversal.top();
            traversal.pop();
            if (increasing_direction) {
                node->get_next(traversal);
            }
            else {
                node->get_previous(traversal);
            }
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using difference_tag = std::ptrdiff_t;
        using value_type = V;
        using pointer = V *;
        using reference = const V &;

        explicit Iterator(std::stack<node_ptr_t> traversal) : traversal(traversal) {}

        explicit Iterator(SplayTree *splay) {
            auto node = splay->root;

            if (increasing_direction) {
                while (node != nullptr) {
                    traversal.push(node);
                    node = node->get_left();
                }
            }
            else {
                while (node != nullptr) {
                    traversal.push(node);
                    node = node->get_right();
                }
            }
        }

        Iterator(const Iterator &other) : Iterator(other.traversal) {}

        bool operator==(const Iterator &other) const {
            if (this->traversal.empty() == other.traversal.empty()) {
                return this->traversal.empty() == true || this->traversal.top() == other.traversal.top();
            }
            return false;
        }

        bool operator!=(const Iterator &other) const {
            return !(*this == other);
        }

        reference operator*() const {
            return traversal.top()->get_value();
        }

        pointer operator->() {
            return &(traversal.top()->get_value());
        }

        Iterator &operator++() {
            next();
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            next();
            return temp;
        }
    };

public:
    SplayTree() {
        root = nullptr;
    }

    explicit SplayTree(node_ptr_t root) : root(root) {}

    explicit SplayTree(std::initializer_list<V> list) {
        for (V value : list) {
            insert(value);
        }
    }

    Iterator<true> begin() {
        return Iterator<true>(this);
    }

    Iterator<true> end() {
        return Iterator<true>(std::stack<node_ptr_t>());
    }

    Iterator<false> rbegin() {
        return Iterator<false>(this);
    }

    Iterator<false> rend() {
        return Iterator<false>(std::stack<node_ptr_t>());
    }

    void insert(V value) {
        if (root == nullptr) {
            root = std::make_shared<Node>(value);
        }
        else {
            root->insert(value, this);
        }
    }

    bool contains(V value) {
        if (root == nullptr) {
            return false;
        }
        root->search(value, this);
        V found = root->get_value();
        return found == value;
    }

    void remove(V value) {
        if (root == nullptr) {
            return;
        }
        else {
            root->remove(value, this);
        }
    }

    SplayTree<V> remove_less(V value) {
        root->search(value, this);
        return SplayTree(root->unpin_left_subtree());
    }

    SplayTree<V> remove_greater(V value) {
        root->search(value, this);
        return SplayTree(root->unpin_right_subtree());
    }

    void print() {
        root->print_all();
    }
};