#pragma once

#include "thirdparty/json.hpp"
#include "lib/log.hpp"
#include "thirdparty/filesystem.hpp"

#include <fstream>

namespace lib
{
	/**
	 * JSON utilities
	 */
	class json
	{
	public:
		/**
		 * Try to find an item and set if found
		 * @param json JSON object to search in
		 * @param names Names of keys to search for
		 * @param item Item to set value to
		 * @return If an item was found
		 */
		template<typename T>
		static bool get_property(const nlohmann::json &json,
			const std::initializer_list<std::string> &names, T &item)
		{
			for (auto &name : names)
			{
				if (json.contains(name) && !json.at(name).is_null())
				{
					json.at(name).get_to(item);
					return true;
				}
			}
			return false;
		}

		/**
		 * Combine two JSON arrays
		 * @param item1 First array
		 * @param item2 Second array
		 * @return Combined array, or an empty one on failure
		 */
		static nlohmann::json combine(const nlohmann::json &item1, const nlohmann::json &item2);

		/**
		 * Get item from JSON if found
		 * @param json JSON to find item in
		 * @param key Key to try and find
		 * @param item Item to set value to
		 */
		template<typename T>
		static void get(const nlohmann::json &json, const std::string &key, T &item)
		{
			if (json.contains(key))
				json.at(key).get_to(item);
		}

		/**
		 * Load and parse JSON from file
		 * @param path Path to json file, including extension
		 * @return JSON object, or null object on failure
		 */
		static nlohmann::json load(const ghc::filesystem::path &path);

		/**
		 * Save specified item to a json file
		 * @param path Path to json file, including extension
		 * @param item Item to save
		 */
		static void save(const ghc::filesystem::path &path, const nlohmann::json &json);

	private:
		json() = default;
	};
}