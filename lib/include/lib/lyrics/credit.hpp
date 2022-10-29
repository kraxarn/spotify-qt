#pragma once

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

			/**
			 * Name of credited
			 */
			std::string name;

			auto operator<(const credit &rhs) const -> bool;
			auto operator==(const credit &rhs) const -> bool;
		};
	}
}
