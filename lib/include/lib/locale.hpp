#pragma once

#include <string>
#include <locale>

#include "strings.hpp"
#include "format.hpp"

namespace lib
{
	/**
	 * Library locale
	 * @deprecated Locale dependent stuff is expected to be handled by the UI toolkit
	 */
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