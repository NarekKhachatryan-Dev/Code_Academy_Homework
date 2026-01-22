#pragma once

#include <iostream>

namespace mylist {
template<typename T>
class list {
private:
    class node {
    public:
        T data;
        node* next;
        node(const T& value) : data(value), next(nullptr) {}
    };

    node* head;

public:
    class iterator {
    public:
        iterator(node* ptr) : current(ptr) {}
        
        T& operator*() const {
            return current->data;
        }
        
        iterator& operator++() {
            current = current->next;
            return *this;
        }
        
        bool operator!=(const iterator& other) const {
            return current != other.current;
        }
        
        T& operator->() const {
            return &(current->data);
        }

        bool operator==(const iterator& other) const {
            return current == other.current;
        }

        T& operator++(int) {
            T& temp = current->data;
            current = current->next;
            return temp;
        }

    private:
        node* current;
    };

    list() : head(nullptr) {}
    ~list() {
        clear();
    }

    list(const list& other) : head(nullptr){
        node* cur = other.head;

        while (cur)
        {
            push_back(cur->data);
            cur = cur -> next;
        }
        
    }

    list(list&& other) noexcept {
        head = other.head;
        other.head = nullptr;
    }

    list& operator=(const list& other) {
        if(this == &other) return *this;

        clear();

        node* cur = other.head;
        while(cur) {
            push_back(cur -> data);
            cur = cur ->next;
        }

        return *this;
    }

    list& operator=(list&& other) noexcept {
        if(this == &other) return *this;

        clear();

        head = other.head;
        other.head = nullptr;

        return *this;
    }

    void push_back(const T& value) {
        node* newNode = new node(value);
        if(!head) {
            head = newNode;
        } else {
            node* cur = head;
            while (cur -> next) {
                cur = cur ->next;
            }
            cur ->next = newNode;
        }
    }

    void insert(size_t index, const T& value) {
        
    }

    void push_front(const T& value) {
        node* temp = head;
        head = new node(value);
        head->next = temp;
    }

    void pop_front() {
        if(!head) return;
        node* temp = head;
        head = head ->next;
        delete temp;
    }

    void pop_back() {
        if(!head) return;

        if(!head ->next) {
            delete head;
            head = nullptr;
        } else { 
            node* cur = head;
            while (cur ->next ->next)
            {
                cur = cur ->next;
            }
            delete cur ->next;
            cur -> next = nullptr;
        }
    }

    void print() const {
        node* cur = head;
        while(cur) {
            std::cout << cur ->data << ", ";
            cur = cur ->next;
        }
        std::cout << std::endl;
    }

    bool empty() const {
        return head == nullptr;
    }

    void clear() {
        while (head) {
            node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};
};