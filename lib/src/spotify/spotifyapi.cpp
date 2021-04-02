#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

spotify_api::spotify_api(lib::settings &settings)
	: settings(settings)
{
}

bool spotify_api::refresh(bool force)
{
	if (!force && seconds_since_epoch() - settings.account.last_refresh < 3600)
	{
		lib::log::info("Last refresh was less than an hour ago, not refreshing access token");
		last_auth = settings.account.last_refresh;
		return true;
	}

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
	auto reply = request_refresh(post_data, auth_header);

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

void spotify_api::get(const std::string &url, const std::string &key,
	lib::callback<nlohmann::json> &callback)
{
	auto api_url = lib::strings::starts_with(url, "https://api.spotify.com/v1/")
		? url.substr(27)
		: url;

	get(api_url, [this, key, callback](const nlohmann::json &json)
	{
		if (!json.contains(key))
		{
			lib::log::error(R"(no such key "{}" in "{}" ({}))", key, json.dump());
		}

		auto items = json.at(key);
		if (json.contains("next") && json.at("next").is_string())
		{
			std::string next = json.at("next").get<std::string>();
			get(next, key, [items, callback](const nlohmann::json &next)
			{
				callback(lib::json::combine(items, next));
			});
			return;
		}
		callback(items);
	});
}

std::string spotify_api::to_uri(const std::string &type, const std::string &id)
{
	return lib::strings::starts_with(id, "spotify:")
		? id
		: lib::fmt::format("spotify:{}:{}", type, id);
}

std::string spotify_api::to_id(const std::string &id)
{
	return lib::strings::split(id, ':').back();
}

std::string spotify_api::follow_type_string(lib::follow_type type)
{
	switch (type)
	{
		case lib::follow_type::artist:
			return "artist";

		case lib::follow_type::user:
			return "user";
	}

	return std::string();
}
