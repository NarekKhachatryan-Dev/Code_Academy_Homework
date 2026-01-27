#include <iostream>
#include <vector>
#include <algorithm>

template<class InputIt, class UnaryFunction>
constexpr UnaryFunction for_each_custom(InputIt first, InputIt last, UnaryFunction f)
{
    for (; first != last; ++first) {
        f(*first);
    }
    return f;
}

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    for_each_custom(v.begin(), v.end(), [](int x) {
        std::cout << x << " ";
    });

    std::cout << "\n";

    std::for_each(v.begin(), v.end(), [](int x) {
        std::cout << x * 2 << " ";
    });
    std::cout << "\n";
}