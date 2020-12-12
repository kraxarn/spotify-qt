#pragma once

#include <string>

#include "../format.hpp"

namespace lib
{
	class path
	{
	public:
		/**
		 * Config path, for example ~/.config/kraxarn/
		 * (only spotify-qt.json should be here)
		 * @return Config path
		 */
		static std::string config();

		/**
		 * Cache path, for example ~/.cache/kraxarn/spotify-qt/
		 * @return Cache path
		 */
		static std::string cache();
	};
}