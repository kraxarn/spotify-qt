#pragma once

namespace lib
{
	namespace time
	{
		/**
		 * Milliseconds in a second
		 */
		static constexpr int ms_in_sec = 1000;

		/**
		 * Seconds in a minute
		 */
		static constexpr int secs_in_min = 60;

		/**
		 * Milliseconds in a minute
		 */
		static constexpr int ms_in_min = ms_in_sec * secs_in_min;
	}
}
