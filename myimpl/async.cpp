#include <iostream>
#include <future>
#include <vector>
#include <numeric>

int main() {
    std::vector<int> nums(1000000, 25);

    auto midpoint = nums.begin() + nums.size() / 2;
    std::future<int> left = std::async(std::launch::async, [&]() {
        return std::accumulate(nums.begin(), midpoint, 0);
    });

    auto right = std::async(std::launch::async, [&]() {
        return std::accumulate(midpoint,nums.end(),0);
    });

    int sum = left.get() + right.get();
    std::cout << "Sum: " << sum << std::endl;
}