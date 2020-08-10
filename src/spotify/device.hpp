#pragma once

#include <QString>
#include <QJsonObject>

namespace spt
{
	class Device
	{
	public:
		Device() = default;
		Device(const QJsonObject &json);

		QString id;
		QString name;
		QString type;
		bool isActive = false;
	};
}