#pragma once

#include "lib/httpclient.hpp"
#include "lib/spotify/callback.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/spotifyerror.hpp"
#include "lib/strings.hpp"
#include "lib/spotify/trackinfo.hpp"

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
			lib::callback<lib::spt::track_info> &callback);

	private:
		const lib::http_client &http;
	};
}
