#pragma once

#include <QJsonObject>
#include <QList>

class SpotifySavedItems
{
public:
	static auto fromJson(const QJsonDocument &json) -> SpotifySavedItems;

	[[nodiscard]]
	auto values() const -> const QList<bool> &;

private:
	SpotifySavedItems() = default;

	QList<bool> mValues;
};
