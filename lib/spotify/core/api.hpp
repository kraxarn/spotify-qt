#pragma once

#include "lib/json.hpp"
#include "settings.hpp"
#include "lib/httplib.h"
#include "playlists.hpp"
#include "lib/base64.h"
#include "result.hpp"

namespace lib
{
	namespace spt
	{
		namespace core
		{
			/**
			 * Core functionality of the Spotify API, basically an API wrapper with
			 * paging and automatic refreshing
			 */
			class api
			{
			public:
				/**
				 * Playlist related API calls
				 */
				playlists playlists;

				/**
				 * Construct a new core object
				 * @param settings Settings for getting and updating authentication details
				 */
				explicit api(Settings &settings);

			private:
				std::chrono::system_clock::time_point last_auth;
				bool refresh_valid = false;
				Settings &settings;
				httplib::Client http_client;

				/**
				 * GET request
				 * @param url URL to get
				 * @return JSON response
				 */
				nlohmann::json get(const std::string &url);

				/**
				 * Refresh access token
				 * @return If successful
				 */
				result refresh();
			};
		}
	}
}