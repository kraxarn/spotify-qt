#pragma once

#include <vector>
#include <algorithm>

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
		static auto combine(const std::vector<T> &vec1,
			const std::vector<T> &vec2) -> std::vector<T>
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

		/**
		 * Create a new sub-vector from another vector
		 * @param vec Vector to take from
		 * @param pos Starting index
		 * @param len Number of elements (-1 to include rest of items)
		 */
		template<typename T>
		static auto sub(const std::vector<T> &vec, size_t pos, size_t len) -> std::vector<T>
		{
			// If pos is larger than element count, return an empty vector
			if (pos > vec.size())
			{
				return std::vector<T>();
			}

			// If size+len is larger than element count, stop as last element
			if (len < 0 || pos + len > vec.size())
			{
				len = vec.size() - pos - 1;
			}

			return std::vector<T>(vec.cbegin() + pos, vec.cbegin() + len);
		}
	};
}
