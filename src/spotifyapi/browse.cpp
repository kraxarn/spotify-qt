#include "spotifyapi.hpp"

// Currently unavailable:
// browse/featured-playlists
// browse/categories
// browse/categories/{category_id}
// browse/categories/{category_id}/playlists
// recommendations
// recommendations/available-genre-seeds

QVector<Album> Spotify::newReleases(int offset)
{
	auto json = getAsObject(QString("browse/new-releases?limit=50&offset=%1")
		.arg(offset))["albums"]
		.toObject();
	auto albumItems = json["items"].toArray();
	QVector<Album> albums;
	albums.reserve(albumItems.size());
	for (auto item : albumItems)
		albums.append(Album(item.toObject()));
	if (json.contains("next") && !json["next"].isNull())
		albums << newReleases(json["offset"].toInt() + json["limit"].toInt());
	return albums;
}
