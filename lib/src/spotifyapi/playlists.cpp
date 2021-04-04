#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

// Some calls are currently only available from the Playlist class

// Currently unavailable:
// users/{user_id}/playlists
// users/{user_id}/playlists
// playlists/{playlist_id}/tracks
// playlists/{playlist_id}/images

void spotify_api::playlists(lib::callback<std::vector<lib::spt::playlist>> &callback)
{
	get("me/playlists?limit=50", "items", callback);
}

void spotify_api::playlist(const std::string &playlist_id,
	lib::callback<lib::spt::playlist> &callback)
{
	get(lib::fmt::format("playlists/{}", playlist_id), callback);
}

void spotify_api::edit_playlist(const lib::spt::playlist &playlist,
	lib::callback<std::string> &callback)
{
	put(lib::fmt::format("playlists/{}", playlist.id), playlist, callback);
}

void spotify_api::playlist_tracks(const lib::spt::playlist &playlist,
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

void spotify_api::add_to_playlist(const std::string &playlist_id, const std::string &track_id,
	lib::callback<std::string> &callback)
{
	post(lib::fmt::format("playlists/{}/tracks?uris={}",
		playlist_id, track_id), callback);
}

void spotify_api::remove_from_playlist(const std::string &playlist_id, const std::string &track_id,
	int pos, lib::callback<std::string> &callback)
{
	del(lib::fmt::format("playlists/%1/tracks", playlist_id), {
		{"tracks", {
			{"uri", track_id},
			{"position", {
				pos
			}}
		}}
	}, callback);
}
