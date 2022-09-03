#pragma once

#include "lib/httpclient.hpp"
#include "lib/result.hpp"
#include "lib/spotify/error.hpp"

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

			//region GET

			/**
			 * GET request
			 * @param url URL to request
			 * @param callback JSON response if successful, or error message on failure
			 */
			template<typename T>
			void get(const std::string &url, lib::callback<lib::result<T>> &callback)
			{
				http.get(lib::spt::request::to_full_url(url), auth_headers(),
					[url, callback](const std::string &response)
					{
						callback(parse_json<T>(response));
					});
			}

			//endregion

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

			/**
			 * Parse JSON from string data
			 * @param url Requested URL (used for error logging)
			 * @param data JSON data
			 * @returns Parsed JSON, or fail on error
			 */
			template<typename T>
			static auto parse_json(const std::string &data) -> lib::result<T>
			{
				if (data.empty())
				{
					return lib::result<T>::ok({});
				}

				try
				{
					const T json = nlohmann::json::parse(data);
					if (!lib::spt::error::is(json))
					{
						return lib::result<T>::ok(json);
					}

					const auto message = lib::spt::error::error_message(json);
					return lib::result<T>::fail(message);
				}
				catch (const nlohmann::json::parse_error &e)
				{
					lib::log::debug("JSON: {}", data);
					return lib::result<T>::fail(e.what());
				}
				catch (const std::exception &e)
				{
					return lib::result<T>::fail(e.what());
				}
			}

			// Until all requests are moved to here
			friend class api;
		};
	}
}
