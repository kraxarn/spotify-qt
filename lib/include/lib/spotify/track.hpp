#pragma once

#include "lib/format.hpp"
#include "lib/spotify/entity.hpp"

#include "thirdparty/json.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		/**
		 * A music track
		 */
		class track : public entity
		{
		public:
			track() = default;

			/**
			 * Local track, not streamed from Spotify
			 */
			bool is_local = false;

			/**
			 * Playable in current region
			 */
			bool is_playable = true;

			/**
			 * Track duration in milliseconds
			 */
			int duration = 0;

			/**
			 * ISO date when it was added to the context
			 */
			std::string added_at;

			/**
			 * Album track belongs in
			 */
			entity album;

			/**
			 * Artist track is made by
			 * @todo Add support for multiple artists
			 */
			entity artist;

			/**
			 * URL to cover of album
			 */
			std::string image;

			/**
			 * Format track as "{name} - {artist}" or "(no track)"
			 */
			auto title() const -> std::string;

			/**
			 * Has a valid name and artist
			 */
			auto is_valid() const -> bool;
		};

		/**
		 * track -> json
		 */
		void to_json(nlohmann::json &j, const track &t);

		/**
		 * json -> track
		 */
		void from_json(const nlohmann::json &j, track &t);
	}
}
