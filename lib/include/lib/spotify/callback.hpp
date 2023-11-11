#pragma once

#include "lib/result.hpp"
#include "lib/spotify/page.hpp"

#include <functional>

namespace lib
{
	/**
	 * API callback
	 */
	template<typename T>
	using callback = const std::function<void(const T &)>;

	/**
	 * Paged API callback with result
	 */
	template<typename T>
	using paged_callback = std::function<bool(const lib::result<lib::spt::page<T>> &)>;
}
