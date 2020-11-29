#pragma once

#include "lib/json.hpp"

namespace lib
{
	/**
	 * JSON utilities
	 */
	class json
	{
	public:
		/**
		 * Get property in JSON from possible keys
		 * @param json JSON to get value from
		 * @param keys Keys to search for
		 * @param value Value to write to if found
		 */
		static nlohmann::json get_property(const nlohmann::json &json, const std::initializer_list<std::string> &keys);

		/**
		 * Get value from json, or a default value if not found
		 * @tparam T Value type
		 * @param json JSON to get value from
		 * @param key Key to find
		 * @param fallback Default value
		 * @return Value if found, or default
		 */
		template<typename T>
		static T get_or_default(const nlohmann::json &json, const std::string &key, const T &fallback)
		{
			return json.contains(key)
				? json.at(key).get<T>()
				: fallback;
		}
	};
}