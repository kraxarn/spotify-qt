#pragma once

#include "thirdparty/json.hpp"

#include <string>
#include <sstream>

namespace lib
{
	class fmt
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
				<< to_string(arg);

			index = next + 2;
		}

		/**
		 * Generic formatter
		 */
		template<typename Arg>
		static auto to_string(const Arg &arg) -> const Arg &
		{
			return arg;
		}

		/**
		 * Format bool as "true" or "false"
		 */
		static auto to_string(const bool arg) -> const char *
		{
			return arg ? "true" : "false";
		}

		/**
		 * Format json by dumping content
		 */
		static auto to_string(const nlohmann::json &json) -> std::string
		{
			return json.dump();
		}
	};
}
