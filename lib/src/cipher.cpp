#include "lib/cipher.hpp"

using namespace lib;

std::string cipher::xor_strings(const std::string &key, const std::string &value)
{
	std::string out(value.size(), '\0');
	for (auto i = 0; i < value.size(); i++)
		out[i] = value.at(i) ^ key.at(i % key.size());
	return out;
}
