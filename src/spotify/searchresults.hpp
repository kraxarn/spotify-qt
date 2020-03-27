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
		QVector<Album>		albums;
		QVector<Artist>		artists;
		QVector<Playlist>	playlists;
		QVector<Track>		tracks;
	} SearchResults;
}