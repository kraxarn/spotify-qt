#include "lib/lyrics/error.hpp"
#include "lib/json.hpp"

LyricsError::LyricsError()
	: mStatusCode(0)
{
}

auto LyricsError::fromJson(const QVariant &json) -> LyricsError
{
	const QVariantMap map = json.toMap();

	LyricsError result;

	JsonUtil::getTo(map, QStringLiteral("message"), result.mMessage);
	JsonUtil::getTo(map, QStringLiteral("name"), result.mName);
	JsonUtil::getTo(map, QStringLiteral("statusCode"), result.mStatusCode);

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
