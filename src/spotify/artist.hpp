#pragma once

namespace spt { class Artist; }

#include "spotify.hpp"
#include "album.hpp"
#include "item.hpp"

#include <QStringList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>
#include <QVector>

namespace spt
{
	class Artist : public Item
	{
	public:
		explicit Artist(const QJsonObject &json);
		int followers, popularity;
		QStringList genres;
		QString id, name, image;
		QVector<Track> topTracks(Spotify &spotify) const;
		QVector<Album> albums(Spotify &spotify) const;
		QVector<Artist> relatedArtists(Spotify &spotify) const;

		QString itemName() override;
		QString itemId() override;
	};
}