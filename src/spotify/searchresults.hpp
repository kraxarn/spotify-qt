#pragma once

namespace spt
{
	class SearchResults;
}

#include "album.hpp"
#include "artist.hpp"
#include "playlist.hpp"
#include "track.hpp"

#include <QString>

namespace spt
{
	class SearchResults
	{
	public:
		explicit SearchResults(const QJsonObject &json);

		QJsonObject toJson() const;

		QVector<Album> albums;
		QVector<Artist> artists;
		QVector<Track> tracks;

		// For playlists, we need more complex information later
		QVector<QJsonObject> playlists;
	};
}