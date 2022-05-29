#pragma once

namespace lib
{
	/**
	 * Size of album cover in ui
	 */
	enum class album_size: char
	{
		/**
		 * Fixed size, around 64x64
		 */
		small = 0,

		/**
		 * Expand size to width of parent content
		 */
		expanded = 1,
	};
}
