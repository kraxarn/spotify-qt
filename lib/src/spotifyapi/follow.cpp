#include "lib/spotify/api.hpp"

// Currently unavailable:
// playlists/{playlist_id}/followers
// playlists/{playlist_id}/followers/contains

void lib::spt::api::followed_artists(lib::callback<std::vector<lib::spt::artist>> &callback)
{
	get_items("me/following?type=artist&limit=50", "artists", callback);
}

void lib::spt::api::follow(lib::follow_type type, const std::vector<std::string> &ids,
	lib::callback<std::string> &callback)
{
	put(lib::fmt::format("me/following?type={}&ids={}",
		follow_type_string(type), lib::strings::join(ids, ",")), callback);
}

void lib::spt::api::unfollow(lib::follow_type type, const std::vector<std::string> &ids,
	lib::callback<std::string> &callback)
{
	del(lib::fmt::format("me/following?type={}&ids={}",
		follow_type_string(type), lib::strings::join(ids, ",")), callback);
}

void lib::spt::api::is_following(lib::follow_type type, const std::vector<std::string> &ids,
	lib::callback<std::vector<bool>> &callback)
{
	get(lib::fmt::format("me/following/contains?type={}&ids={}",
		follow_type_string(type), lib::strings::join(ids, "")), callback);
}
