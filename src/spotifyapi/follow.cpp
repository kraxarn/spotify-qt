#include "spotifyapi.hpp"

// Currently unavailable:
// playlists/{playlist_id}/followers
// playlists/{playlist_id}/followers/contains

std::vector<lib::spt::artist> Spotify::followedArtists(const std::string &offset)
{
	auto json = getAsJson(lib::fmt::format("me/following?type=artist&limit=50{}",
		offset.empty() ? "" : lib::fmt::format("&after={}", offset))).at("artists");
	auto items = json["items"];
	std::vector<lib::spt::artist> artists;
	for (auto &item : items)
		artists.push_back(item);
	if (!json.at("cursors").is_null() && !json.at("cursors").at("after").is_null())
	{
		lib::vector::append(artists,
			followedArtists(json.at("cursors").at("after").get<std::string>()));
	}
	return artists;
}

void Spotify::follow(lib::follow_type type, const std::vector<std::string> &ids,
	lib::callback<std::string> &callback)
{
	lib::spt::spotify_api::put(lib::fmt::format("me/following?type={}&ids={}",
		follow_type_string(type), lib::strings::join(ids, ",")), callback);
}

void Spotify::unfollow(lib::follow_type type, const std::vector<std::string> &ids,
	lib::callback<std::string> &callback)
{
	lib::spt::spotify_api::del(lib::fmt::format("me/following?type={}&ids={}",
		follow_type_string(type), lib::strings::join(ids, ",")), callback);
}

void Spotify::isFollowing(lib::follow_type type, const std::vector<std::string> &ids,
	lib::callback<std::vector<bool>> &callback)
{
	get(lib::fmt::format("me/following/contains?type={}&ids={}",
		follow_type_string(type), lib::strings::join(ids, "")), callback);
}
