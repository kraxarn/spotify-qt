#include "thirdparty/doctest.h"
#include "lib/format.hpp"
#include "lib/spotify/artist.hpp"
#include "lib/spotify/track.hpp"

TEST_CASE("fmt::format")
{
	SUBCASE("time")
	{
		CHECK_EQ(lib::format::time(0), "0:00");
		CHECK_EQ(lib::format::time(999), "0:00");
		CHECK_EQ(lib::format::time(5000), "0:05");
		CHECK_EQ(lib::format::time(60 * 1000), "1:00");
		CHECK_EQ(lib::format::time(65 * 1000), "1:05");
	}

	SUBCASE("time_pretty")
	{
		CHECK_EQ(lib::format::time_pretty(0), "0 m");
		CHECK_EQ(lib::format::time_pretty(59 * 1000 * 60), "59 m");
		CHECK_EQ(lib::format::time_pretty(60 * 1000 * 60), "1 h");
		CHECK_EQ(lib::format::time_pretty(61 * 1000 * 60), "1 h 1 m");
	}

	SUBCASE("size")
	{
		CHECK_EQ(lib::format::size(1), "1 B");
		CHECK_EQ(lib::format::size(1000), "1 kB");
		CHECK_EQ(lib::format::size(1000000), "1 MB");
		CHECK_EQ(lib::format::size(1000000000), "1 GB");
	}

	SUBCASE("title")
	{
		lib::spt::track track;
		track.name = "track";

		lib::spt::artist artist1;
		artist1.name = "artist1";
		track.artists.push_back(artist1);

		lib::spt::artist artist2;
		artist2.name = "artist2";
		track.artists.push_back(artist2);

		CHECK_EQ(lib::format::title(track, "{track}"), "track");
		CHECK_EQ(lib::format::title(track, "{track"), "{track");
		CHECK_EQ(lib::format::title(track, "prefix - {track}"), "prefix - track");
		CHECK_EQ(lib::format::title(track, "{track} - suffix"), "track - suffix");
		CHECK_EQ(lib::format::title(track, "{track} - {invalid}"), "track - {invalid}");

		CHECK_EQ(lib::format::title(track, "{artist} - {track}"), "artist1 - track");
		CHECK_EQ(lib::format::title(track, "{artists} - {track}"), "artist1, artist2 - track");
	}
}
