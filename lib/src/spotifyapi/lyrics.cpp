#include "lib/spotify/api.hpp"

void lib::spt::api::lyrics(const lib::spt::track &track,
	lib::callback<std::string> &callback)
{
	// Can be fetched from dev tools in web client
	auto headers = auth_headers();
	headers["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64; rv:96.0) Gecko/20100101 Firefox/96.0";
	headers["Accept"] = "application/json";
	headers["Accept-Language"] = "en";
	headers["Referer"] = "https://open.spotify.com/";
	headers["app-platform"] = "WebPlayer";
	headers["spotify-app-version"] = "1.1.76.64.gfb5febfc";
	headers["Origin"] = "https://open.spotify.com";
	headers["DNT"] = "1";
	headers["Sec-Fetch-Dest"] = "empty";
	headers["Sec-Fetch-Mode"] = "cors";
	headers["Sec-Fetch-Site"] = "same-site";
	headers["Sec-GPC"] = "1";
	headers["Connection"] = "keep-alive";

	http.get(lib::fmt::format(
			"https://spclient.wg.spotify.com/color-lyrics/v2/track/{}/image/{}"
			"?format=json"
			"&vocalRemoval=false"
			"&market=from_token",
			track.id, track.image), headers,
		[callback](const std::string &response)
		{
			try
			{
				lib::log::debug("{}", nlohmann::json::parse(response).dump());
			}
			catch (const std::exception &e)
			{
				lib::log::debug(response);
			}
		});
}
