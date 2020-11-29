#pragma once

#include "enum/logtype.hpp"

#include <string>

namespace lib
{
	/**
	 * Formatting utilities
	 */
	class fmt
	{
		template<typename Format>
		static std::string collect(const Format &fmt, const std::string &arg)
		{
			auto str = std::string(fmt);
			auto index = str.find("{}");
			if (index == std::string::npos)
				return str;

			return str.replace(index, 2, arg);
		}

		template<typename Format, typename Arg>
		static std::string collect(const Format &fmt, const Arg &arg)
		{
			return collect(fmt, std::to_string(arg));
		}

		template<typename Format>
		static std::string collect(const Format &fmt, const char *arg)
		{
			return collect(fmt, std::string(arg));
		}

		template<typename Format>
		static std::string collect(const Format &fmt, char *arg)
		{
			return collect(fmt, std::string(arg));
		}

		template<typename Format>
		static std::string format(const Format &fmt)
		{
			return fmt;
		}

		template<typename Format, typename Arg, typename... Args>
		static std::string format(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return format(collect(fmt, arg), args...);
		}
	}
}