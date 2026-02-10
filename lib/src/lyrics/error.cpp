#include "lib/lyrics/error.hpp"
#include "lib/json.hpp"

LyricsError::LyricsError()
	: mStatusCode(0)
{
}

auto LyricsError::fromJson(const QJsonObject &json) -> LyricsError
{
	LyricsError result;

	JsonUtil::getTo(json, QStringLiteral("message"), result.mMessage);
	JsonUtil::getTo(json, QStringLiteral("name"), result.mName);
	JsonUtil::getTo(json, QStringLiteral("statusCode"), result.mStatusCode);

	return result;
}

auto LyricsError::message() const -> const QString &
{
	return mMessage;
}

auto LyricsError::name() const -> const QString &
{
	return mName;
}

auto LyricsError::statusCode() const -> int
{
	return mStatusCode;
}
