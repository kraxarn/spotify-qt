#include "lib/spotify/api.hpp"

using namespace lib::spt;

// Some calls are currently only available from the Playlist class

// Currently unavailable:
// users/{user_id}/playlists
// users/{user_id}/playlists
// playlists/{playlist_id}/tracks
// playlists/{playlist_id}/images

void api::playlists(lib::callback<std::vector<lib::spt::playlist>> &callback)
{
	get_items("me/playlists?limit=50", callback);
}

void api::playlist(const std::string &playlist_id,
	lib::callback<lib::spt::playlist> &callback)
{
	get(lib::fmt::format("playlists/{}", playlist_id), callback);
}

void api::edit_playlist(const std::string &playlist_id,
	const lib::spt::playlist_details &playlist,
	lib::callback<std::string> &callback)
{
	put(lib::fmt::format("playlists/{}", playlist_id), playlist, callback);
}

void api::playlist_tracks(const lib::spt::playlist &playlist,
	lib::callback<std::vector<lib::spt::track>> &callback)
{
	auto fetch = [this, callback](const std::string &url)
	{
		get_items(lib::strings::contains(url, "market=")
			? url : lib::fmt::format("{}{}market=from_token",
				url, lib::strings::contains(url, "?") ? "&" : "?"), callback);
	};

	if (playlist.tracks_href.empty())
	{
		this->playlist(playlist.id, [fetch](const lib::spt::playlist &newPlaylist)
		{
			fetch(newPlaylist.tracks_href);
		});
	}
	else
	{
		fetch(playlist.tracks_href);
	}
}

void api::add_to_playlist(const std::string &playlist_id, const std::string &track_id,
	lib::callback<std::string> &callback)
{
	post(lib::fmt::format("playlists/{}/tracks?uris={}",
		playlist_id, track_id), callback);
}

void api::remove_from_playlist(const std::string &playlist_id, const std::string &track_id,
	int pos, lib::callback<std::string> &callback)
{
	del(lib::fmt::format("playlists/{}/tracks", playlist_id), {
		{"tracks", {
			{
				{"uri", track_id},
				{"positions", {
					pos
				}}
			}
		}}
	}, callback);
}
