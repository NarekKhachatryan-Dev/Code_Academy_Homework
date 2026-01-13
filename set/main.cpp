#include <iostream>
#include "set.hpp"

int main() {
    Set<int> mySet = {5, 3, 8, 1, 4};

    std::cout << "Inserting 6: " << (mySet.insert(6) ? "Success" : "Already exists") << std::endl;
    std::cout << "Inserting 3: " << (mySet.insert(3) ? "Success" : "Already exists") << std::endl;

    std::cout << "Contains 4: " << (mySet.contains(4) ? "Yes" : "No") << std::endl;
    std::cout << "Contains 7: " << (mySet.contains(7) ? "Yes" : "No") << std::endl;

    return 0;
}