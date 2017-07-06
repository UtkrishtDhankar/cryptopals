#include "../include/cryptopals.hpp"

#include <cstdint>
#include <string>
#include <iostream>
#include <algorithm>

std::pair<char, char> byte_to_hex_pair(uint8_t bytes)
{
	static const std::string hexChars = "0123456789ABCDEF";

	uint8_t firstFourBits = (bytes & 0xF0) >> 4;
	uint8_t lastFourBits = bytes & 0x0F;

	char first = hexChars[firstFourBits];
	char second = hexChars[lastFourBits];

	return std::make_pair(first, second);
}

std::string bytes_to_hex(std::vector<uint8_t> bytes)
{
	// reserve the space for the string
	std::string hex(bytes.size() * 2, 0);

	auto bytes_it = bytes.begin();
	auto hex_it = hex.begin();
	for(;
			bytes_it < bytes.end() && hex_it < hex.end();
			bytes_it++, hex_it += 2) {
		auto hex_pair = byte_to_hex_pair(*bytes_it);
		hex_it[0] = hex_pair.first;
		hex_it[1] = hex_pair.second;
	}

	return hex;
}

int main()
{
	std::vector<std::vector<uint8_t> > input_bytes; 
	std::string buffer;
	while (std::cin >> buffer) {
		input_bytes.push_back(hex_to_bytes(buffer));
	}

	// A vector that holds the scores for all the stuff we got
	std::vector<float> scores(input_bytes.size());

	// A vector that holds the decoding keys for all the stuff we got
	std::vector<uint8_t> keys(input_bytes.size());
	
	auto bytes_it = input_bytes.begin();
	auto scores_it = scores.begin();
	auto keys_it = keys.begin();
	for (;
			bytes_it < input_bytes.end() 
				&& scores_it < scores.end() 
				&& keys_it < keys.end();
			bytes_it++, scores_it++, keys_it++) {
		*keys_it = find_single_xor_key(*bytes_it, *scores_it);
	}

	auto min_score = std::min_element(scores.begin(), scores.end());
	int min_score_index = std::distance(scores.begin(), min_score);

	auto min_key = keys.begin();
	std::advance(min_key, min_score_index);

	auto min_bytes = input_bytes.begin();
	std::advance(min_bytes, min_score_index);

	std::cout << bytes_to_hex(*min_bytes) << std::endl;
	std::cout << "Decoding with key " << *min_key << std::endl;
	std::cout << decode_single_xor(*min_bytes, *min_key) << std::endl;

	return 0;
}
