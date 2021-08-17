#include "lib/spotify/auth.hpp"

lib::spt::auth::auth(lib::settings &settings, const lib::http_client &http_client)
	: settings(settings),
	http_client(http_client)
{
}

auto lib::spt::auth::url(const std::string &client_id,
	const std::string &redirect_url) -> std::string
{
	// Scopes for request, for clarity
	std::vector<std::string> scopes = {
		"playlist-read-collaborative",
		"playlist-read-private",
		"playlist-modify-private",
		"playlist-modify-public",
		"user-follow-read",
		"user-follow-modify",
		"user-library-modify",
		"user-library-read",
		"user-modify-playback-state",
		"user-read-currently-playing",
		"user-read-playback-state",
		"user-read-private",
		"user-read-recently-played",
		"streaming",
		"user-read-email",
		"user-top-read"
	};

	// Prepare url
	return lib::fmt::format("https://accounts.spotify.com/authorize"
							"?client_id={}&response_type=code"
							"&redirect_uri={}&scope={}",
		client_id, redirect_url, lib::strings::join(scopes, "%20"));
}

auto lib::spt::auth::get(const std::string &code, const std::string &redirect_url,
	const std::string &id, const std::string &secret) -> std::string
{
	if (code.empty())
	{
		return "No code specified";
	}

	// Prepare form to send
	auto post_data = lib::fmt::format("grant_type=authorization_code&code={}&redirect_uri={}",
		code, redirect_url);

	// Prepare request
	lib::headers headers = {
		{"Content-Type", "application/x-www-form-urlencoded"},
		{"Authorization", lib::fmt::format("Basic {}",
			lib::base64::encode(lib::fmt::format("{}:{}", id, secret)))},
	};

	// Send request
	auto reply = http_client.post("https://accounts.spotify.com/api/token",
		headers, post_data);

	nlohmann::json json;
	try
	{
		json = nlohmann::json::parse(reply);
	}
	catch (const std::exception &e)
	{
		return e.what();
	}

	if (json.contains("error_description"))
	{
		return json.at("error_description").get<std::string>();
	}

	const auto &access_token = json.at("access_token");
	const auto &refresh_token = json.at("refresh_token");
	if (access_token.is_null() || refresh_token.is_null())
	{
		return "No access token";
	}

	// Save access/refresh token
	access_token.get_to(settings.account.access_token);
	refresh_token.get_to(settings.account.refresh_token);

	// Everything hopefully went fine
	return {};
}
