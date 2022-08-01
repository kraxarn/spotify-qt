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
		if (lyrics_id == 0)
		{
			callback({});
			return;
		}
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
			callback(0);
			return;
		}

		const auto json = nlohmann::json::parse(response);
		if (!json.contains("result"))
		{
			callback(0);
			return;
		}

		const auto songs = json.at("result").at("songs");
		if (!songs.is_array() || songs.empty())
		{
			callback(0);
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
		if (!json.contains("lrc"))
		{
			callback({});
			return;
		}

		const auto lyric = json.at("lrc").at("lyric");
		const auto lines = lib::strings::split(lyric, '\n');

		std::vector<lib::lyrics_part> lyrics;
		lyrics.reserve(lines.size());

		for (const auto &line: lines)
		{
			const auto timestamp_start = line.find('[');
			const auto timestamp_end = line.find(']');

			long timestamp;

			if (timestamp_start == std::string::npos
				|| timestamp_end == std::string::npos)
			{
				timestamp = 0;
			}
			else
			{
				const auto length = timestamp_end - timestamp_start - 2;
				timestamp = parse_timestamp(line.substr(timestamp_start + 1, length));
			}

			std::string text;
			if (line.empty())
			{
				text = "♪";
			}
			else if (timestamp_end != std::string::npos)
			{
				text = line.substr(timestamp_end + 1);
			}
			else
			{
				text = line;
			}

			lyrics.emplace_back(timestamp, text);
		}

		callback(lyrics);
	});
}

auto lib::lyrics::parse_timestamp(const std::string &timestamp) -> long
{
	// minute:second.millisecond

	const auto second = timestamp.find(':');
	const auto millisecond = timestamp.find('.');

	if (second == std::string::npos
		|| millisecond == std::string::npos)
	{
		return 0;
	}

	const auto minutes = std::stoi(timestamp.substr(0, 2));
	const auto seconds = std::stoi(timestamp.substr(second + 1, 2));
	const auto milliseconds = std::stoi(timestamp.substr(millisecond + 1, 3));

	constexpr int milliseconds_in_second = 1000;
	constexpr int milliseconds_in_minute = milliseconds_in_second * 60;

	return minutes * milliseconds_in_minute
		+ seconds * milliseconds_in_second
		+ milliseconds;
}
