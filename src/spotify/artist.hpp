#pragma once

namespace spt { class Artist; }

#include "album.hpp"
#include "spotify.hpp"

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
		int followers, popularity;
		QStringList genres;
		QString id, name, image;
		QVector<Track> topTracks(Spotify &spotify) const;
		QVector<Album> albums(Spotify &spotify) const;
		QVector<Artist> relatedArtists(Spotify &spotify) const;
		QVector<QPair<QString, QString>> externalUrls;
	};
}