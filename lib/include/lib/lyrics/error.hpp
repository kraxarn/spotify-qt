#pragma once

#include <string>

#include "thirdparty/json.hpp"

namespace lib::lrc
{
	class error
	{
	public:
		/**
		 * Error message
		 */
		std::string message;

		/**
		 * Error type
		 */
		std::string name;

		/**
		 * HTTP status code
		 */
		int status_code;
	};

	void from_json(const nlohmann::json &json, error &error);
}
