#pragma once

#include "lib/enum/experiment.hpp"

namespace lib
{
	/**
	 * Developer mode,
	 * usually enabled with --dev
	 */
	class developer_mode
	{
	public:
		/**
		 * Developer mode is enabled,
		 * this can for example enable more verbose logging,
		 * @note false by default
		 */
		static bool enabled;

		/**
		 * Enable experimental feature
		 */
		static void add_experiment(lib::experiment experiment);

		/**
		 * Check if specific experimental feature is enabled
		 */
		static auto is_experiment_enabled(lib::experiment experiment) -> bool;

	private:
		/**
		 * Static class
		 */
		developer_mode() = default;

		/**
		 * Enabled experiments
		 */
		static unsigned short experiments;
	};
}
