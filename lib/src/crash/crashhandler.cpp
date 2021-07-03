#include "lib/crash/crashhandler.hpp"

lib::crash_handler::crash_handler()
{
#ifdef IS_GNU_CXX
#endif
}

#ifdef IS_GNU_CXX
void lib::crash_handler::handle(int signal, struct sigcontext context)
{
	std::array<void*, backtrace_size> trace;
	std::stringstream ss;

	ss << "Signal: " << signal;
	if (signal == SIGSEGV)
	{
		ss << ", faulty address at " << context.cr2
			<< " from " << context.rip;
	}
	ss << std::endl;

	auto trace_size = backtrace(trace.data(), backtrace_size);
	trace[1] = (void*)context.rip;
	auto **messages = backtrace_symbols(trace.data(), trace_size);

	ss << "Stack trace:" << std::endl;
	for (auto i = 0; i < trace_size; i++)
	{
		ss << "#" << i << '\t' << messages[i] << std::endl;
	}
}
#endif
