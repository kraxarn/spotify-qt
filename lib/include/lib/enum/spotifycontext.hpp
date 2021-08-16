#pragma once

namespace lib
{
	/**
	 * Spotify context, usually something that can be played from
	 * @note These can be combined (where it makes sense), and therefore uses
	 * bit order
	 */
	enum class spotify_context
	{
		/**
		 * No context
		 */
		none = 0,

		/**
		 * All contexts, used for some settings
		 */
		all = 1,

		/**
		 * Tracks in an album
		 */
		album = 2,

		/**
		 * Top tracks from an artist
		 */
		artist = 4,

		/**
		 * Tracks in a playlist
		 */
		playlist = 8
	};
}
