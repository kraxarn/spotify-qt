#pragma once

namespace lib
{
	/**
	 * Application palette
	 * @note Mostly Qt specific, but palette_app and palette_dark can work as a
	 * dark theme toggle
	 */
	enum class palette
	{
		/**
		 * Default app palette
		 */
		app = 0,

		/**
		 * Palette from current style
		 */
		style = 1,

		/**
		 * Custom dark palette
		 */
		dark = 2,
	};
}
