#pragma once

#include <filesystem>

namespace lib
{
	/**
	 * Utilities for getting paths to files used by the library
	 * @note Required to be manually implemented depending on the target system
	 */
	class paths
	{
	public:
		/**
		 * Get path to spotify-qt.json
		 * @return Path, for example ~/.config/kraxarn/spotify-qt.json
		 */
		virtual auto config_file() const -> std::filesystem::path = 0;

		/**
		 * Get path where cached files should be
		 * @return Path, for example ~/.cache/kraxarn/spotify-qt
		 */
		virtual auto cache() const -> std::filesystem::path = 0;

	protected:
		/**
		 * @note Does nothing
		 */
		explicit paths() = default;
	};
}
