#include "lib/lyrics.hpp"
#include "lib/uri.hpp"
#include "lib/spotify/api.hpp"

lib::lyrics::lyrics(const lib::http_client &http_client)
	: http(http_client)
{
}

void lib::lyrics::get(const spt::track &track, lib::callback<lib::spt::track_info> &callback)
{
	// Based off:
	// https://github.com/spicetify/spicetify-cli/blob/master/CustomApps/lyrics-plus

	const auto duration = track.duration / 1000.0;
	lib::uri uri("https://apic-desktop.musixmatch.com/ws/1.1/macro.subtitles.get");

	uri.set_search_params({
		{"format", "json"},
		{"namespace", "lyrics_richsynched"},
		{"subtitle_format", "mxm"},
		{"app_id", "web-desktop-app-v1.0"},
		{"q_album", lib::uri::encode(track.album.name)},
		{"q_artist", lib::uri::encode(track.artists.front().name)},
		{"q_artists", lib::uri::encode(track.artists.front().name)},
		{"q_track", lib::uri::encode(track.name)},
		{"track_spotify_id", lib::uri::encode(lib::spt::api::to_uri("track", track.id))},
		{"q_duration", std::to_string(duration)},
		{"f_subtitle_length", std::to_string(static_cast<int>(duration))},
		{"usertoken", "220731b2962dde74b84ecd92b9a36194267992d61b163d7ed443f0"},
	});

	lib::headers headers{
		{"authority", "apic-desktop.musixmatch.com"},
		{"cookie", "x-mxm-token-guid="},
	};

	try
	{
		http.post(uri.get_url(), headers, [callback](const std::string &result)
		{
			if (result.empty())
			{
				callback(lib::spt::track_info());
				return;
			}
			callback(nlohmann::json::parse(result));
		});
	}
	catch (const std::exception &e)
	{
		lib::log::error("Failed to get lyrics: {}", e.what());
		callback(lib::spt::track_info());
	}
}
