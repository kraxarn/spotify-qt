#pragma once

namespace lib
{
	/**
	 * Different contexts to follow
	 */
	enum class follow_type: unsigned char
	{
		/**
		 * Following artist
		 */
		artist,

		/**
		 * Following user
		 */
		user
	};
}
