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
