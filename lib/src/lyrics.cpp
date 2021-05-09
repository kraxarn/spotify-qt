#include "lib/lyrics.hpp"

lib::lyrics::lyrics(const lib::http_client &http_client)
	: http(http_client)
{
}

void lib::lyrics::get(const spt::track &track, callback<std::string> &callback)
{
	auto artist = lib::strings::capitalize(format(track.artists.front().name));
	auto name = lib::strings::to_lower(format(track.name));

	http.get(lib::fmt::format("https://genius.com/{}-{}-lyrics",
		artist, name), lib::headers(), [callback](const std::string &body)
	{
		if (body.empty())
		{
			callback("No response");
			return;
		}

		std::string start_str = "song_body-lyrics";
		auto start = body.find(start_str);
		if (start == std::string::npos)
		{
			callback("No results");
			return;
		}

		callback(body.substr(start + start_str.size()));
	});
}

auto lib::lyrics::format(const std::string &str) -> std::string
{
	return lib::strings::replace_all(str, ' ', '-');
}
