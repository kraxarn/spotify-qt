#pragma once

namespace lib
{
	/**
	 * Position of something, relative to something else
	 */
	enum class position: unsigned short
	{
		none = 0,
		top = 1 << 0,
		right = 1 << 1,
		bottom = 1 << 2,
		left = 1 << 3,
	};
}
