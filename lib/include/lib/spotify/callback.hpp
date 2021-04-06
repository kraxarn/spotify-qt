#pragma once

namespace lib
{
	/**
	 * API callback
	 */
	template<typename T>
	using callback = const std::function<void(const T &)>;
}
