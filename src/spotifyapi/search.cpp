#include "spotifyapi.hpp"

void Spotify::search(const QString &query,
	const std::function<void(const SearchResults &results)> &callback)
{
	get(QString("search?q=%1&type=album,artist,playlist,track&limit=50&market=from_token")
		.arg(query), [callback](const QJsonDocument &json)
	{
		callback(SearchResults(json.object()));
	});
}
