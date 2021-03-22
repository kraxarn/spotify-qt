#include "spotifyapi.hpp"

// Currently unavailable:
// users/{user_id}

void Spotify::me(lib::callback<lib::spt::user> &callback)
{
	get("me", callback);
}
