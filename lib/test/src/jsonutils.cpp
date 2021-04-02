#include "thirdparty/doctest.h"
#include "lib/json.hpp"

TEST_CASE("json")
{
	auto json = nlohmann::json{
		{"a", 1},
		{"b", 2},
		{"c", 3},
	};

	SUBCASE("get_property")
	{
		int value = 0;

		CHECK(lib::json::get_property(json, {"a"}, value));
		CHECK_EQ(value, 1);

		CHECK(lib::json::get_property(json, {"b"}, value));
		CHECK_EQ(value, 2);

		CHECK(lib::json::get_property(json, {"c"}, value));
		CHECK_EQ(value, 3);

		CHECK_FALSE(lib::json::get_property(json, {"d"}, value));
	}

	SUBCASE("combine")
	{
		nlohmann::json item1{
			1, 2, 3
		};
		nlohmann::json item2{
			4, 5, 6
		};

		auto combined = lib::json::combine(item1, item2);
		for (auto i = 0; i < item1.size() + item2.size(); i++)
			CHECK_EQ(combined.at(i), i + 1);
	}

	SUBCASE("get")
	{
		int value = 0;

		lib::json::get(json, "a", value);
		CHECK_EQ(value, 1);

		lib::json::get(json, "d", value);
		CHECK_EQ(value, 1);
	}
}