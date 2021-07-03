#pragma once

#include "thirdparty/backward.hpp"
#include "lib/log.hpp"

namespace lib
{
	class crash_handler
	{
	public:
		crash_handler();

	private:
		backward::SignalHandling signal_handling;
	};
}
