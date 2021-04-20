#pragma once

#include "lib/json.hpp"

#include "thirdparty/json.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		/**
		 * Playlist details, meant to be used when editing playlists
		 */
		class playlist_details
		{
		public:
			playlist_details() = default;

			/**
			 * Name of playlist
			 */
			std::string name;

			/**
			 * If the playlist is public
			 */
			bool is_public = false;

			/**
			 * Others can edit the playlist
			 */
			bool collaborative = false;

			/**
			 * Playlist description
			 */
			std::string description;
		};

		/**
		 * Playlist details -> JSON
		 */
		void to_json(nlohmann::json &j, const playlist_details &p);

		/**
		 * JSON -> Playlist details
		 * @note JSON can be playlist object
		 */
		void from_json(const nlohmann::json &j, playlist_details &p);
	}
}
