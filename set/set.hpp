#pragma once

#include <initializer_list>
#include <functional>
#include <vector>

template<typename T, typename Compare = std::less<T>>
class Set {
    struct Node
    {
        T data;
        Node* left;
        Node* right;

        Node(const T& value) : data(value), left(nullptr), right(nullptr) {}
        ~Node() {
            delete left;
            delete right;
        }
    };

    Node* root;
    Compare comp;
    size_t tree_size;
    
    public:
    Set() : root(nullptr), comp(Compare()), tree_size(0) {}
    Set(std::initializer_list<T> init) : Set() {
        for (const auto& value : init) {
            insert(value);
        }
    }

    bool insert(const T& value) {
        bool inserted = false;
        root = insertRec(root, value, inserted);
        if (inserted) ++tree_size;
        return inserted;
    }

    Node* insertRec(Node* node, const T& value, bool& inserted) {
         if (!node) {
            inserted = true;
            return new Node(value);
        }
        if (comp(value, node->data)) {
            node->left = insertRec(node->left, value, inserted);
        } else if (comp(node->data, value)) {
            node->right = insertRec(node->right, value, inserted);
        }
        return node;
    }

    bool contains(const T& value) const {
        return containsRec(root, value);
    }

    private:
    bool containsRec(Node* node, const T& value) const {
        if (!node) {
            return false;
        }
        if (comp(value, node->data)) {
            return containsRec(node->left, value);
        } else if (comp(node->data, value)) {
            return containsRec(node->right, value);
        } else {
            return true;
        }
    }

    public:
    std::vector<T> inorder() const {
        std::vector<T> result;
        inorderRec(root, result);
        return result;
    }

    std::vector<T> preorder() const {
        std::vector<T> result;
        preorderRec(root, result);
        return result;
    }

    std::vector<T> postorder() const {
        std::vector<T> result;
        postorderRec(root, result);
        return result;
    }

    private:
    void inorderRec(Node* node, std::vector<T>& result) const {
        if (node) {
            inorderRec(node->left, result);
            result.push_back(node->data);
            inorderRec(node->right, result);
        }
    }

    void preorderRec(Node* node, std::vector<T>& result) const {
        if (node) {
            result.push_back(node->data);
            preorderRec(node->left, result);
            preorderRec(node->right, result);
        }
    }

    void postorderRec(Node* node, std::vector<T>& result) const {
        if (node) {
            postorderRec(node->left, result);
            postorderRec(node->right, result);
            result.push_back(node->data);
        }
    }

    public:
    Node* getRoot() const {
        return root;
    }

    Node* getLeftChild(Node* node) const {
        return node ? node->left : nullptr;
    }

    Node* getRightChild(Node* node) const {
        return node ? node->right : nullptr;
    }

    size_t size() const {
        return tree_size;
    }
    
    bool empty() const {
        return tree_size == 0;
    }

    ~Set() {
        delete root;
    }
};