#include "lib/spotify/api.hpp"

// Currently unavailable:
// browse/featured-playlists
// browse/categories
// browse/categories/{category_id}
// browse/categories/{category_id}/playlists
// recommendations
// recommendations/available-genre-seeds

void lib::spt::api::new_releases(const paged_callback<spt::album> &callback) const
{
	const std::string url("browse/new-releases?limit=50");
	request.get_page(url, "albums", callback);
}
