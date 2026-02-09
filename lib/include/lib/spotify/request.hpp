#pragma once

#include "lib/fmt.hpp"
#include "lib/httpclient.hpp"
#include "lib/log.hpp"
#include "lib/result.hpp"
#include "lib/spotify/deviceselect.hpp"
#include "lib/spotify/error.hpp"
#include "lib/spotify/page.hpp"
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
			request(lib::settings &settings, const lib::http_client &http_client,
				const lib::spt::device_select &device_select);

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
			void get(const std::string &url, lib::callback<Result<T>> &callback)
			{
				http.get(lib::spt::to_full_url(url), auth_headers(),
					[callback](const Result<std::string> &response)
					{
						if (!response.success())
						{
							const auto message = parseErrorMessage(response.message());
							callback(Result<T>::fail(message));
							return;
						}
						callback(parse_json<T>(response.value()));
					});
			}

			/**
			 * Get a paged list of items contained in specified key
			 * @tparam T Type of item to fetch
			 * @param url Initial URL to fetch from
			 * @param key Key to fetch items from
			 * @param callback Callback with page index, returning true to fetch the next page
			 */
			template<typename T>
			void get_page(const std::string &url, const std::string &key,
				const std::function<bool(const Result<lib::spt::page<T>> &)> &callback)
			{
				const auto api_url = lib::strings::starts_with(url, "https://")
					? lib::spt::to_relative_url(url)
					: url;

				get<nlohmann::json>(api_url, [this, key, callback](const Result<nlohmann::json> &result)
				{
					if (!result.success())
					{
						const QString message = parseErrorMessage(result.message());
						callback(Result<lib::spt::page<T>>::fail(message));
						return;
					}

					const auto &json = result.value();
					if (!key.empty() && !json.contains(key))
					{
						const QString message = QStringLiteral("No such key: %1")
							.arg(QString::fromStdString(key));

						callback(Result<lib::spt::page<T>>::fail(message));
						return;
					}

					lib::spt::page<T> page;
					try
					{
						page = key.empty() ? json : json.at(key);
					}
					catch (const std::exception &exception)
					{
						const QString message = QString::fromStdString(exception.what());
						callback(Result<lib::spt::page<T>>::fail(message));
						return;
					}

					if (!callback(Result<lib::spt::page<T>>::ok(page))
						|| !page.has_next())
					{
						return;
					}

					get_page(page.next, key, callback);
				});
			}

			/**
			 * POST request without body
			 */
			void post(const std::string &url, lib::callback<Result<void *>> &callback)
			{
				auto headers = auth_headers();
				headers["Content-Type"] = "application/x-www-form-urlencoded";

				http.post(lib::spt::to_full_url(url), headers,
					[callback](const Result<std::string> &response)
					{
						if (!response.success())
						{
							const QString message = parseErrorMessage(response.message());
							callback(Result<void *>::fail(message));
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
			const lib::spt::device_select &device_select;

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
			 * Get last used device
			 */
			auto get_current_device() const -> const std::string &;

			/**
			 * Set last used device
			 * @param id Device ID
			 */
			void set_current_device(const std::string &device_id);

			/**
			 * Parse JSON from string data
			 * @param url Requested URL (used for error logging)
			 * @param data JSON data
			 * @returns Parsed JSON, or fail on error
			 */
			template<typename T>
			static auto parse_json(const std::string &data) -> Result<T>
			{
				if (data.empty())
				{
					return Result<T>::ok({});
				}

				try
				{
					const auto json = nlohmann::json::parse(data);
					if (!lib::spt::error::is(json))
					{
						return Result<T>::ok(json);
					}

					const QString message = SpotifyErrorUtil::errorMessage(json);
					return Result<T>::fail(message);
				}
				catch (const nlohmann::json::parse_error &e)
				{
					lib::log::debug("JSON: {}", data);
					return Result<T>::fail(e.what());
				}
				catch (const std::exception &e)
				{
					return Result<T>::fail(e.what());
				}
			}

			/**
			 * Parse error from JSON
			 */
			static auto parse_json(const std::string &data) -> Result<void *>
			{
				if (data.empty())
				{
					return Result<void *>::ok(nullptr);
				}

				try
				{
					const nlohmann::json json = nlohmann::json::parse(data);
					if (!lib::spt::error::is(json))
					{
						return Result<void *>::ok(nullptr);
					}

					const auto message = SpotifyErrorUtil::errorMessage(json);
					return Result<void *>::fail(message);
				}
				catch (const nlohmann::json::parse_error &e)
				{
					log::debug("Failed to parse json: {}", e.what());
					log::debug("JSON: {}", data);
					return Result<void *>::ok(nullptr);
				}
				catch (const std::exception &e)
				{
					return Result<void *>::fail(e.what());
				}
			}

			[[nodiscard]]
			static auto parseErrorMessage(const QString &data) -> QString
			{
				try
				{
					const nlohmann::json json = nlohmann::json::parse(data.toStdString());
					return SpotifyErrorUtil::isErrorObject(json)
						? SpotifyErrorUtil::errorMessage(json)
						: data;
				}
				catch (const std::exception &e)
				{
					lib::log::error("Failed to parse error message: {}", e.what());
					return data;
				}
			}

			// Until all requests are moved to here
			friend class api;
		};
	}
}
