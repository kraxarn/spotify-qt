#pragma once

#include "lib/httpclient.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * Request, and token, manager
		 */
		class request
		{
		public:
			request(lib::settings &settings, const lib::http_client &http_client);

			/**
			 * Refresh access token with refresh token
			 */
			void refresh(bool force = false);

		private:
			lib::settings &settings;
			const lib::http_client &http;

			/**
			 * Timestamp of last refresh
			 */
			unsigned long last_auth = 0;

			/**
			 * Get full API url from relative URL
			 */
			static auto to_full_url(const std::string &relative_url) -> std::string;

			/**
			 * Get authorization header, and refresh if needed
			 */
			auto auth_headers() -> lib::headers;

			/**
			 * Send request to refresh access token
			 * @param post_data POST form data
			 * @param authorization Authorization header
			 * @return JSON response with (maybe) new access token
			 */
			auto request_refresh(const std::string &post_data,
				const std::string &authorization) -> std::string;

			// Until all requests are moved to here
			friend class api;
		};
	}
}
