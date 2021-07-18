#pragma once

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

	private:
		system() = default;
	};
}
