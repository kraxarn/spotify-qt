#pragma once

#include "../lib/base64.h"
#include "../lib/httplib.h"
#include "../lib/json.hpp"
#include "../settings.hpp"
#include "../util/strings.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * Initial user authentication
		 */
		class Auth
		{
		public:
			/**
			 * Create new instance
			 * @param settings Settings
			 */
			explicit Auth(Settings &settings);

			/**
			 * Get authentication URL
			 * @param clientId Spotify client ID
			 * @param redirect URL to redirect to after authentication
			 * @return Any possible error message
			 */
			static std::string authUrl(const std::string &clientId, const std::string &redirect);

			/**
			 * Authenticate
			 * @param code Code from authentication URL
			 * @param redirect Same redirect URL as when getting authentication URL
			 * @param id Spotify client ID
			 * @param secret Spotify client secret
			 * @return Any possible error message
			 */
			std::string auth(const std::string &code, const std::string &redirect,
				const std::string &id, const std::string &secret);

		private:
			httplib::Client httpClient;
			Settings &settings;
		};
	}
}