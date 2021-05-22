#include "lib/vector.hpp"
#include "thirdparty/doctest.h"

#include <vector>

TEST_CASE("vector")
{
	auto verify_eq = [](const std::vector<int> &v1, const std::vector<int> &v2) -> bool
	{
		if (v1.size() != v2.size())
		{
			return false;
		}

		for (auto i = 0; i < v1.size(); i++)
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
}
