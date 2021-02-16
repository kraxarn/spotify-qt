#pragma once

namespace lib
{
	/**
	 * Order to show playlists in
	 */
	enum playlist_order
	{
		/**
		 * As they are fetched from the Spotify API
		 */
		playlist_order_default = 0,

		/**
		 * Alphabetical, A-Z
		 */
		playlist_order_alphabetical = 1,

		/**
		 * Most recent first
		 */
		playlist_order_recent = 2,

		/**
		 * Custom order as specified in settings
		 */
		playlist_order_custom = 3
	};
}
