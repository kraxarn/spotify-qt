#pragma once

namespace spt { class Artist; }

#include "spotify.hpp"

#include <QStringList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>

namespace spt
{
	class Artist
	{
	public:
		explicit Artist(const QJsonObject &json);
		int followers, popularity;
		QStringList genres;
		QString name, image;
	};
}