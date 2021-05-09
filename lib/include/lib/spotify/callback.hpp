#pragma once

#include <functional>

namespace lib
{
	/**
	 * API callback
	 */
	template<typename T>
	using callback = const std::function<void(const T &)>;

	/**
	 * Callback with bool indicating success
	 */
	template<typename T>
	using result = const std::function<void(bool success, const T &result)>;
}
