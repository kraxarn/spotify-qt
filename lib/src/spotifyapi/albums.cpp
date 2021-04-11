#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

void api::album(const std::string &id, lib::callback<lib::spt::album> &callback)
{
	get(lib::fmt::format("albums/{}", id), callback);
}

void api::album_tracks(const lib::spt::album &album,
	lib::callback<std::vector<lib::spt::track>> &callback)
{
	get_items(lib::fmt::format("albums/{}/tracks?limit=50", album.id),
		[album, callback](const std::vector<lib::spt::track> &results)
		{
			std::vector<lib::spt::track> tracks;
			tracks.reserve(results.size());
			for (const auto &result : results)
			{
				lib::spt::track track = result;
				track.album.name = album.name;
				tracks.push_back(track);
			}
			callback(tracks);
		});
}
