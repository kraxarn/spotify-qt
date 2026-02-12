#include "lib/spotify/saveditems.hpp"

#include <QJsonArray>

auto SpotifySavedItems::fromJson(const QJsonDocument &json) -> SpotifySavedItems
{
	SpotifySavedItems result;

	for (const QJsonValueRef ref: json.array())
	{
		result.mValues.append(ref.toBool());
	}

	return result;
}

auto SpotifySavedItems::values() const -> const QList<bool> &
{
	return mValues;
}
