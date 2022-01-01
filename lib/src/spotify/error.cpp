
#include "lib/spotify/error.hpp"

lib::spt::error::error(const char *what, const char *url)
	: std::runtime_error(what),
	request_url(url)
{
}

lib::spt::error::error(const std::string &what, const std::string &url)
	: lib::spt::error(what.c_str(), url.c_str())
{
}

auto lib::spt::error::url() -> const char *
{
	return request_url;
}

auto lib::spt::error::is(const nlohmann::json &json) -> bool
{
	return !json.is_null()
		&& json.is_object()
		&& json.contains("error");
}

auto lib::spt::error::error_message(const nlohmann::json &json) -> std::string
{
	return lib::spt::error::is(json)
		? json.at("error").at("message").get<std::string>()
		: std::string();
}
