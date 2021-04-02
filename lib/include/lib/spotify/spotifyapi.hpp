#pragma once

#include "lib/settings.hpp"
#include "lib/base64.hpp"
#include "lib/vector.hpp"
#include "lib/strings.hpp"
#include "lib/spotify/playback.hpp"
#include "lib/enum/repeatstate.hpp"
#include "lib/json.hpp"

#include "thirdparty/json.hpp"

namespace lib
{
	/**
	 * API callback
	 */
	template<typename T>
	using callback = const std::function<void(const T &)>;

	namespace spt
	{
		/**
		 * Spotify Web API
		 */
		class spotify_api
		{
		public:
			/**
			 * Construct a new instance and refresh if needed
			 * @param settings Settings for access token and refresh token
			 */
			explicit spotify_api(lib::settings &settings);

			//region Albums

			//endregion

			//region Artists

			//endregion

			//region Browse

			//endregion

			//region Follow

			//endregion

			//region Library

			//endregion

			//region Personalization

			//endregion

			//region Player

			/**
			 * Get what's currently playing
			 */
			void current_playback(lib::callback<lib::spt::playback> &callback);

			/**
			 * Set current active device by device ID
			 */
			void set_device(const std::string &deviceId, lib::callback<std::string> &callback);

			/**
			 * Set current active device
			 */
			void set_device(const lib::spt::device &device, lib::callback<std::string> &callback);

			/**
			 * Get all available devices
			 */
			void devices(lib::callback<std::vector<lib::spt::device>> &callback);

			/**
			 * Play track from specific context
			 * @param track_index Index to start playback from
			 * @param context Context to play from
			 */
			void play_tracks(int track_index, const std::string &context,
				lib::callback<std::string> &callback);

			/**
			 * Play track from specified set of tracks
			 * @param track_index Index to start playback from
			 * @param all IDs of all tracks
			 */
			void play_tracks(int track_index, const std::vector<std::string> &all,
				lib::callback<std::string> &callback);

			/**
			 * Convenience method for playing specified set of tracks
			 */
			void play_tracks(int track_index, const std::initializer_list<std::string> &all,
				lib::callback<std::string> &callback);

			/**
			 * Play track from specific context
			 * @param context Context to play from
			 * @note If shuffle is enabled, a random track is chosen to start playback from
			 */
			void play_tracks(const std::string &context, lib::callback<std::string> &callback);

			/**
			 * Resume playback
			 */
			void resume(lib::callback<std::string> &callback);

			/**
			 * Pause playback
			 */
			void pause(lib::callback<std::string> &callback);

			/**
			 * Skip to next track
			 */
			void next(lib::callback<std::string> &callback);

			/**
			 * Go to previous track, or restart current
			 */
			void previous(lib::callback<std::string> &callback);

			/**
			 * Seek in current track
			 * @param position Position to seek to in milliseconds
			 */
			void seek(int position, lib::callback<std::string> &callback);

			/**
			 * Change repeat mode
			 * @param state New repeat mode
			 */
			void set_repeat(const lib::repeat_state state, lib::callback<std::string> &callback);

			/**
			 * Change player volume
			 * @param volume Player volume, from 0%-100%
			 */
			void set_volume(int volume, lib::callback<std::string> &callback);

			/**
			 * Change shuffle mode
			 * @param enabled Shuffle mode is enabled
			 */
			void set_shuffle(bool enabled, lib::callback<std::string> &callback);

			/**
			 * Get all recently played tracks
			 */
			void recently_played(lib::callback<std::vector<lib::spt::track>> &callback);

			/**
			 * Add specified track to play next
			 * @param uri URI of track to add
			 */
			void add_to_queue(const std::string &uri, lib::callback<std::string> &callback);

			//endregion

			//region Playlists

			//endregion

			//region Search

			//endregion

			//region Tracks

			//endregion

			//region User Profile

			//endregion

			/**
			 * Refresh access token with refresh token
			 * @return Refresh was successful
			 */
			bool refresh(bool force = false);

			/**
			 * Spotify ID (4uLU6hMCjMI75M1A2tKUQC) to Spotify URI
			 * (spotify:track:4uLU6hMCjMI75M1A2tKUQC)
			 * @param type URI type, for example artist, album, track, etc.
			 * @param id Spotify ID
			 * @note Returns result if already an URI
			 * @return Spotify URI
			 */
			static std::string to_uri(const std::string &type, const std::string &id);

			/**
			 * Spotify URI (spotify:track:4uLU6hMCjMI75M1A2tKUQC) to Spotify ID
			 * (4uLU6hMCjMI75M1A2tKUQC)
			 * @param id Spotify ID
			 * @note Returns result if already an ID
			 * @return Spotify ID
			 */
			static std::string to_id(const std::string &id);

		protected:
			/**
			 * Number of seconds since 1970-01-01
			 */
			static long seconds_since_epoch();

			/**
			 * Timestamp if last refresh
			 */
			long last_auth = 0;

			/**
			 * Settings
			 */
			lib::settings &settings;

			/**
			 * ID of current playback device
			 */
			std::string current_device;

			/**
			 * GET a collection of items
			 * @param url URL to request
			 * @param key Key to collection
			 * @note Automatically handles paging
			 * @note Temporarily protected
			 * @throws std::exception
			 */
			void get(const std::string &url, const std::string &key,
				lib::callback<nlohmann::json> &callback);

		private:
			/**
			 * Send request to refresh access token
			 * @param post_data POST form data
			 * @param authorization Authorization header
			 * @note Only required until networking is properly implemented
			 * @return JSON response with (maybe) new access token
			 */
			virtual std::string request_refresh(const std::string &post_data,
				const std::string &authorization) = 0;

			/**
			 * GET request
			 * @param url URL to request
			 * @param callback Response as JSON
			 */
			virtual void get(const std::string &url, lib::callback<nlohmann::json> &callback) = 0;

			/**
			 * PUT request
			 * @param url URL to request
			 * @param body JSON body or null if no body
			 * @param callback Error message, or empty if none
			 */
			virtual void put(const std::string &url, const nlohmann::json &body,
				lib::callback<std::string> &callback) = 0;

			/**
			 * Convenience method for PUT request with no body
			 */
			virtual void put(const std::string &url, lib::callback<std::string> &callback) = 0;

			/**
			 * POST request
			 * @param url URL to request
			 * @param callback Error message, or empty if none
			 */
			virtual void post(const std::string &url, lib::callback<std::string> &callback) = 0;

			/**
			 * DELETE request
			 * @param url URL to request
			 * @param json JSON body or null if no body
			 * @param callback Error message, or empty if none
			 */
			virtual void del(const std::string &url, const nlohmann::json &json,
				lib::callback<std::string> &callback) = 0;

			/**
			 * Convenience method for DELETE request with no body
			 */
			virtual void del(const std::string &url, lib::callback<std::string> &callback) = 0;
		};
	}
}