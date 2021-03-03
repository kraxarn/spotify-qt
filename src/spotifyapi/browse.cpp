#include "spotifyapi.hpp"

// Currently unavailable:
// browse/featured-playlists
// browse/categories
// browse/categories/{category_id}
// browse/categories/{category_id}/playlists
// recommendations
// recommendations/available-genre-seeds

void Spotify::newReleases(int offset,
	const std::function<void(const std::vector<Album> &albums)> &callback)
{
	get(QString("browse/new-releases?limit=50&offset=%1")
		.arg(offset), [this, callback](const QJsonDocument &jsonDoc)
	{
		auto json = jsonDoc["album"].toObject();

		auto albumItems = json["items"].toArray();
		std::vector<Album> albums;

		for (auto item : albumItems)
			albums.emplace_back(item.toObject());

		if (json.contains("next") && !json["next"].isNull())
		{
			this->newReleases(json["offset"].toInt() + json["limit"].toInt(),
				[albums, callback](const std::vector<Album> &result)
				{
					callback(lib::vector::combine(albums, result));
				});
		}
	});
}

void Spotify::newReleases(const std::function<void(const std::vector<Album> &)> &callback)
{
	newReleases(0, callback);
}