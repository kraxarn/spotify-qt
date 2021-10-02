#include "lib/spotify/api.hpp"

// Unavailable:
// shows

void lib::spt::api::show(const std::string &show_id, lib::callback<lib::spt::show> &callback)
{
	get(lib::fmt::format("shows/{}", show_id), callback);
}

void lib::spt::api::show_episodes(const lib::spt::show &show,
	lib::callback<std::vector<lib::spt::episode>> &callback)
{
	get_items(lib::fmt::format("shows/{}/episodes", show.id), callback);
}
