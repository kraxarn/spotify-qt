#pragma once

#include <optional>
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
			return result<T>(std::string(), std::optional<T>(response));
		}

		/**
		 * Fail response
		 */
		static auto fail(const std::string &message) -> result
		{
			return result<T>(message, std::optional<T>());
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
		auto value() const -> const T &
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
		std::optional<T> response;

		result(std::string message, std::optional<T> response)
			: error(std::move(message)), response(std::move(response))
		{
		}
	};
}
