#pragma once

#include "lib/httpclient.hpp"
#include "lib/spotify/callback.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotifyerror.hpp"
#include "lib/strings.hpp"

namespace lib
{
	/**
	 * Lyrics helper
	 * @note May or not be against ToS, use at your own risk
	 */
	class lyrics
	{
	public:
		/**
		 * Instance a new lyrics helper
		 * @param http_client HTTP client instance
		 */
		explicit lyrics(const lib::http_client &http_client);

		/**
		 * Try to get lyrics
		 * @param track Track to get lyrics for
		 * @param callback Callback with lyrics
		 * @throws runtime_error Failed to get lyrics or none found
		 */
		void get(const lib::spt::track &track,
			lib::result<std::string> &callback);

	private:
		const lib::http_client &http;

		/**
		 * Format string in a URL-friendly way
		 */
		static auto format(const std::string &str) -> std::string;

		/**
		 * Try to get lyrics from lyrics class
		 */
		static auto get_from_lyrics(const std::string &lyrics) -> std::string;

		/**
		 * Try to get lyrics from lyrics root class
		 */
		 static auto get_from_lyrics_root(const std::string &lyrics) -> std::string;
	};
}
