#pragma once

#include <string>

namespace lib
{
	namespace lrc
	{
		class line
		{
		public:
			explicit line(const std::string &line);

			/**
			 * Timestamp in milliseconds
			 */
			long timestamp;

			/**
			 * Lyrics text
			 */
			std::string text;

		private:
			static auto parse_timestamp(const std::string &timestamp) -> long;
		};
	}
}
