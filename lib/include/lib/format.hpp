#pragma once

#include <string>

namespace lib
{
	/**
	 * Formatting utilities
	 */
	class format
	{
	public:
		/**
		 * Format time as M:SS
		 * @param milliseconds Milliseconds
		 */
		static auto time(int milliseconds) -> std::string;

		/**
		 * Format size as B, kB, MB or GB (bytes)
		 * @param bytes Bytes
		 */
		static auto size(unsigned int bytes) -> std::string;

		/**
		 * Format as k or M
		 * @param count Amount
		 */
		static auto count(unsigned int count) -> std::string;

	private:
		/** Milliseconds in a second */
		static constexpr int ms_in_sec = 1000;
		/** Seconds in a minute */
		static constexpr int secs_in_min = 60;
		/** Kilo, 1,000 */
		static constexpr int kilo = 1000;
		/** Mega, 1,000,000 */
		static constexpr int mega = 1000000;
		/** Giga, 1,000,000,000 */
		static constexpr int giga = 1000000000;
	};
}
