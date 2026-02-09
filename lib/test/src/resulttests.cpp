#include "thirdparty/doctest.h"
#include "lib/result.hpp"

TEST_CASE("Result")
{
	SUBCASE("ok")
	{
		constexpr int value = 1;
		const auto result = Result<int>::ok(value);

		CHECK(result.success());
		CHECK_EQ(result.value(), value);
		CHECK_EQ(result.message(), std::string());
	}

	SUBCASE("fail")
	{
		const std::string message("1");
		const auto result = Result<int>::fail(message);

		CHECK_FALSE(result.success());
		CHECK_THROWS(result.value());
		CHECK_EQ(result.message(), message);
	}
}
