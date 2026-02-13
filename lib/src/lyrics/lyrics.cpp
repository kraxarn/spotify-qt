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

auto Lyrics::fromJson(const QVariant &json) -> Lyrics
{
	const QVariantMap map = json.toMap();

	Lyrics lyrics;

	JsonUtil::getTo(map, QStringLiteral("id"), lyrics.mId);
	JsonUtil::getTo(map, QStringLiteral("trackName"), lyrics.mTrackName);
	JsonUtil::getTo(map, QStringLiteral("artistName"), lyrics.mArtistName);
	JsonUtil::getTo(map, QStringLiteral("albumName"), lyrics.mAlbumName);
	JsonUtil::getTo(map, QStringLiteral("duration"), lyrics.mDuration);
	JsonUtil::getTo(map, QStringLiteral("instrumental"), lyrics.mInstrumental);

	if (const QString &str = map.value(QStringLiteral("plainLyrics")).toString(); !str.isEmpty())
	{
		lyrics.mPlainLyrics = str.split(QChar::fromLatin1('\n'));
	}

	if (const QString &str = map.value(QStringLiteral("syncedLyrics")).toString(); !str.isEmpty())
	{
		const QStringList lines = str.split(QChar::fromLatin1('\n'));
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
