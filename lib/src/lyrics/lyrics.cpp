#include "lib/lyrics/lyrics.hpp"
#include "lib/json.hpp"
#include "lib/log.hpp"
#include "lib/strings.hpp"

Lyrics::Lyrics()
	: mId(0),
	mDuration(0),
	mInstrumental(false)
{
}

auto Lyrics::fromJson(const QJsonObject &json) -> Lyrics
{
	Lyrics lyrics;

	JsonUtil::getTo(json, QStringLiteral("id"), lyrics.mId);
	JsonUtil::getTo(json, QStringLiteral("trackName"), lyrics.mTrackName);
	JsonUtil::getTo(json, QStringLiteral("artistName"), lyrics.mArtistName);
	JsonUtil::getTo(json, QStringLiteral("albumName"), lyrics.mAlbumName);
	JsonUtil::getTo(json, QStringLiteral("duration"), lyrics.mDuration);
	JsonUtil::getTo(json, QStringLiteral("instrumental"), lyrics.mInstrumental);

	if (const QJsonValue &val = json.value(QStringLiteral("plainLyrics")); val.isString())
	{
		const QString &plainLyrics = val.toString();
		lyrics.mPlainLyrics = plainLyrics.split(QChar::fromLatin1('\n'));
	}

	if (const QJsonValue &val = json.value(QStringLiteral("syncedLyrics")); val.isString())
	{
		const QString &syncedLyrics = val.toString();
		const QStringList lines = syncedLyrics.split(QChar::fromLatin1('\n'));

		lyrics.mSyncedLyrics.reserve(lines.length());

		for (const QString &line: lines)
		{
			const LyricsLine parsed(line);
			lyrics.mSyncedLyrics.append(parsed);
		}
	}

	return lyrics;
}

auto Lyrics::id() const -> quint32
{
	return mId;
}

auto Lyrics::trackName() const -> const QString &
{
	return mTrackName;
}

auto Lyrics::artistName() const -> const QString &
{
	return mArtistName;
}

auto Lyrics::albumName() const -> const QString &
{
	return mAlbumName;
}

auto Lyrics::duration() const -> quint32
{
	return mDuration;
}

auto Lyrics::instrumental() const -> bool
{
	return mInstrumental;
}

auto Lyrics::plainLyrics() const -> const QStringList &
{
	return mPlainLyrics;
}

auto Lyrics::syncedLyrics() const -> const QList<LyricsLine> &
{
	return mSyncedLyrics;
}
