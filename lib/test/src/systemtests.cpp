#include "lib/system.hpp"
#include "thirdparty/doctest.h"

TEST_CASE("system")
{
	// PATH should always be set right?
	const char *exists = "PATH";
	const char *doesnt_exist = "SOMETHING_THAT_HOPEFULLY_DOESNT_EXIST";

	SUBCASE("has_env")
	{
		CHECK(lib::system::has_env(exists));
		CHECK_FALSE(lib::system::has_env(doesnt_exist));
	}

	SUBCASE("env")
	{
		CHECK(lib::system::env(exists).size() > 0);
		CHECK(lib::system::env(doesnt_exist).empty());
	}
}
