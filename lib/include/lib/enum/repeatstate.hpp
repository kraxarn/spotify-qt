#pragma once

namespace lib
{
	/**
	 * Type of player repeat
	 */
	enum class repeat_state
	{
		/**
		 * Repeat current track
		 */
		track,

		/**
		 * Repeat current context
		 */
		context,

		/**
		 * No repeat
		 */
		off
	};
}