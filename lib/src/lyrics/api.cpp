#include "lib/lyrics/api.hpp"
#include "lib/uri.hpp"

lib::lrc::api::api(const lib::http_client &http_client)
	: http(http_client)
{
}

auto lib::lrc::api::headers() -> lib::headers
{
	return {
		{"Content-Type", "application/json"},
		{
			"User-Agent",
			"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/102.1",
		},
	};
}

void lib::lrc::api::search(const lib::spt::track &track,
	lib::callback<lib::result<std::vector<lib::lrc::search_result>>> &callback)
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
			callback(lib::result<std::vector<lib::lrc::search_result>>::fail("No response"));
			return;
		}

		const auto json = nlohmann::json::parse(response);
		if (!json.contains("result"))
		{
			callback(lib::result<std::vector<lib::lrc::search_result>>::fail("No results"));
			return;
		}

		const auto songs = json.at("result").at("songs");
		if (!songs.is_array() || songs.empty())
		{
			callback(lib::result<std::vector<lib::lrc::search_result>>::fail("No lyrics found"));
			return;
		}

		std::vector<lib::lrc::search_result> results;
		results.reserve(songs.size());

		for (const auto &song: songs)
		{
			results.push_back(song);
		}

		callback(lib::result<std::vector<lib::lrc::search_result>>::ok(results));
	});
}

void lib::lrc::api::lyrics(int lyrics_id, lib::callback<lib::result<lib::lrc::lyrics>> &callback)
{
	lib::uri uri("https://music.xianqiao.wang/neteaseapiv2/lyric");
	uri.set_search_params({
		{"id", std::to_string(lyrics_id)},
	});

	http.get(uri.get_url(), headers(), [callback](const std::string &response)
	{
		if (response.empty())
		{
			callback(lib::result<lib::lrc::lyrics>::fail("No response"));
			return;
		}

		const auto json = nlohmann::json::parse(response);
		if (!json.contains("lrc"))
		{
			callback(lib::result<lib::lrc::lyrics>::fail("No lyrics"));
			return;
		}

		const auto lyric = json.at("lrc").at("lyric");
		callback(lib::result<lib::lrc::lyrics>::ok(lyric));
	});
}


