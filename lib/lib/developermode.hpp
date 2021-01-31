#pragma once

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

	private:
		/**
		 * Static class
		 */
		developer_mode() = default;
	};
}
