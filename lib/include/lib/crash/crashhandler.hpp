#pragma once

#include "lib/log.hpp"
#include "lib/crash/crashinfo.hpp"
#include "lib/cache/jsoncache.hpp"

#ifdef __linux__
#ifdef __LP64__
#ifdef IS_GNU_CXX
#define USE_GCC_CRASH_HANDLER
#endif
#endif
#endif

#ifdef USE_GCC_CRASH_HANDLER
#include <execinfo.h>
#endif

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

#ifdef USE_GCC_CRASH_HANDLER
		/** Handle signal (gcc) */
		static void handle(int signal, struct sigcontext context);
#endif
	};
}
