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

auto lib::system::window_system() -> lib::window_system
{
	if (has_env("WAYLAND_DISPLAY"))
	{
		return lib::window_system::wayland;
	}

	if (has_env("DISPLAY"))
	{
		return lib::window_system::x11;
	}

	return lib::window_system::unknown;
}
