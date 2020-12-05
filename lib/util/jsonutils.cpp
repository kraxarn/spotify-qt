#include "jsonutils.hpp"

QJsonValue JsonUtils::getProperty(const QJsonObject &json, const QStringList &names)
{
	for (auto &name : names)
		if (json.contains(name))
			return json[name];

	return QJsonValue();
}