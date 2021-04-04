#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

void spotify_api::top_artists(lib::callback<std::vector<lib::spt::artist>> &callback)
{
	get("me/top/artists?limit=10", "items", callback);
}

void spotify_api::top_tracks(lib::callback<std::vector<lib::spt::track>> &callback)
{
	get("me/top/tracks?limit=50", "items", callback);
}
