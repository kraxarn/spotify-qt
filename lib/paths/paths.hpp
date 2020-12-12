#pragma once

#include <string>

namespace lib
{
	class paths
	{
	public:
		/**
		 * Get path to spotify-qt.json
		 * @example ~/.config/kraxarn/spotify-qt.json
		 * @return Path
		 */
		virtual std::string config_file() const = 0;

		/**
		 * Get path where cached files should be
		 * @example ~/.cache/kraxarn/spotify-qt
		 * @return Path
		 */
		virtual std::string cache() const = 0;

	protected:
		explicit paths() = default;
	};
}
