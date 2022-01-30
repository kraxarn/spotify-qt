#pragma once

namespace lib
{
	/**
	 * Order to show playlists in
	 */
	enum class playlist_order: char
	{
		/**
		 * As they are fetched from the Spotify API
		 */
		none = 0,

		/**
		 * Alphabetical, A-Z
		 */
		alphabetical = 1,

		/**
		 * Most recent first
		 */
		recent = 2,

		/**
		 * Custom order as specified in settings
		 */
		custom = 3
	};
}
