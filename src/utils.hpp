#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>

class Utils
{
public:
	static QJsonValue getProperty(const QJsonObject &json, const QStringList &names);

private:
	Utils() = default;
};
