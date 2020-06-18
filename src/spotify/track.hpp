#pragma once

namespace spt { class Track; }

#include "utils.hpp"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace spt
{
	class Track
	{
	public:
		Track();
		explicit Track(const QJsonObject &item);
		QString id, album, albumId, artist, artistId, name, image;
		int duration;
		bool isLocal;
		QDateTime addedAt;
		QJsonObject toJson();
	};
}