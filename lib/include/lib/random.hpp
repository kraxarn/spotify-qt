#pragma once

#include <random>

namespace lib
{
	/**
	 * Random number generator
	 */
	class random
	{
	public:
		random();

		/**
		 * Get a random number
		 * @tparam T Number type
		 * @param min Lower bound
		 * @param max Upper bound
		 */
		template<typename T>
		T next(T min, T max)
		{
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(rng);
		}

		int next_int(int min, int max);

	private:
		std::random_device device;
		std::mt19937 rng;
	};
}