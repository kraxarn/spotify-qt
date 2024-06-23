#include "lib/spotify/api.hpp"

// Currently unavailable:
// me/shows
// me/shows/contains

void lib::spt::api::saved_albums(const paged_callback<saved_album> &callback) const
{
	request.get_page<saved_album>("me/albums", {}, callback);
}

void lib::spt::api::add_saved_albums(const std::vector<std::string> &album_ids,
	lib::callback<std::string> &callback)
{
	put("me/albums", {
		{"ids", album_ids},
	}, callback);
}

void lib::spt::api::remove_saved_albums(const std::vector<std::string> &album_ids,
	lib::callback<std::string> &callback)
{
	del("me/albums", {
		{"ids", album_ids},
	}, callback);
}

void lib::spt::api::is_saved_album(const std::vector<std::string> &album_ids,
	lib::callback<std::vector<bool>> &callback)
{
	get(lib::fmt::format("me/albums/contains?ids={}",
		lib::strings::join(album_ids, ",")), callback);
}

void lib::spt::api::saved_tracks(lib::callback<std::vector<lib::spt::track>> &callback)
{
	get_items("me/tracks?limit=50", callback);
}

void lib::spt::api::saved_tracks(const lib::paged_callback<lib::spt::track> &callback)
{
	request.get_page<lib::spt::track>("me/tracks?limit=50", {}, callback);
}

void lib::spt::api::add_saved_tracks(const std::vector<std::string> &track_ids,
	lib::callback<std::string> &callback)
{
	put("me/tracks", {
		{"ids", track_ids},
	}, callback);
}

void lib::spt::api::remove_saved_tracks(const std::vector<std::string> &track_ids,
	lib::callback<std::string> &callback)
{
	del("me/tracks", {
		{"ids", track_ids},
	}, callback);
}

void lib::spt::api::is_saved_track(const std::vector<std::string> &track_ids,
	lib::callback<std::vector<bool>> &callback)
{
	get(lib::fmt::format("me/tracks/contains?ids={}",
		lib::strings::join(track_ids, ",")), callback);
}
