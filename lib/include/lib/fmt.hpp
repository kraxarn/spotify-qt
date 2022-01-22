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
		static void vformat(const std::string &str, std::stringstream &stream, size_t &index)
		{
		}

		template<typename Arg, typename... Args>
		static void vformat(const std::string &str, std::stringstream &stream, size_t &index,
			const Arg &arg, Args &&... args)
		{
			collect(str, stream, index, arg);
			vformat(str, stream, index, args...);
		}

		template<typename Arg>
		static void collect(const std::string &str, std::stringstream &stream,
			size_t &index, const Arg &arg)
		{
			const auto next = str.find("{}", index);

			stream
				<< str.substr(index, next - index)
				<< arg;

			index += next + 2;
		}
	};
}
