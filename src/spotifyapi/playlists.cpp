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

void Spotify::playlistTracks(const lib::spt::playlist &playlist,
	lib::callback<std::vector<lib::spt::track>> &callback)
{
	auto fetch = [this, callback](const std::string &url)
	{
		lib::spt::spotify_api::get(url, "items", callback);
	};

	if (playlist.tracks_href.empty())
	{
		this->playlist(playlist.id, [fetch](const lib::spt::playlist &newPlaylist)
		{
			fetch(newPlaylist.tracks_href);
		});
	}
	else
		fetch(playlist.tracks_href);
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