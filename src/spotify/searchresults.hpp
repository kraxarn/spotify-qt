#pragma once

#include "album.hpp"
#include "artist.hpp"
#include "playlist.hpp"
#include "track.hpp"

#include <QString>

namespace spt
{
	typedef struct
	{
		QVector<Album> albums;
		QVector<Artist> artists;
		// For playlists, we need more complex information later
		QVector<QJsonObject> playlists;
		QVector<Track> tracks;
	} SearchResults;
}