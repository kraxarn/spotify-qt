#pragma once

#include "lib/optional.hpp"

#include <string>

namespace lib
{
	/**
	 * Result response with error message, or a value
	 */
	template<typename T>
	class result
	{
	public:
		/**
		 * Success response
		 */
		static auto ok(T response) -> result
		{
			return result<T>(std::string(), lib::optional<T>(response));
		}

		/**
		 * Fail response
		 */
		static auto fail(const std::string &message) -> result
		{
			return result<T>(message, lib::optional<T>());
		}

		/**
		 * Result was successful
		 */
		auto success() const -> bool
		{
			return response.has_value();
		}

		/**
		 * Try and get value, throws if not successful result
		 */
		auto value() const -> T &
		{
			return response.value();
		}

		/**
		 * Get error message, or an empty string if successful result
		 */
		auto message() const -> std::string
		{
			return error;
		}

	private:
		std::string error;
		lib::optional<T> response;

		result(std::string message, lib::optional<T> response)
			: error(std::move(message)), response(std::move(response))
		{
		}
	};
}
