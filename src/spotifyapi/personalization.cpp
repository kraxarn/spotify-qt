#include "spotifyapi.hpp"

void Spotify::topArtists(const std::function<void(const std::vector<Artist> &artists)> &callback)
{
	get("me/top/artists?limit=10", callback);
}

void Spotify::topTracks(const std::function<void(const std::vector<Track> &tracks)> &callback)
{
	get("me/top/tracks?limit=50", callback);
}
