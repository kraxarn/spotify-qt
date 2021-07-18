#pragma once

namespace lib
{
	/**
	 * Window systems
	 */
	enum class window_system
	{
		/** Unknown or no window system */
		unknown,

		/** X11 */
		x11,

		/** Wayland */
		wayland,
	};
}
