#include "util/appconfig.hpp"

auto AppConfig::useNativeMenuBar() -> bool
{
#ifdef __APPLE__
	return true;
#else
	return false;
#endif
}
