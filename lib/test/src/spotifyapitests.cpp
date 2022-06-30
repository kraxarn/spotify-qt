#include "thirdparty/doctest.h"
#include "lib/spotify/api.hpp"

TEST_CASE("spt::api")
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

	SUBCASE("url_to_id")
	{
		CHECK_EQ(lib::spt::api::url_to_id("https://open.spotify.com/track/4uLU6hMCjMI75M1A2tKUQC"),
			"4uLU6hMCjMI75M1A2tKUQC");

		CHECK_EQ(lib::spt::api::url_to_id("4uLU6hMCjMI75M1A2tKUQC"),
			"4uLU6hMCjMI75M1A2tKUQC");
	}

	SUBCASE("get_device_url")
	{
		lib::spt::device device;
		device.id = "device_id";
		std::string base_url = "me/player/play";

		CHECK_EQ(lib::spt::api::get_device_url(base_url, device),
			lib::fmt::format("{}?device_id={}", base_url, device.id));

		CHECK_EQ(lib::spt::api::get_device_url(lib::fmt::format(
			"{}?offset=0",
			base_url), device),
			lib::fmt::format("{}?device_id={}&offset=0", base_url, device.id));

		CHECK_EQ(lib::spt::api::get_device_url(lib::fmt::format(
			"{}?offset=0&device_id=invalid_device",
			base_url), device),
			lib::fmt::format("{}?device_id={}&offset=0", base_url, device.id));
	}
}
