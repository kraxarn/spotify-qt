#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

spotify_api::spotify_api(lib::settings &settings)
	: settings(settings)
{
	if (seconds_since_epoch() - settings.account.last_refresh < 3600)
	{
		lib::log::info("Last refresh was less than an hour ago, not refreshing access token");
		last_auth = settings.account.last_refresh;
		refresh_valid = true;
		return;
	}

	refresh_valid = refresh();
}

bool spotify_api::refresh()
{
	// Make sure we have a refresh token
	auto refresh_token = settings.account.refresh_token;
	if (refresh_token.empty())
	{
		lib::log::error("Failed to refresh: no refresh token");
		return false;
	}

	// Create form
	auto post_data = lib::fmt::format("grant_type=refresh_token&refresh_token={}",
		refresh_token);

	// Create request
	auto auth_header = lib::fmt::format("Basic {}",
		lib::base64::encode(lib::fmt::format("{}:{}",
			settings.account.client_id, settings.account.client_secret)));

	// Send request
	auto reply = refresh(post_data, auth_header);

	// Parse as JSON
	nlohmann::json json;
	try
	{
		json = nlohmann::json::parse(reply);
	}
	catch (const std::exception &e)
	{
		lib::log::error("Failed to refresh: {}", e.what());
		return false;
	}

	// Check if error
	if (json.contains("error_description") || !json.contains("access_token"))
	{
		auto error = json.at("error_description").get<std::string>();
		lib::log::error("Failed to refresh: {}", error.empty()
			? "no access token" : error);
		return false;
	}

	// Save as access token
	last_auth = seconds_since_epoch();
	settings.account.last_refresh = last_auth;
	settings.account.access_token = json.at("access_token").get<std::string>();
	settings.save();

	return true;
}

long spotify_api::seconds_since_epoch()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()
		.time_since_epoch()).count();
}

bool spotify_api::is_valid() const
{
	return refresh_valid;
}
