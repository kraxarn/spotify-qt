#include "lib/spotify/api.hpp"

void lib::spt::api::followed_artists(const paged_callback<spt::artist> &callback) const
{
	const std::string url("me/following?type=artist&limit=50");
	request.get_page<spt::artist>(url, "artists", callback);
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

void lib::spt::api::follow_playlist(const std::string &playlist_id,
	lib::callback<std::string> &callback)
{
	put(lib::fmt::format("playlists/{}/followers", playlist_id), callback);
}

void lib::spt::api::unfollow_playlist(const std::string &playlist_id,
	lib::callback<std::string> &callback)
{
	del(lib::fmt::format("playlists/{}/followers", playlist_id), callback);
}

void lib::spt::api::is_following_playlist(const std::string &playlist_id,
	const std::vector<std::string> &user_ids, lib::callback<std::vector<bool>> &callback)
{
	get(lib::fmt::format("playlists/{}/followers/contains?ids={}",
		playlist_id, lib::strings::join(user_ids, ",")), callback);
}
