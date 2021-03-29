#include "spotifyapi.hpp"

std::vector<lib::spt::artist> Spotify::topArtists()
{
	return loadItemsAsJson<lib::spt::artist>("me/top/artists?limit=10");
}

std::vector<lib::spt::track> Spotify::topTracks()
{
	return loadItemsAsJson<lib::spt::track>("me/top/tracks?limit=50");
}
