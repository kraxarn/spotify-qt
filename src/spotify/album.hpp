#pragma once

#include <QJsonObject>
#include <QString>
#include <QJsonArray>
#include <QDateTime>

namespace spt
{
	class Album
	{
	public:
		Album(const QJsonObject &json);
		QString id, albumGroup, image, name;
		QDateTime releaseDate;
	};
}