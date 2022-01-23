#include "thirdparty/doctest.h"
#include "lib/format.hpp"

TEST_CASE("fmt::format")
{
	SUBCASE("time")
	{
		CHECK_EQ(lib::format::time(0), "0:00");
		CHECK_EQ(lib::format::time(999), "0:00");
		CHECK_EQ(lib::format::time(5000), "0:05");
		CHECK_EQ(lib::format::time(60 * 1000), "1:00");
		CHECK_EQ(lib::format::time(65 * 1000), "1:05");
	}

	SUBCASE("size")
	{
		CHECK_EQ(lib::format::size(1), "1 B");
		CHECK_EQ(lib::format::size(1000), "1 kB");
		CHECK_EQ(lib::format::size(1000000), "1 MB");
		CHECK_EQ(lib::format::size(1000000000), "1 GB");
	}
}
