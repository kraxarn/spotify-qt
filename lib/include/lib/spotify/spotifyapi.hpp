#pragma once

#include "lib/settings.hpp"
#include "lib/base64.hpp"
#include "lib/vector.hpp"
#include "lib/strings.hpp"
#include "lib/spotify/playback.hpp"
#include "lib/enum/repeatstate.hpp"
#include "lib/json.hpp"
#include "lib/enum/followtype.hpp"
#include "lib/spotifyerror.hpp"
#include "lib/spotify/album.hpp"
#include "lib/spotify/artist.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/searchresults.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/audiofeatures.hpp"

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
		class api
		{
		public:
			/**
			 * Construct a new instance and refresh if needed
			 * @param settings Settings for access token and refresh token
			 */
			explicit api(lib::settings &settings);

			//region Albums

			void album(const std::string &id,
				lib::callback<lib::spt::album> &callback);

			void album_tracks(const lib::spt::album &album,
				lib::callback<std::vector<lib::spt::track>> &callback);

			//endregion

			//region Artists

			void artist(const std::string &id,
				lib::callback<lib::spt::artist> &callback);

			void top_tracks(const lib::spt::artist &artist,
				lib::callback<std::vector<lib::spt::track>> &callback);

			void related_artists(const lib::spt::artist &artist,
				lib::callback<std::vector<lib::spt::artist>> &callback);

			void albums(const lib::spt::artist &artist,
				lib::callback<std::vector<lib::spt::album>> &callback);

			//endregion

			//region Browse

			void new_releases(lib::callback<std::vector<lib::spt::album>> &callback);

			//endregion

			//region Follow

			void followed_artists(lib::callback<std::vector<lib::spt::artist>> &callback);

			void follow(lib::follow_type type, const std::vector<std::string> &ids,
				lib::callback<std::string> &callback);

			void unfollow(lib::follow_type type, const std::vector<std::string> &ids,
				lib::callback<std::string> &callback);

			void is_following(lib::follow_type type, const std::vector<std::string> &ids,
				lib::callback<std::vector<bool>> &callback);

			//endregion

			//region Library

			void saved_albums(lib::callback<std::vector<lib::spt::album>> &callback);

			void saved_tracks(lib::callback<std::vector<lib::spt::track>> &callback);

			void add_saved_track(const std::string &track_id,
				lib::callback<std::string> &callback);

			void remove_saved_track(const std::string &track_id,
				lib::callback<std::string> &callback);

			void is_saved_track(const std::vector<std::string> &track_ids,
				lib::callback<std::vector<bool>> &callback);

			//endregion

			//region Personalization

			void top_artists(lib::callback<std::vector<lib::spt::artist>> &callback);

			void top_tracks(lib::callback<std::vector<lib::spt::track>> &callback);

			//endregion

			//region Player

			/**
			 * Get what's currently playing
			 */
			void current_playback(lib::callback<lib::spt::playback> &callback);

			/**
			 * Set current active device by device ID
			 */
			void set_device(const std::string &device_id, lib::callback<std::string> &callback);

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
			void set_repeat(lib::repeat_state state, lib::callback<std::string> &callback);

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

			void playlists(lib::callback<std::vector<lib::spt::playlist>> &callback);

			void playlist(const std::string &playlist_id,
				lib::callback<lib::spt::playlist> &callback);

			void edit_playlist(const lib::spt::playlist &playlist,
				lib::callback<std::string> &callback);

			void playlist_tracks(const lib::spt::playlist &playlist,
				lib::callback<std::vector<lib::spt::track>> &callback);

			void add_to_playlist(const std::string &playlist_id, const std::string &track_id,
				lib::callback<std::string> &callback);

			void remove_from_playlist(const std::string &playlist_id, const std::string &track_id,
				int pos, lib::callback<std::string> &callback);

			//endregion

			//region Search

			void search(const std::string &query,
				lib::callback<lib::spt::search_results> &callback);

			//endregion

			//region Tracks

			void track(const std::string &id,
				lib::callback<lib::spt::track> &callback);

			void track_audio_features(const std::string &track_id,
				lib::callback<lib::spt::audio_features> &callback);

			//endregion

			//region User Profile

			void me(lib::callback<lib::spt::user> &callback);

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

			//region GET

			/**
			 * GET request
			 * @param url URL to request
			 * @param callback Response as JSON
			 * @note Temporarily protected
			 */
			virtual void get(const std::string &url,
				lib::callback<nlohmann::json> &callback) = 0;

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

			//endregion

			//region PUT

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
			void put(const std::string &url, lib::callback<std::string> &callback);

			//endregion

			//region POST

			/**
			 * POST request
			 * @param url URL to request
			 * @param callback Error message, or empty if none
			 */
			virtual void post(const std::string &url, lib::callback<std::string> &callback) = 0;

			//endregion

			//region DELETE

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
			void del(const std::string &url, lib::callback<std::string> &callback);

			//endregion

			/**
			 * Get string interpretation of a follow type
			 * @param type Follow type
			 */
			static std::string follow_type_string(lib::follow_type type);

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
			 * Parse JSON from string data
			 * @param url Requested URL (used for error logging)
			 * @param data JSON data
			 * @note Throws if JSON failed to parse or JSON contains an error object
			 * @returns Parsed JSON, or null object if no data
			 */
			static nlohmann::json parse_json(const std::string &url, const std::string &data);
		};
	}
}