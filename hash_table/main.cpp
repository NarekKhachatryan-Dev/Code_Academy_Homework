#include <iostream>
#include "hash.hpp"

int main() {
    HashTable ht;

    ht.insert("apple", 1);
    ht.insert("banana", 2);
    ht.insert("orange", 3);

    int value;
    if (ht.find("banana", value)) {
        std::cout << "Found banana: " << value << std::endl;
    } else {
        std::cout << "Banana not found" << std::endl;
    }

    ht.remove("banana");
    if (ht.find("banana", value)) {
        std::cout << "Found banana: " << value << std::endl;
    } else {
        std::cout << "Banana not found" << std::endl;
    }

    return 0;
}