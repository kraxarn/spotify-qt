#pragma once

#include "thirdparty/json.hpp"

#include <stdexcept>

#include <QString>

class SpotifyErrorUtil
{
public:
	/**
	 * If JSON is an error object
	 */
	[[nodiscard]]
	static auto isErrorObject(const nlohmann::json &json) -> bool;

	/**
	 * Get error message from JSON
	 * @returns Error, or an empty string if no error
	 */
	[[nodiscard]]
	static auto errorMessage(const nlohmann::json &json) -> QString;

private:
	SpotifyErrorUtil() = default;
};

namespace lib
{
	namespace spt
	{
		/**
		 * Spotify request error
		 */
		class [[deprecated("Don't use exceptions")]] error : public std::runtime_error
		{
		public:
			/**
			 * Construct a new Spotify error
			 * @param what What went wrong
			 * @param url URL that failed
			 */
			error(const char *what, const char *url);

			/**
			 * Construct a new Spotify error with C++ strings
			 */
			error(const std::string &what, const std::string &url);

			/**
			 * URL that returned an error
			 */
			auto url() -> const char *;

			[[deprecated("Use SpotifyErrorUtil::isErrorObject instead")]]
			static auto is(const nlohmann::json &json) -> bool;

			[[deprecated("Use SpotifyErrorUtil::errorMessage instead")]]
			static auto error_message(const nlohmann::json &json) -> std::string;

		private:
			const char *request_url;
		};
	}
}
