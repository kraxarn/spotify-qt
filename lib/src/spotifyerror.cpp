
#include "lib/spotifyerror.hpp"

lib::spotify_error::spotify_error(const char *what, const char *url)
	: std::runtime_error(what),
	request_url(url)
{
}

lib::spotify_error::spotify_error(const std::string &what, const std::string &url)
	: lib::spotify_error(what.c_str(), url.c_str())
{
}

const char *lib::spotify_error::url()
{
	return request_url;
}

bool lib::spotify_error::is(const nlohmann::json &json)
{
	return !json.is_null()
		&& json.is_object()
		&& json.contains("error");
}

std::string lib::spotify_error::error_message(const nlohmann::json &json)
{
	return lib::spotify_error::is(json)
		? json.at("error").at("message").get<std::string>()
		: std::string();
}
