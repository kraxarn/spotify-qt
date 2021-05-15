#include "thirdparty/doctest.h"
#include "lib/spotify/api.hpp"

TEST_CASE("spotify_api")
{
	SUBCASE("to_uri")
	{
		CHECK_EQ(lib::spt::api::to_uri("track", "4uLU6hMCjMI75M1A2tKUQC"),
			"spotify:track:4uLU6hMCjMI75M1A2tKUQC");

		CHECK_EQ(lib::spt::api::to_uri("track",
			"spotify:track:4uLU6hMCjMI75M1A2tKUQC"),
			"spotify:track:4uLU6hMCjMI75M1A2tKUQC");
	}

	SUBCASE("to_id")
	{
		CHECK_EQ(lib::spt::api::to_id("spotify:track:4uLU6hMCjMI75M1A2tKUQC"),
			"4uLU6hMCjMI75M1A2tKUQC");

		CHECK_EQ(lib::spt::api::to_id("4uLU6hMCjMI75M1A2tKUQC"),
			"4uLU6hMCjMI75M1A2tKUQC");
	}
}
