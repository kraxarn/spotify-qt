#include "auth.hpp"

using namespace lib::spt;

Auth::Auth(lib::Settings &settings)
	: httpClient("https://accounts.spotify.com"), settings(settings)
{
}

std::string Auth::authUrl(const std::string &clientId, const std::string &redirect)
{
	// Scopes for request, for clarity
	// For now, these are mostly identical to spotify-tui
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

	// Prepare url and open browser
	return fmt::format("https://accounts.spotify.com/authorize?client_id={}&response_type=code&redirect_uri={}&scope={}",
		clientId, redirect, strings::join(scopes, "%20"));
}

std::string Auth::auth(const std::string &code, const std::string &redirect,
	const std::string &id, const std::string &secret)
{
	if (code.empty())
		return "no code specified";

	// Prepare form to send
	auto postData = fmt::format("grant_type=authorization_code&code={}&redirect_uri={}", code, redirect);

	// Prepare and send request
	httplib::Headers headers = {
		{"Authorization", fmt::format("Basic {}",
			base64::base64_encode(fmt::format("{}:{}", id, secret)))}
	};
	auto response = httpClient.Post("/api/token", headers, postData,
		"application/x-www-form-urlencoded");

	// Wait for response
	auto jsonData = nlohmann::json::parse(response->body);
	if (jsonData.contains("error_description"))
		return jsonData.at("error_description").get<std::string>();

	// Save access/refresh token to settings
	auto accessToken = jsonData.at("access_token").get<std::string>();
	auto refreshToken = jsonData.at("refresh_token").get<std::string>();
	settings.account.accessToken = accessToken;
	settings.account.refreshToken = refreshToken;
	settings.save();

	// Everything hopefully went fine
	return std::string();
}