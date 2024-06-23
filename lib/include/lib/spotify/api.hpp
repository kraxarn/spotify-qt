#pragma once

#include "lib/httpclient.hpp"
#include "lib/settings.hpp"
#include "lib/vector.hpp"
#include "lib/enum/followtype.hpp"
#include "lib/enum/repeatstate.hpp"
#include "lib/spotify/album.hpp"
#include "lib/spotify/artist.hpp"
#include "lib/spotify/audiofeatures.hpp"
#include "lib/spotify/callback.hpp"
#include "lib/spotify/episode.hpp"
#include "lib/spotify/page.hpp"
#include "lib/spotify/playback.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/playlistdetails.hpp"
#include "lib/spotify/queue.hpp"
#include "lib/spotify/request.hpp"
#include "lib/spotify/savedalbum.hpp"
#include "lib/spotify/searchresults.hpp"
#include "lib/spotify/track.hpp"

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
			 * @param request Request instance for http requests
			 */
			api(lib::settings &settings, const lib::http_client &http_client,
				lib::spt::request &request);

			//region Albums

			void album(const std::string &id,
				lib::callback<lib::spt::album> &callback);

			void album_tracks(const spt::album &album,
				const paged_callback<spt::track> &callback) const;

			//endregion

			//region Artists

			void artist(const std::string &id,
				lib::callback<lib::spt::artist> &callback);

			void top_tracks(const lib::spt::artist &artist,
				lib::callback<std::vector<lib::spt::track>> &callback);

			void related_artists(const lib::spt::artist &artist,
				lib::callback<std::vector<lib::spt::artist>> &callback);

			void albums(const spt::artist &artist, const std::vector<album_group> &groups,
				const paged_callback<spt::album> &callback) const;

			//endregion

			//region Browse

			void new_releases(const paged_callback<spt::album> &callback) const;

			//endregion

			//region Follow

			void followed_artists(const paged_callback<spt::artist> &callback) const;

			void follow(lib::follow_type type, const std::vector<std::string> &ids,
				lib::callback<std::string> &callback);

			void unfollow(lib::follow_type type, const std::vector<std::string> &ids,
				lib::callback<std::string> &callback);

			void is_following(lib::follow_type type, const std::vector<std::string> &ids,
				lib::callback<std::vector<bool>> &callback);

			void follow_playlist(const std::string &playlist_id,
				lib::callback<std::string> &callback);

			void unfollow_playlist(const std::string &playlist_id,
				lib::callback<std::string> &callback);

			void is_following_playlist(const std::string &playlist_id,
				const std::vector<std::string> &user_ids,
				lib::callback<std::vector<bool>> &callback);

			//endregion

			//region Library

			void saved_albums(const paged_callback<saved_album> &callback) const;

			void add_saved_albums(const std::vector<std::string> &album_ids,
				lib::callback<std::string> &callback);

			void remove_saved_albums(const std::vector<std::string> &album_ids,
				lib::callback<std::string> &callback);

			void is_saved_album(const std::vector<std::string> &album_ids,
				lib::callback<std::vector<bool>> &callback);

			/**
			 * @deprecated Use with pagination instead
			 */
			void saved_tracks(lib::callback<std::vector<lib::spt::track>> &callback);

			void saved_tracks(const lib::paged_callback<lib::spt::track> &callback);

			void add_saved_tracks(const std::vector<std::string> &track_ids,
				lib::callback<std::string> &callback);

			void remove_saved_tracks(const std::vector<std::string> &track_ids,
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
			void current_playback(lib::callback<lib::result<lib::spt::playback>> &callback);

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
			void add_to_queue(const std::string &uri,
				lib::callback<lib::result<void *>> &callback);

			/**
			 * Get all queued tracks
			 */
			void queue(lib::callback<lib::result<lib::spt::queue>> &callback);

			//endregion

			//region Playlists

			/**
			 * Create a new playlist
			 * @param name Playlist name
			 * @param description Playlist description
			 * @param is_public Public playlist visible to everyone
			 * @param is_collaborative Collaborative playlist
			 * @param callback Created playlist
			 */
			void create_playlist(const std::string &name,
				const lib::optional<std::string> &description,
				const lib::optional<bool> &is_public,
				const lib::optional<bool> &is_collaborative,
				lib::callback<lib::spt::playlist> &callback);

			void playlists(const paged_callback<playlist> &callback) const;

			void playlist(const std::string &playlist_id,
				callback<result<playlist>> &callback) const;

			void edit_playlist(const std::string &playlist_id,
				const lib::spt::playlist_details &playlist,
				lib::callback<std::string> &callback);

			/**
			 * @deprecated Use with pagination instead
			 */
			void playlist_tracks(const lib::spt::playlist &playlist,
				lib::callback<std::vector<lib::spt::track>> &callback);

			/**
			 * @note Experimental
			 */
			void playlist_tracks(const lib::spt::playlist &playlist,
				const std::function<bool(const lib::result<lib::spt::page<lib::spt::track>> &)> &callback);

			void add_to_playlist(const std::string &playlist_id,
				const std::vector<std::string> &track_uris,
				lib::callback<std::string> &callback);

			void remove_from_playlist(const std::string &playlist_id,
				const std::vector<std::pair<int, std::string>> &track_index_uris,
				lib::callback<std::string> &callback);

			//endregion

			//region Search

			void search(const std::string &query,
				lib::callback<lib::spt::search_results> &callback);

			//endregion

			//region Tracks

			void track(const std::string &track_id,
				lib::callback<lib::spt::track> &callback);

			void track_audio_features(const std::string &track_id,
				lib::callback<lib::spt::audio_features> &callback);

			void track_audio_features(const std::vector<std::string> &track_ids,
				lib::callback<std::vector<lib::spt::audio_features>> &callback);

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

			void refresh(bool force);

		protected:
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
			 * @deprecated Use `lib::spt::request::post` instead
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
			const lib::http_client &http;
			lib::spt::request &request;

			/**
			 * Get error message from JSON response
			 */
			static auto error_message(const std::string &url,
				const std::string &data) -> std::string;
		};
	}
}
