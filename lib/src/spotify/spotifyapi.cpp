#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

api::api(lib::settings &settings)
	: settings(settings)
{
}

void api::refresh(bool force)
{
	if (!force && seconds_since_epoch() - settings.account.last_refresh < 3600)
	{
		lib::log::info("Last refresh was less than an hour ago, not refreshing access token");
		last_auth = settings.account.last_refresh;
		return;
	}

	// Make sure we have a refresh token
	auto refresh_token = settings.account.refresh_token;
	if (refresh_token.empty())
	{
		throw lib::spotify_error("No refresh token", "token");
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
	const auto json = nlohmann::json::parse(reply);

	// Check if error
	if (json.contains("error_description") || !json.contains("access_token"))
	{
		auto error = json.at("error_description").get<std::string>();
		throw lib::spotify_error(error.empty()
			? "No access token" : error, "token");
	}

	// Save as access token
	last_auth = seconds_since_epoch();
	settings.account.last_refresh = last_auth;
	settings.account.access_token = json.at("access_token").get<std::string>();
	settings.save();
}

nlohmann::json api::parse_json(const std::string &url, const std::string &data)
{
	// No data, no response, no error
	if (data.empty())
		return nlohmann::json();

	auto json = nlohmann::json::parse(data);

	if (!lib::spotify_error::is(json))
		return json;

	auto err = lib::spotify_error::error_message(json);
	lib::log::error("{} failed: {}", url, err);
	throw lib::spotify_error(err, url);
}

long api::seconds_since_epoch()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()
		.time_since_epoch()).count();
}

std::string api::to_uri(const std::string &type, const std::string &id)
{
	return lib::strings::starts_with(id, "spotify:")
		? id
		: lib::fmt::format("spotify:{}:{}", type, id);
}

std::string api::to_id(const std::string &id)
{
	return lib::strings::split(id, ':').back();
}

std::string api::follow_type_string(lib::follow_type type)
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

void api::set_current_device(const std::string &id)
{
	settings.general.last_device = id;
	settings.save();
}

auto api::get_current_device() const -> std::string
{
	return settings.general.last_device;
}

//region GET

void api::get_items(const std::string &url, const std::string &key,
	lib::callback<nlohmann::json> &callback)
{
	auto api_url = lib::strings::starts_with(url, "https://api.spotify.com/v1/")
		? url.substr(27)
		: url;

	get(api_url, [this, key, callback](const nlohmann::json &json)
	{
		if (!key.empty() && !json.contains(key))
		{
			lib::log::error(R"(no such key "{}" in "{}" ({}))", key, json.dump());
		}

		auto items = (key.empty() ? json : json.at(key)).at("items");
		if (json.contains("next") && json.at("next").is_string())
		{
			std::string next = json.at("next").get<std::string>();
			get_items(next, key, [items, callback](const nlohmann::json &next)
			{
				callback(lib::json::combine(items, next));
			});
			return;
		}
		callback(items);
	});
}

void api::get_items(const std::string &url, lib::callback<nlohmann::json> &callback)
{
	get_items(url, std::string(), callback);
}

//endregion

//region PUT

void api::put(const std::string &url, lib::callback<std::string> &callback)
{
	put(url, nlohmann::json(), callback);
}

//endregion

//region POST

//endregion

//region DELETE

void api::del(const std::string &url, lib::callback<std::string> &callback)
{
	del(url, nlohmann::json(), callback);
}

//endregion
