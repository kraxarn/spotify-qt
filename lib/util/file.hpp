#pragma once

#include <fstream>
#include <string>
#include <iterator>

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

		/**
		 * Read entire content of file to a string
		 * @param file File to read from
		 * @return File content
		 */
		static std::string read_all(std::ifstream &file);
	};
}