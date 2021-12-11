#pragma once

#include <memory>
#include <stdexcept>

namespace lib
{
	/**
	 * Basic std::optional-like implementation using std::unique_ptr
	 */
	template<typename T>
	class optional
	{
	public:
		/**
		 * Instance with no value
		 */
		optional() = default;

		/**
		 * Instance with value
		 */
		optional(T value)
		{
			ptr = std::unique_ptr<T>(new T(std::move(value)));
		}

		/**
		 * Contains a value
		 */
		auto has_value() const -> bool
		{
			return ptr.get() != nullptr;
		}

		/**
		 * Get contained value, or throw if no value
		 */
		auto value() const -> T &
		{
			if (!has_value())
			{
				throw std::runtime_error("bad optional access");
			}
			return *ptr;
		}

		/**
		 * Get contained value, or specified if no value
		 */
		auto value_or(const T &default_value) const -> const T &
		{
			if (has_value())
			{
				return value();
			}
			return default_value;
		}

	private:
		std::unique_ptr<T> ptr;
	};
}
