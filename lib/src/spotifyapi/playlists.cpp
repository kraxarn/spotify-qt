#include "lib/spotify/api.hpp"

// Currently unavailable:
// users/{user_id}/playlists
// users/{user_id}/playlists
// playlists/{playlist_id}/tracks
// playlists/{playlist_id}/images

void lib::spt::api::create_playlist(const std::string &name,
	const lib::optional<std::string> &description,
	const lib::optional<bool> &is_public,
	const lib::optional<bool> &is_collaborative,
	lib::callback<lib::spt::playlist> &callback)
{
	nlohmann::json json{
		{"name", name},
	};

	lib::json::set(json, "description", description);
	lib::json::set(json, "public", is_public);
	lib::json::set(json, "collaborative", is_collaborative);

	post("me/playlists", json, callback);
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

void lib::spt::api::add_to_playlist(const std::string &playlist_id,
	const std::vector<std::string> &track_uris,
	lib::callback<std::string> &callback)
{
	post(lib::fmt::format("playlists/{}/tracks?uris={}",
		playlist_id, lib::strings::join(track_uris, ",")), callback);
}

void lib::spt::api::remove_from_playlist(const std::string &playlist_id,
	const std::vector<std::pair<int, std::string>> &track_index_uris,
	lib::callback<std::string> &callback)
{
	auto tracks = nlohmann::json::array();

	for (const auto &track: track_index_uris)
	{
		tracks.push_back({
			{"uri", track.second},
			{"positions", {
				track.first,
			}},
		});
	}

	del(lib::fmt::format("playlists/{}/tracks", playlist_id), {
		{"tracks", tracks},
	}, callback);
}
