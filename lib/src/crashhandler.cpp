#include "lib/crashhandler.hpp"

lib::crash_handler::crash_handler()
{
	backward::SignalHandling::make_default_signals();

	if (!signal_handling.loaded())
	{
		lib::log::warn("Failed to initialize crash handler");
	}
}
