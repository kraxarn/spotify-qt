#pragma once

#include "lib/spotify/entity.hpp"
#include "lib/spotify/image.hpp"

#include "thirdparty/json.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		/**
		 * A music track
		 */
		class track: public entity
		{
		public:
			track() = default;

			virtual ~track() = default;

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
			 */
			std::vector<entity> artists;

			/**
			 * URLs to cover of album
			 */
			std::vector<lib::spt::image> images;

			/**
			 * Format track as "{artist} - {name}" or "(no track)"
			 * @note Only shows first artist, to show all, use details()
			 */
			auto title() const -> std::string;

			/**
			 * Format track as "{artists}\n{name}" or "(no track)"
			 * @note Multiple artists are separated using commas
			 */
			auto details() const -> std::string;

			/**
			 * 64x64, or smallest image
			 */
			auto image_small() const -> std::string;

			/**
			 * 300x300, or largest image
			 */
			auto image_large() const -> std::string;

			/**
			 * Has a valid name and artist
			 */
			auto is_valid() const -> bool override;
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
