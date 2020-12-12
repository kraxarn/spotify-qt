#include "api.hpp"

using namespace lib::spt::core;

api::api(lib::Settings &settings)
	: settings(settings),
	http_client("https://api.spotify.com"),
	last_auth(std::chrono::system_clock::time_point::min())
{
}

result api::refresh()
{
	// Make sure we have a refresh token
	auto refreshToken = settings.account.refreshToken;
	if (refreshToken.empty())
	{
		return result::err("No refresh token found");
	}

	// Create request
	httplib::Params postData{
		{"grant_type", "refresh_token"},
		{"refresh_token", refreshToken}
	};

	httplib::Headers headers{
		{"Authorization", fmt::format("Basic {}",
			base64::base64_encode(fmt::format("{}:{}",
				settings.account.clientId, settings.account.clientSecret)))}
	};

	// Send request
	httplib::Client client("https://accounts.spotify.com");
	auto response = client.Post("/api/token", headers, postData);

	if (response.error() != httplib::Error::Success)
	{
		return result::err(fmt::format("Failed to refresh token: err {}", response.error()));
	}

	// Parse json
	nlohmann::json json(response->body);

	// Check if error
	if (json.contains("error_description") || !json.contains("access_token"))
	{
		auto error = json.at("error_description").get<std::string>();
		return result::err(fmt::format("Failed to refresh token: {}", error.empty()
			? "no access token"
			: error));
	}

	// Save as access token
	last_auth = std::chrono::system_clock::now();
	settings.account.accessToken = json.at("access_token").get<std::string>();
	settings.save();

	return result::ok();
}
