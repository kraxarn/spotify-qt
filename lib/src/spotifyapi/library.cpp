#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

// Currently unavailable:
// me/albums
// me/albums/contains
// me/tracks/contains
// me/shows
// me/shows/contains

void spotify_api::saved_albums(lib::callback<std::vector<lib::spt::album>> &callback)
{
	get("me/albums", "items", callback);
}

void spotify_api::saved_tracks(lib::callback<std::vector<lib::spt::track>> &callback)
{
	get("me/tracks?limit=50", "items", callback);
}

void spotify_api::add_saved_track(const std::string &trackId,
	lib::callback<std::string> &callback)
{
	put("me/tracks", {
		{"ids", {
			lib::strings::starts_with(trackId, "spotify:track:")
				? trackId.substr(0, std::string("spotify:track").size())
				: trackId
		}}
	}, callback);
}

void spotify_api::remove_saved_track(const std::string &trackId,
	lib::callback<std::string> &callback)
{
	del("me/tracks", {
		{"ids", {
			lib::strings::starts_with(trackId, "spotify:track:")
				? trackId.substr(0, std::string("spotify:track").size())
				: trackId
		}}
	}, callback);
}

void spotify_api::is_saved_track(const std::vector<std::string> &trackIds,
	lib::callback<std::vector<bool>> &callback)
{
	get(lib::fmt::format("me/tracks/contains?ids={}",
		lib::strings::join(trackIds, ",")), callback);
}
