#include <iostream>
#include <vector>
#include <random>
#include <execution>

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);

    const long long size = 1000000000;
    std::vector<int> vec(size);
    for(auto& i : vec) {
        i = dis(gen);
    }

    long long sum = std::reduce(std::execution::par, vec.begin(), vec.end(), 0LL);
    std::cout << "Sum: " << sum << std::endl;

    return 0;
}