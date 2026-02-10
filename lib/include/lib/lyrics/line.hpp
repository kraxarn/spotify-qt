#pragma once

#include <QString>

class LyricsLine
{
public:
	explicit LyricsLine(const QString &line);

	/**
	 * Timestamp in milliseconds
	 */
	[[nodiscard]]
	auto timestamp() const -> long;

	/**
	 * Lyrics text
	 */
	[[nodiscard]]
	auto text() const -> const QString &;

	/**
	 * Raw line data
	 */
	[[nodiscard]]
	auto data() const -> const QString &;

private:
	[[nodiscard]]
	static auto parseTimestamp(const QString &timestamp) -> long;

	long mTimestamp;
	QString mText;
	QString mData;
};
