#include "lib/lyrics.hpp"
#include "lib/uri.hpp"

lib::lyrics::lyrics(const lib::http_client &http_client)
	: http(http_client)
{
}

void lib::lyrics::get(const lib::spt::track &track,
	lib::callback<std::vector<lib::lyrics_part>> &callback)
{
	// Based off:
	// https://github.com/spicetify/spicetify-cli/blob/master/CustomApps/lyrics-plus

	search(track, [this, callback](const int lyrics_id)
	{
		lib::log::debug("Found lyrics: {}", lyrics_id);
		lyric(lyrics_id, callback);
	});
}

auto lib::lyrics::headers() -> lib::headers
{
	return {
		{"Content-Type", "application/json"},
		{
			"User-Agent",
			"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/102.1",
		},
	};
}

void lib::lyrics::search(const lib::spt::track &track, lib::callback<int> &callback)
{
	lib::uri uri("https://music.xianqiao.wang/neteaseapiv2/search");
	uri.set_search_params({
		{"limit", "10"},
		{"type", "1"},
		{"keywords", lib::fmt::format("{} {}",
			lib::uri::encode(track.name),
			lib::uri::encode(track.artists.front().name))},
	});

	http.get(uri.get_url(), headers(), [callback](const std::string &response)
	{
		if (response.empty())
		{
			lib::log::debug("empty response");
			callback({});
			return;
		}

		const auto json = nlohmann::json::parse(response);
		const auto songs = json.at("result").at("songs");
		if (!songs.is_array() || songs.empty())
		{
			callback({});
			return;
		}

		callback(songs.front().at("id"));
	});
}

void lib::lyrics::lyric(int lyrics_id, lib::callback<std::vector<lib::lyrics_part>> &callback)
{
	lib::uri uri("https://music.xianqiao.wang/neteaseapiv2/lyric");
	uri.set_search_params({
		{"id", std::to_string(lyrics_id)},
	});

	http.get(uri.get_url(), headers(), [callback](const std::string &response)
	{
		if (response.empty())
		{
			callback({});
			return;
		}

		const auto json = nlohmann::json::parse(response);
		const auto lyric = json.at("lrc").at("lyric");
		const auto lines = lib::strings::split(lyric, '\n');

		std::vector<lib::lyrics_part> lyrics;
		lyrics.reserve(lines.size());

		for (const auto &line: lines)
		{
			lyrics.emplace_back(0, line);
		}

		callback(lyrics);
	});
}
