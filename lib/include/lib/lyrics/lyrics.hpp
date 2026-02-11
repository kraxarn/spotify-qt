#pragma once

#include "lib/lyrics/line.hpp"

#include <QJsonObject>
#include <QList>

class Lyrics
{
public:
	static auto fromJson(const QJsonObject &json) -> Lyrics;

	[[nodiscard]]
	auto id() const -> quint32;

	[[nodiscard]]
	auto trackName() const -> const QString &;

	[[nodiscard]]
	auto artistName() const -> const QString &;

	[[nodiscard]]
	auto albumName() const -> const QString &;

	[[nodiscard]]
	auto duration() const -> quint32;

	[[nodiscard]]
	auto instrumental() const -> bool;

	/**
	 * Plain text lyrics, one string per line
	 */
	[[nodiscard]]
	auto plainLyrics() const -> const QStringList &;

	/**
	 * Synced lyrics with timestamps
	 */
	[[nodiscard]]
	auto syncedLyrics() const -> const QList<LyricsLine> &;

private:
	Lyrics();

	quint32 mId;
	QString mTrackName;
	QString mArtistName;
	QString mAlbumName;
	quint32 mDuration;
	bool mInstrumental;
	QStringList mPlainLyrics;
	QList<LyricsLine> mSyncedLyrics;
};
