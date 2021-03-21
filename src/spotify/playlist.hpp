#pragma once

namespace spt
{
	class Playlist;
}

#include "../util/jsonutils.hpp"
#include "spotify.hpp"
#include "lib/spotify/track.hpp"
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

		std::vector<lib::spt::track> loadTracks(Spotify &spotify) const;
		bool loadTracks(Spotify &spotify, std::vector<lib::spt::track> &trackList) const;
		QJsonObject toJson(Spotify &spotify) const;
		QJsonObject toJson(const QJsonArray &jsonTracks = QJsonArray()) const;
		bool isOwner(const User &user) const;

		QString description, id, image, name, snapshot, ownerId, ownerName;
		bool collaborative = false;
		bool isPublic = false;

	private:
		static bool loadTracksFromUrl(std::vector<lib::spt::track> &trackList, QString &url,
			int offset, Spotify &spotify);

		QJsonObject tracks;
	};
}