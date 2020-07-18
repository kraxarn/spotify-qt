#pragma once

namespace spt { class Playlist; }

#include "spotify.hpp"
#include "track.hpp"
#include "../utils.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVector>

namespace spt
{
	class Playlist
	{
	public:
		Playlist() = default;
		explicit Playlist(const QJsonObject &json);
		bool collaborative;
		QString description, id, image, name, snapshot, ownerId, ownerName;
		bool isPublic;
		QVector<Track> loadTracks(Spotify &spotify) const;
		QJsonObject toJson(Spotify &spotify);
	private:
		QJsonObject tracks;
		static bool loadTracksFromUrl(QVector<Track> &trackList, QString &url, int offset, Spotify &spotify);
	};
}