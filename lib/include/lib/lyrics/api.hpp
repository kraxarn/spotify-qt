#pragma once

#include "lib/httpclient.hpp"
#include "lib/spotify/callback.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/error.hpp"
#include "lib/strings.hpp"
#include "lib/spotify/trackinfo.hpp"
#include "lib/result.hpp"
#include "lib/lyrics/searchresult.hpp"
#include "lib/lyrics/lyrics.hpp"

// Based off:
// https://github.com/spicetify/spicetify-cli/blob/master/CustomApps/lyrics-plus

namespace lib
{
	namespace lrc
	{
		/**
		 * Lyrics API
		 * @note May or not be against ToS, use at your own risk
		 */
		class api
		{
		public:
			/**
			 * Instance a new lyrics helper
			 * @param http_client HTTP client instance
			 */
			explicit api(const lib::http_client &http_client);

			/**
			 * Search for lyrics
			 * @param track Track to search for
			 * @param callback A list of possible lyric ids if successful
			 */
			void search(const lib::spt::track &track,
				lib::callback<lib::result<std::vector<lib::lrc::search_result>>> &callback);

			/**
			 * Search for lyrics
			 * @param query Query to search for
			 * @param callback A list of possible lyric ids if successful
			 */
			void search(const std::string &query,
				lib::callback<lib::result<std::vector<lib::lrc::search_result>>> &callback);

			/**
			 * Load lyrics from ID
			 * @param lyrics_id Lyrics ID from search
			 * @param callback Lyrics
			 */
			void lyrics(int lyrics_id, lib::callback<lib::result<lib::lrc::lyrics>> &callback);

		private:
			const lib::http_client &http;

			static auto headers() -> lib::headers;
		};
	}
}
