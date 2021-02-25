#include "spotifyapi.hpp"

QVector<Artist> Spotify::topArtists()
{
	return loadItems<Artist>("me/top/artists?limit=10");
}

QVector<Track> Spotify::topTracks()
{
	return loadItems<Track>("me/top/tracks?limit=50");
}
