#pragma once

namespace lib
{
	/**
	 * Shape to mask album
	 */
	enum class album_shape
	{
		/**
		 * No masking, probably square
		 */
		none = 0,

		/**
		 * Masking in the shape of the app's logo
		 */
		app = 1,

		/**
		 * A round circle
		 */
		circle = 2,

		/**
		 * A round circle with a hole in the middle, like a CD
		 */
		disc = 3,

		/**
		 * Rounded corners
		 */
		rounded = 4,
	};
}
