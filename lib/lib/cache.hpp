#pragma once

#include "thirdparty/filesystem.hpp"
#include "thirdparty/json.hpp"
#include "paths/paths.hpp"
#include "format.hpp"
#include "log/log.hpp"

namespace lib
{
	/**
	 * Handles cache
	 */
	class cache
	{
	public:
		/**
		 * Instance a new cache manager, does not create any directories
		 * @param paths Paths to get cache directory
		 */
		explicit cache(const paths &paths);

		/**
		 * Get tracks saved in cache
		 * @note Will soon return std::vector<spt::track>
		 * @param id Id of album for example
		 * @return JSON stored in cache, or an empty object if none
		 */
		nlohmann::json tracks(const std::string &id);

		/**
		 * Get all tracks saved in cache
		 * @note Will soon return std::map<std::string, std::vector<spt::track>>
		 * @return Map as id: tracks
		 */
		std::map<std::string, nlohmann::json> all_tracks();

	private:
		const lib::paths &paths;
	};
}
