#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <array>
#include <cstring>
#include <stdexcept>
#include <limits>
#include <algorithm>

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

// Calculates a metric for plaintext. The closer this is, the more likely answer is plaintext
float chi_squared(std::string str)
{
    std::vector<float> english_freq = {
        0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228, 0.02015,  // A-G
        0.06094, 0.06966, 0.00153, 0.00772, 0.04025, 0.02406, 0.06749,  // H-N
        0.07507, 0.01929, 0.00095, 0.05987, 0.06327, 0.09056, 0.02758,  // O-U
        0.00978, 0.02360, 0.00150, 0.01974, 0.00074                     // V-Z
    };

    // Get a vector to count the letter counts of all things from 'a' to 'z'
    std::vector<unsigned int> letter_count(26, 0);
    int ignored = 0;

    // get a lowercase version of string
    std::string lower_str;
    lower_str.resize(str.size());
    std::transform(str.begin(), str.end(), lower_str.begin(), [](unsigned char c) {return std::tolower(c);});

    for (auto it = lower_str.begin(); it < lower_str.end(); it++) {
        // if it is a valid character...
        if (std::islower(*it)) {
            letter_count[*it - 'a']++;
        } else {
            ignored++;
        }
    }

    float ans = 0;
    for (int i = 0; i < 26; i++) {
        // multiplying by 10000 because the expected number could be very low, or even zero
        // so this artificially inflates the sample size, basically just stops divide by zero errors
        int num_expected = (int) (10000 * str.size() * english_freq[i]);
        int num_got = 10000 * letter_count[i];

        float numerator = num_got - num_expected;
        numerator *= numerator;

        ans += numerator / num_expected;
    }

    // Add the ignored to the thing
    ans += (10000 * ignored * ignored);

    return ans / 10000;
}


// decodes the given coded bytes with a single xor cipher, returning a string
// really, this could return a vector as well but I'm lazy
std::string decode_single_xor(std::vector<uint8_t> code, uint8_t key)
{
    std::string decoded;
    decoded.resize(code.size());

    auto decoded_it = decoded.begin();
    auto code_it = code.begin();
    for(; code_it < code.end() && decoded_it < decoded.end(); code_it++, decoded_it++) {
        *decoded_it = key ^ *code_it;
    }

    return decoded;
}

// Finds and returns the key that was most likely used to 
// encrypt the given bytes via the single byte xor cipher.
// Also returns the likely score in the supplied score param
char find_single_xor_key(const std::vector<uint8_t>& bytes, float& score)
{
    float min_score = std::numeric_limits<float>::max();
    std::string best_match = "";
    char best_key;

    // using unsigned ints here because I can't really make a loop iterate through all
    // values that it possibly can
    for (unsigned int key = 0; key < 256; key++) {
        std::string decoded = decode_single_xor(bytes, key);
        float score = chi_squared(decoded);

        if (score < min_score) {
            min_score = score;
            best_match = std::move(decoded);
            best_key = key;
        }
    }

    score = min_score;
    return best_key;
}
