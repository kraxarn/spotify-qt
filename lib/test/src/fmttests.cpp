#include "thirdparty/doctest.h"
#include "lib/fmt.hpp"

TEST_CASE("fmt::format")
{
	SUBCASE("args")
	{
		CHECK_EQ(lib::fmt::format("[{}, {}, {}]", 1, 2, 3), "[1, 2, 3]");
		CHECK_EQ(lib::fmt::format("[{}, {}, {}]", 1, 2), "[1, 2, {}]");
		CHECK_EQ(lib::fmt::format("[{}, {}, {}]", 1, 2, 3, 4), "[1, 2, 3]");
	}

	SUBCASE("string")
	{
		CHECK_EQ(lib::fmt::format("string: {}", std::string("value")), "string: value");
		CHECK_EQ(lib::fmt::format("string: {}", std::string()), "string: ");
	}

	SUBCASE("char*")
	{
		CHECK_EQ(lib::fmt::format("char*: {}", "value"), "char*: value");
		CHECK_EQ(lib::fmt::format("char*: {}", ""), "char*: ");
	}

	SUBCASE("bool")
	{
		CHECK_EQ(lib::fmt::format("bool: {}", true), "bool: true");
		CHECK_EQ(lib::fmt::format("bool: {}", false), "bool: false");
	}

	SUBCASE("int")
	{
		constexpr int val = 2147483647;
		CHECK_EQ(lib::fmt::format("int: {}", val), "int: 2147483647");
	}

	SUBCASE("long long")
	{
		constexpr long long val = 9223372036854775807;
		CHECK_EQ(lib::fmt::format("long long: {}", val), "long long: 9223372036854775807");
	}

	SUBCASE("float")
	{
		constexpr float val = 0.123456;
		CHECK_EQ(lib::fmt::format("float: {}", val), "float: 0.123456");
	}

	SUBCASE("double")
	{
		constexpr double val = 0.123456;
		CHECK_EQ(lib::fmt::format("double: {}", val), "double: 0.123456");
	}

	SUBCASE("json")
	{
		nlohmann::json val{
			{"a", 1},
			{"b", 2},
		};

		CHECK_EQ(lib::fmt::format("json: {}", val), R"(json: {"a":1,"b":2})");
	}
}
