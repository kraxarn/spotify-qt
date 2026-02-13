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
		{QStringLiteral("Content-Type"), QStringLiteral("application/json")},
		{QStringLiteral("User-Agent"), mUserAgent},
	};
}

void LyricsApi::setAppInfo(const QString &name, const QString &version, const QString &homepage)
{
	mUserAgent = QStringLiteral("%1 %2 (%3)").arg(name, version, homepage);
}

void LyricsApi::search(const QString &query, ApiCallback<Result<QList<Lyrics>>> &callback) const
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
			callback(Result<QList<Lyrics>>::fail(result.message()));
			return;
		}

		const QByteArray &response = result.value();
		if (response.isEmpty())
		{
			callback(Result<QList<Lyrics>>::fail(QStringLiteral("No response")));
			return;
		}

		const Result<QList<Lyrics>> lyricsResult = JsonUtil::parseList<Lyrics>(response);
		if (!lyricsResult.success())
		{
			callback(Result<QList<Lyrics>>::fail(lyricsResult.message()));
			return;
		}

		callback(Result<QList<Lyrics>>::ok(lyricsResult.value()));
	});
}

void LyricsApi::get(const lib::spt::track &track, ApiCallback<Result<Lyrics>> &callback) const
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
			const auto errorResult = JsonUtil::parse<LyricsError>(result.message().toUtf8());

			callback(Result<Lyrics>::fail(errorResult.success()
				? errorResult.value().message()
				: errorResult.message()));

			return;
		}

		const Result<Lyrics> item = JsonUtil::parse<Lyrics>(result.value());
		if (!item.success())
		{
			callback(Result<Lyrics>::fail(item.message()));
			return;
		}

		callback(Result<Lyrics>::ok(item.value()));
	});
}

void LyricsApi::get(const quint32 lyricsId, ApiCallback<Result<Lyrics>> &callback) const
{
	const auto url = QUrl(QStringLiteral("https://lrclib.net/api/get/%1").arg(lyricsId));

	mHttp.get(url, headers(), [callback](const Result<QByteArray> &result)
	{
		if (!result.success())
		{
			callback(Result<Lyrics>::fail(result.message()));
			return;
		}

		const QByteArray &response = result.value();
		if (response.isEmpty())
		{
			callback(Result<Lyrics>::fail(QStringLiteral("No response")));
			return;
		}

		const Result<Lyrics> item = JsonUtil::parse<Lyrics>(response);
		if (!item.success())
		{
			callback(Result<Lyrics>::fail(item.message()));
			return;
		}

		callback(Result<Lyrics>::ok(item.value()));
	});
}
