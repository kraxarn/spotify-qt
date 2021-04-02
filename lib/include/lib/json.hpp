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
		static nlohmann::json load_json(const ghc::filesystem::path &path);

		template<typename T>
		static std::vector<T> load_items(const nlohmann::json &json)
		{
			if (json.is_array())
				return json.get<std::vector<T>>();

			throw std::runtime_error(lib::fmt::format(
				"wrong type, expected array, but was {}", json.type_name()));
		}

		/**
		 * Load a vector of items from a json file
		 * @param path Path to json file, including extension
		 * @return Vector of items on success, or an empty vector on failure
		 */
		template<typename T>
		static std::vector<T> load_items(const ghc::filesystem::path &path)
		{
			try
			{
				return load_items<T>(load_json(path));
			}
			catch (const std::exception &e)
			{
				log::warn("Failed to load items from \"{}\": {}", path.string(), e.what());
			}

			return std::vector<T>();
		}

		/**
		 * Load a single item from a json file
		 * @param path Path to json file, including extension
		 * @return Item on success, or default on failure
		 */
		template<typename T>
		static T load(const ghc::filesystem::path &path)
		{
			try
			{
				return load_json(path);
			}
			catch (const std::exception &e)
			{
				log::warn("Failed to load item from \"{}\": {}", path.string(), e.what());
			}

			return T();
		}

		static void save_json(const ghc::filesystem::path &path, const nlohmann::json &json);

		/**
		 * Save a vector of items to a json file
		 * @param path Path to json file, including extension
		 * @param items Items to save
		 */
		template<typename T>
		static void save_items(const std::string &path, const std::vector<T> &items)
		{
			try
			{
				save_json(path, items);
			}
			catch (const std::exception &e)
			{
				log::warn("Failed to save items to \"{}\": {}", path, e.what());
			}
		}

		template<typename T>
		static void save(const ghc::filesystem::path &path, const T &item)
		{
			try
			{
				save_json(path, item);
			}
			catch (const std::exception &e)
			{
				log::warn("Failed to save items to \"{}\": {}",
					path.string(), e.what());
			}
		}

	private:
		json() = default;
	};
}