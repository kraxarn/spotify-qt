#pragma once

#include "lib/spotify/user.hpp"
#include "lib/spotify/track.hpp"
#include "lib/json.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		/**
		 * Playlist
		 */
		class playlist
		{
		public:
			playlist() = default;

			/**
			 * Optional playlist description
			 */
			std::string description;

			/**
			 * Playlist ID
			 */
			std::string id;

			/**
			 * URL to playlist image
			 */
			std::string image;

			/**
			 * Name of playlist
			 */
			std::string name;

			/**
			 * Snapshot ID
			 */
			std::string snapshot;

			/**
			 * Spotify ID of owner
			 */
			std::string owner_id;

			/**
			 * Display name of owner
			 */
			std::string owner_name;

			/**
			 * Collaborative playlist, where anyone can edit it
			 */
			bool collaborative = false;

			/**
			 * Readable and searchable by the public
			 */
			bool is_public = false;

			/**
			 * Tracks in playlist
			 */
			std::vector<lib::spt::track> tracks;

			/**
			 * URL to load tracks from
			 */
			std::string tracks_href;

			/**
			 * Number of tracks in playlist
			 * @note Prefer using track_count()
			 */
			int tracks_total = -1;

			/**
			 * Specified user is the owner of this playlist
			 */
			bool is_owner(const lib::spt::user &user) const;

			/**
			 * If no playlist has been parsed (invalid ID)
			 */
			bool is_null() const;

			/**
			 * Get the number of tracks in playlist
			 */
			int track_count();
		};

		void to_json(nlohmann::json &j, const playlist &p);

		void from_json(const nlohmann::json &j, playlist &p);
	}
}