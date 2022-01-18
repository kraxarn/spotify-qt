#include "lib/search/api.hpp"
#include "lib/uri.hpp"

lib::ddg::api::api(const lib::http_client &http_client)
	: http(http_client)
{
}

void lib::ddg::api::search(const lib::spt::artist &artist,
	lib::callback<lib::ddg::results> &callback)
{
	const auto query = lib::uri::encode(artist.name);

	const auto url = lib::fmt::format("https://api.duckduckgo.com"
									  "?q={}"
									  "&format=json"
									  "&t=spotify-qt", query);

	http.get(url, lib::headers(), [callback](const std::string &result)
	{
		try
		{
			callback(nlohmann::json::parse(result));
		}
		catch (const std::exception &e)
		{
			lib::log::error("Search failed: {}", e.what());
			callback({});
		}
	});
}
