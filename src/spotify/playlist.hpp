#pragma once

namespace spt { class Playlist; }

#include "track.hpp"
#include "spotify.hpp"

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVector>

namespace spt
{
	class Playlist
	{
	public:
		Playlist(bool collaborative, QString description, QString id, QString image,
			QString name, bool isPublic, QJsonObject tracks);
		bool collaborative;
		QString description;
		QString id;
		QString image;
		QString name;
		bool isPublic;
		QVector<Track> *loadTracks(Spotify &spotify);
	private:
		QJsonObject tracks;
		static bool loadTracksFromUrl(QVector<Track> *trackList, QString &url, int offset, Spotify &spotify);
	};
}