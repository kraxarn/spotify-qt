#include "spotifyapi.hpp"

SearchResults Spotify::search(const QString &query)
{
	return SearchResults(getAsObject(
		QString("search?q=%1&type=album,artist,playlist,track&limit=50&market=from_token")
			.arg(query)));
}
