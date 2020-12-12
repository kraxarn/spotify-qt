#pragma once

#include "core/spotify.hpp"
#include "track.hpp"
#include "user.hpp"
#include "util/strings.hpp"
#include "format.hpp"
#include "lib/json.hpp"
#include "util/json.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * A Spotify playlist
		 */
		class playlist
		{
		public:
			/**
			 * Construct an empty playlist
			 */
			playlist() = default;

			/**
			 * Construct a playlist from a json object
			 * @param json JSON
			 */
			explicit playlist(const nlohmann::json &json);

			/**
			 * @deprecated Use spotify::playlist::load_tracks
			 */
			std::vector<Track> load_tracks(spotify &spotify) const;

			/**
			 * Format playlist as JSON
			 * @param spotify spotify instance to fetch tracks
			 * @return Playlist as JSON
			 */
			nlohmann::json to_json(spotify &spotify) const;

			/**
			 * Format playlist as JSON
			 * @param jsonTracks Known tracks in playlist
			 * @return Playlist as JSON
			 */
			nlohmann::json to_json(const nlohmann::json &jsonTracks = nlohmann::json::array()) const;

			/**
			 * Utility method to check if user is owner of the playlist
			 * @param user User to check
			 * @return If the specified user is owner of this playlist
			 */
			bool is_owner(const user &user) const;

			/**
			 * Playlists description
			 */
			std::string description;

			/**
			 * Spotify ID
			 */
			std::string id;

			/**
			 * Coner image for the playlist
			 */
			std::string image;

			/**
			 * Name of playlist
			 */
			std::string name;

			/**
			 * Version identifier
			 */
			std::string snapshot;

			/**
			 * ID of owner
			 */
			std::string owner_id;

			/**
			 * Name of owner
			 */
			std::string ownerName;

			/**
			 * If other users can modify the playlist
			 */
			bool collaborative = false;

			/**
			 * If the playlist is public for everyone to see
			 */
			bool is_public = false;

		private:
			/**
			 * @deprecated
			 */
			static bool loadTracksFromUrl(std::vector<Track> &trackList,
				std::string &url,
				int offset,
				spotify &spotify);

			/**
			 * Information about the tracks in the playlist, not the actual tracks
			 */
			nlohmann::json tracks;
		};
	}
}