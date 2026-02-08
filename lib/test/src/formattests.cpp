#include "thirdparty/doctest.h"
#include "lib/format.hpp"
#include "lib/spotify/artist.hpp"
#include "lib/spotify/track.hpp"

TEST_CASE("Format")
{
	SUBCASE("time")
	{
		CHECK_EQ(Format::time(0), QStringLiteral("0:00"));
		CHECK_EQ(Format::time(999), QStringLiteral("0:00"));
		CHECK_EQ(Format::time(5000), QStringLiteral("0:05"));
		CHECK_EQ(Format::time(60 * 1000), QStringLiteral("1:00"));
		CHECK_EQ(Format::time(65 * 1000), QStringLiteral("1:05"));
	}

	SUBCASE("timePretty")
	{
		CHECK_EQ(Format::timePretty(0), QStringLiteral("0 m"));
		CHECK_EQ(Format::timePretty(59 * 1000 * 60), QStringLiteral("59 m"));
		CHECK_EQ(Format::timePretty(60 * 1000 * 60), QStringLiteral("1 h"));
		CHECK_EQ(Format::timePretty(61 * 1000 * 60), QStringLiteral("1 h 1 m"));
	}

	SUBCASE("size")
	{
		CHECK_EQ(Format::size(1), QStringLiteral("1 B"));
		CHECK_EQ(Format::size(1000), QStringLiteral("1 kB"));
		CHECK_EQ(Format::size(1000000), QStringLiteral("1 MB"));
		CHECK_EQ(Format::size(1000000000), QStringLiteral("1 GB"));
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

		CHECK_EQ(Format::title(track, QStringLiteral("{track}")),
			QStringLiteral("track"));
		CHECK_EQ(Format::title(track, QStringLiteral("{track")),
			QStringLiteral("{track"));
		CHECK_EQ(Format::title(track, QStringLiteral("prefix - {track}")),
			QStringLiteral("prefix - track"));
		CHECK_EQ(Format::title(track, QStringLiteral("{track} - suffix")),
			QStringLiteral("track - suffix"));
		CHECK_EQ(Format::title(track, QStringLiteral("{track} - {invalid}")),
			QStringLiteral("track - {invalid}"));

		CHECK_EQ(Format::title(track, QStringLiteral("{artist} - {track}")),
			QStringLiteral("artist1 - track"));
		CHECK_EQ(Format::title(track, QStringLiteral("{artists} - {track}")),
			QStringLiteral("artist1, artist2 - track"));

		CHECK_EQ(Format::title(track, QStringLiteral("")),
			QString::fromStdString(track.title()));
	}
}
