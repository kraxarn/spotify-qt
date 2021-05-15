#pragma once

#include "lib/settings.hpp"
#include "lib/httpclient.hpp"
#include "lib/strings.hpp"
#include "lib/base64.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * Authentication helper
		 */
		class auth
		{
		public:
			auth(lib::settings &settings, const lib::http_client &http_client);

			/**
			 * Get authentication URL
			 */
			static auto url(const std::string &client_id,
				const std::string &redirect) -> std::string;

			/**
			 * Authenticate
			 * @param code Authorization code
			 * @param redirect Redirect URL
			 * @param id Client ID
			 * @param secret Client Secret
			 * @param accessToken Outputted access token
			 * @param refreshToken Outputted refresh token
			 * @return Error, or empty string on success
			 */
			auto get(const std::string &code, const std::string &redirect_url,
				const std::string &id, const std::string &secret) -> std::string;

		private:
			lib::settings &settings;
			const lib::http_client &http_client;
		};
	}
}
