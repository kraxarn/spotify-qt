#pragma once

#include <fstream>
#include <string>

namespace lib
{
	class file
	{
	public:
		/**
		 * Check if a file exists in the specified path
		 * @param path Path of file
		 * @return File exists
		 */
		static bool exists(const std::string &path);
	};
}