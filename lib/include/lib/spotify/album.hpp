#pragma once

#include "lib/enum/albumgroup.hpp"

#include "thirdparty/json.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		/**
		 * Spotify album
		 */
		class album
		{
		public:
			album() = default;

			/**
			 * Spotify ID
			 */
			std::string id;

			/**
			 * Album group, relation to artist
			 * @note Always none when fetching albums outside of an artist
			 */
			lib::album_group album_group = lib::album_group::none;

			/**
			 * URL to cover image
			 */
			std::string image;

			/**
			 * Name of album
			 */
			std::string name;

			/**
			 * Primary artist
			 */
			std::string artist;

			/**
			 * Date when album was released
			 */
			std::string release_date;
		};

		/**
		 * Album -> JSON
		 */
		void to_json(nlohmann::json &j, const album &a);

		/**
		 * JSON -> Album
		 */
		void from_json(const nlohmann::json &j, album &a);
	}
}
