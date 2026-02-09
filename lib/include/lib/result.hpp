#pragma once

#include <optional>

#include <QString>

/**
* Result response with an error message, or a value
*/
template<typename T>
class Result
{
public:
	/**
	* Success response
	*/
	static auto ok(T response) -> Result
	{
		return Result({}, std::optional<T>(response));
	}

	/**
	* Fail response
	*/
	static auto fail(const QString &message) -> Result
	{
		return Result(message, std::optional<T>());
	}

	/**
	* Result was successful
	*/
	[[nodiscard]]
	auto success() const -> bool
	{
		return response.has_value();
	}

	/**
	* Try and get value, throws if not successful result
	*/
	[[nodiscard]]
	auto value() const -> const T &
	{
		return response.value();
	}

	/**
	* Get the error message, or an empty string if successful result
	*/
	[[nodiscard]]
	auto message() const -> QString
	{
		return error;
	}

private:
	QString error;
	std::optional<T> response;

	Result(QString message, std::optional<T> response)
		: error(std::move(message)),
		response(std::move(response))
	{
	}
};

namespace lib
{
	template<typename T>
	using result [[deprecated("Use Result instead")]] = Result<T>;
}
