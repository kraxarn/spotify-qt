#include "lib/lyrics.hpp"

lib::lyrics::lyrics(const lib::http_client &http_client)
	: http(http_client)
{
}

void lib::lyrics::get(const spt::track &track, lib::result<std::string> &callback)
{
	std::string url = "https://spotify-lyrics.azurewebsites.net/lyrics";
	nlohmann::json body{
		{"artist", track.artists.front().name},
		{"name", track.name},
	};
	lib::headers headers{
		{"Content-Type", "application/json"},
	};

	try
	{
		http.post(url, body.dump(), lib::headers(), [url, callback]
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
