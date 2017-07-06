#include "../include/cryptopals.hpp"

#include <string>
#include <iostream>
#include <locale>
#include <algorithm>
#include <cctype>
#include <limits>

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

int main()
{
    std::string hex_in;
    std::cin >> hex_in;

    std::vector<uint8_t> bytes = hex_to_bytes(hex_in);

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

    std::cout << best_key << ": " << best_match << std::endl;

    return 0;
}
