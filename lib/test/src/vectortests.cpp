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

	SUBCASE("unique")
	{
		std::vector<int> vec{
			2, 1, 2, 3,
		};

		lib::vector::unique(vec);
		CHECK_EQ(vec.size(), 3);
		CHECK_EQ(vec[0], 1);
		CHECK_EQ(vec[1], 2);
		CHECK_EQ(vec[2], 3);
	}

	SUBCASE("sub")
	{
		const std::vector<int> vec{
			1, 2, 3, 4, 5,
		};

		const auto vec1 = lib::vector::sub(vec, 0, 3);
		CHECK_EQ(vec1.size(), 3);
		CHECK_EQ(vec1[0], 1);
		CHECK_EQ(vec1[1], 2);
		CHECK_EQ(vec1[2], 3);

		const auto vec2 = lib::vector::sub(vec, 1, 3);
		CHECK_EQ(vec2.size(), 3);
		CHECK_EQ(vec2[0], 2);
		CHECK_EQ(vec2[1], 3);
		CHECK_EQ(vec2[2], 4);

		const auto vec3 = lib::vector::sub(vec, 2, 5);
		CHECK_EQ(vec3.size(), 3);
		CHECK_EQ(vec3[0], 3);
		CHECK_EQ(vec3[1], 4);
		CHECK_EQ(vec3[2], 5);

		const auto vec4 = lib::vector::sub(vec, 3, static_cast<size_t>(-1));
		CHECK_EQ(vec4.size(), 2);
		CHECK_EQ(vec4[0], 4);
		CHECK_EQ(vec4[1], 5);
	}
}
