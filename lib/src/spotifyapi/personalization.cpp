#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

void api::top_artists(lib::callback<std::vector<lib::spt::artist>> &callback)
{
	get_items("me/top/artists?limit=10", callback);
}

void api::top_tracks(lib::callback<std::vector<lib::spt::track>> &callback)
{
	get_items("me/top/tracks?limit=50", callback);
}
