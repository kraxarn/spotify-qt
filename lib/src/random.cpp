#include "lib/random.hpp"

lib::random::random()
	: rng(device())
{
}

int lib::random::next_int(int min, int max)
{
	return next(min, max);
}
