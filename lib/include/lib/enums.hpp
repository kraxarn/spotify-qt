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
		static auto to_string(T value) -> std::string
		{
			std::string str;
			enum_to_string(value, str);
			return str;
		}

		static auto parse(const std::string &str) -> T
		{
			T value;
			enum_from_string(str, value);
			return value;
		}

		/**
		 * If value has flag, eg. "value & flag == flag"
		 * @param value Enum value
		 * @param flag Enum flag to check value for
		 * @return Value contains flag
		 */
		static auto has_flag(T value, T flag) -> bool
		{
			const auto valueFlag = static_cast<unsigned int>(value);
			const auto flagFlag = static_cast<unsigned int>(flag);
			return (valueFlag & flagFlag) == flagFlag;
		}

	private:
		// region media_type

		static void enum_to_string(media_type media_type, std::string &str)
		{
			switch (media_type)
			{
				case media_type::unknown:
					str = "unknown";
					break;

				case media_type::audio:
					str = "audio";
					break;
			}
		}

		static void enum_from_string(const std::string &str, media_type &media_type)
		{
			if (str == "audio")
			{
				media_type = media_type::audio;
				return;
			}

			media_type = media_type::unknown;
		}

		//endregion
	};
}
