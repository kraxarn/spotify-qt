#pragma once

#include "lib/settings.hpp"
#include "lib/base64.hpp"
#include "lib/vector.hpp"
#include "lib/strings.hpp"
#include "lib/spotify/playback.hpp"
#include "lib/enum/repeatstate.hpp"
#include "lib/json.hpp"
#include "lib/enum/followtype.hpp"
#include "lib/spotify/error.hpp"
#include "lib/spotify/album.hpp"
#include "lib/spotify/artist.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/playlistdetails.hpp"
#include "lib/spotify/searchresults.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/audiofeatures.hpp"
#include "lib/spotify/savedalbum.hpp"
#include "lib/spotify/episode.hpp"
#include "lib/spotify/callback.hpp"
#include "lib/httpclient.hpp"
#include "lib/datetime.hpp"

#include "thirdparty/json.hpp"

namespace lib
{
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
			 * @param http_client HTTP Client for requests
			 */
			api(lib::settings &settings, const lib::http_client &http_client);

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

			void saved_albums(lib::callback<std::vector<lib::spt::saved_album>> &callback);

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
			 * Get me/player/play with device_id set if available
			 */
			auto play_tracks_url() -> std::string;

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

			void create_playlist(const std::string &name, const std::string &description,
				const bool is_public, bool is_collaborative,
				lib::callback<lib::spt::playlist> &callback);

			void playlists(lib::callback<std::vector<lib::spt::playlist>> &callback);

			void playlist(const std::string &playlist_id,
				lib::callback<lib::spt::playlist> &callback);

			void edit_playlist(const std::string &playlist_id,
				const lib::spt::playlist_details &playlist,
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

			//region Shows

			/** Get a show */
			void show(const std::string &show_id,
				lib::callback<lib::spt::show> &callback);

			/** Get episodes in a show */
			void show_episodes(const lib::spt::show &show,
				lib::callback<std::vector<lib::spt::episode>> &callback);

			//endregion

			/**
			 * Refresh access token with refresh token
			 * @return Refresh was successful
			 */
			void refresh(bool force = false);

			/**
			 * Spotify ID (4uLU6hMCjMI75M1A2tKUQC) to Spotify URI
			 * (spotify:track:4uLU6hMCjMI75M1A2tKUQC)
			 * @param type URI type, for example artist, album, track, etc.
			 * @param id Spotify ID
			 * @note Returns result if already an URI
			 * @return Spotify URI
			 */
			static auto to_uri(const std::string &type, const std::string &id) -> std::string;

			/**
			 * Spotify URI (spotify:track:4uLU6hMCjMI75M1A2tKUQC) to Spotify ID
			 * (4uLU6hMCjMI75M1A2tKUQC)
			 * @param id Spotify ID
			 * @note Returns result if already an ID
			 * @return Spotify ID
			 */
			static auto to_id(const std::string &id) -> std::string;

		protected:
			/**
			 * Allow use to select device, by default, none is chosen
			 * @param devices Devices to select from
			 * @param callback Selected device
			 */
			virtual void select_device(const std::vector<lib::spt::device> &devices,
				lib::callback<lib::spt::device> &callback);

			/**
			 * Timestamp of last refresh
			 */
			long last_auth = 0;

			/**
			 * Settings
			 */
			lib::settings &settings;

			//region GET

			/**
			 * GET request
			 * @param response URL to request
			 * @param callback Response as JSON
			 * @note Temporarily protected
			 */
			void get(const std::string &response,
				lib::callback<nlohmann::json> &callback);

			/**
			 * GET a collection of items
			 * @param url URL to request
			 * @note Automatically handles paging
			 * @note Temporarily protected
			 * @throws std::exception
			 */
			void get_items(const std::string &url,
				lib::callback<nlohmann::json> &callback);

			/**
			 * Custom get_items when items are contained in a key
			 */
			void get_items(const std::string &url, const std::string &key,
				lib::callback<nlohmann::json> &callback);

			//endregion

			//region PUT

			/**
			 * PUT request
			 * @param url URL to request
			 * @param body JSON body or null if no body
			 * @param callback Error message, or empty if none
			 */
			void put(const std::string &url, const nlohmann::json &body,
				lib::callback<std::string> &callback);

			/**
			 * Convenience method for PUT request with no body
			 */
			void put(const std::string &url, lib::callback<std::string> &callback);

			//endregion

			//region POST

			/**
			 * POST request
			 * @param url URL to request
			 * @param json JSON body or null if no body
			 * @param callback Error message, or empty if none
			 */
			void post(const std::string &url, const nlohmann::json &json,
				lib::callback<nlohmann::json> &callback);

			/**
			 * Convenience method for POST request with no body
			 */
			void post(const std::string &url, lib::callback<std::string> &callback);

			//endregion

			//region DELETE

			/**
			 * DELETE request
			 * @param url URL to request
			 * @param json JSON body or null if no body
			 * @param callback Error message, or empty if none
			 */
			void del(const std::string &url, const nlohmann::json &json,
				lib::callback<std::string> &callback);

			/**
			 * Convenience method for DELETE request with no body
			 */
			void del(const std::string &url, lib::callback<std::string> &callback);

			//endregion

			/**
			 * Get string interpretation of a follow type
			 * @param type Follow type
			 */
			static auto follow_type_string(lib::follow_type type) -> std::string;

		private:
			/**
			 * Implementation of HTTP Client
			 */
			const lib::http_client &http;

			/**
			 * Send request to refresh access token
			 * @param post_data POST form data
			 * @param authorization Authorization header
			 * @note Only required until networking is properly implemented
			 * @return JSON response with (maybe) new access token
			 */
			auto request_refresh(const std::string &post_data,
				const std::string &authorization) -> std::string;

			/**
			 * Parse JSON from string data
			 * @param url Requested URL (used for error logging)
			 * @param data JSON data
			 * @note Throws if JSON failed to parse or JSON contains an error object
			 * @returns Parsed JSON, or null object if no data
			 */
			static auto parse_json(const std::string &url,
				const std::string &data) -> nlohmann::json;

			/**
			 * Get error message from JSON response
			 */
			static auto error_message(const std::string &url,
				const std::string &data) -> std::string;

			/**
			 * Get authorization header, and refresh if needed
			 */
			auto auth_headers() -> lib::headers;

			/**
			 * Get full API url from relative URL
			 */
			static auto to_full_url(const std::string &relative_url) -> std::string;

			/**
			 * Set last used device
			 * @param id Device ID
			 */
			void set_current_device(const std::string &id);

			/**
			 * Get last used device
			 */
			auto get_current_device() const -> const std::string &;
		};
	}
}
