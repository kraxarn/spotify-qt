#include "lib/spotify/api.hpp"

void lib::spt::api::top_artists(lib::callback<std::vector<lib::spt::artist>> &callback)
{
	get_items("me/top/artists?limit=10", callback);
}

void lib::spt::api::top_tracks(lib::callback<std::vector<lib::spt::track>> &callback)
{
	get_items("me/top/tracks?limit=50", callback);
}
