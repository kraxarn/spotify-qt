#pragma once

#include "lib/log.hpp"
#include "lib/crash/crashinfo.hpp"
#include "lib/cache.hpp"

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
		crash_handler(lib::cache &cache);

	private:
		/** How far back to trace */
		static constexpr size_t backtrace_size = 16;

		lib::cache &cache;

#ifdef IS_GNU_CXX
		/** Handle signal (gcc) */
		void handle(int signal, struct sigcontext context);
#endif
	};
}
