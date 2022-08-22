#pragma once

#include "lib/enum/lyricscredit.hpp"

#include <string>

namespace lib
{
	namespace lrc
	{
		/**
		 * Lyrics credit
		 */
		class credit
		{
		public:
			explicit credit(const std::string &line);

			credit(lib::lyrics_credit type, const std::string &name);

			/**
			 * Type of credit
			 */
			lib::lyrics_credit type;

			/**
			 * Name of credited
			 */
			std::string name;
		};
	}
}
