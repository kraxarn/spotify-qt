#pragma once

namespace spt
{
	class Artist;
}

#include "lib/spotify/album.hpp"
#include "spotify.hpp"
#include "lib/spotify/track.hpp"

#include <QJsonArray>
#include <QJsonObject>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QVector>

namespace spt
{
	class Artist
	{
	public:
		Artist() = default;
		explicit Artist(const QJsonObject &json);

		QVector<lib::spt::track> topTracks(Spotify &spotify) const;
		std::vector<lib::spt::album> albums(Spotify &spotify) const;

		QJsonObject toJson() const;

		int followers = 0;
		int popularity = 0;
		QStringList genres;
		QString id, name, image;
		QVector<QPair<QString, QString>> externalUrls;
	};
}