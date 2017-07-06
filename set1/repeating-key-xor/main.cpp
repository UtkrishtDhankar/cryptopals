#include "../include/cryptopals.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>

std::vector<uint8_t> vector_from_string(std::string s)
{
	std::vector<uint8_t> v(s.size());

	std::memcpy(v.data(), s.data(), s.size());

	return v;
}

// Advances the iterator it in a circular manner
// If the iterator passes the given end,
// then it loops back to the start.
template<class Iterator>
void circular_advance(Iterator& it, 
		const Iterator& begin, const Iterator& end)
{
	it++;	
	if (it == end) {
		it = begin;
	}
}

std::vector<uint8_t> rotating_key_xor(std::vector<uint8_t> v, std::vector<uint8_t> key)
{
	std::vector<uint8_t> decoded(v.size());

    auto decoded_it = decoded.begin();
    auto v_it = v.begin();
	auto key_it = key.begin();
    for(; v_it < v.end() && decoded_it < decoded.end(); v_it++, decoded_it++) {
        *decoded_it =  *key_it ^ *v_it;

		circular_advance(key_it, key.begin(), key.end());
    }

    return decoded;
	
}

int main(int argc, char* argv[])
{
	std::string inp(std::istreambuf_iterator<char> (std::cin),
			std::istreambuf_iterator<char> ());

	std::string key = argv[1];
	std::vector<uint8_t> key_v = vector_from_string(key);

	std::vector<uint8_t> bytes = vector_from_string(inp);
	std::vector<uint8_t> encoded = rotating_key_xor(bytes, key_v);

	std::string encoded_hex = bytes_to_hex(encoded);
	std::cout << encoded_hex << std::endl;
	
	return 0;
}
