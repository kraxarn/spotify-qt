#include "lib/lyrics/line.hpp"

LyricsLine::LyricsLine(const QString &line)
	: mTimestamp(0),
	mData(line)
{
	const qsizetype timestampStart = line.indexOf(QChar::fromLatin1('['));
	const qsizetype timestampEnd = line.indexOf(QChar::fromLatin1(']'));

	if (timestampStart < 0 || timestampEnd < 0)
	{
		mTimestamp = -1L;
	}
	else
	{
		const qsizetype length = timestampEnd - timestampStart - 1;
		mTimestamp = parseTimestamp(line.mid(timestampStart + 1, length));
	}

	if (line.isEmpty() || timestampEnd == line.length() - 2)
	{
		mText = QStringLiteral("♪");
	}
	else if (timestampEnd >= 0 && mTimestamp > 0)
	{
		mText = line.mid(timestampEnd + 2);
	}
	else
	{
		mText = line;
	}
}

auto LyricsLine::timestamp() const -> long
{
	return mTimestamp;
}

auto LyricsLine::text() const -> const QString &
{
	return mText;
}

auto LyricsLine::data() const -> const QString &
{
	return mData;
}

auto LyricsLine::parseTimestamp(const QString &timestamp) -> long
{
	// minute:second.millisecond

	const qsizetype second = timestamp.indexOf(QChar::fromLatin1(':'));
	const qsizetype millisecond = timestamp.indexOf(QChar::fromLatin1('.'));

	if (second < 0 || millisecond < 0)
	{
		return -1L;
	}

	const QString minutesStr = timestamp.mid(0, 2);
	const QString secondsStr = timestamp.mid(second + 1, 2);
	const QString millisecondsStr = timestamp.mid(millisecond + 1, 3);

	const int minutes = minutesStr.toInt();
	const int seconds = secondsStr.toInt();
	const int milliseconds = millisecondsStr.toInt();

	constexpr int millisecondsInSecond = 1000;
	constexpr int millisecondsInMinute = millisecondsInSecond * 60;

	return (minutes * millisecondsInMinute)
		+ (seconds * millisecondsInSecond)
		+ milliseconds;
}
