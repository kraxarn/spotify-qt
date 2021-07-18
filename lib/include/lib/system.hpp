#pragma once

#include "lib/enum/windowsystem.hpp"

#include <string>

namespace lib
{
	/**
	 * System utilities
	 */
	class system
	{
	public:
		/**
		 * Get environmental variable
		 */
		static auto env(const char *name) -> std::string;

		/**
		 * Environmental variable is set in system
		 */
		static auto has_env(const char *name) -> bool;

		/**
		 * Try and detect window system in use
		 * @note Doesn't reflect how the application is displayed
		 */
		static auto window_system() -> window_system;

	private:
		system() = default;
	};
}
