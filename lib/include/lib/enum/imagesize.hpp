#pragma once

namespace lib
{
	/**
	 * Various image sizes
	 */
	enum class image_size: short
	{
		/**
		 * Unexpected size
		 */
		unknown = 0,

		/**
		 * 64x64
		 */
		small = 64,

		/**
		 * 300x300
		 */
		medium = 300,

		/**
		 * 640x640
		 */
		large = 640,
	};
}
