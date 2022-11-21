#include "lib/system.hpp"

auto lib::system::env(const char *name) -> std::string
{
	return has_env(name)
		? std::string(std::getenv(name))
		: std::string();
}

auto lib::system::has_env(const char *name) -> bool
{
	return std::getenv(name) != nullptr;
}
