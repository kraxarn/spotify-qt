#pragma once

#include "lib/httpclient.hpp"
#include "lib/result.hpp"
#include "lib/spotify/error.hpp"
#include "lib/spotify/util.hpp"

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
				http.get(lib::spt::to_full_url(url), auth_headers(),
					[url, callback](const lib::result<std::string> &response)
					{
						if (!response.success())
						{
							const auto message = parse_error_message(response.message());
							callback(lib::result<T>::fail(message));
							return;
						}
						callback(parse_json<T>(response.value()));
					});
			}

			/**
			 * POST request without body
			 */
			void post(const std::string &url, lib::callback<lib::result<void *>> &callback)
			{
				auto headers = auth_headers();
				headers["Content-Type"] = "application/x-www-form-urlencoded";

				http.post(lib::spt::to_full_url(url), headers,
					[callback](const lib::result<std::string> &response)
					{
						if (!response.success())
						{
							const auto message = parse_error_message(response.message());
							callback(lib::result<void *>::fail(message));
							return;
						}
						callback(parse_json(response.value()));
					});
			}

			//endregion

		private:
			/**
			 * Seconds in an hour
			 */
			static constexpr long secs_in_hour = 60L * 60L;

			lib::settings &settings;
			const lib::http_client &http;

			/**
			 * Timestamp of last refresh
			 */
			unsigned long last_auth = 0;

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

			/**
			 * Parse error from JSON
			 */
			static auto parse_json(const std::string &data) -> lib::result<void *>
			{
				if (data.empty())
				{
					return lib::result<void *>::ok(nullptr);
				}

				try
				{
					const nlohmann::json json = nlohmann::json::parse(data);
					if (!lib::spt::error::is(json))
					{
						return lib::result<void *>::ok(nullptr);
					}

					const auto message = lib::spt::error::error_message(json);
					return lib::result<void *>::fail(message);
				}
				catch (const nlohmann::json::parse_error &e)
				{
					lib::log::debug("JSON: {}", data);
					return lib::result<void *>::fail(e.what());
				}
				catch (const std::exception &e)
				{
					return lib::result<void *>::fail(e.what());
				}
			}

			static auto parse_error_message(const std::string &data) -> std::string
			{
				try
				{
					const nlohmann::json json = nlohmann::json::parse(data);
					return lib::spt::error::is(json)
						? lib::spt::error::error_message(json)
						: data;
				}
				catch (const std::exception &e)
				{
					return data;
				}
			}

			// Until all requests are moved to here
			friend class api;
		};
	}
}
