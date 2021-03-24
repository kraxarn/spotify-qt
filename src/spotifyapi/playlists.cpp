#include "spotifyapi.hpp"

// Some calls are currently only available from the Playlist class

// Currently unavailable:
// users/{user_id}/playlists
// users/{user_id}/playlists
// playlists/{playlist_id}/tracks
// playlists/{playlist_id}/images

std::vector<lib::spt::playlist> Spotify::playlists(int offset)
{
	// Request playlists
	auto json = getAsJson(QString("me/playlists?limit=50&offset=%1").arg(offset));

	// Parse as playlists
	auto items = json["items"].items();

	// Create list of playlists
	std::vector<lib::spt::playlist> playlists;

	// Loop through all items
	for (auto &item : items)
		playlists.push_back(item.value());

	// Paging
	if (json.contains("next") && !json["next"].is_null())
	{
		lib::vector::append(playlists,
			this->playlists(json["offset"].get<int>() + json["limit"].get<int>()));
	}

	return playlists;
}

lib::spt::playlist Spotify::playlist(const std::string &playlistId)
{
	return getAsJson(QString::fromStdString(lib::fmt::format("playlists/{}",
		playlistId)));
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
		lib::strings::contains(playlist.tracks_href, '?') ? '&' : '?');

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
	auto newUrl = url.substr(0, std::string("https://api.spotify.com/v1/").size());
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

QString Spotify::addToPlaylist(const std::string &playlistId, const std::string &trackId)
{
	return post(QString("playlists/%1/tracks?uris=%2")
		.arg(QString::fromStdString(playlistId))
		.arg(QString::fromStdString(trackId)));
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