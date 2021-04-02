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
	lib::spt::spotify_api::get(lib::fmt::format("artists/{}/top-tracks?country=from_token",
		artist.id), "tracks", callback);
}

void Spotify::relatedArtists(const lib::spt::artist &artist,
	lib::callback<std::vector<lib::spt::artist>> &callback)
{
	lib::spt::spotify_api::get(lib::fmt::format("artists/{}/related-artists",
		artist.id), "artists", callback);
}

void Spotify::albums(const lib::spt::artist &artist,
	lib::callback<std::vector<lib::spt::album>> &callback)
{
	lib::spt::spotify_api::get(lib::fmt::format("artists/{}/albums?country=from_token",
		artist.id), "items", callback);
}
