#include "lib/lyrics/api.hpp"
#include "lib/fmt.hpp"
#include "lib/log.hpp"
#include "lib/lyrics/error.hpp"

#include <QUrl>
#include <QUrlQuery>

LyricsApi::LyricsApi(const HttpClient &httpClient)
	: mHttp(httpClient)
{
}

auto LyricsApi::headers() const -> RequestHeaders
{
	return {
		{QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json")},
		{QNetworkRequest::UserAgentHeader, mUserAgent},
	};
}

void LyricsApi::setAppInfo(const QString &name, const QString &version, const QString &homepage)
{
	mUserAgent = QStringLiteral("%1 %2 (%3)").arg(name, version, homepage);
}

void LyricsApi::search(const QString &query, ApiCallback<Result<std::vector<lib::lrc::lyrics>>> &callback) const
{
	QUrl url(QStringLiteral("https://lrclib.net/api/search"));
	url.setQuery({
		{
			{QStringLiteral("q"), query},
		}
	});

	mHttp.get(url, headers(), [callback](const Result<QByteArray> &result) -> void
	{
		if (!result.success())
		{
			callback(Result<std::vector<lib::lrc::lyrics>>::fail(result.message()));
			return;
		}

		const QByteArray &response = result.value();
		if (response.isEmpty())
		{
			callback(Result<std::vector<lib::lrc::lyrics>>::fail("No response"));
			return;
		}

		std::vector<lib::lrc::lyrics> items;
		try
		{
			items = nlohmann::json::parse(response.toStdString());
		}
		catch (const std::exception &e)
		{
			callback(Result<std::vector<lib::lrc::lyrics>>::fail(e.what()));
			return;
		}

		callback(Result<std::vector<lib::lrc::lyrics>>::ok(std::move(items)));
	});
}

void LyricsApi::get(const lib::spt::track &track, lib::callback<Result<lib::lrc::lyrics>> &callback) const
{
	QUrl url(QStringLiteral("https://lrclib.net/api/get"));
	url.setQuery({
		{
			{QStringLiteral("track_name"), QString::fromStdString(track.name)},
			{QStringLiteral("artist_name"), QString::fromStdString(track.artists.front().name)},
			{QStringLiteral("album_name"), QString::fromStdString(track.album.name)},
			{QStringLiteral("duration"), QString::number(track.duration / 1000)},
		}
	});

	mHttp.get(url, headers(), [callback](const Result<QByteArray> &result) -> void
	{
		if (!result.success())
		{
			const auto errorResult = JsonUtil::parse<LyricsError>(result.value());

			callback(Result<lib::lrc::lyrics>::fail(errorResult.success()
				? errorResult.value().message()
				: errorResult.message()));

			return;
		}

		lib::lrc::lyrics item;
		try
		{
			item = nlohmann::json::parse(result.value().toStdString());
		}
		catch (const std::exception &e)
		{
			callback(Result<lib::lrc::lyrics>::fail(e.what()));
			return;
		}

		callback(Result<lib::lrc::lyrics>::ok(item));
	});
}

void LyricsApi::get(const unsigned int lyricsId, lib::callback<Result<lib::lrc::lyrics>> &callback) const
{
	const auto url = QUrl(QStringLiteral("https://lrclib.net/api/get/%1").arg(lyricsId));

	mHttp.get(url, headers(), [callback](const Result<QByteArray> &result)
	{
		if (!result.success())
		{
			callback(Result<lib::lrc::lyrics>::fail(result.message()));
			return;
		}

		const QByteArray &response = result.value();
		if (response.isEmpty())
		{
			callback(Result<lib::lrc::lyrics>::fail(QStringLiteral("No response")));
			return;
		}

		lib::lrc::lyrics item;
		try
		{
			item = nlohmann::json::parse(response);
		}
		catch (const std::exception &e)
		{
			callback(Result<lib::lrc::lyrics>::fail(QString::fromStdString(e.what())));
			return;
		}

		callback(Result<lib::lrc::lyrics>::ok(item));
	});
}
