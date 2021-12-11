#include "lib/spotify/api.hpp"

// Currently unavailable:
// users/{user_id}/playlists
// users/{user_id}/playlists
// playlists/{playlist_id}/tracks
// playlists/{playlist_id}/images

void api::create_playlist(const std::string &name,
	const std::string &description, const bool is_public,
	const bool is_collaborative, lib::callback<lib::spt::playlist> &callback)
{
	post("me/playlists", {
			{"name", name},
			{"description", description},
			{"public", is_public},
			{"collaborative", is_collaborative},
		}, callback);
}

void lib::spt::api::playlists(lib::callback<std::vector<lib::spt::playlist>> &callback)
{
	get_items("me/playlists?limit=50", callback);
}

void lib::spt::api::playlist(const std::string &playlist_id,
	lib::callback<lib::spt::playlist> &callback)
{
	get(lib::fmt::format("playlists/{}", playlist_id), callback);
}

void lib::spt::api::edit_playlist(const std::string &playlist_id,
	const lib::spt::playlist_details &playlist,
	lib::callback<std::string> &callback)
{
	put(lib::fmt::format("playlists/{}", playlist_id), playlist, callback);
}

void lib::spt::api::playlist_tracks(const lib::spt::playlist &playlist,
	lib::callback<std::vector<lib::spt::track>> &callback)
{
	auto fetch = [this, callback](const std::string &url)
	{
		auto item_url = lib::strings::contains(url, "market=")
			? url : lib::fmt::format("{}{}market=from_token",
				url, lib::strings::contains(url, "?") ? "&" : "?");
		get_items(item_url, callback);
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

void lib::spt::api::add_to_playlist(const std::string &playlist_id, const std::string &track_id,
	lib::callback<std::string> &callback)
{
	post(lib::fmt::format("playlists/{}/tracks?uris={}",
		playlist_id, track_id), callback);
}

void lib::spt::api::remove_from_playlist(const std::string &playlist_id,
	const std::string &track_id, int pos,
	lib::callback<std::string> &callback)
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
