#include "lib/lyrics/api.hpp"
#include "lib/fmt.hpp"
#include "lib/log.hpp"
#include "lib/uri.hpp"
#include "lib/lyrics/error.hpp"

lib::lrc::api::api(const http_client &http_client)
	: http(http_client)
{
}

auto lib::lrc::api::headers() const -> lib::headers
{
	return {
		{"Content-Type", "application/json"},
		{"User-Agent", user_agent},
	};
}

void lib::lrc::api::set_app_info(const std::string &name, const std::string &version, const std::string &homepage)
{
	user_agent = fmt::format("{} {} ({})", name, version, homepage);
}

void lib::lrc::api::search(const std::string &query, callback<Result<std::vector<lyrics>>> &callback) const
{
	uri uri("https://lrclib.net/api/search");
	uri.set_search_params({
		{"q", query},
	});

	http.get(uri.get_url(), headers(), [callback](const std::string &response)
	{
		if (response.empty())
		{
			callback(Result<std::vector<lyrics>>::fail("No response"));
			return;
		}

		std::vector<lyrics> items;
		try
		{
			items = nlohmann::json::parse(response);
		}
		catch (const std::exception &e)
		{
			callback(Result<std::vector<lyrics>>::fail(e.what()));
			return;
		}

		callback(Result<std::vector<lyrics>>::ok(items));
	});
}

void lib::lrc::api::get(const spt::track &track, callback<Result<lyrics>> &callback) const
{
	uri uri("https://lrclib.net/api/get");
	uri.set_search_params({
		{"track_name", track.name},
		{"artist_name", track.artists.front().name},
		{"album_name", track.album.name},
		{"duration", std::to_string(track.duration / 1000)},
	});

	http.get(uri.get_url(), headers(), [callback](const Result<std::string> &response)
	{
		if (!response.success())
		{
			QString errorMessage;
			try
			{
				const error error = nlohmann::json::parse(response.message().toStdString());
				errorMessage = QString::fromStdString(error.message);
			}
			catch (nlohmann::json::parse_error &e)
			{
				log::error("Failed to parse error message: {}", e.what());
				errorMessage = response.message();
			}

			callback(Result<lyrics>::fail(errorMessage));
			return;
		}

		lyrics item;
		try
		{
			item = nlohmann::json::parse(response.value());
		}
		catch (const std::exception &e)
		{
			callback(Result<lyrics>::fail(e.what()));
			return;
		}

		callback(Result<lyrics>::ok(item));
	});
}

void lib::lrc::api::get(const unsigned int lyricsId, callback<Result<lyrics>> &callback) const
{
	const auto url = fmt::format("https://lrclib.net/api/get/{}", lyricsId);

	http.get(url, headers(), [callback](const std::string &response)
	{
		if (response.empty())
		{
			callback(Result<lyrics>::fail("No response"));
			return;
		}

		lyrics item;
		try
		{
			item = nlohmann::json::parse(response);
		}
		catch (const std::exception &e)
		{
			callback(Result<lyrics>::fail(e.what()));
			return;
		}

		callback(Result<lyrics>::ok(item));
	});
}
