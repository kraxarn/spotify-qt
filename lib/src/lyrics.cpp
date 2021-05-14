#include "lib/lyrics.hpp"

lib::lyrics::lyrics(const lib::http_client &http_client)
	: http(http_client)
{
}

void lib::lyrics::get(const spt::track &track, lib::result<std::string> &callback)
{
	std::string url = "spotify-lyrics.azurewebsites.net";
	nlohmann::json body{
		{"artist", track.artists.front().name},
		{"name", track.name},
	};

	try
	{
		http.put(url, body, lib::headers(), [url, callback]
			(const nlohmann::json &result)
		{
			if (result.is_null())
			{
				callback(false, "No response");
				return;
			}

			const auto &lyrics = result.at("lyrics");
			if (lyrics.is_null())
			{
				callback(false, "No results");
				return;
			}

			callback(true, lyrics.get<std::string>());
		});
	}
	catch (const std::exception &e)
	{
		callback(false, e.what());
	}
}

auto lib::lyrics::format(const std::string &str) -> std::string
{
	auto val = lib::strings::replace_all(str, ' ', '-');
	val.erase(std::remove_if(val.begin(), val.end(), [](unsigned char c) -> bool
	{
		return c != '-' && std::isalpha(c) == 0;
	}), val.end());
	return val;
}

auto lib::lyrics::get_from_lyrics(const std::string &lyrics) -> std::string
{
	std::string start_str = "<div class=\"lyrics\">";
	auto start = lyrics.find(start_str);
	auto end = lyrics.find("</div>", start);

	if (start == std::string::npos
		|| end == std::string::npos)
	{
		return std::string();
	}

	return lyrics.substr(start + start_str.size(), end);
}

auto lib::lyrics::get_from_lyrics_root(const std::string &/*lyrics*/) -> std::string
{
	return std::string();
}
