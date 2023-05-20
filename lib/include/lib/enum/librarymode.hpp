#pragma once

namespace lib
{
	/**
	 * How to display library/playlists in sidebar
	 */
	enum class library_mode
	{
		/**
		 * Stacked on top of each other
		 */
		stacked = 0,

		/**
		 * Tabbed, only showing one at a time
		 */
		tabbed = 1,
	};
}
