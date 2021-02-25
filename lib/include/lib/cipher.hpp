#pragma once

#include <string>

namespace lib
{
	/**
	 * Basic cipher tools
	 */
	class cipher
	{
	public:
		/**
		 * XOR cipher using strings as key and value
		 */
		static std::string xor_strings(const std::string &key, const std::string &value);

	private:
		/**
		 * Static class
		 */
		cipher() = default;
	};
}
