#include "lib/random.hpp"

lib::random::random()
	: rng(device())
{
}