#pragma once

#include <chrono>

namespace lib
{
	/**
	 * Basic stopwatch
	 */
	class stopwatch
	{
	public:
		/**
		 * Milliseconds
		 */
		using ms = std::chrono::milliseconds;

		/**
		 * Construct a new stopwatch
		 * @note Doesn't start it
		 */
		stopwatch() = default;

		/**
		 * Start stopwatch
		 * @note Resets any current time
		 */
		void start();

		/**
		 * Stop stopwatch
		 */
		void stop();

		/**
		 * Get elapsed time from when timer was started and stopped
		 * @tparam Unit Duration unit
		 * @tparam Result Returned type
		 * @return Time in specified unit
		 */
		template<typename Unit, typename Result>
		auto elapsed() -> Result
		{
			return static_cast<Result>(std::chrono::duration_cast<Unit>(stopped - started).count());
		}

	private:
		/**
		 * Time point in use
		 */
		using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

		/**
		 * When stopwatch was started
		 */
		time_point started;

		/**
		 * When stopwatch was stopped
		 */
		time_point stopped;

		/**
		 * Get current time point
		 */
		static auto now() -> time_point;
	};
}
