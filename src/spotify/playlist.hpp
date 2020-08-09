#pragma once

namespace spt { class Playlist; }

#include "../util/utils.hpp"
#include "spotify.hpp"
#include "track.hpp"

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
		QJsonObject toJson(Spotify &spotify) const;
	private:
		QJsonObject tracks;
		static bool loadTracksFromUrl(QVector<Track> &trackList, QString &url, int offset, Spotify &spotify);
	};
}