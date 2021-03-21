#pragma once

#include "lib/settings.hpp"
#include "lib/base64.hpp"
#include "lib/vector.hpp"

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
		};
	}
}