#pragma once

#include <QJsonObject>
#include <QString>

class LyricsError
{
public:
	[[nodiscard]]
	static auto fromJson(const QJsonObject &json) -> LyricsError;

	/**
	 * Error message
	 */
	[[nodiscard]]
	auto message() const -> const QString &;

	/**
	 * Error type
	 */
	[[nodiscard]]
	auto name() const -> const QString &;

	/**
	 * HTTP status code
	 */
	[[nodiscard]]
	auto statusCode() const -> int;

private:
	LyricsError();

	QString mMessage;
	QString mName;
	int mStatusCode;
};
