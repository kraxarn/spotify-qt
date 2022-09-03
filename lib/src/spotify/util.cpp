#include "lib/spotify/util.hpp"
#include "lib/fmt.hpp"
#include "lib/uri.hpp"

auto lib::spt::to_full_url(const std::string &relative_url) -> std::string
{
	return lib::fmt::format("https://api.spotify.com/v1/{}", relative_url);
}

auto lib::spt::id_to_uri(const std::string &type, const std::string &spotify_id) -> std::string
{
	return lib::strings::starts_with(spotify_id, "spotify:")
		? spotify_id
		: lib::fmt::format("spotify:{}:{}", type, spotify_id);
}

auto lib::spt::uri_to_id(const std::string &spotify_uri) -> std::string
{
	auto index = spotify_uri.rfind(':');

	return index != std::string::npos
		? spotify_uri.substr(index + 1)
		: spotify_uri;
}

auto lib::spt::url_to_uri(const std::string &url) -> std::string
{
	const auto id_index = url.rfind('/');
	if (id_index == std::string::npos)
	{
		return {};
	}

	const auto type_index = url.rfind('/', id_index - 1);
	if (type_index == std::string::npos)
	{
		return {};
	}

	return lib::fmt::format("spotify:{}:{}",
		url.substr(type_index + 1, id_index - type_index - 1),
		url.substr(id_index + 1));
}

auto lib::spt::get_device_url(const std::string &url, const lib::spt::device &device) -> std::string
{
	lib::uri uri(lib::strings::starts_with(url, "https://")
		? url
		: to_full_url(url));

	auto params = uri.get_search_params();
	auto device_id = params.find("device_id");

	if (device_id != params.end())
	{
		device_id->second = device.id;
	}
	else
	{
		params.insert({
			"device_id",
			device.id,
		});
	}

	uri.set_search_params(params);
	auto pathname = uri.pathname();
	return lib::strings::remove(pathname, "/v1/");
}
