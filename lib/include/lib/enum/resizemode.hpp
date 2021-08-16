#pragma once

namespace lib
{
	/**
	 * How to resize columns in lists
	 */
	enum class resize_mode
	{
		/**
		 * Resize to fit current size
		 */
		auto_size,

		/**
		 * Resize to fit current content
		 */
		fit_content,

		/**
		 * No automatic resizing
		 */
		custom,
	};
}
