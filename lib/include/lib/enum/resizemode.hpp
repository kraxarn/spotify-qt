#pragma once

namespace lib
{
	/**
	 * How to resize columns in lists
	 */
	enum resize_mode
	{
		/**
		 * Resize to fit current size
		 */
		resize_auto,
		/**
		 * Resize to fit current content
		 */
		resize_fit_content,
		/**
		 * No automatic resizing
		 */
		resize_custom,
	};
}