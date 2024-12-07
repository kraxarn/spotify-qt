#include "lib/spotify/api.hpp"

// Currently unavailable:
// tracks

void lib::spt::api::track(const std::string &track_id,
	lib::callback<lib::spt::track> &callback)
{
	get(lib::fmt::format("tracks/{}", track_id), callback);
}
