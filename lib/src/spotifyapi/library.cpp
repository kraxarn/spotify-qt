#include "lib/spotify/api.hpp"

// Currently unavailable:
// me/albums/contains
// me/shows
// me/shows/contains

void lib::spt::api::saved_albums(lib::callback<std::vector<lib::spt::saved_album>> &callback)
{
	get_items("me/albums", callback);
}

void lib::spt::api::saved_tracks(lib::callback<std::vector<lib::spt::track>> &callback)
{
	get_items("me/tracks?limit=50", callback);
}

void lib::spt::api::add_saved_tracks(const std::vector<std::string> &track_ids,
	lib::callback<std::string> &callback)
{
	put("me/tracks", {
		{"ids", track_ids},
	}, callback);
}

void lib::spt::api::remove_saved_track(const std::string &track_id,
	lib::callback<std::string> &callback)
{
	del("me/tracks", {
		{"ids", {
			lib::spt::api::to_id(track_id),
		}}
	}, callback);
}

void lib::spt::api::is_saved_track(const std::vector<std::string> &track_ids,
	lib::callback<std::vector<bool>> &callback)
{
	get(lib::fmt::format("me/tracks/contains?ids={}",
		lib::strings::join(track_ids, ",")), callback);
}
