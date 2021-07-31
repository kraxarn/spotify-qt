#include "lib/crash/crashhandler.hpp"

lib::cache *lib::crash_handler::cache = nullptr;

bool lib::crash_handler::initialized = false;

#ifdef USE_GCC_CRASH_HANDLER
auto lib::crash_handler::init() -> bool
{
	std::array<int, 5> signals{{
		SIGABRT, // Abnormal termination
		SIGFPE,  // Erroneous arithmetic operation
		SIGILL,  // Illegal instruction
		SIGSEGV, // Invalid access to storage
		SIGSYS,  // Bad system call
	}};
	initialized = true;
	auto handler = reinterpret_cast<__sighandler_t>(lib::crash_handler::handle);
	for (const auto s : signals)
	{
		if (signal(s, handler) == nullptr)
		{
			initialized = false;
		}
	}
	return initialized;
}
#else
auto lib::crash_handler::init() -> bool
{
	return false;
}
#endif

void lib::crash_handler::set_cache(lib::cache &c)
{
	lib::crash_handler::cache = &c;
}

void lib::crash_handler::log(const lib::crash_info &info)
{
	if (cache != nullptr)
	{
		cache->add_crash(info);
	}
	std::cerr << info.to_string() << std::endl;
}

auto lib::crash_handler::is_init() -> bool
{
	return initialized;
}

#ifdef USE_GCC_CRASH_HANDLER
void lib::crash_handler::handle(int signal, struct sigcontext context)
{
	std::array<void *, backtrace_size> trace;
	lib::crash_info info;

	info.signal = signal;
	if (signal == SIGSEGV)
	{
		std::stringstream stream;
		stream << "faulty address at 0x"
			<< std::hex << context.cr2
			<< " from 0x" << context.rip;

		info.info = stream.str();
	}

	auto trace_size = backtrace(trace.data(), backtrace_size);
	trace[1] = (void *) context.rip;
	auto **messages = backtrace_symbols(trace.data(), trace_size);

	info.stack_trace.reserve(trace_size);
	for (auto i = 0; i < trace_size; i++)
	{
		info.stack_trace.emplace_back(messages[i]);
	}

	log(info);
	exit(signal);
}
#endif
