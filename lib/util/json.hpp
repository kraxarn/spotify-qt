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
	};
}