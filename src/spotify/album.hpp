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
		explicit Album(const QJsonObject &json);
		QString id, albumGroup, image, name, artist;
		QDateTime releaseDate;
	};
}