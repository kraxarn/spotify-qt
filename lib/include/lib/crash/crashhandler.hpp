#pragma once

#include "lib/log.hpp"
#include "lib/crash/crashinfo.hpp"
#include "lib/cache/jsoncache.hpp"

#include <sstream>

namespace lib
{
	/**
	 * Handles crashes and logs to file
	 * @note Currently only compatible with gcc, does nothing on other compilers
	 */
	class crash_handler
	{
	public:
		/**
		 * Initialize crash handler
		 * @return Initialization was successful
		 */
		static auto init() -> bool;

		/** Set cache instance for logging crashes to file */
		static void set_cache(lib::cache &cache);

		/** Log crash to file, if cache instance is set, and stderr */
		static void log(const lib::crash_info &info);

	private:
		/** Static class */
		crash_handler() = default;

		/** How far back to trace */
		static constexpr size_t backtrace_size = 16;

		/** Cache instance */
		static lib::cache *cache;
	};
}
