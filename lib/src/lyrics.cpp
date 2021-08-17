#include "lib/lyrics.hpp"

lib::lyrics::lyrics(const lib::http_client &http_client)
	: http(http_client)
{
}

void lib::lyrics::get(const spt::track &track, lib::callback<lib::spt::track_info> &callback)
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
		http.post(url, body.dump(), headers,
			[url, callback](const std::string &result)
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
