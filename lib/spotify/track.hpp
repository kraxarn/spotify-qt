#pragma once

#include "util/date.hpp"
#include "util/json.hpp"

#include "lib/json.hpp"

namespace lib
{
	namespace spt
	{
		class Track
		{
		public:
			Track() = default;
			explicit Track(const nlohmann::json &item);

			nlohmann::json toJson() const;

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
			std::string albumId;

			/**
			 * Name of artist
			 */
			std::string artist;

			/**
			 * ID of artist
			 */
			std::string artistId;

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
			bool isLocal = false;

			/**
			 * Is playable (for example in current region)
			 */
			bool isPlayable = false;

			/**
			 * Added to current context at (for example playlist)
			 */
			date addedAt;
		};
	}
}