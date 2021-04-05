#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

// Currently unavailable:
// artists

void api::artist(const std::string &id,
	lib::callback<lib::spt::artist> &callback)
{
	get(lib::fmt::format("artists/{}", id), callback);
}

void api::top_tracks(const lib::spt::artist &artist,
	lib::callback<std::vector<lib::spt::track>> &callback)
{
	get(lib::fmt::format("artists/{}/top-tracks?country=from_token",
		artist.id), "tracks", callback);
}

void api::related_artists(const lib::spt::artist &artist,
	lib::callback<std::vector<lib::spt::artist>> &callback)
{
	get(lib::fmt::format("artists/{}/related-artists",
		artist.id), "artists", callback);
}

void api::albums(const lib::spt::artist &artist,
	lib::callback<std::vector<lib::spt::album>> &callback)
{
	get(lib::fmt::format("artists/{}/albums?country=from_token",
		artist.id), "items", callback);
}
