#include "thirdparty/doctest.h"
#include "lib/optional.hpp"

#define VALUE 1

TEST_CASE("optional")
{
	lib::optional<int> value(VALUE);
	lib::optional<int> no_value;

	SUBCASE("has_value")
	{
		CHECK(value.has_value());
		CHECK_FALSE(no_value.has_value());
	}

	SUBCASE("value")
	{
		CHECK_EQ(value.value(), VALUE);
		CHECK_THROWS(no_value.value());
	}

	SUBCASE("value_or")
	{
		CHECK_EQ(value.value_or(VALUE + 1), VALUE);
		CHECK_EQ(no_value.value_or(VALUE + 1), VALUE + 1);
	}
}
