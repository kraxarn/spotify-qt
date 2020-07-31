#pragma once

namespace spt
{
	class Playlist;
}

#include "../util/utils.hpp"
#include "spotify.hpp"
#include "track.hpp"
#include "user.hpp"

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

		QVector<Track> loadTracks(Spotify &spotify) const;
		QJsonObject toJson(Spotify &spotify) const;
		QJsonObject toJson(const QJsonArray &jsonTracks = QJsonArray()) const;
		bool isOwner(const User &user) const;

		bool collaborative;
		QString description, id, image, name, snapshot, ownerId, ownerName;
		bool isPublic;

	private:
		static bool loadTracksFromUrl(QVector<Track> &trackList, QString &url, int offset, Spotify &spotify);

		QJsonObject tracks;
	};
}