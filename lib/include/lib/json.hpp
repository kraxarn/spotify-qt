#pragma once

#include "thirdparty/json.hpp"
#include "lib/log.hpp"

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
				if (json.contains(name))
				{
					json.at(name).get_to(item);
					return true;
				}
			}
			return false;
		}

		/**
		 * Get item from JSON with fallback
		 * @param json JSON to find item in
		 * @param key Key to try and find
		 * @param fallback Fallback value if key wasn't found
		 * @param item Item to set value to
		 */
		template<typename T>
		static void get(const nlohmann::json &json, const std::string &key, const T &fallback,
			T &item)
		{
			if (json.contains(key))
				json.at(key).get_to(item);
			else
				item = fallback;
		}

		/**
		 * Load a vector of items from a json file
		 * @param path Path to json file, including extension
		 * @return Vector of items on success, or an empty vector on failure
		 */
		template<typename T>
		static std::vector<T> load_items(const std::string &path)
		{
			std::ifstream file(path);
			if (!file.is_open() || file.bad())
			{
				// File not found errors fail silently
				return std::vector<T>();
			}

			try
			{
				nlohmann::json json;
				file >> json;
				return json.get<std::vector<T>>();
			}
			catch (const std::exception &e)
			{
				log::warn("Failed to load items from \"{}\": {}", path, e.what());
			}

			return std::vector<T>();
		}

		/**
		 * Save a vector of items to a json file
		 * @param path Path to json file, including extension
		 * @param items Items to save
		 */
		template<typename T>
		static void save_items(const std::string &path, const std::vector<T> &items)
		{
			std::ofstream file(path);

			try
			{
				nlohmann::json json = items;
				file << std::setw(4) << json;
			}
			catch (const std::exception &e)
			{
				log::warn("Failed to save items to \"{}\": {}", path, e.what());
			}
		}

	private:
		json() = default;
	};
}