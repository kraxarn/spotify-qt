#pragma once

#include <QJsonValue>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

class JsonUtils
{
public:
	static QJsonValue getProperty(const QJsonObject &json, const QStringList &names);

	template<typename T>
	static QJsonArray toJsonArray(const QList<T> &list)
	{
		QJsonArray array;
		for (auto &item : list)
			array.append(item);
		return array;
	}

private:
	JsonUtils() = default;
};