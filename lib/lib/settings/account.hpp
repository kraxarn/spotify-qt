#pragma once

#include <string>

namespace lib
{
	namespace set
	{
		/**
		 * Account related settings
		 */
		typedef struct account
		{
			/**
			 * Access token used for Spotify API requests
			 */
			std::string access_token;

			/**
			 * Refresh token used to refresh access token
			 */
			std::string refresh_token;

			/**
			 * Application client id
			 */
			std::string client_id;

			/**
			 * Application client secret
			 */
			std::string client_secret;

			/**
			 * When access token was last refreshed in unix time
			 */
			long last_refresh = 0;
		} account;
	}
}
