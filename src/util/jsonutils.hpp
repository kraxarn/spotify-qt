#pragma once

#include "thirdparty/json.hpp"

#include <QJsonValue>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVariantMap>

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

	/**
	 * Qt JSON -> C++ JSON
	 */
	static nlohmann::json toJson(const QJsonValue &json);

	/**
	 * C++ JSON item -> Qt JSON
	 */
	template<typename T>
	static QJsonDocument toQtJson(const T &item)
	{
		nlohmann::json json = item;
		return QJsonDocument::fromJson(QByteArray::fromStdString(json.dump()));
	}

	template<typename T>
	static QVariantMap toVariantMap(const T &item)
	{
		return toQtJson(item).object().toVariantMap();
	}

private:
	JsonUtils() = default;
};