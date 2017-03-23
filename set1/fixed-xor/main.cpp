#include "../include/cryptopals.hpp"

#include <string>
#include <cstddef>
#include <stdexcept>

std::vector<uint8_t> fixed_xor(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b)
{
    if (a.size() != b.size()) {
        throw std::domain_error("a and b should have the same size for fixed_xor.");
    }

    std::vector<uint8_t> ans;
    ans.resize(a.size());
    auto ait = a.begin();
    auto bit = b.begin();
    auto ansit = ans.begin();
    for (; ait < a.end() && bit < b.end(); ait++, bit++, ansit++) {
        *ansit = *ait ^ *bit;
    }

    return ans;
}

int main()
{
    std::string hex_in1;
    std::cin >> hex_in1;

    std::string hex_in2;
    std::cin >> hex_in2;

    std::vector<uint8_t> bytes1 = hex_to_bytes(hex_in1);
    std::vector<uint8_t> bytes2 = hex_to_bytes(hex_in2);

    std::vector<uint8_t> xored = fixed_xor(bytes1, bytes2);

    std::cout << std::hex;
    for (auto elem : xored) {
        std::cout << (int) elem;
    }
    std::cout << std::endl;

    return 0;
}
