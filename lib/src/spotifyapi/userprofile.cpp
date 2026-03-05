#include "lib/spotify/api.hpp"

void lib::spt::api::me(ApiCallback<lib::spt::user> &callback)
{
	get("me", callback);
}
