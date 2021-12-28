#include "lib/spotify/api.hpp"

// Currently unavailable:
// users/{user_id}

void lib::spt::api::me(lib::callback<lib::spt::user> &callback)
{
	get("me", callback);
}
