#include "lib/spotify/api.hpp"

// Currently unavailable:
// users/{user_id}

void lib::spt::api::me(ApiCallback<lib::spt::user> &callback)
{
	get("me", callback);
}
