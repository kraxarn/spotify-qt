#pragma once

#include "util/date.hpp"
#include "util/json.hpp"

#include "lib/json.hpp"

namespace lib
{
	namespace spt
	{
		class track
		{
		public:
			track() = default;
			explicit track(const nlohmann::json &item);

			nlohmann::json to_json() const;

			/**
			 * ID of track
			 */
			std::string id;

			/**
			 * Name of album
			 */
			std::string album;

			/**
			 * ID of album
			 */
			std::string album_id;

			/**
			 * Name of artist
			 */
			std::string artist;

			/**
			 * ID of artist
			 */
			std::string artist_id;

			/**
			 * Name of track
			 */
			std::string name;

			/**
			 * URL to album image
			 */
			std::string image;

			/**
			 * Duration of track in ms
			 */
			int duration = 0;

			/**
			 * Local track
			 */
			bool is_local = false;

			/**
			 * Is playable (for example in current region)
			 */
			bool is_playable = false;

			/**
			 * Added to current context at (for example playlist)
			 */
			date added_at;
		};
	}
}