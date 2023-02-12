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
		/** Kilo, 1,000 */
		static constexpr int kilo = 1000;
		/** Mega, 1,000,000 */
		static constexpr int mega = 1000000;
		/** Giga, 1,000,000,000 */
		static constexpr int giga = 1000000000;

		/**
		 * Format time as M:SS
		 * @param milliseconds Milliseconds
		 */
		static auto time(int milliseconds) -> std::string;

		/**
		 * Format time as "... h ... m"
		 */
		static auto time_pretty(int milliseconds) -> std::string;

		/**
		 * Format size as B, kB, MB or GB (bytes)
		 * @param bytes Bytes
		 */
		static auto size(unsigned long bytes) -> std::string;

		/**
		 * Format as k or M
		 * @param count Amount
		 */
		static auto count(unsigned int count) -> std::string;
	};
}
