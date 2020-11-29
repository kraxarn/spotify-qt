#pragma once

#include "album.hpp"
#include "artist.hpp"
#include "playlist.hpp"
#include "track.hpp"

#include "lib/json.hpp"

namespace lib
{
	namespace spt
	{
		class SearchResults
		{
		public:
			explicit SearchResults(const nlohmann::json &json);

			nlohmann::json toJson() const;

			std::vector<Album> albums;
			std::vector<Artist> artists;
			std::vector<Track> tracks;

			// For playlists, we need more complex information later
			std::vector<nlohmann::json> playlists;
		};
	}
}