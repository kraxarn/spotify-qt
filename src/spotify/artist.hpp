#pragma once

namespace spt { class Artist; }

#include "spotify.hpp"
#include "album.hpp"

#include <QStringList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>
#include <QVector>

namespace spt
{
	class Artist
	{
	public:
		explicit Artist(const QJsonObject &json);
		int followers, popularity;
		QStringList genres;
		QString id, name, image;
		template<typename F>
		void topTracks(Spotify &spotify, F func);
		QVector<Album> albums(Spotify &spotify);
		QVector<Artist> relatedArtists(Spotify &spotify);
	};
}