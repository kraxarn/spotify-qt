#pragma once

#include "thirdparty/json.hpp"

#include <QJsonDocument>
#include <QVariantMap>

class JsonUtils
{
public:
	/**
	 * C++ JSON item -> Qt JSON
	 */
	template<typename T>
	static auto toQtJson(const T &item) -> QJsonDocument
	{
		nlohmann::json json = item;
		return QJsonDocument::fromJson(QByteArray::fromStdString(json.dump()));
	}

	template<typename T>
	static auto toVariantMap(const T &item) -> QVariantMap
	{
		return toQtJson(item).object().toVariantMap();
	}

private:
	JsonUtils() = default;
};
