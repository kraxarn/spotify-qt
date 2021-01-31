#include "base64.hpp"

using namespace lib;

std::string base64::encode(const std::string &str)
{
	auto charset = get_charset();
	std::string out;
	int val = 0;
	int val_b = -6;
	for (unsigned char c : str)
	{
		val = (val << 8) + c;
		val_b += 8;
		while (val_b >= 0)
		{
			out.push_back(charset[(val >> val_b) & 0x3F]);
			val_b -= 6;
		}
	}
	if (val_b > -6)
		out.push_back(charset[((val << 8) >> (val_b + 8)) & 0x3F]);
	while (out.size() % 4)
		out.push_back('=');
	return out;
}

std::string base64::decode(const std::string &str)
{
	auto charset = get_charset();
	std::string out;
	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++)
		T[charset[i]] = i;
	int val = 0;
	int val_b = -8;
	for (unsigned char c : str)
	{
		if (T[c] == -1)
			break;
		val = (val << 6) + T[c];
		val_b += 6;
		if (val_b >= 0)
		{
			out.push_back(char((val >> val_b) & 0xFF));
			val_b -= 8;
		}
	}
	return out;
}

std::string base64::get_charset()
{
	return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}
