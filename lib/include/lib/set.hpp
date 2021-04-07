#pragma once

#include <unordered_set>

namespace lib
{
	class set
	{
	public:

		/**
		 * If a set contains a specific item
		 * @param vec Set to search in
		 * @param item Item to search for
		 */
		template<typename T>
		static auto contains(const std::unordered_set<T> &set, const T &item) -> bool
		{
			return set.find(item) != set.end();
		}
	};
}
