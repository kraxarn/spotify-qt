#include "spotifyapi.hpp"

QVector<Artist> Spotify::topArtists()
{
	return loadItems<Artist>("me/top/artists?limit=10");
}

std::vector<lib::spt::track> Spotify::topTracks()
{
	return loadItemsAsJson<lib::spt::track>("me/top/tracks?limit=50");
}
