#pragma once

#include "lib/httpclient.hpp"
#include "lib/result.hpp"
#include "lib/lyrics/lyrics.hpp"
#include "lib/spotify/callback.hpp"
#include "lib/spotify/track.hpp"

namespace lib::lrc
{
	/**
		 * Lyrics API
		 */
	class api
	{
	public:
		/**
		 * Instance a new lyrics helper
		 * @param http_client HTTP client instance
		 */
		explicit api(const http_client &http_client);

		/**
		 * Set app info for use in user-agent
		 * @param name Application name
		 * @param version Application version
		 * @param homepage Application homepage
		 */
		void set_app_info(const std::string &name, const std::string &version, const std::string &homepage);

		/**
		 * Search for lyrics
		 * @param query Keyword of title, artist or album
		 * @param callback Lyrics
		 */
		void search(const std::string &query, callback<Result<std::vector<lyrics>>> &callback) const;

		/**
		 * Fetch lyrics
		 * @param track Track to fetch lyrics for
		 * @param callback Lyrics
		 */
		void get(const spt::track &track, callback<Result<lyrics>> &callback) const;

		/**
		 * Fetch lyrics by ID
		 * @param lyricsId ID to fetch lyrics for
		 * @param callback Lyrics
		 */
		void get(unsigned int lyricsId, callback<Result<lyrics>> &callback) const;

	private:
		const http_client &http;
		std::string user_agent;

		[[nodiscard]]
		auto headers() const -> headers;
	};
}
