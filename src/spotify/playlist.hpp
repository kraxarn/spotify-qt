#pragma once

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
		bool collaborative;
		QString description;
		QString id;
		QString image;
		QString name;
		bool isPublic;
		QVector<Track> loadTracks(Spotify &spotify);
	private:
		QJsonObject tracks;
		static bool loadTracks(QVector<Track> &trackList, QString &url, int offset, Spotify &spotify);
	};
}