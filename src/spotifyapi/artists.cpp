#include "spotifyapi.hpp"

// Currently unavailable:
// artists

void Spotify::artist(const std::string &artistId, lib::callback<lib::spt::artist> &callback)
{
	get(lib::fmt::format("artists/{}", artistId), callback);
}

void Spotify::topTracks(const lib::spt::artist &artist,
	lib::callback<std::vector<lib::spt::track>> &callback)
{
	get<lib::spt::track>(lib::fmt::format("artists/{}/top-tracks?country=from_token",
		artist.id), "tracks", callback);
}

void Spotify::relatedArtists(const lib::spt::artist &artist,
	lib::callback<std::vector<lib::spt::artist>> &callback)
{
	get<lib::spt::artist>(lib::fmt::format("artists/{}/related-artists", artist.id),
		"artists", callback);
}

void Spotify::albums(const lib::spt::artist &artist,
	lib::callback<std::vector<lib::spt::album>> &callback)
{
	get<lib::spt::album>(lib::fmt::format("artists/{}/albums?country=from_token",
		artist.id), "items", callback);
}
