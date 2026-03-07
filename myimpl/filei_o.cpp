#include <iostream>
#include <cstdio>

int main() {
    FILE *file = fopen("abc.txt", "w");
    if (!file) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return 1;
    }

    fprintf(file, "Hello, World!");
    fclose(file);

    return 0;
}