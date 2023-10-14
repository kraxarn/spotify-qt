#pragma once

enum class AutoplaySupport: short
{
	/**
	 * No support for overriding autoplay
	 */
	None,

	/**
	 * Overriden by flag, (--autoplay)
	 */
	Flag,

	/**
	 * Overriden by option (--autoplay true)
	 */
	Option,
};
