#include "spotifyapi.hpp"

// Some calls are currently only available from the Playlist class

// Currently unavailable:
// users/{user_id}/playlists
// users/{user_id}/playlists
// playlists/{playlist_id}/tracks
// playlists/{playlist_id}/images

void Spotify::playlists(int offset, lib::callback<std::vector<lib::spt::playlist>> &callback)
{
	// Request playlists
	get(lib::fmt::format("me/playlists?limit=50&offset={}", offset),
		[this, callback](const nlohmann::json &json)
		{
			auto playlists = json.at("items").get<std::vector<lib::spt::playlist>>();
			// Paging
			if (json.contains("next") && !json.at("next").is_null())
			{
				this->playlists(json.at("offset").get<int>() + json["limit"].get<int>(),
					[playlists, callback](const std::vector<lib::spt::playlist> &results)
					{
						callback(lib::vector::combine(playlists, results));
					});
			}
			callback(playlists);
		});
}

void Spotify::playlists(lib::callback<std::vector<lib::spt::playlist>> &callback)
{
	playlists(0, callback);
}

void Spotify::playlist(const std::string &playlistId,
	lib::callback<lib::spt::playlist> &callback)
{
	get(lib::fmt::format("playlists/{}", playlistId), callback);
}

void Spotify::editPlaylist(const lib::spt::playlist &playlist,
	lib::callback<std::string> &callback)
{
	put(lib::fmt::format("playlists/%1", playlist.id), playlist, callback);
}

bool Spotify::playlistTracks(const lib::spt::playlist &playlist,
	std::vector<lib::spt::track> &trackList)
{
	if (playlist.tracks_href.empty())
	{
		lib::log::warn("Attempting to load tracks without href");
		return false;
	}

	// Load tracks
	auto href = lib::fmt::format("{}{}market=from_token",
		playlist.tracks_href,
		lib::strings::contains(playlist.tracks_href, '?') ? "&" : "?");

	return playlistTracks(playlist, trackList, href, 0);
}

std::vector<lib::spt::track> Spotify::playlistTracks(const lib::spt::playlist &playlist)
{
	std::vector<lib::spt::track> trackList;
	playlistTracks(playlist, trackList);
	return trackList;
}

bool Spotify::playlistTracks(const lib::spt::playlist &playlist,
	std::vector<lib::spt::track> &trackList, const std::string &url, int offset)
{
	// Load tracks from api
	auto newUrl = url.substr(std::string("https://api.spotify.com/v1/").size());
	auto current = getAsJson(newUrl);

	// No items, request probably failed
	if (!current.contains("items"))
		return false;

	// Load from url
	auto items = current["items"];
	for (auto &item : items)
		trackList.push_back(item.get<lib::spt::track>());

	// Check if there's a next page
	auto nextPage = current.contains("next") && !current.at("next").is_null()
		? current.at("next").get<std::string>()
		: std::string();
	if (!nextPage.empty())
		playlistTracks(playlist, trackList, nextPage, offset + items.size());

	return true;
}

void Spotify::addToPlaylist(const std::string &playlistId, const std::string &trackId,
	lib::callback<std::string> &callback)
{
	post(lib::fmt::format("playlists/{}/tracks?uris={}",
		playlistId, trackId), callback);
}

void Spotify::removeFromPlaylist(const std::string &playlistId, const std::string &trackId,
	int pos, lib::callback<std::string> &callback)
{
	del(lib::fmt::format("playlists/%1/tracks", playlistId), {
		{"tracks", {
			{"uri", trackId},
			{"position", {
				pos
			}}
		}}
	}, callback);
}