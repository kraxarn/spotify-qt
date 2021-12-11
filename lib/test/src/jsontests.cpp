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
			1, 2, 3,
		};
		nlohmann::json item2{
			4, 5, 6,
		};

		auto combined = lib::json::combine(item1, item2);
		for (auto i = 0; i < item1.size() + item2.size(); i++)
		{
			CHECK_EQ(combined.at(i), i + 1);
		}
	}

	SUBCASE("get")
	{
		int value = 0;

		lib::json::get(json, "a", value);
		CHECK_EQ(value, 1);

		lib::json::get(json, "d", value);
		CHECK_EQ(value, 1);
	}

	SUBCASE("find_item")
	{
		// Images for albums, artists, etc. from Web API
		nlohmann::json item1{
			{"images", {
				{{"url", "image1url"}},
				{{"url", "image2url"}},
				{{"url", "image3url"}},
			}},
		};

		std::string result;
		lib::json::find_item("image", item1, result);
		CHECK_EQ(result, "image3url");

		// Single image from cache
		nlohmann::json item2{
			{"image", "image4url"},
		};

		result.clear();
		lib::json::find_item("image", item2, result);
		CHECK_EQ(result, "image4url");

		// Something not currently used, but maybe some day
		nlohmann::json item3{
			{"items", {
				"item1",
				"item2",
				"item3",
			}},
		};

		result.clear();
		lib::json::find_item("item", item3, result);
		CHECK_EQ(result, "item3");

		// Something not found
		nlohmann::json item4{
			{"image", "image5url"},
			{"item", "item4"},
		};

		result.clear();
		lib::json::find_item("url", item4, result);
		CHECK(result.empty());
	}
}
