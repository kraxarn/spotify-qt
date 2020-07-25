#pragma once

namespace spt { class Track; }

#include "../util/utils.hpp"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

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
		QJsonObject toJson() const;
	};
}