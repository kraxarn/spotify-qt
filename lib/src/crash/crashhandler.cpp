#include "lib/crash/crashhandler.hpp"

lib::cache *lib::crash_handler::cache = nullptr;

void lib::crash_handler::init()
{
#ifdef IS_GNU_CXX
#endif
}

void lib::crash_handler::set_cache(lib::cache &c)
{
	lib::crash_handler::cache = &c;
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

	if (cache != nullptr)
	{
		cache->add_crash(info);
	}
	std::cerr << info.to_string() << std::endl;
}
#endif
