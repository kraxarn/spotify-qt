#pragma once

namespace spt
{
	class Track;
}

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

		QJsonObject toJson() const;

		QString id, album, albumId, artist, artistId, name, image;
		int duration = 0;
		bool isLocal = false;
		bool isPlayable = false;
		QDateTime addedAt;
	};
}