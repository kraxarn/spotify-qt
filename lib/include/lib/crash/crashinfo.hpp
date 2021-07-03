#pragma once

#include "thirdparty/json.hpp"
#include "lib/datetime.hpp"

#include <string>
#include <vector>

namespace lib
{
	/**
	 * Crash info with stack trace
	 */
	class crash_info
	{
	public:
		crash_info();

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
		 * Stack trace
		 */
		std::vector<std::string> stack_trace;

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
