#pragma once

#include <QJsonValue>
#include <QJsonObject>
#include <QStringList>

class Utils
{

public:
	static QJsonValue getProperty(const QJsonObject &json, const QStringList &names);

private:
	Utils() = default;
};


