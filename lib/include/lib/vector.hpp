#pragma once

#include <vector>

namespace lib
{
	class vector
	{
	public:
		/**
		 * Combine two vectors
		 * @param vec1 First vector
		 * @param vec2 Second vector
		 * @return Combined vector
		 */
		template<typename T>
		static std::vector<T> combine(const std::vector<T> &vec1, const std::vector<T> &vec2)
		{
			std::vector<T> vec;
			vec.reserve(vec1.size() + vec2.size());
			vec.insert(vec.end(), vec1.begin(), vec1.end());
			vec.insert(vec.end(), vec2.begin(), vec2.end());

			return vec;
		}

		/**
		 * Append a vector to the end of another vector
		 * @param vec Vector to append to
		 * @param append Vector to append
		 */
		template<typename T>
		static void append(std::vector<T> &vec, const std::vector<T> &append)
		{
			vec.insert(vec.end(), append.begin(), append.end());
		}
	};
}
