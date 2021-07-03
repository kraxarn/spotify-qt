#pragma once

#include "lib/log.hpp"
#include "lib/crash/crashinfo.hpp"
#include "lib/cache/jsoncache.hpp"

#ifdef IS_GNU_CXX
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

	private:
		crash_handler() = default;

		/** How far back to trace */
		static constexpr size_t backtrace_size = 16;

		/** Cache instance */
		static lib::cache *cache;

#ifdef IS_GNU_CXX
		/** Handle signal (gcc) */
		static void handle(int signal, struct sigcontext context);
#endif
	};
}
