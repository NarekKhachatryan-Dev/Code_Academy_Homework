#include <iostream>
#include <fstream>

int main() {
    std::ofstream file("abc.txt", std::ios::out);
    if (!file) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return 1;
    }

    file << "Hello, World!";
    file.close();

    return 0;
}