#include "lib/cipher.hpp"
#include "thirdparty/doctest.h"

TEST_CASE("cipher")
{
	SUBCASE("xor_strings")
	{
		std::string key("sample_key");
		std::string val("sample_value");

		auto enc = lib::cipher::xor_strings(key, val);
		CHECK_NE(enc, val);

		auto dec = lib::cipher::xor_strings(key, enc);
		CHECK_EQ(dec, val);
	}
}
