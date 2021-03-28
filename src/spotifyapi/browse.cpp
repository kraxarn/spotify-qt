#include "spotifyapi.hpp"

// Currently unavailable:
// browse/featured-playlists
// browse/categories
// browse/categories/{category_id}
// browse/categories/{category_id}/playlists
// recommendations
// recommendations/available-genre-seeds

std::vector<lib::spt::album> Spotify::newReleases(int offset)
{
	auto json = getAsJson(lib::fmt::format("browse/new-releases?limit=50&offset={}",
		offset)).at("albums");
	auto albumItems = json["items"];
	std::vector<lib::spt::album> albums;
	albums.reserve(albumItems.size());
	for (auto &item : albumItems)
		albums.push_back(item.get<lib::spt::album>());
	if (json.contains("next") && !json["next"].is_null())
	{
		lib::vector::append(albums,
			newReleases(json["offset"].get<int>() + json["limit"].get<int>()));
	}
	return albums;
}
