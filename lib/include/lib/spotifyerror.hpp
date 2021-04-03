#pragma once

#include "thirdparty/json.hpp"

#include <stdexcept>

namespace lib
{
	/**
	 * Spotify request error
	 */
	class spotify_error: public std::runtime_error
	{
	public:
		/**
		 * Construct a new Spotify error
		 * @param what What went wrong
		 * @param url URL that failed
		 */
		spotify_error(const char* what, const char* url);

		/**
		 * Construct a new Spotify error with C++ strings
		 */
		spotify_error(const std::string &what, const std::string &url);

		/**
		 * URL that returned an error
		 */
		const char* url();

		/**
		 * If JSON is an error object
		 */
		static bool is(const nlohmann::json &json);

		/**
		 * Get error message from JSON
		 * @returns Error, or an empty string if no error
		 */
		static std::string error_message(const nlohmann::json &json);

	private:
		const char* request_url;
	};
}
