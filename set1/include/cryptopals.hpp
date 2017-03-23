#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <array>
#include <cstring>
#include <stdexcept>

// Converts the pair h1, h2 (where h1 is the most significant hex) to a uint8_t
std::uint8_t hex_pair_to_uint8(const char& h1, const char& h2)
{
    uint8_t result = 0;

    if (h1 >= '0' && h1 <= '9') {
        result += (h1 - '0') * 16;
    } else if (h1 >= 'a' && h1 <= 'f') {
        result += (10 + h1 - 'a') * 16;
    } else {
        throw std::domain_error("Input h1 is not a valid input to hex_pair_to_uint8.");
    }

    if (h2 >= '0' && h2 <= '9') {
        result += h2 - '0';
    } else if (h2 >= 'a' && h2 <= 'f') {
        result += 10 + h2 - 'a';
    } else {
        throw std::domain_error("Input h2 is not a valid input to hex_pair_to_uint8.");
    }

    return result;
}

std::vector<uint8_t> hex_to_bytes(const std::string& hex)
{
    std::vector<uint8_t> bytes;

    std::size_t size_to_reserve = hex.size() / 2;
    bool pad_first = false;
    if (hex.size() % 2 == 1) {
        size_to_reserve++;
        pad_first = true;
    }

    bytes.reserve(size_to_reserve);

    auto it = hex.begin();
    if (pad_first) {
        bytes.push_back(hex_pair_to_uint8('0', *it));
        it++;
    }

    for (; it < hex.end(); it += 2) {
        bytes.push_back(hex_pair_to_uint8(*it, *(it + 1)));
    }

    return bytes;
}

// Converts a 3 character array to 4 base64 characters.
std::string chunk_3_to_base64(const std::array<uint8_t, 3>& bytes)
{
    static std::string char_set = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    uint32_t bytes_in_one = (uint32_t) bytes[0] << 16 | (uint32_t) bytes[1] << 8 | (uint32_t) bytes[2];

    std::string base64;
    base64.resize(4);
    base64[0] = char_set[63 & bytes_in_one >> 18];
    base64[1] = char_set[63 & bytes_in_one >> 12];
    base64[2] = char_set[63 & bytes_in_one >> 6];
    base64[3] = char_set[63 & bytes_in_one];

    return base64;
}

// Converts a 2 character array to 4 base64 characters.
std::string chunk_2_to_base64(const std::array<uint8_t, 2>& bytes)
{
    static std::string char_set = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    uint32_t bytes_in_one = (uint32_t) bytes[0] << 16 | (uint32_t) bytes[1] << 8 | 0x00;

    std::string base64;
    base64.resize(4);
    base64[0] = char_set[63 & bytes_in_one >> 10];
    base64[1] = char_set[63 & bytes_in_one >> 6];
    base64[2] = char_set[(15 & bytes_in_one) << 2];
    base64[3] = '=';

    return base64;
}

// Converts a byte to 4 base64 characters.
std::string chunk_1_to_base64(const uint8_t& bytes)
{
    static std::string char_set = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


    std::string base64;
    base64.resize(4);
    base64[0] = char_set[63 & bytes >> 2];
    base64[1] = char_set[(3 & bytes) << 4];
    base64[2] = '=';
    base64[3] = '=';

    return base64;
}

std::string bytes_to_base64(const std::vector<uint8_t>& bytes)
{
    std::string base64;

    int size_to_reserve = (bytes.size() * 4) / 3 + 4 - (bytes.size() * 4) % 3;
    base64.reserve(size_to_reserve);

    auto it = bytes.begin();
    while (it < bytes.end()) {
        if (bytes.end() - it > 2) {
            base64.append(chunk_3_to_base64(std::array<uint8_t, 3> ({*it, *(it + 1), *(it + 2)})));
            it += 3;
        } else if (bytes.end() - it == 2) {
            base64.append(chunk_2_to_base64(std::array<uint8_t, 2> ({*it, *(it + 1)})));
            it += 2;
        } else {
            base64.append(chunk_1_to_base64(*it));
            it++;
        }
    }

    return base64;
}

