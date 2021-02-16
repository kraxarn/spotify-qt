#pragma once

namespace lib
{
	/**
	 * Spotify context, usually something that can be played from
	 * @note These can be combined (where it makes sense), and therefore uses
	 * bit order
	 */
	enum spotify_context
	{
		/**
		 * No context
		 */
		context_none = 0,

		/**
		 * All contexts, used for some settings
		 */
		context_all = 1,

		/**
		 * Tracks in an album
		 */
		context_album = 2,

		/**
		 * Top tracks from an artist
		 */
		context_artist = 4,

		/**
		 * Tracks in a playlist
		 */
		context_playlist = 8
	};
}
