#pragma once

namespace spt
{
	class SearchResults;
}

#include "album.hpp"
#include "artist.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/track.hpp"
#include "../util/jsonutils.hpp"

#include <QString>

namespace spt
{
	class SearchResults
	{
	public:
		SearchResults() = default;

		explicit SearchResults(const QJsonObject &json);

		QJsonObject toJson() const;

		QVector<Album> albums;
		QVector<Artist> artists;
		QVector<lib::spt::track> tracks;
		QVector<lib::spt::playlist> playlists;
	};
}