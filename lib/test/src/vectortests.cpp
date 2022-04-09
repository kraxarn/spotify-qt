#include "lib/vector.hpp"
#include "thirdparty/doctest.h"

#include <vector>
#include <string>

TEST_CASE("vector")
{
	auto verify_eq = [](const std::vector<int> &v1, const std::vector<int> &v2) -> bool
	{
		if (v1.size() != v2.size())
		{
			return false;
		}

		for (size_t i = 0; i < v1.size(); i++)
		{
			if (v1.at(i) != v2.at(i))
			{
				return false;
			}
		}

		return true;
	};

	SUBCASE("combine")
	{
		std::vector<int> v1 = {
			1, 2, 3
		};
		std::vector<int> v2 = {
			4, 5, 6
		};

		auto v3 = lib::vector::combine(v1, v2);
		CHECK(verify_eq(v3, {
			1, 2, 3, 4, 5, 6
		}));
	}

	SUBCASE("append")
	{
		std::vector<int> v1 = {
			1, 2, 3
		};
		std::vector<int> v2 = {
			4, 5, 6
		};

		lib::vector::append(v1, v2);
		CHECK(verify_eq(v1, {
			1, 2, 3, 4, 5, 6
		}));
	}

	SUBCASE("index_of")
	{
		std::vector<int> vec1 = {
			1, 2, 3
		};

		CHECK_EQ(lib::vector::index_of(vec1, 1), 0);
		CHECK_EQ(lib::vector::index_of(vec1, 2), 1);
		CHECK_EQ(lib::vector::index_of(vec1, 3), 2);
		CHECK_EQ(lib::vector::index_of(vec1, 4), vec1.size());

		std::vector<std::string> vec2 = {
			"item1",
			"item2",
			"item3",
		};

		CHECK_EQ(lib::vector::index_of(vec2, std::string("item1")), 0);
		CHECK_EQ(lib::vector::index_of(vec2, std::string("item0")), vec1.size());
	}

	SUBCASE("average")
	{
		std::vector<int> vec_empty;
		CHECK_EQ(lib::vector::average(vec_empty), 0);

		std::vector<int> vec_int = {
			1, 4, 5, 7,
		};
		CHECK_EQ(lib::vector::average(vec_int), 4);

		std::vector<float> vec_float = {
			1.F, 4.F, 5.F, 7.F,
		};
		CHECK_EQ(lib::vector::average(vec_float), 4.25F);
	}
}
