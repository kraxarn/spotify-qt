#include "path.hpp"

using namespace lib;

// TODO: Very temporary paths

std::string path::config()
{
	return fmt::format("{}/.config/kraxarn", std::getenv("HOME"));
}

std::string path::cache()
{
	return fmt::format("{}/.cache/kraxarn/spotify-qt", std::getenv("HOME"));
}
