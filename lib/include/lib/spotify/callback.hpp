#pragma once

#include "lib/result.hpp"
#include "lib/spotify/page.hpp"

#include <functional>

/**
* API callback
*/
template<typename T>
using ApiCallback = const std::function<void(const T &)>;

/**
* Paged API callback with a result
*/
template<typename T>
using ApiPagedCallback = std::function<bool(const Result<lib::spt::page<T>> &)>;

namespace lib
{
	template<typename T>
	using callback [[deprecated("Use ApiCallback instead")]] = ApiCallback<T>;

	template<typename T>
	using paged_callback [[deprecated("Use ApiPagedCallback instead")]] = ApiPagedCallback<T>;
}
