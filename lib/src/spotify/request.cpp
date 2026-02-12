#include "lib/spotify/request.hpp"
#include "lib/base64.hpp"
#include "lib/datetime.hpp"
#include "lib/fmt.hpp"
#include "lib/spotify/error.hpp"

lib::spt::request::request(lib::settings &settings, const HttpClient &http_client,
	const lib::spt::device_select &device_select)
	: settings(settings),
	http(http_client),
	device_select(device_select)
{
}

auto lib::spt::request::authHeaders() -> RequestHeaders
{
	// See when the last refresh was
	const qint64 lastRefresh = QDateTime::currentSecsSinceEpoch() - last_auth;
	if (lastRefresh >= secs_in_hour)
	{
		qDebug() << "Access token probably expired, refreshing";

		try
		{
			refresh();
		}
		catch (const std::exception &e)
		{
			qCritical() << "Refresh failed:" << e.what();
		}
	}

	return {
		{
			QStringLiteral("Authorization"),
			QStringLiteral("Bearer %1").arg(QString::fromStdString(settings.account.access_token)),
		},
	};
}

auto lib::spt::request::auth_headers() -> lib::headers
{
	const RequestHeaders headers = authHeaders();
	return {
		{
			headers.firstKey().toStdString(),
			headers.first().toStdString(),
		},
	};
}

void lib::spt::request::refresh(bool force)
{
	if (!force
		&& lib::date_time::seconds_since_epoch() - settings.account.last_refresh < secs_in_hour)
	{
		lib::log::debug("Last refresh was less than an hour ago, not refreshing access token");
		last_auth = settings.account.last_refresh;
		return;
	}

	// Make sure we have a refresh token
	auto refresh_token = settings.account.refresh_token;
	if (refresh_token.empty())
	{
		throw lib::spt::error("No refresh token", "token");
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
	if (reply.empty())
	{
		throw lib::spt::error("No response", "token");
	}

	// Parse as JSON
	const auto json = nlohmann::json::parse(reply);

	// Check if error
	if (json.contains("error_description") || !json.contains("access_token"))
	{
		auto error = json.at("error_description").get<std::string>();
		throw lib::spt::error(error.empty()
			? "No access token" : error, "token");
	}

	// Save as access token
	last_auth = lib::date_time::seconds_since_epoch();
	settings.account.last_refresh = last_auth;
	settings.account.access_token = json.at("access_token").get<std::string>();
	settings.save();
}

auto lib::spt::request::request_refresh(const std::string &post_data,
	const std::string &authorization) -> std::string
{
	return http.post("https://accounts.spotify.com/api/token", {
		{"Content-Type", "application/x-www-form-urlencoded"},
		{"Authorization", authorization},
	}, post_data);
}

auto lib::spt::request::get_current_device() const -> const std::string &
{
	return settings.general.last_device;
}

void lib::spt::request::set_current_device(const std::string &device_id)
{
	settings.general.last_device = device_id;
	settings.save();
}
