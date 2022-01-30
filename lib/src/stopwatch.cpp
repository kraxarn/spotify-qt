#include "lib/stopwatch.hpp"

lib::stopwatch::stopwatch()
{
	started = time_point::min();
	stopped = time_point::min();
}

auto lib::stopwatch::now() -> time_point
{
	return std::chrono::high_resolution_clock::now();
}

void lib::stopwatch::start()
{
	started = now();
}

void lib::stopwatch::stop()
{
	stopped = now();
}
