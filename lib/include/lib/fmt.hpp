#pragma once

#include <string>
#include <sstream>

namespace lib
{
	class fmt2
	{
	public:
		template<typename Format, typename Arg, typename... Args>
		static auto format(const Format &str, const Arg &arg, Args &&... args) -> std::string
		{
			std::stringstream stream;
			size_t index = 0;

			vformat(std::string(str), stream, index, arg, args...);
			return stream.str();
		}

	private:
		/**
		 * Format with no arguments
		 */
		static void vformat(const std::string &str, std::stringstream &stream, size_t &index)
		{
			stream << str.substr(index);
		}

		/**
		 * Format with multiple arguments
		 */
		template<typename Arg, typename... Args>
		static void vformat(const std::string &str, std::stringstream &stream, size_t &index,
			const Arg &arg, Args &&... args)
		{
			replace(str, stream, index, arg);
			vformat(str, stream, index, args...);
		}

		/**
		 * Format with single argument
		 */
		template<typename Arg>
		static void replace(const std::string &str, std::stringstream &stream,
			size_t &index, const Arg &arg)
		{
			const auto next = str.find("{}", index);

			if (next == std::string::npos)
			{
				stream << str.substr(index);
				index = str.size();
				return;
			}

			stream
				<< str.substr(index, next - index)
				<< arg;

			index = next + 2;
		}
	};
}
