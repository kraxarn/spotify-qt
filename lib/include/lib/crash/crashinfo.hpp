#pragma once

#include "thirdparty/json.hpp"

#include <string>
#include <unordered_map>

namespace lib
{
	/**
	 * Crash info with stack trace
	 */
	class crash_info
	{
	public:
		crash_info() = default;

		/**
		 * When did the crash occur
		 */
		long timestamp = 0;

		/**
		 * Crash signal
		 */
		int signal = -1;

		/**
		 * Additional information
		 */
		std::string info;

		/**
		 * Stack trace with stack number
		 */
		std::unordered_map<int, std::string> stack_trace;

		/**
		 * Handled by application
		 */
		bool handled = false;
	};

	/** Crash info -> JSON */
	void to_json(nlohmann::json &j, const crash_info &i);

	/** JSON -> Crash info */
	void from_json(const nlohmann::json &j, crash_info &i);
}
