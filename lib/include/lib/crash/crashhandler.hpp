#pragma once

#include "lib/log.hpp"

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
		crash_handler();

	private:
		/** How far back to trace */
		static constexpr size_t backtrace_size = 16;

#ifdef IS_GNU_CXX
		/** Handle signal (gcc) */
		static void handle(int signal, struct sigcontext context);
#endif
	};
}
