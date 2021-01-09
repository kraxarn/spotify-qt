#pragma once

#include <string>
#include <locale>

#include "strings.hpp"
#include "format.hpp"

namespace lib
{
	class locale
	{
	public:
		/**
		 * Set global locale if none is set
		 * @param locale Locale to set
		 * @param force Force set locale, even if it's already valid
		 * @return If successful
		 */
		static bool set(const std::string &locale, bool force = false);

	private:
		/**
		 * Static class
		 */
		locale() = default;
	};
}