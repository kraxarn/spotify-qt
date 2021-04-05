#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

// Currently unavailable:
// browse/featured-playlists
// browse/categories
// browse/categories/{category_id}
// browse/categories/{category_id}/playlists
// recommendations
// recommendations/available-genre-seeds

void api::new_releases(lib::callback<std::vector<lib::spt::album>> &callback)
{
	// TODO: No paging
	get("browse/new-releases?limit=50", [callback](const nlohmann::json &json)
	{
		callback(json.at("albums"));
	});
}
