#pragma once

#define FMT_HEADER_ONLY
#include "thirdparty/fmt/format.h"

#include "enum/logtype.hpp"

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
		 * @param ms Milliseconds
		 */
		static std::string time(int ms);

		/**
		 * Format size as B, kB, MB or GB (bytes)
		 * @param bytes Bytes
		 */
		static std::string size(unsigned int bytes);

		/**
		 * Format as k or M
		 * @param count Amount
		 */
		static auto count(unsigned int count) -> std::string;
	};
}
