#pragma once

#include "lib/spotify/album.hpp"
#include "lib/spotify/artist.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/show.hpp"

#include <vector>

namespace lib
{
	namespace spt
	{
		/**
		 * Spotify search results
		 */
		class search_results
		{
		public:
			search_results() = default;

			/**
			 * Albums found
			 */
			std::vector<lib::spt::album> albums;

			/**
			 * Artists found
			 */
			std::vector<lib::spt::artist> artists;

			/**
			 * Tracks found
			 */
			std::vector<lib::spt::track> tracks;

			/**
			 * Playlists found
			 */
			std::vector<lib::spt::playlist> playlists;

			/**
			 * Shows/podcasts found
			 */
			std::vector<lib::spt::show> shows;
		};

		/**
		 * Search results -> JSON
		 */
		void to_json(nlohmann::json &j, const search_results &s);

		/**
		 * JSON -> Search results
		 */
		void from_json(const nlohmann::json &j, search_results &s);
	}
}
