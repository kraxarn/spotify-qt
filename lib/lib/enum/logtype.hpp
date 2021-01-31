#pragma once

namespace lib
{
	/**
	 * Type of log message
	 */
	enum class log_type
	{
		/**
		 * Information, only informing the user of something
		 */
		information,

		/**
		 * Non-critical error
		 */
		warning,

		/**
		 * Critical error, something is wrong
		 */
		error,

		/**
		 * Verbose logging, not shown by default
		 */
		verbose
	};
}
