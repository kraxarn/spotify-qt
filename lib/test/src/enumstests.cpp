#include "thirdparty/doctest.h"
#include "lib/enums.hpp"

enum class test_enum
{
	one = 1,
	two = 2,
	three = one | two,
};

TEST_CASE("enums")
{
	SUBCASE("has_flag")
	{
		CHECK(lib::enums<test_enum>::has_flag(test_enum::three, test_enum::two));
		CHECK_FALSE(lib::enums<test_enum>::has_flag(test_enum::two, test_enum::three));
	}
}
