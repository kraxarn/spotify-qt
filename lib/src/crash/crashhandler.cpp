#include "lib/crash/crashhandler.hpp"

lib::crash_handler::crash_handler(lib::cache &cache)
	: cache(cache)
{
#ifdef IS_GNU_CXX
#endif
}

#ifdef IS_GNU_CXX
void lib::crash_handler::handle(int signal, struct sigcontext context)
{
	std::array<void *, backtrace_size> trace;
	lib::crash_info info;

	info.signal = signal;
	if (signal == SIGSEGV)
	{
		info.info = lib::fmt::format("faulty address at {} from {}",
			context.cr2, context.rip);
	}

	auto trace_size = backtrace(trace.data(), backtrace_size);
	trace[1] = (void *) context.rip;
	auto **messages = backtrace_symbols(trace.data(), trace_size);

	info.stack_trace.reserve(trace_size);
	for (auto i = 0; i < trace_size; i++)
	{
		info.info.insert(i, std::string(messages[i]));
	}

	cache.add_crash(info);
}
#endif
