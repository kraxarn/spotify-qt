#pragma once

#include <vector>

class vector
{
public:
	/**
	 * Concat two vectors
	 * @param vec1 First vector
	 * @param vec2 Second vector
	 * @return Combined vector
	 */
	template<typename T>
	std::vector<T> combine(const std::vector<T> &vec1, const std::vector<T> &vec2)
	{
		std::vector<T> vec;
		vec.reserve(vec1.size() + vec2.size());
		vec.insert(vec.end(), vec1.begin(), vec1.end());
		vec.insert(vec.end(), vec2.begin(), vec2.end());

		return vec;
	}
};
