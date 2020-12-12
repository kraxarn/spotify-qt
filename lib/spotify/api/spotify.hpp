#pragma once

#include "api.hpp"
#include "lib/json.hpp"
#include "settings.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * Main handler for everything Spotify
		 */
		class spotify
		{
		public:
			/**
			 * Construct a new spotify manager
			 * @param settings Settings for API credentials
			 */
			explicit spotify(Settings &settings);

			/**
			 * GET request to a Spotify endpoint
			 * @note Should usually not be called manually
			 * @param url URL to request
			 * @return JSON responses
			 */
			nlohmann::json get(const std::string &url);

		private:
			/**
			 * api
			 */
			api::api api;
		};
	}
}