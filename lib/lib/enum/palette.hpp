#pragma once

namespace lib
{
	/**
	 * Application palette
	 * @note Mostly Qt specific, but palette_app and palette_dark can work as a
	 * dark theme toggle
	 */
	enum palette
	{
		/**
		 * Default app palette
		 */
		palette_app = 0,

		/**
		 * Palette from current style
		 */
		palette_style = 1,

		/**
		 * Custom dark palette
		 */
		palette_dark = 2,
	};
}
