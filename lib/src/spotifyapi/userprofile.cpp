#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

// Currently unavailable:
// users/{user_id}

void spotify_api::me(lib::callback<lib::spt::user> &callback)
{
	get("me", callback);
}
