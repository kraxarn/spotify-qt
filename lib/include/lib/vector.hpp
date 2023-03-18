#pragma once

#include <vector>
#include <algorithm>
#include <numeric>

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

			// If pos+len would overflow or is larger than
			// element count, include remaining elements
			if ((pos + len) < len || pos + len > vec.size())
			{
				len = vec.size() - pos;
			}

			return std::vector<T>(vec.cbegin() + pos, vec.cbegin() + pos + len);
		}

		/**
		 * Get index of specific item
		 * @param vec Vector to search in
		 * @param item Item to search for
		 * @return Index if found, otherwise size
		 */
		template<typename T>
		static auto index_of(const std::vector<T> &vec, const T &item) -> size_t
		{
			return std::distance(vec.begin(), std::find(vec.begin(), vec.end(), item));
		}

		/**
		 * Get the average value of all values in a vector
		 * @param vec Vector of items
		 * @return Average item, or 0 if empty
		 */
		template<typename T>
		static auto average(const std::vector<T> &vec) -> T
		{
			const T init = 0;
			if (vec.empty())
			{
				return init;
			}

			return std::accumulate(vec.cbegin(), vec.cend(), init) / static_cast<T>(vec.size());
		}

		/**
		 * Remove all duplicate items from a vector
		 * @note Sorts the vector
		 * @note Requires \< and == operators
		 * @param vec Vector of items
		 */
		template<typename T>
		static void unique(std::vector<T> &vec)
		{
			std::sort(vec.begin(), vec.end());
			const auto duplicates = std::unique(vec.begin(), vec.end());
			vec.erase(duplicates, vec.end());
		}
	};
}
