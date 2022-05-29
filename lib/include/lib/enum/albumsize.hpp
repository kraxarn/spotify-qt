#pragma once

namespace lib
{
	/**
	 * Size of album cover in ui
	 */
	enum class album_size
	{
		/**
		 * Fixed size, around 64x64
		 */
		small,

		/**
		 * Expand size to width of parent content
		 */
		expanded,
	};
}
