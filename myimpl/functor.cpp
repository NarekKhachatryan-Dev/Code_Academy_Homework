#include <iostream>

auto f(int x) {
    return [x](int y) {
        return x * y;
    };
}

int main() {
    std::cout << f(5)(6) << std::endl;
    return 0;
}