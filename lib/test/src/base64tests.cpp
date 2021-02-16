#include "thirdparty/doctest.h"
#include "lib/base64.hpp"

TEST_CASE("base64")
{
	std::string decoded("Hello, World!");
	std::string encoded("SGVsbG8sIFdvcmxkIQ==");

	SUBCASE("encode/decode")
	{
		CHECK_EQ(lib::base64::decode(lib::base64::encode(decoded)), decoded);
	}

	SUBCASE("encode")
	{
		CHECK_EQ(lib::base64::encode(decoded), encoded);
	}

	SUBCASE("decode")
	{
		CHECK_EQ(lib::base64::decode(encoded), decoded);
	}
}
