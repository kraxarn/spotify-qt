#include "lib/spotify/api.hpp"

void lib::spt::api::track(const std::string &track_id,
	ApiCallback<lib::spt::track> &callback)
{
	get(lib::fmt::format("tracks/{}", track_id), callback);
}
