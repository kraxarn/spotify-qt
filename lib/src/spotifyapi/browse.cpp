#include "lib/spotify/api.hpp"

// Currently unavailable:
// browse/featured-playlists
// browse/categories
// browse/categories/{category_id}
// browse/categories/{category_id}/playlists
// recommendations
// recommendations/available-genre-seeds

void lib::spt::api::new_releases(lib::callback<std::vector<lib::spt::album>> &callback)
{
	get_items("browse/new-releases?limit=50", "albums", callback);
}
