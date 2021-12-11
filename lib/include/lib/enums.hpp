#pragma once

#include "lib/enum/mediatype.hpp"

#include <string>

namespace lib
{
	/**
	 * Enum parser
	 * @tparam T Enum
	 */
	template<typename T>
	class enums
	{
	public:
		static auto to_string(T t) -> std::string
		{
			return enum_to_string(t);
		}

		static auto parse(const std::string &str) -> T
		{
			return enum_from_string(str);
		}

		/**
		 * If value has flag, eg. "value & flag == flag"
		 * @param value Enum value
		 * @param flag Enum flag to check value for
		 * @return Value contains flag
		 */
		static auto has_flag(T value, T flag) -> bool
		{
			const auto v = static_cast<unsigned int>(value);
			const auto f = static_cast<unsigned int>(flag);
			return (v & f) == f;
		}

	private:
		// region media_type

		static auto enum_to_string(media_type media_type) -> std::string
		{
			switch (media_type)
			{
				case media_type::unknown:
					return "unknown";

				case media_type::audio:
					return "audio";
			}

			return std::string();
		}

		static auto enum_from_string(const std::string &str) -> media_type
		{
			if (str == "audio")
			{
				return media_type::audio;
			}

			return media_type::unknown;
		}

		//endregion
	};
}
