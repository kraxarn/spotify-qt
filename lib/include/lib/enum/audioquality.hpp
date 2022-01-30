#pragma once

namespace lib
{
	/**
	 * Audio qualities offered by Spotify in kbit/s
	 */
	enum class audio_quality: short
	{
		/**
		 * Low quality
		 */
		low = 24,

		/**
		 * Normal quality
		 * @note Highest quality for podcasts on most devices
		 */
		normal = 96,

		/**
		 * High quality
		 */
		high = 160,

		/**
		 * Very high quality
		 * @note Only available for Spotify Premium accounts
		 */
		very_high = 320,
	};
}
