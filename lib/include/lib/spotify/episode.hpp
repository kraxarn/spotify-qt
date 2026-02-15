#pragma once

#include "lib/spotify/entity.hpp"
#include "lib/json.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/show.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * A specific episode of a show
		 */
		class episode: public entity
		{
		public:
			/**
			 * Length in milliseconds
			 */
			int duration_ms = 0;

			/**
			 * External URLs
			 */
			std::map<std::string, std::string> external_urls;

			/**
			 * URLs to cover art
			 */
			std::vector<lib::spt::image> images;

			/**
			 * Playable in current market
			 */
			bool is_playable = false;

			/**
			 * Date when first released
			 * @note Varying precision
			 */
			std::string release_date;

			/**
			 * Convert to track entity, mostly for compatibility
			 */
			auto to_track(const lib::spt::show &show) const -> lib::spt::track;
		};

		/** JSON -> Episode */
		void from_json(const nlohmann::json &j, episode &e);
	}
}
