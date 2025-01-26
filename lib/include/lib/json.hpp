#pragma once

#include "lib/filesystem.hpp"
#include "thirdparty/json.hpp"

#include <optional>

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
		static auto get_property(const nlohmann::json &json,
			const std::initializer_list<std::string> &names, T &item) -> bool
		{
			for (const auto &name: names)
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
		static auto combine(const nlohmann::json &item1,
			const nlohmann::json &item2) -> nlohmann::json;

		/**
		 * Get item from JSON if found
		 * @param json JSON to find item in
		 * @param key Key to try and find
		 * @param item Item to set value to
		 */
		template<typename T>
		static void get(const nlohmann::json &json, const std::string &key, T &item)
		{
			if (json.contains(key) && !json.at(key).is_null())
			{
				json.at(key).get_to(item);
			}
		}

		/**
		 * Load and parse JSON from file
		 * @param path Path to json file, including extension
		 * @return JSON object, or null object on failure
		 */
		static auto load(const std::filesystem::path &path) -> nlohmann::json;

		/**
		 * Convenience method to parse generic class from JSON,
		 * returns instance of T on failure
		 */
		template<typename T>
		static auto load(const std::filesystem::path &path) -> T
		{
			auto json = load(path);
			if (json.is_null())
			{
				return T();
			}
			return json;
		}

		/**
		 * Save specified item to a json file
		 * @param path Path to json file, including extension
		 * @param item Item to save
		 */
		static void save(const std::filesystem::path &path, const nlohmann::json &json);

		/**
		 * Find the last value in an "item"s array, or from "item" directly
		 * @param name Name of item to search for, excluding s-suffix
		 * @param json JSON to search in
		 * @param item String to output to
		 */
		static void find_item(const std::string &name,
			const nlohmann::json &json, std::string &item);

		template<typename T>
		static void set(nlohmann::json &json, const std::string &key,
			const std::optional<T> &value)
		{
			if (value.has_value())
			{
				json[key] = value.value();
			}
		}

	private:
		json() = default;
	};
}
