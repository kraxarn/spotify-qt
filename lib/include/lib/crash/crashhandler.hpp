#pragma once

#include "lib/log.hpp"
#include "lib/crash/crashinfo.hpp"
#include "lib/cache/jsoncache.hpp"

#ifdef __linux__
#ifdef IS_GNU_CXX
#define USE_GCC_CRASH_HANDLER
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
		static void init();

		static void set_cache(lib::cache &cache);

		static void log(const lib::crash_info &info);

	private:
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
