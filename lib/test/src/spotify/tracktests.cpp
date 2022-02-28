#include "thirdparty/doctest.h"
#include "thirdparty/json.hpp"
#include "lib/spotify/track.hpp"

TEST_CASE("track")
{
	SUBCASE("json")
	{
		lib::spt::track track1;
		track1.id = "track_id";
		track1.name = "track_name";
		track1.is_local = true;
		track1.is_playable = false;
		track1.duration = 1;
		track1.added_at = "track_added";
		track1.album = lib::spt::entity("album_id", "album_name");
		track1.artists.emplace_back("artist_id", "artist_name");

		lib::spt::image image;
		image.width = lib::spt::image::size_small;
		image.height = image.width;
		image.url = "image_url";
		track1.images.push_back(image);

		const nlohmann::json track_json = track1;
		const lib::spt::track track2 = track_json;

		CHECK_EQ(track1.id, track2.id);
		CHECK_EQ(track1.name, track2.name);
		CHECK_EQ(track1.is_local, track2.is_local);
		CHECK_EQ(track1.is_playable, track2.is_playable);
		CHECK_EQ(track1.duration, track2.duration);
		CHECK_EQ(track1.added_at, track2.added_at);

		CHECK_EQ(track1.album.id, track2.album.id);
		CHECK_EQ(track1.album.name, track2.album.name);

		CHECK_EQ(track1.artists.size(), track2.artists.size());
		for (size_t i = 0; i < track1.artists.size(); i++)
		{
			const auto &artist1 = track1.artists.at(i);
			const auto &artist2 = track2.artists.at(i);

			CHECK_EQ(artist1.id, artist2.id);
			CHECK_EQ(artist1.name, artist2.name);
		}

		CHECK_EQ(track1.images.size(), track2.images.size());
		for (size_t i = 0; i < track1.images.size(); i++)
		{
			const auto &image1 = track1.images.at(i);
			const auto &image2 = track2.images.at(i);

			CHECK_EQ(image1.url, image2.url);
			CHECK_EQ(image1.width, image2.width);
			CHECK_EQ(image1.height, image2.height);
		}
	}
}
