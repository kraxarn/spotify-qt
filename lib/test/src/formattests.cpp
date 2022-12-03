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

	SUBCASE("time_pretty")
	{
		CHECK_EQ(lib::format::time_pretty(0), "0 m");
		CHECK_EQ(lib::format::time_pretty(59 * 1000 * 60), "59 m");
		CHECK_EQ(lib::format::time_pretty(60 * 1000 * 60), "1 h");
		CHECK_EQ(lib::format::time_pretty(61 * 1000 * 60), "1 h 1 m");
	}

	SUBCASE("size")
	{
		CHECK_EQ(lib::format::size(1), "1 B");
		CHECK_EQ(lib::format::size(1000), "1 kB");
		CHECK_EQ(lib::format::size(1000000), "1 MB");
		CHECK_EQ(lib::format::size(1000000000), "1 GB");
	}
}
