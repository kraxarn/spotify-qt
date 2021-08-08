#include "lib/stopwatch.hpp"

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
